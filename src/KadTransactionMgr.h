#ifndef KADTRANSACTIONMGR_H_
#define KADTRANSACTIONMGR_H_

#include <XList.h>
#include <XLog.h>
#include <XTime.h>
#include <XThread.h>
#include <XDebug.h>
#include <XDelegate.h>
#include <XTimer.h>
#include <XLocks.h>
#include <net/XSocketUdp.h>

#include <KadConfig.h>
#include <KadProtocol.h>

class KadTransactionMgr : public XThread
{
private:
	virtual int Run()
	{
		XSockAddr from;

		LOG(mLog, "Bound to address " << mSocket.GetBindAddr().ToString())

		char buffer[KADEMLIA_MAX_MSG_SIZE];
		while (!IsStopping()) {
			ssize_t len = mSocket.RecvFrom(&buffer, sizeof(buffer), &from);
			if (len <= 0) {
				continue;
			}
			buffer[len] = '\0';

			LOG(mLog, FMT("Recv (%ld): ", len) << buffer);

			/*std::istringstream strm(buffer);
			json::Object req;
			json::Reader::Read(req, strm);
			json::String& type = req["type"];
			if(type.Value() == "ping") {
				json::Object rsp;
				rsp["id"] = json::String("");
				rsp["tid"] = req["tid"];
				rsp["type"] = json::String("pong");
				SendJsonTo(rsp, from);
			} else if(type.Value() == "pong") {
				json::Number& tid = req["tid"];
				XList<PingTransaction*>::It t = mPings.FindAfter(mPings.First(), Transaction::SelectById(tid.Value()));
				if (t != mPings.End()) {
					mPings[t]->Unlock();
				} else {
					LOG(mLog, "Ping transaction not found");
				}
				break;
			}*/

		}
		return 0;
	}

	struct Transaction {
		Transaction(KadTransactionMgr* mgr, uint32_t id) : mMgr(mgr), mId (id) {}

		KadTransactionMgr* mMgr;
		uint32_t mId;
		XMutex mWaitMx;

		class SelectById {
		public:
			SelectById(uint32_t id) : mId (id) {};
			bool operator()(const Transaction& t) { return mId == t.mId; }
			bool operator()(const Transaction* t) { return mId == t->mId; }
		private:
			uint32_t mId;
		};
	};

	struct PingTransaction : public Transaction {
		PingTransaction(KadTransactionMgr* mgr, uint32_t id, const XSockAddr& addr)
			: Transaction (mgr, id), mAddr(addr)
		{
			XPlatGetTime(&mSent, NULL);
		}
		XSockAddr mAddr;
		XPlatDateTime mSent;

		void Unlock() {
			mWaitMx.Unlock();
		}

		void Execute() {
			mWaitMx.TryLock();

			/*json::Object obj;
			obj["id"] = json::String("");
			obj["tid"] = json::Number(mId);
			obj["type"] = json::String("ping");
			mMgr->SendJsonTo(obj, mAddr);*/

			if (mWaitMx.Lock(5)) {
				LOG(NULL, "Pong!!!");
			} else {
				LOG(NULL, "Timeout!!!");
			}

			//mWaitMx.Unlock();
		}
	};



private:
	template <typename T>
	uint32_t FindUnusedTransactionId(const T& list) const {
		uint32_t id;
		for (int gen = 0; gen < 100; gen++) {
			MemRand(&id, sizeof(id));
			for (typename T::It it = list.First(); it != list.End(); ++it) {
				if (id == list[it]->mId) {
					LOG_MORE(mLog, "Transaction id conflict");
					id = 0;
				}
			}
			if (id) {
				return id;
			}
		}
		//Todo: XFATAL("Could not find unused transaction id")
		return 0;
	}

public:
	KadTransactionMgr(const KadNodeId& id, uint16_t port)
		: mMyId (id)
	{
		//mSocket.Bind(port);
		Start();
	}

	virtual ~KadTransactionMgr() {

	}

	//bool SendJsonTo(const json::Object& obj, const KadAddr& addr) {
		//std::ostringstream strm;
		//json::Writer::Write(obj, strm);
		//return mSocket.SendTo(strm.str().c_str(), strm.str().length(), addr) == sizeof(obj);
	//}


	void Ping(const XSockAddr& address)
	{
		PingTransaction* t = new PingTransaction(this, FindUnusedTransactionId(mPings), address);
		XList<PingTransaction*>::It ping = mPings.Append(t);
		t->Execute();
		mPings.Remove(ping);
	}

private:
	XList<PingTransaction*> mPings;

	XLog mLog;
	XSocketUdp mSocket;
	XTimerContext mTimers;
	KadNodeId mMyId;
};

#endif /* KADTRANSACTIONMGR_H_ */
