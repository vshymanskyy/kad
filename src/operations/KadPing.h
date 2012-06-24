#include "KadOperation.h"
#include "KadMsg.h"

class KadMsgPing : public KadMsg
{
public:
	KadMsgPing(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsg(KadMsg::KAD_MSG_PING, msgId, nodeId)
	{}

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgPong : public KadMsgRsp
{

public:
	KadMsgPong(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsgRsp(KadMsg::KAD_MSG_PONG, msgId, nodeId, KadMsgRsp::KAD_MSG_STATUS_OK)
	{
	}

} GCC_SPECIFIC(__attribute__((packed)));

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadMsgPing& v)
{
	return str << "KAD_MSG_PING";
}

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadMsgPong& v)
{
	return str << "KAD_MSG_PONG";
}
