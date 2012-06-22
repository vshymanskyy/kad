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

#include "operations/KadPing.h"
#include "operations/KadFindNode.h"

class KadOpMgr : public XThread
{

private:
	virtual int Run()
	{
		XSockAddr from;

		char buffer[KADEMLIA_MAX_MSG_SIZE];
		const KadMsg& req = (const KadMsg&)buffer;

		while (!IsStopping()) {
			ssize_t len = mSocket.RecvFrom(&buffer, sizeof(buffer), &from);
			if (len <= 0 || IsStopping()) {
				continue;
			}

			switch (req.MsgType()) {
			case KadMsg::PING: {
				LOG(mLog, "PING...");
				//mSocket.SendStructTo(KadMsgPong(req.MsgId(), mMyId), from); //TODO
			} break;
			case KadMsg::PONG: {
				XList<KadPing>::It t = mPings.FindAfter(mPings.First(), KadOperation::SelectById(req.MsgId()));
				if (t != mPings.End()) {
					mPings[t].Process((const KadMsgPong&)buffer);
					LOG(mLog, "PONG!");
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
	template <typename T>
	KadMsgId FindUnusedTransactionId(const T& list) const {
		KadMsgId id;
		for (int gen = 0; gen < 100; gen++) {
			MemRand(&id, sizeof(id));
			if (mPings.End() == mPings.FindAfter(mPings.First(), KadOperation::SelectById(id))) {
				return id;
			}
		}
		//Todo: XFATAL("Could not find unused transaction id")
		return 0;
	}

public:


	KadOpMgr(const KadNodeId& id, uint16_t port)
		: mMyId (id)
	{
		mSocket.Bind(port);
		//LOG(mLog, "Bound to address " << mSocket.GetBindAddr());
		Start();
	}

	virtual ~KadOpMgr() {
		XThread::Stop();

		mSocket.SendTo("wake!", 5, mSocket.GetBindAddr());
	}


	void Ping(const XSockAddr& address, KadPingHandler h)
	{
		KadMsgId msgId = FindUnusedTransactionId(mPings);
		mPings.Append(KadPing(msgId, address, h));
		//mSocket.SendStructTo(KadMsgPing(msgId, mMyId), address); TODO
		//mPings.Remove(ping);
	}

private:
	XList<KadPing> mPings;

	XLog mLog;
	XSocketUdp mSocket;
	XTimerContext mTimers;
	KadNodeId mMyId;
};

#endif /* KADTRANSACTIONMGR_H_ */