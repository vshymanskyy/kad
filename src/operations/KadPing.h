#include "KadMsg.h"

class KadMsgPing : public KadMsg
{
public:
	KadMsgPing()
		: KadMsg(KadMsg::KAD_MSG_PING)
	{}

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgPong : public KadMsgRsp
{

public:
	KadMsgPong()
		: KadMsgRsp(KadMsg::KAD_MSG_PONG, KadMsgRsp::KAD_MSG_STATUS_OK)
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
