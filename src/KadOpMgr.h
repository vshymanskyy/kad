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
	class KadRspHandler {
	public:
		typedef XDelegate< void (const KadMsgId& msg, const KadMsgRsp* rsp) > Handler;

		KadRspHandler(const KadMsgId& id, Handler handler)
			: mId		(id)
			, mHandler	(handler)
		{
			XPlatGetTime(&mStarted, NULL);
		}

		void operator()(const KadMsgRsp* rsp) {
			mHandler(mId, rsp);
		}

		class SelectById {
		public:
			SelectById(const KadMsgId& id) : mId (id) {};
			bool operator()(const KadRspHandler& t) { return mId == t.mId; }
			bool operator()(const KadRspHandler* t) { return mId == t->mId; }
		private:
			const KadMsgId& mId;
		};

	protected:
		const KadMsgId mId;
		XPlatDateTime	mStarted;
		const Handler	mHandler;
	};

	class KadIterativeFindOp
	{
		class Contact : public KadContact
		{
		public:
			Contact (const KadContact& c)
				: KadContact(c)
				, mState(NEW)
			{
				mFailQty = 0;
			}

			bool IsNew() { return mState == NEW && mMsg.IsZero(); }

			void GotResponse() { mFailQty = 0; mState = CONTACTED; mMsg = KadMsgId(); }
			void GotTimeout() { mFailQty++; mMsg = KadMsgId(); }

			KadMsgId mMsg;
		private:
			enum State {
				NEW, PENDING, CONTACTED
			} mState;
		};
	public:
		typedef XDelegate< void (XList<Contact>&) > Handler;

		KadIterativeFindOp(KadOpMgr* mgr, const KadId& key, Handler h)
			: mHandler(h)
			, mKey(key)
			, mPendingQty(0)
			, mMgr(mgr)
		{
			XList<const KadContact*> lst = mMgr->FindClosest(mKey, KADEMLIA_BUCKET_SIZE);
			for (XList<const KadContact*>::It it=lst.First(); it!=lst.End(); ++it) {
				Insert(Contact(*lst[it]));
			}
		}

		void Start() {
			int sentQty = Send(KADEMLIA_ALPHA);
			LOG(mMgr->mLog, FMT("%d sent", sentQty));
		}

		void OnResponce(const KadMsgId& msgId, const KadMsgRsp* rsp) {

			if (msgId.IsZero()) {
				LOG_WARN(mMgr->mLog, "Zero msgId event");
				return;
			}

			XList<Contact>::It rspContact = mList.First();
			for (; rspContact!=mList.End(); ++rspContact) {
				if (mList[rspContact].mMsg == msgId) {
					break;
				}
			}

			mPendingQty--;

			if (!rsp) {
				if (rspContact != mList.End()) {
					mList[rspContact].GotTimeout();
					if (mList[rspContact].IsStale()) {
						mList.Remove(rspContact);
					}
				}
				int sentQty = Send(1);//KADEMLIA_ALPHA-mPendingQty);
				if (sentQty) {
					LOG(mMgr->mLog, FMT("Timeout: %d pend, %d sent", mPendingQty-sentQty, sentQty));
				} else if (!mPendingQty) {
					LOG(mMgr->mLog, "Timeout: 0 pend, 0 sent -> Complete");
					Complete();
				}

				return;
			} else {
				if (rspContact != mList.End()) {
					mList[rspContact].GotResponse();
				}
				const KadMsgFindRsp* findRsp = (const KadMsgFindRsp*)rsp;

				// Merge lists
				int insertQty = 0;
				for (int i=0; i<KADEMLIA_BUCKET_SIZE; i++) {
					if (!findRsp->mContacts[i].id.IsZero() && findRsp->mContacts[i].id != mMgr->LocalId()) {
						KadContact c = findRsp->mContacts[i];
						if (Insert(Contact(c))) {
							insertQty++;
						}
					}
				}

				if (insertQty) {
					int sentQty = Send(KADEMLIA_ALPHA-mPendingQty);
					LOG(mMgr->mLog, FMT("%d pend, %d insert -> %d sent", mPendingQty-sentQty, insertQty, sentQty));

				} else if (!mPendingQty && !insertQty) {
					LOG(mMgr->mLog, "0 pend, 0 sent -> Complete");
					Complete();
				} else {
					LOG(mMgr->mLog, FMT("%d pend, %d insert", mPendingQty, insertQty));
				}
			}

		}

	private:
		bool Insert(const Contact& c) {
			XList<Contact>::It it=mList.First();
			for ( ;it!=mList.End(); ++it) {
				if (c.mId == mList[it].mId) {
					//LOG(mMgr->mLog, "Existing");
					return false;
				} else if (mKey.Closer(c.mId, mList[it].mId)) {
					break;
				}
			}

			if (it == mList.End()) {
				if (mList.Count() < KADEMLIA_BUCKET_SIZE) {
					//LOG(mMgr->mLog, "Append " << (c.mId^mKey));
					mList.Append(c);
					return true;
				}
			} else {
				if (mList.Count() < KADEMLIA_BUCKET_SIZE) {
					//LOG(mMgr->mLog, "Insert " << (c.mId^mKey));
					mList.InsertBefore(it, c);
					return true;
				} else {
					//LOG(mMgr->mLog, "Replace " << (mList.Back().mId^mKey) << " with" << (c.mId^mKey));
					mList.InsertBefore(it, c);
					mList.PopBack();
					return true;
				}
			}
			//LOG(mMgr->mLog, "Too far");
			return false;
		}

		// Return qty of pending contacts
		/*int Cleanup() {
			int pending = 0;
			for (XList<Contact>::It it=mList.First(); it!=mList.End(); ++it) {
				if (mList[it].IsPending()) {
					pending++;
				} else if (mList[it].IsTimeout()) {
					mList[it].GotTimeout();
					LOG(mMgr->mLog, (mList[it].IsStale()?"Stale:":"Timeout:")
							<< mList[it].mAddr.ToString());
				}
				if (mList[it].IsStale()) {
					mList.Remove(it);
					continue;
				}
			}

			X_ASSERT_LE(mList.Count(), KADEMLIA_BUCKET_SIZE, "%d");
			return pending;
		}*/

		int Send(int qty) {
			X_ASSERT_GE(qty, 0, "%d");
			X_ASSERT_LE(qty, KADEMLIA_ALPHA, "%d");
			int sent = 0;

			for (int i=0; i<qty; i++) {
				for (unsigned stale=0; stale<KADEMLIA_STALE; stale++) {
					for (XList<Contact>::It it=mList.First(); it!=mList.End(); ++it) {
						if (mList[it].IsNew() && stale == mList[it].mFailQty) {
							mList[it].mMsg = KadMsgId::Random();
							mMgr->SendRequest(KadMsgFindReq(mList[it].mMsg, mMgr->LocalId(), mKey), mList[it].mAddr, new KadRspHandler(mList[it].mMsg, KadRspHandler::Handler(this, &KadIterativeFindOp::OnResponce)));

							sent++;
							mPendingQty++;
							goto next_req;
						}
					}
				}
				next_req:
				;
			}
			return sent;
		}

		void Complete () {
			LOG(mMgr->mLog, "Found " << mList.Count() << "nodes");

			for (XList<Contact>::It it=mList.First(); it!=mList.End(); ++it) {
				LOG(NULL, "    " << (mList[it].mId ^ mKey));
			}

			if (mHandler) {
				mHandler(mList);
			}
		}

		Handler mHandler;
		KadId mKey;
		unsigned mPendingQty;
		XList<Contact> mList;
		KadOpMgr* mMgr;
	};


