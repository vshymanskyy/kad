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
#include "KadConnMgr.h"

#include "operations/KadPing.h"
#include "operations/KadFind.h"

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

class KadOpMgr
	: public XThread
{
	friend class KadOpMgrTS;

public:
	class KadPing
		: public KadOperation
	{
	public:
		typedef XDelegate< void () > Handler;

		KadPing(KadOpMgr* mgr, const XSockAddr& addr, Handler h)
			: KadOperation (mgr, mgr->GenId()), mAddr(addr), mHandler(h)
		{
			mMgr->mTimers.SetTimer(XTimerContext::Handler(this, &KadPing::SendPing), 0, 200);
			mMgr->mTimers.SetTimer(XTimerContext::Handler(this, &KadPing::Timeout), 1100);
		}

		~KadPing() {
			Stop();
		}

		void Stop() {
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadPing::SendPing));
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadPing::Timeout));
		}

		void Process(const KadMsgRsp* rsp) {
			Stop();
			if (mHandler) mHandler();
		}
	private:
		void SendPing() {
			mMgr->SendStructTo(KadMsgPing(mId, mMgr->mLocalId), mAddr);
		}

		void Timeout() {
			LOG(mMgr->mLog, "Ping timeout " << mAddr.ToString());
			Stop();
		}
		XSockAddr mAddr;
		Handler mHandler;
	};


	class KadFind
		: public KadOperation
	{
			class Contact
				: public KadContact
			{
			public:
				bool IsPending() const { return mState == PENDING && !IsTimeout(); }
				bool IsTimeout() const { return mState == PENDING && mTime.Elapsed() >= KADEMLIA_TIMEOUT_RESPONCE; }
				bool IsContacted() const { return mState == CONTACTED; }
				bool IsNew() const { return mState == NEW && !IsStale(); }

				void GotTimeout() { mFailQty += 1;	mState = NEW; }
				void GotResponce() { mFailQty = 0;	mState = CONTACTED; }
				void SentRequest() { mTime.Reset();	mState = PENDING; }

				Contact (const KadContact& c)
					: KadContact(c)
					, mState(NEW)
				{
					mFailQty = 0;
				}
			private:
				enum State {
					NEW, PENDING, CONTACTED, STALE
				} mState;
				XTimeCounter mTime;
			};
	public:
		typedef XDelegate< void (XList<Contact>&) > Handler;

		KadFind(KadOpMgr* mgr, const KadId& key, Handler h)
			: KadOperation (mgr, mgr->GenId())
			, mHandler(h)
			, mKey(key)
			, mCompleted(false)
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

		void Process(const KadMsgRsp* rsp) {
			const KadMsgFindRsp* msg = (const KadMsgFindRsp*)rsp;

			//LOG_WARN(mMgr->mLog, *msg);

			bool found = false;
			for (XList<Contact>::It it = mList.First(); it!=mList.End(); ++it) {
				if (mList[it].mId == rsp->NodeId()) {
					mList[it].GotResponce();
					found = true;
					break;
				}
			}
			if (!found) {
				//LOG_WARN(mMgr->mLog, "Response from unknown contact");
				return;
			}

			int pendingQty = Cleanup();
			int insertQty = 0;
			for (int i=0; i<KADEMLIA_BUCKET_SIZE; i++) {
				if (!msg->mContacts[i].id.IsZero() && msg->mContacts[i].id != mMgr->mLocalId) {
					KadContact c = msg->mContacts[i];
					if (Insert(Contact(c))) {
						insertQty++;
					}
				}
			}

			if (insertQty) {
				int sentQty = Send(KADEMLIA_ALPHA-pendingQty);
				LOG(mMgr->mLog, FMT("%d pend, %d insert, %d sent", pendingQty, insertQty, sentQty));
			} else if (!pendingQty && !insertQty) {
				Complete();
			} else {
				LOG(mMgr->mLog, FMT("%d pend, %d insert", pendingQty, insertQty));
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
		int Cleanup() {
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
		}

		int Send(int qty) {
			int sent = 0;
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadFind::Timeout));

			for (int i=0; i<qty; i++) {
				for (unsigned stale=0; stale<KADEMLIA_STALE; stale++) {
					for (XList<Contact>::It it=mList.First(); it!=mList.End(); ++it) {
						if (mList[it].IsNew() && stale == mList[it].mFailQty) {
							mList[it].SentRequest();
							mMgr->SendStructTo(KadMsgFindReq(mId, mMgr->mLocalId, mKey), mList[it].mAddr);

							sent++;
							goto next_req;
						}
					}
				}
				next_req:;
			}
			if (sent) {
				mMgr->mTimers.SetTimer(XTimerContext::Handler(this, &KadFind::Timeout), KADEMLIA_TIMEOUT_RESPONCE);
			}
			return sent;
		}

		void Timeout() {
			mMgr->mLock.Lock();
			LOG(mMgr->mLog, "Timeout timer:");
			int pendingQty = Cleanup();
			int sentQty = Send(KADEMLIA_ALPHA-pendingQty);
			LOG(mMgr->mLog, FMT("%d pend, %d sent", pendingQty, sentQty));
			if (!sentQty) {
				Complete();
			}
			mMgr->mLock.Unlock();
		}

		void Complete () {
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadFind::Timeout));

			if(mCompleted) return;
			mCompleted = true;

			LOG(mMgr->mLog, "Found " << mList.Count() << "nodes");

			for (XList<Contact>::It it=mList.First(); it!=mList.End(); ++it) {
				LOG(mMgr->mLog, "    " << (mList[it].mId ^ mKey));
			}

			if (mHandler) {
				mHandler(mList);
			}
		}

		Handler mHandler;
		KadId mKey;
		bool mCompleted;
		XList<Contact> mList;
	};


