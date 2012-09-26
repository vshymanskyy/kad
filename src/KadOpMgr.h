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

class KadOpMgr {
	friend class KadOpMgrTS;

private:
	class ReqTracker {
	public:
		typedef XDelegate< void (const msgpack::object* rsp, KadContactPtr& contact) > Handler;

		ReqTracker (KadContactPtr& contact, Handler handler)
			: mDst			(contact)
			, mMsgId		(random()) // Todo: whole range
			, mHandler		(handler)
		{
		}

		void OnTimeout() {
			mDst->mFailQty++;
			mHandler(NULL, mDst);
		}

		void OnResponse(const msgpack::object* rsp) {
			mDst->mFailQty=0;
			mHandler(rsp, mDst);
		}

		class SelectById {
		public:
			SelectById(const uint64_t& id) : mMsgId (id) {};
			bool operator()(const ReqTracker& t) { return mMsgId == t.mMsgId; }
			bool operator()(const ReqTracker* t) { return mMsgId == t->mMsgId; }
		private:
			const uint64_t mMsgId;
		};

		KadContactPtr mDst;
		const uint64_t mMsgId;
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
			, mPendingQty(0)
		{
			KadContactList lst = mMgr->FindClosest(mKey, KADEMLIA_BUCKET_SIZE);
			for (KadContactList::It it=lst.First(); it!=lst.End(); ++it) {
				Insert(lst[it], mList);
			}
		}

		void Start() {
			int sentQty = Send(KADEMLIA_ALPHA);
			LOG(mMgr->mLog, FMT("%d sent", sentQty));
		}

