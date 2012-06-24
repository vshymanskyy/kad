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

		}

	private:
		void Send() {
			KadContact contacts[KADEMLIA_ALPHA];
			mMgr->FindClosest(mKey, contacts, KADEMLIA_ALPHA);
			for (int i=0; i<KADEMLIA_ALPHA; ++i) {

				for(XList<KadAddrInfo>::It it = contacts[i].mAddresses.First();
					it != contacts[i].mAddresses.End(); ++it)
				{
					mMgr->SendStructTo(KadMsgFindReq(mId, mMgr->mLocalId, mKey), contacts[i].mAddresses[it].mAddr);
					LOG(mMgr->mLog, "KadFind sent to " << contacts[i].mAddresses[it].mAddr.ToString());
				}

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

			switch (req.MsgType()) {
			case KadMsg::KAD_MSG_PING: {
				LOG(mLog, "PING...");
				SendStructTo(KadMsgPong(req.MsgId(), mLocalId), from);
			} break;
			case KadMsg::KAD_MSG_JOIN_REQ: {

			} break;
			case KadMsg::KAD_MSG_FIND_REQ: {

			} break;
			case KadMsg::KAD_MSG_PONG:
			case KadMsg::KAD_MSG_JOIN_RSP:
			case KadMsg::KAD_MSG_FIND_RSP: {
				XList<KadOperation*>::It t = mOps.FindAfter(mOps.First(), KadOperation::SelectById(req.MsgId()));
				if (t != mOps.End()) {
					mOps[t]->Process((const KadMsgRsp*)buffer);
				} else {
					LOG(mLog, "Ping transaction not found");
				}
			} break;
			default:
				LOG(mLog, "Message not handled (type: " << req.MsgType() << ")");
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
		if (RandRange(0,100) < 90) { // TODO: Remove
			mSocket.SendTo(&s, sizeof(s), addr);
		}
	}
public:

	KadOpMgr(const KadNodeId& id, const XSockAddr& addr, const XList<KadContact>& bsp)
		: XThread(XString("KadOpMgr on ") + addr.ToString())
		, mLocalId (id)
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

	void Ping(const XSockAddr& address, KadPing::Handler h) {
		mOps.Append(new KadPing(this, address, h));
	}

	void Find(const KadKey& key, KadPing::Handler h) {
		mOps.Append(new KadFind(this, key, h));
	}

private:
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
	KadNodeId		mLocalId;
	KadRtNode*			mRoutingTable;

};

#endif /* KADTRANSACTIONMGR_H_ */