private:
	virtual int Run()
	{
		XSockAddr from;

		char buffer[KADEMLIA_MAX_MSG_SIZE];
		const KadMsg& req = (const KadMsg&)buffer;

		while (!IsStopping()) {
			ssize_t len = mSocket.RecvFrom(&buffer, sizeof(buffer), &from);
			if (len <= 0) continue;
			if (IsStopping()) break;

			if (DELAY_RX) {
				XThread::SleepMs(RandRange(0,DELAY_RX));
			}

			LOG_DEEP(mLog, "<< " << req);

			switch (req.MsgType()) {
			case KadMsg::KAD_MSG_PING: {
				const KadMsgPing& req = (const KadMsgPing&)buffer;
				SendStructTo(KadMsgPong(req.MsgId(), mLocalId), from);

				mLock.Lock();
				AddNode(KadContact(req.NodeId(), from));
				mLock.Unlock();
			} break;
			case KadMsg::KAD_MSG_FIND_REQ: {
				const KadMsgFindReq& req = (const KadMsgFindReq&)buffer;
				mLock.Lock();
				AddNode(KadContact(req.NodeId(), from));

				XList<const KadContact*> lst = FindClosest(req.FindId(), KADEMLIA_BUCKET_SIZE);

				// TODO: Remove the originating id

				SendStructTo(KadMsgFindRsp(req.MsgId(), mLocalId, KadMsgRsp::KAD_MSG_STATUS_OK, lst), from);
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
				mLock.Lock();
				AddNode(KadContact(req.NodeId(), from));
				if (KadMsgId::Zero() == req.MsgId()) {
					mLock.Unlock();
					break;
				}
				XList<KadOperation*>::It t = mOps.FindAfter(mOps.First(), KadOperation::SelectById(req.MsgId()));
				if (t != mOps.End()) {
					mOps[t]->Process((const KadMsgRsp*)buffer);
				} else {
					LOG_WARN(mLog, "Transaction not found");
				}
				mLock.Unlock();
			} break;
			default:
				LOG_WARN(mLog, "Message not handled (type: " << req.MsgType() << ")");
				break;
			}
		}
		return 0;
	}

private:
	KadMsgId GenId() const {
		KadMsgId id;
		for (int gen = 0; gen < 100; gen++) {
			id = KadMsgId::Random();
			if (KadMsgId::Zero() != id && mOps.End() == mOps.FindAfter(mOps.First(), KadOperation::SelectById(id))) {
				return id;
			}
		}
		X_FATAL("Could not find unused transaction id");
		return KadMsgId::Zero();
	}

	template <typename T>
	void SendStructTo(const T& s, const XSockAddr& addr) const {
		LOG(mLog, ">>" << addr.ToString() << ":" << s);

		if (RandRange(0,100) >= DROP_RATE_TX) { // TODO: Remove
			mSocket.SendTo(&s, sizeof(s), addr);
		}
	}
public:

	static int DROP_RATE_TX;
	static int DROP_RATE_RX;
	static int DELAY_RX;

	KadOpMgr(const KadId& id, const XSockAddr& addr)
		: XThread(addr.ToString())
		, mLocalId (id)
	{
		//mLog.SetLevel(XLog::WARN);
		if (mSocket.Bind(addr)) {
			mBindAddr = mSocket.GetBindAddr();
			LOG(mLog, "Bound to address " << mBindAddr.ToString());
		} else {
			LOG_CRIT(mLog, "Could not bind to address " << mBindAddr.ToString());
			return;
		}

		Start();
	}

	virtual ~KadOpMgr() {
		XThread::Stop();
		mSocket.SendTo("wake!", 6, mSocket.GetBindAddr());
		XThread::Wait();
	}

	const KadId& LocalId() const { return mLocalId; }
	const XSockAddr& BindAddr() const { return mBindAddr; }


	void Find(const KadId& key, KadFind::Handler h) {
		mLock.Lock();

		KadFind* op = new KadFind(this, key, h);
		mOps.Append(op);
		op->Start();
		mLock.Unlock();
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
			SendStructTo(KadMsgPing(KadMsgId::Zero(), mLocalId), bsp[it]);
		}
		XThread::SleepMs(KADEMLIA_TIMEOUT_RESPONCE);
		// Perform a node lookup for your own LocalId
		Find(mLocalId, NULL);
	}

	XList<const KadContact*> GetContacts() const {
		return mRoutingTable.GetContacts();
	}

private:

	void AddNode(const KadContact& contact) {
		if (contact.mId != mLocalId) {
			mRoutingTable.AddNode(contact, contact.mId ^ mLocalId);
		}
	}

	void RemoveNode(const KadContact& contact) {
		mRoutingTable.RemoveNode(contact.mId, contact.mId ^ mLocalId);
	}

	XList<const KadContact*> FindClosest(const KadId& id, int qty) const {
		return mRoutingTable.FindClosest(id, id ^ mLocalId, qty);
	}

	void Ping(const XSockAddr& address, KadPing::Handler h) {
		mOps.Append(new KadPing(this, address, h));
	}

public:
	void DumpTable() const {
		LOG(mLog, "Local ID : " << mLocalId);
		mRoutingTable.Print();
	}

private:
	XList<KadOperation*> mOps;

	//KadConnMgr		mConnections;
	XSocketUdp		mSocket;
	XTimerContext	mTimers;
	//XTimerContext	mPeriodicTimers;
	KadId			mLocalId;
	XSockAddr		mBindAddr;
	KadRtNode		mRoutingTable;
	XMutexRecursive	mLock;
};

#endif /* KADTRANSACTIONMGR_H_ */
