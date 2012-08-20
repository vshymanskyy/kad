#ifndef KADTRANSACTIONMGR_H_
#define KADTRANSACTIONMGR_H_

#include <XList.h>
#include <XLog.h>
#include <XThread.h>
#include <XDebug.h>
#include <XDelegate.h>
#include <XTimer.h>
#include <XLocks.h>
#include <net/XSocketUdp.h>
#include <XTimeCounter.h>

#include "KadConfig.h"
#include "KadRtNode.h"

#include "operations/KadPing.h"
#include "operations/KadFind.h"

#include "KadOverIP.h"

#include <iostream>
#include <map>
using namespace std;

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadMsg& v)
{
	switch (v.MsgType()) {
	case KadMsg::KAD_MSG_PING: return str << (const KadMsgPing&)v;
	case KadMsg::KAD_MSG_PONG: return str << (const KadMsgPong&)v;
	case KadMsg::KAD_MSG_FIND_REQ: return str << (const KadMsgFindReq&)v;
	case KadMsg::KAD_MSG_FIND_RSP: return str << (const KadMsgFindRsp&)v;
	default: return str << "KAD_MSG_UNKNOWN";
	}
}

class KadOpMgr {
	friend class KadOpMgrTS;

private:
	class ReqTracker {
	public:
		typedef XDelegate< void (const KadMsgRsp* rsp, KadContactPtr& contact) > Handler;

		ReqTracker (KadContactPtr& contact, Handler handler)
			: mDst			(contact)
			, mMsgId		(KadMsgId::Random())
			, mHandler		(handler)
		{
		}

		void OnTimeout() {
			mDst->mFailQty++;
			mHandler(NULL, mDst);
		}

		void OnResponse(const KadMsgRsp* rsp) {
			mDst->mFailQty=0;
			mHandler(rsp, mDst);
		}

		class SelectById {
		public:
			SelectById(const KadMsgId& id) : mMsgId (id) {};
			bool operator()(const ReqTracker& t) { return mMsgId == t.mMsgId; }
			bool operator()(const ReqTracker* t) { return mMsgId == t->mMsgId; }
		private:
			const KadMsgId mMsgId;
		};

		KadContactPtr mDst;
		const KadMsgId mMsgId;
	protected:
		Handler			mHandler;
		//time	mStarted;
	};


	class KadIterativeFindOp
	{

	public:
		KadIterativeFindOp(KadOpMgr* mgr, const KadId& key)
			: mKey(key)
			, mMgr(mgr)
		{
			KadContactList lst = mMgr->FindClosest(mKey, KADEMLIA_BUCKET_SIZE);
			for (KadContactList::It it=lst.First(); it!=lst.End(); ++it) {
				Insert(lst[it], mNew);
			}
		}

		void Start() {
			int sentQty = Send(KADEMLIA_ALPHA);
			LOG(mMgr->mLog, FMT("%d sent", sentQty));
		}

		void OnResponce(const KadMsgRsp* rsp, KadContactPtr& contact) {
			KadContactList::It pending = mPending.FindAfter(mPending.First(), contact);
			if (pending == mPending.End()) {
				LOG_WARN(mMgr->mLog, "Wrong pending");
				return;
			}

			mPending.Remove(pending);

			if (!rsp) {
				if (contact->IsStale()) {
					KadContactList::It n = mNew.FindAfter(mNew.First(), contact);
					mNew.Remove(n);
					LOG(mMgr->mLog, "Stale: " << contact->mAddrExt.ToString());
				}

				int sentQty = Send(1);
				if (sentQty) {
					LOG(mMgr->mLog, FMT("Timeout: %d pend, %d sent", mPending.Count()-sentQty, sentQty));
				} else if (!mPending.Count()) {
					LOG(mMgr->mLog, "Timeout: 0 pend, 0 sent -> Complete");
					Complete();
				}

				return;
			} else {
				mContacted.Append(contact);

				const KadMsgFindRsp* findRsp = (const KadMsgFindRsp*)rsp;

				// Merge lists
				int insertQty = 0;
				for (int i=0; i<KADEMLIA_BUCKET_SIZE; i++) {
					const KadId& id = findRsp->mContacts[i].id;
					if (!id.IsZero() && id != mMgr->LocalId()) {
						if (!FindById(id, mContacted) && !FindById(id, mNew)) {
							KadContact c(findRsp->mContacts[i].id, XSockAddr(findRsp->mContacts[i].addr));
							if (Insert(mMgr->FindAddContact(c), mNew)) {
								insertQty++;
							}
						} else {

						}
					}
				}

				if (insertQty) {
					X_ASSERT_LE(mPending.Count(), KADEMLIA_ALPHA, "%d");
					int sentQty = Send(KADEMLIA_ALPHA-mPending.Count());
					LOG(mMgr->mLog, FMT("%d pend, %d insert -> %d sent", mPending.Count()-sentQty, insertQty, sentQty));

				} else if (!mPending.Count() && !insertQty) {
					LOG(mMgr->mLog, "0 pend, 0 sent -> Complete");
					Complete();
				} else {
					LOG(mMgr->mLog, FMT("%d pend, %d insert", mPending.Count(), insertQty));
				}
			}
		}

