#include "KadOperation.h"
#include "KadMsg.h"

class KadMsgPing : public KadMsg
{
public:
	KadMsgPing(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsg(KadMsg::PING, msgId, nodeId)
	{}

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgPong : public KadMsgRsp
{

public:
	KadMsgPong(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsgRsp(KadMsg::PONG, msgId, nodeId, KadMsgRsp::STATUS_OK)
	{
	}

} GCC_SPECIFIC(__attribute__((packed)));

typedef XDelegate< void () > KadPingHandler;

class KadPing : public KadOperation {
public:
	KadPing(KadMsgId id, const KadAddr& addr, KadPingHandler h)
		: KadOperation (id), mAddr(addr), mHandler(h)
	{

	}

	void Process(const KadMsgPong& rsp) {
		if (mHandler) {
			mHandler();
		}
	}
private:
	KadAddr mAddr;
	KadPingHandler mHandler;
};
