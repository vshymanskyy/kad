#ifndef KADTRANSACTIONMGR_H_
#define KADTRANSACTIONMGR_H_

#include <XList.h>
#include <XLog.h>
#include <XTime.h>
#include <XThread.h>
#include <XDebug.h>
#include <XDelegate.h>
#include <XTimer.h>

#include <KadConfig.h>
#include <KadSocket.h>
#include <KadProtocol.h>

class KadTransactionMgr : public XThread
{
private:
	virtual int Run()
	{
		KadAddr from;

		mSocket.Bind(0);
		mSocket.GetBindAddr(&from);

		LOG(mLog, "Bound to address " << from)

		uint8_t buffer[KADEMLIA_MAX_MSG_SIZE];
		while (!IsStopping()) {
			ssize_t len = mSocket.RecvFrom(&buffer, sizeof(buffer), &from);
			KadMsgCommon* msgCommon = (KadMsgCommon*)(&buffer);

			if (msgCommon->Version() == 0xCAD1) {
				switch (msgCommon->Type()) {
				case KADMSG_PING:
					break;
				case KADMSG_PONG:
					if (const PingTransaction* t = FindTransaction(mPings, msgCommon->Cookie())) {

					}
					break;
				default:
					LOG_WARN(mLog, "Message not handled (type = " << msgCommon->Type() << ")");
					break;
				}
			} else {
				LOG_WARN(mLog, "Version mismatch");
			}

		}
		return 0;
	}

	struct Transaction {
		Transaction(uint32_t id) : mId (id) {}
		uint32_t mId;
	};

	struct PingTransaction : public Transaction {
		PingTransaction(uint32_t id, const KadAddr& addr)
			: Transaction (id), mAddr(addr)
		{
			XPlatGetTime(&mSent, NULL);
		}
		KadAddr mAddr;
		XPlatDateTime mSent;
	};

	struct JoinTransaction : public Transaction {
		JoinTransaction(uint32_t id) : Transaction (id) {}
	};

	struct LeaveTransaction : public Transaction {
		LeaveTransaction(uint32_t id) : Transaction (id) {}
	};

	struct StoreTransaction : public Transaction {
		StoreTransaction(uint32_t id) : Transaction (id) {}
	};

	struct FindNodeTransaction : public Transaction {
		FindNodeTransaction(uint32_t id) : Transaction (id) {}
	};

	struct FindDataTransaction : public Transaction {
		FindDataTransaction(uint32_t id) : Transaction (id) {}
	};

private:
	template <typename T>
	uint32_t FindUnusedTransactionId(const T& list) const {
		uint32_t id;
		for (int gen = 0; gen < 100; gen++) {
			MemRand(&id, sizeof(id));
			for (typename T::It it = list.First(); it != list.End(); ++it) {
				if (id == list[it].mId) {
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

	template <typename T>
	const T* FindTransaction(const XList<T>& list, uint32_t id) const {
		for (typename XList<T>::It it = list.First(); it != list.End(); ++it) {
			if (id == list[it].mId) {
				return &(list[it]);
			}
		}
		LOG_WARN(mLog, "Transaction not found");
		return NULL;
	}

public:
	KadTransactionMgr() {

	}

	~KadTransactionMgr() {

	}

	void Ping(const KadNodeId& myId, const KadAddr& address)
	{
		uint32_t cookie = FindUnusedTransactionId(mPings);
		mPings.Append(PingTransaction(cookie, address));
		mSocket.SendStructTo(KadMsgPing(cookie, myId), address);
	}

private:
	XList<PingTransaction> mPings;
	XList<JoinTransaction> mJoins;
	XList<LeaveTransaction> mLeaves;
	XList<StoreTransaction> mStores;

	XLog mLog;
	KadSocket mSocket;
	XTimerContext mTimers;
};

#endif /* KADTRANSACTIONMGR_H_ */
