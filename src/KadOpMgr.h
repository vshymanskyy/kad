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

#include "KadConfig.h"
#include "KadRtNode.h"

#include "operations/KadPing.h"
#include "operations/KadFind.h"

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
			LOG(mMgr->mLog, "KadPing started");
			mMgr->mTimers.SetTimer(XTimerContext::Handler(this, &KadPing::SendPing), 0, 200);
			mMgr->mTimers.SetTimer(XTimerContext::Handler(this, &KadPing::Timeout), 1100);
		}

		~KadPing() {
			Stop();
		}

		void Stop() {
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadPing::SendPing));
			mMgr->mTimers.CancelTimer(XTimerContext::Handler(this, &KadPing::Timeout));
			LOG(mMgr->mLog, "KadPing stopped");
		}

		void Process(const KadMsgRsp* rsp) {
			const KadMsgPong* msg = (const KadMsgPong*)rsp;
			LOG(mMgr->mLog, "<< " << *msg);
			Stop();
			mHandler();
		}
	private:
		void SendPing() {
			LOG(mMgr->mLog, "KadPing sent...");
			mMgr->SendStructTo(KadMsgPing(mId, mMgr->mLocalId), mAddr);
		}

		void Timeout() {
			LOG(mMgr->mLog, "KadPing timeout");
			Stop();
		}
		XSockAddr mAddr;
		Handler mHandler;
	};


	class KadFind
		: public KadOperation
	{
	public:
		typedef XDelegate< void () > Handler;

		KadFind(KadOpMgr* mgr, const KadKey& key, Handler h)
			: KadOperation (mgr, mgr->GenId())
			, mHandler(h)
			, mKey(key)
		{
			Send();
		}

		void Process(const KadMsgRsp* rsp) {
			const KadMsgFindRsp* msg = (const KadMsgFindRsp*)rsp;
			LOG(mMgr->mLog, "<< " << *msg);
		}

	private:
		void Send() {
			KadContact contacts[KADEMLIA_ALPHA];
			int qty = mMgr->FindClosest(mKey, contacts, KADEMLIA_ALPHA);
			for (int i=0; i<qty; ++i) {
				mMgr->SendStructTo(KadMsgFindReq(mId, mMgr->mLocalId, mKey), contacts[i].mAddrExt);
			}
		}

		Handler mHandler;
		KadKey mKey;
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

			Seen(KadContact(req.NodeId(), from));

			switch (req.MsgType()) {
			case KadMsg::KAD_MSG_PING: {
				const KadMsgPing& req = (const KadMsgPing&)buffer;
				LOG(mLog, "<< " << req);

				SendStructTo(KadMsgPong(req.MsgId(), mLocalId), from);
			} break;
			case KadMsg::KAD_MSG_FIND_REQ: {
				const KadMsgFindReq& req = (const KadMsgFindReq&)buffer;
				LOG(mLog, "<< " << req);

				KadContact contacts[KADEMLIA_BUCKET_SIZE];
				int qty = FindClosest(req.FindId(), contacts, KADEMLIA_BUCKET_SIZE);
				SendStructTo(KadMsgFindRsp(req.MsgId(), mLocalId, KadMsgRsp::KAD_MSG_STATUS_OK, contacts, qty), from);
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
				XList<KadOperation*>::It t = mOps.FindAfter(mOps.First(), KadOperation::SelectById(req.MsgId()));
				if (t != mOps.End()) {
					mOps[t]->Process((const KadMsgRsp*)buffer);
				} else {
					LOG_WARN(mLog, "Transaction not found");
				}
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
			MemRand(&id, sizeof(id));
			if (mOps.End() == mOps.FindAfter(mOps.First(), KadOperation::SelectById(id))) {
				return id;
			}
		}
		X_FATAL("Could not find unused transaction id");
		return 0;
	}

	template <typename T>
	void SendStructTo(const T& s, const XSockAddr& addr) const {
		LOG(mLog, ">> " << s);
		//if (RandRange(0,100) < 90) { // TODO: Remove
			mSocket.SendTo(&s, sizeof(s), addr);
		//}
	}
public:

	KadOpMgr(const KadNodeId& id, const XSockAddr& addr)
		: XThread(addr.ToString())
		, mLocalId (id)
		, mBindAddr (addr)
		, mRoutingTable(new KadRtNode(0, KadNodeId::Zero()))
	{
		mSocket.Bind(addr);
		LOG(mLog, "Bound to address " << mSocket.GetBindAddr().ToString());
		Start();
	}

	virtual ~KadOpMgr() {
		XThread::Stop();
		mSocket.SendTo("wake!", 6, mSocket.GetBindAddr());
		XThread::Wait();
		delete mRoutingTable;
	}

	const KadNodeId& LocalId() const { return mLocalId; }
	const XSockAddr& BindAddr() const { return mBindAddr; }


	void Find(const KadKey& key, KadPing::Handler h) {
		mOps.Append(new KadFind(this, key, h));
	}

	void Join(const XList<KadContact>& bsp) {
		// Fill local routing table with bootstrap peers
		for (XList<KadContact>::It it = bsp.First(); it != bsp.End(); ++it) {
			Seen(bsp[it]);
		}
		// Perform a node lookup for your own LocalId
		Find(mLocalId, NULL);
	}

	XList<KadContact> Leave() {
		return mRoutingTable->GetContacts();
	}

private:
	void Ping(const XSockAddr& address, KadPing::Handler h) {
		mOps.Append(new KadPing(this, address, h));
	}

	void Seen(const KadContact& contact) {
		mRoutingTable->AddNode(contact, contact.mId ^ mLocalId);
	}

	int FindClosest(const KadNodeId& id, KadContact* contacts, int qty) const {
		return mRoutingTable->GatherClosest(id, id ^ mLocalId, contacts, qty);
	}

	void DumpTable() const {
		LOG(mLog, "Local ID : " << mLocalId);
		mRoutingTable->Print();
	}

private:
	XList<KadOperation*> mOps;

	XSocketUdp		mSocket;
	XTimerContext	mTimers;
	XTimerContext	mPeriodicTimers;
	KadNodeId		mLocalId;
	XSockAddr		mBindAddr;
	KadRtNode*		mRoutingTable;

};

#endif /* KADTRANSACTIONMGR_H_ */