		void OnResponce(const msgpack::object* rsp, KadContactPtr& contact) {
			KadContactList::It pending = mBlock.FindAfter(mBlock.First(), contact);
			if (pending == mBlock.End()) {
				LOG_WARN(mMgr->mLog, "Wrong pending");
				return;
			}

			mPendingQty--;

			if (!rsp) {
				if (contact->IsStale()) {
					LOG(mMgr->mLog, "Stale: " << contact->mAddrExt.ToString());

					KadContactList::It n = mList.FindAfter(mList.First(), contact);
					if (n != mList.End()) {
						mList.Remove(n);
					}
					//TODO: mMgr->RemoveNode();
				} else {
					mBlock.Remove(pending);
				}

				int sentQty = Send(1);
				if (sentQty) {
					LOG(mMgr->mLog, FMT("Timeout: %d pend, %d sent", mPendingQty-sentQty, sentQty));
				} else if (!mPendingQty) {
					LOG(mMgr->mLog, "Timeout: 0 pend, 0 sent -> Complete");
					Complete();
				}

				return;
			} else {
				const KadMsgFindRsp findRsp = rsp->convert();

				// Merge lists
				int insertQty = 0;
				for (std::vector<KadMsgContact>::const_iterator i=findRsp.mContacts.begin();
						i != findRsp.mContacts.end(); i++)
				{
					const KadId& id = i->mId;
					if (!id.IsZero() && id != mMgr->LocalId()) {
						if (!FindById(id, mBlock) && !FindById(id, mList)) {
							if (Insert(mMgr->FindAddContact(id, i->mAddr), mList)) {
								insertQty++;
							}
						}
					}
				}

				if (insertQty) {
					X_ASSERT_LE(mPendingQty, KADEMLIA_ALPHA, "%d");
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
					//LOG(mMgr->mLog, "Append " << (c->mId^mKey));
					lst.Append(c);
					return true;
				}
			} else {
				if (lst.Count() < KADEMLIA_BUCKET_SIZE) {
					//LOG(mMgr->mLog, "Insert " << (c->mId^mKey));
					lst.InsertBefore(it, c);
					return true;
				} else {
					//LOG(mMgr->mLog, "Replace " << (mList.Back()->mId^mKey) << " with" << (c->mId^mKey));
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
				for (KadContactList::It it=mList.First(); it!=mList.End(); ++it) {
					KadContactPtr& c = mList[it];
					if (stale == c->mFailQty &&
						mBlock.FindAfter(mBlock.First(), c) == mBlock.End())
					{
						mBlock.Append(c);

						KadMsgFindReq findReq(mKey);
						mMgr->SendRequest(findReq, new ReqTracker(c, ReqTracker::Handler(this, &KadIterativeFindOp::OnResponce)));

						mPendingQty++;

						if (++sent >= qty) {
							return sent;
						}
					}
				}
			}

			return sent;
		}

		void Complete () {
			LOG(mMgr->mLog, "Key: " << mKey.ToString());
			LOG(mMgr->mLog, "Contacted: " << mBlock.Count());
			LOG(mMgr->mLog, "List: " << mList.Count());

			for (XList<KadContactPtr>::It it=mList.First(); it!=mList.End(); ++it) {
				LOG(NULL, "    " << mList[it]->mId);
			}

		}
	private:
		KadId mKey;
		KadContactList mList;
		KadContactList mBlock;
		KadOpMgr* mMgr;
		int mPendingQty;
	};

private:
	void RecieveCbk(const void* buff, size_t len, const KadNet::Address& from) {
        msgpack::unpacked msg;
        msgpack::unpack(&msg, (const char*)buff, len);

        msgpack::object obj = msg.get();

        // Sanity check
        if (obj.type != msgpack::type::ARRAY ||		// Object
        	obj.via.array.size == 0 ||				// With items
        	obj.via.array.ptr[0].type !=
        		msgpack::type::ARRAY ||				// Header
        	obj.via.array.ptr[0].via.array.size !=
        		3 ||								// 3 fields in header
        	obj.via.array.ptr[0].via.array.ptr[0].type !=
        		msgpack::type::POSITIVE_INTEGER ||	// first field is Type number
        	obj.via.array.ptr[0].via.array.ptr[0].via.u64
        		>= KadMsg::KAD_MSG_QTY)				// proper range
        {
        	LOG_WARN(mLog, "Invalid format");
        	return;
        }

		//XThread::SleepMs(RandRange(0,8));

		//LOG_DEEP(mLog, "<< " << *req);

        KadMsg req = msg.get().via.array.ptr[0].convert();

		if (KadId(req.mSrcId) == LocalId()) {
			LOG_WARN(mLog, "Message from self -> dropped");
			return;
		}

		mLock.Lock();
		KadContactPtr contact = FindAddContact(req.mSrcId, from);
		mLock.Unlock();

		switch (req.mMsgType) {
		case KadMsg::KAD_MSG_PING: {
			KadMsgPong pong;
			SendResponse(pong, from, req);
		} break;
		case KadMsg::KAD_MSG_FIND_REQ: {
			const KadMsgFindReq findReq = msg.get().convert();
			mLock.Lock();
			KadContactList lst = mRoutingTable.FindClosest(findReq.mTargetId, KADEMLIA_BUCKET_SIZE);
			// TODO: Remove the originating id
			KadMsgFindRsp findRsp(lst);
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
			HandleResponse(req, &obj, from);
		} break;
		default:
			LOG_WARN(mLog, "Message not handled (type: " << req.mMsgType << ")");
			break;
		}
	}

private:
	template <typename T>
	void SendRequest(T& s, ReqTracker* handler) {
		//LOG_DEEP(mLog, "[> " << s);

		s.mSrcId = LocalId();
		s.mMsgId = handler->mMsgId;

		mOps.Append(handler);

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, s);
		mListener.SendTo(sbuf.data(), sbuf.size(), handler->mDst->mAddrExt);
		XTimerContext::Global().SetTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), KADEMLIA_TIMEOUT_RESPONSE, 0, handler);
	}

	template <typename T>
	void SendRequest(T& s, const KadNet::Address& addr) {
		//LOG_DEEP(mLog, "[> " << s);

		s.mSrcId = LocalId();
		s.mMsgId = 0;

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, s);
		mListener.SendTo(sbuf.data(), sbuf.size(), addr);
	}

	template <typename T>
	void SendResponse(T& s, const KadNet::Address& addr, const KadMsg& req) const {
		//LOG_DEEP(mLog, ">> " << s);

		s.mSrcId = LocalId();
		s.mMsgId = req.mMsgId;

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, s);
		mListener.SendTo(sbuf.data(), sbuf.size(), addr);
	}

	void HandleResponse(const KadMsg& hdr, const msgpack::object* obj, const KadNet::Address& addr) {
		X_UNUSED(addr);

		if (hdr.mMsgId == 0) {
			return;
		}

		mLock.Lock(); // TODO: Blocks
		XList<ReqTracker*>::It t = mOps.FindAfter(mOps.First(), ReqTracker::SelectById(hdr.mMsgId));
		if (t != mOps.End()) {
			ReqTracker* handler = mOps[t];
			mOps.Remove(t);
			XTimerContext::Global().CancelTimer(XTimerContext::Handler(this, &KadOpMgr::HandleTimeout), handler);
			handler->OnResponse(obj);
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

	KadContactPtr FindAddContact(const KadId& id, const KadNet::Address& addr) {
		if (id == LocalId()) {
			return KadContactPtr();
		}

		// look in RT
		if (KadContactPtr rt = mRoutingTable.AddNode(id, addr)) {
			return rt;
		}

		// look in the cache
		if (KadContactPtr cc = CheckInCache(id, addr)) {
			return cc;
		}

		return KadContactPtr();
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

	KadContactPtr CheckInCache(const KadId& id, const KadNet::Address& addr) {

		for (KadContactList::It it = mContactCache.First(); it != mContactCache.End(); ++it) {
			if (id == mContactCache[it]->mId) {	// TODO: Also check IP?
				// Hit -> move to back and return it
				KadContactPtr result = mContactCache[it];
				if (it != mContactCache.Last()) {
					mContactCache.Remove(it);
					mContactCache.Append(result);
				}
				return result;
			}
		}

		{
			// Miss -> append a new contact
			KadContactPtr result(new KadContact(id, addr));
			mContactCache.Append(result);
			// If cache too large -> forget oldest
			while (mContactCache.Count() > 64) {
				mContactCache.PopFront();
			}
			return result;
		}
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

	KadContactList mContactCache;

	XMutexRecursive	mLock;
	XLog 			mLog;

	KadNet::Listener mListener;
};

#endif /* KADTRANSACTIONMGR_H_ */