	private:
		bool Insert(const KadContactPtr& c, XList<KadContactPtr>& lst) {
			XList<KadContactPtr>::It it=lst.First();
			for ( ;it!=lst.End(); ++it) {
				if (c->mId == lst[it]->mId) {
					//LOG(mMgr->mLog, "Existing");
					return false;
				} else if (mKey.Closer(c->mId, lst[it]->mId)) {
					break;
				}
			}

			if (it == lst.End()) {
				if (lst.Count() < KADEMLIA_BUCKET_SIZE) {
					//LOG(mMgr->mLog, "Append " << (c.mId^mKey));
					lst.Append(c);
					return true;
				}
			} else {
				if (lst.Count() < KADEMLIA_BUCKET_SIZE) {
					//LOG(mMgr->mLog, "Insert " << (c.mId^mKey));
					lst.InsertBefore(it, c);
					return true;
				} else {
					//LOG(mMgr->mLog, "Replace " << (mList.Back().mId^mKey) << " with" << (c.mId^mKey));
					lst.InsertBefore(it, c);
					lst.PopBack();
					return true;
				}
			}
			//LOG(mMgr->mLog, "Too far");
			return false;
		}

		KadContactPtr FindById(const KadId& id, const KadContactList& lst) {
			for (KadContactList::It it=lst.First(); it!=lst.End(); ++it) {
				if(lst[it]->mId == id) {
					return lst[it];
				}
			}
			return KadContactPtr();
		}

		int Send(int qty) {
			X_ASSERT(qty > 0);

			int sent = 0;
			for (unsigned stale = 0; stale<KADEMLIA_STALE; stale++) {
				for (KadContactList::It it=mNew.First(); it!=mNew.End(); ++it) {
					KadContactPtr& c = mNew[it];
					if (stale == c->mFailQty &&
						mContacted.FindAfter(mContacted.First(), c) == mContacted.End() &&
						mPending.FindAfter(mPending.First(), c) == mPending.End())
					{
						mPending.Append(c);
						X_ASSERT_LE(mPending.Count(), KADEMLIA_ALPHA, "%d");

						KadMsgFindReq findReq(mKey);
						mMgr->SendRequest(findReq, new ReqTracker(c, ReqTracker::Handler(this, &KadIterativeFindOp::OnResponce)));

						if (++sent >= qty) {
							return sent;
						}
					}
				}
			}

			return sent;
		}