private:
	void RecieveCbk(const void* buff, unsigned len, const KadNet::Address& from) {
		const KadMsg* req = (const KadMsg*)buff;

		XThread::SleepMs(1);
		if (RandRange(0,100) < 10) return;

		LOG_DEEP(mLog, "<< " << *req);

		if (req->NodeId() == LocalId()) {
			LOG_WARN(mLog, "Message from self -> dropped");
			return;
		}

		mLock.Lock();
		AddNode(KadContact(req->NodeId(), from));
		mLock.Unlock();

		switch (req->MsgType()) {
		case KadMsg::KAD_MSG_PING: {
			const KadMsgPing* pingReq = (const KadMsgPing*)req;
			SendResponse(KadMsgPong(pingReq->MsgId(), LocalId()), from);
		} break;
		case KadMsg::KAD_MSG_FIND_REQ: {
			const KadMsgFindReq* findReq = (const KadMsgFindReq*)req;
			mLock.Lock();
			XList<const KadContact*> lst = FindClosest(findReq->FindId(), KADEMLIA_BUCKET_SIZE);
			// TODO: Remove the originating id
			SendResponse(KadMsgFindRsp(req->MsgId(), LocalId(), KadMsgRsp::KAD_MSG_STATUS_OK, lst), from);
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
	void SendRequest(const T& s, const KadNet::Address& addr, KadRspHandler* handler = NULL) {
		if (handler) {
			mOps.Append(handler);
		}
		//LOG_DEEP(mLog, "[> " << s);
		mListener.SendTo(&s, sizeof(s), addr);
		if (handler) {
			XTimerContext::Global().SetTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), KADEMLIA_TIMEOUT_RESPONSE, 0, handler);
		}
	}

	template <typename T>
	void SendResponse(const T& s, const KadNet::Address& addr) const {
		//LOG_DEEP(mLog, ">> " << s);
		mListener.SendTo(&s, sizeof(s), addr);
	}

	void HandleResponse(const KadMsgRsp* rsp, const KadNet::Address& addr) {
		X_UNUSED(addr);

		if (rsp->MsgId().IsZero()) {
			return;
		}

		mLock.Lock(); // TODO: Blocks
		XList<KadRspHandler*>::It t = mOps.FindAfter(mOps.First(), KadRspHandler::SelectById(rsp->MsgId()));
		if (t != mOps.End()) {
			KadRspHandler* handler = mOps[t];
			mOps.Remove(t);
			XTimerContext::Global().CancelTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), handler);
			(*handler)(rsp);
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
		KadRspHandler* handler = (KadRspHandler*)rsp;
		XList<KadRspHandler*>::It t = mOps.FindAfter(mOps.First(), handler);
		if (t != mOps.End()) {
			mOps.Remove(t);
			(*handler)(NULL);
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
		, mLog("KadOpMgr", XLog::DEEP)
	{

	}

	const KadId& LocalId() const { return mRoutingTable.LocalId(); }
	const KadNet::Address BindAddr() const { return mListener.BindAddr(); }

	void Find(const KadId& key, KadIterativeFindOp::Handler h) {
		//mLock.Lock();

		KadIterativeFindOp* op = new KadIterativeFindOp(this, key, h);
		//mOps.Append(op);
		op->Start();
		//mLock.Unlock();
	}

	void Init(const XList<KadContact>& bsp) {
		// Fill local routing table
		for (XList<KadContact>::It it = bsp.First(); it != bsp.End(); ++it) {
			AddNode(bsp[it]);
		}
	}

	void PrintNodes(const XList<KadContact>& lst) {
		for (XList<KadContact>::It it=lst.First(); it!=lst.End(); ++it) {
			LOG(mLog, "   " << lst[it].mId);
		}
	}

	void Join(const XList<XSockAddr>& bsp) {
		for (XList<XSockAddr>::It it = bsp.First(); it != bsp.End(); ++it) {
			SendRequest(KadMsgPing(KadMsgId(), LocalId()), bsp[it]);
		}
		XThread::SleepMs(KADEMLIA_TIMEOUT_RESPONSE*3);
		// Perform a node lookup for your own LocalId
		Find(LocalId(), NULL);
	}
	XList<const KadContact*> GetContacts() const {
		return mRoutingTable.GetContacts();
	}

private:

	void AddNode(const KadContact& contact) {
		if (contact.mId != LocalId()) {
			mRoutingTable.AddNode(contact);
		}
	}

	XList<const KadContact*> FindClosest(const KadId& id, unsigned qty) const {
		return mRoutingTable.FindClosest(id, qty);
	}

	void SendPing(const KadNet::Address& address, KadRspHandler::Handler h) {
		KadMsgId id = KadMsgId::Random();
		SendRequest(KadMsgPing(id, LocalId()), address, new KadRspHandler(id, h));
	}

public:
	void DumpTable() const {
		LOG(mLog, "Local ID : " << LocalId());
		mRoutingTable.Print();
	}

	void DumpTableDot(ostream& s) const {
		s << "digraph G {" << std::endl;
		mRoutingTable.PrintDot(s);
		s << "}" << std::endl;
	}

private:
	XList<KadRspHandler*> mOps;

	//XTimerContext	mTimeoutTimers;
	KadRtNode		mRoutingTable;
	XMutexRecursive	mLock;
	XLog 			mLog;

	KadNet::Listener mListener;
};

#endif /* KADTRANSACTIONMGR_H_ */