		void Complete () {
			LOG(mMgr->mLog, "List: " << mNew.Count());

			for (XList<KadContactPtr>::It it=mNew.First(); it!=mNew.End(); ++it) {
				LOG(NULL, "    " << (mNew[it]->mId ^ mKey));
			}

			LOG(mMgr->mLog, "Contacted: " << mContacted.Count());

			for (XList<KadContactPtr>::It it=mContacted.First(); it!=mContacted.End(); ++it) {
				LOG(NULL, "    " << (mContacted[it]->mId ^ mKey));
			}
		}
	private:
		KadId mKey;
		KadContactList mNew;
		KadContactList mPending;
		KadContactList mContacted;
		KadOpMgr* mMgr;
	};

private:
	void RecieveCbk(const void* buff, unsigned len, const KadNet::Address& from) {
		const KadMsg* req = (const KadMsg*)buff;

		XThread::SleepMs(RandRange(0,8));

		LOG_DEEP(mLog, "<< " << *req);

		if (req->NodeId() == LocalId()) {
			LOG_WARN(mLog, "Message from self -> dropped");
			return;
		}

		mLock.Lock();
		KadContact newCont(req->NodeId(), from);
		KadContactPtr contact = FindAddContact(newCont);
		mLock.Unlock();

		switch (req->MsgType()) {
		case KadMsg::KAD_MSG_PING: {
			KadMsgPong pong;
			SendResponse(pong, from, req);
		} break;
		case KadMsg::KAD_MSG_FIND_REQ: {
			const KadMsgFindReq* findReq = (const KadMsgFindReq*)req;
			mLock.Lock();
			KadContactList lst = mRoutingTable.FindClosest(findReq->FindId(), KADEMLIA_BUCKET_SIZE);
			// TODO: Remove the originating id
			KadMsgFindRsp findRsp(KadMsgRsp::KAD_MSG_STATUS_OK, lst);
			SendResponse(findRsp, from, req);
			mLock.Unlock();
		} break;
		case KadMsg::KAD_MSG_STORE_REQ: {

		} break;
		case KadMsg::KAD_MSG_REMOVE_REQ: {

		} break;
		case KadMsg::KAD_MSG_PONG:
		case KadMsg::KAD_MSG_JOIN_RSP:
		case KadMsg::KAD_MSG_FIND_RSP:
		case KadMsg::KAD_MSG_STORE_RSP:
		case KadMsg::KAD_MSG_REMOVE_RSP: {
			HandleResponse((const KadMsgRsp*)buff, from);
		} break;
		default:
			LOG_WARN(mLog, "Message not handled (type: " << req->MsgType() << ")");
			break;
		}
	}

private:
	template <typename T>
	void SendRequest(T& s, ReqTracker* handler) {
		//LOG_DEEP(mLog, "[> " << s);

		s.SetNodeId(LocalId());
		s.SetMsgId(handler->mMsgId);

		mOps.Append(handler);
		mListener.SendTo(&s, sizeof(s), handler->mDst->mAddrExt);
		XTimerContext::Global().SetTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), KADEMLIA_TIMEOUT_RESPONSE, 0, handler);
	}

	template <typename T>
	void SendRequest(T& s, const KadNet::Address& addr) {
		//LOG_DEEP(mLog, "[> " << s);

		s.SetNodeId(LocalId());
		s.SetMsgId(0);

		mListener.SendTo(&s, sizeof(s), addr);
	}

	template <typename T>
	void SendResponse(T& s, const KadNet::Address& addr, const KadMsg* req) const {
		//LOG_DEEP(mLog, ">> " << s);

		s.SetNodeId(LocalId());
		s.SetMsgId(req->MsgId());
		mListener.SendTo(&s, sizeof(s), addr);
	}

	void HandleResponse(const KadMsgRsp* rsp, const KadNet::Address& addr) {
		X_UNUSED(addr);

		if (rsp->MsgId().IsZero()) {
			return;
		}

		mLock.Lock(); // TODO: Blocks
		XList<ReqTracker*>::It t = mOps.FindAfter(mOps.First(), ReqTracker::SelectById(rsp->MsgId()));
		if (t != mOps.End()) {
			ReqTracker* handler = mOps[t];
			mOps.Remove(t);
			XTimerContext::Global().CancelTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), handler);
			handler->OnResponse(rsp);
			delete handler;
		} else {
			LOG_WARN(mLog, "Transaction not found (response)");
		}
		mLock.Unlock();
	}

	void HandleTimeout(void* rsp) {
		X_ASSERT(rsp);
		mLock.Lock(); // TODO: Blocks
		// Check if the handler still exists
		ReqTracker* handler = (ReqTracker*)rsp;
		XList<ReqTracker*>::It t = mOps.FindAfter(mOps.First(), handler);
		if (t != mOps.End()) {
			mOps.Remove(t);
			handler->OnTimeout();
			delete handler;
		} else {
			LOG_WARN(mLog, "Transaction not found (timeout)");
		}
		mLock.Unlock();
	}

public:

	KadOpMgr(const KadId& id, const XSockAddr& addr)
		: mRoutingTable (id)
		, mListener (addr, KadNet::Listener::Handler(this, &KadOpMgr::RecieveCbk))
	{
	}

	const KadId& LocalId() const { return mRoutingTable.LocalId(); }
	//const KadRtNode& RT() const { return mRoutingTable; }
	const KadNet::Address BindAddr() const { return mListener.BindAddr(); }

	KadContactList FindClosest(const KadId& id, unsigned qty) {
		return mRoutingTable.FindClosest(id, qty);
	}

	KadContactPtr FindAddContact(KadContact& cont) {
		if (cont.mId == LocalId()) {
			return KadContactPtr();
		}

		return mRoutingTable.AddNode(cont);
	}

	void Find(const KadId& key) {
		mLock.Lock();

		KadIterativeFindOp* op = new KadIterativeFindOp(this, key);
		op->Start();

		mLock.Unlock();
	}

	void Init(const KadContactList& bsp) {
		// Fill local routing table
		for (KadContactList::It it = bsp.First(); it != bsp.End(); ++it) {
			//FindAddContact(bsp[it]);
		}

	}

	void Join(const XList<XSockAddr>& bsp) {
		for (XList<XSockAddr>::It it = bsp.First(); it != bsp.End(); ++it) {
			KadMsgPing ping;
			SendRequest(ping, bsp[it]);
		}
		XThread::SleepMs(KADEMLIA_TIMEOUT_RESPONSE);
		// Perform a node lookup for your own LocalId
		Find(LocalId());

		//XThread::SleepMs(KADEMLIA_TIMEOUT_OPERATION);
	}
	KadContactList GetContacts() const {
		return mRoutingTable.GetContacts();
	}

private:

	void SendPing(const KadNet::Address& addr) {
		KadMsgPing ping;
		SendRequest(ping, addr);
	}

public:

	void DumpTableDot(ostream& s) const {
		s << "digraph G {" << std::endl;
		mRoutingTable.PrintDot(s);
		s << "}" << std::endl;
	}

private:
	XList<ReqTracker*> mOps;

	//XTimerContext	mTimeoutTimers;
	KadRtNode		mRoutingTable;
	XMutexRecursive	mLock;
	XLog 			mLog;

	KadNet::Listener mListener;
};

#endif /* KADTRANSACTIONMGR_H_ */
