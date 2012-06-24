#include "KadOperation.h"
#include "KadMsg.h"

class KadMsgAddr
{
public:
	KadMsgAddr() {}
	KadMsgAddr(const XSockAddr& addr) {}
	operator XSockAddr() {}
private:
	uint16_t	family;
	uint16_t	port;
	uint8_t		addr[16];
} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgContact
{
public:
	KadMsgId	id;
	KadMsgAddr	local;
	KadMsgAddr	global;
} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgFindReq : public KadMsg
{

public:
	KadMsgFindReq(KadMsgId msgId, const KadNodeId& nodeId,const KadNodeId& findId)
		: KadMsg(KadMsg::KAD_MSG_FIND_REQ, msgId, nodeId)
	{
	}

	const KadNodeId& FindId() const { return mFindId; }

private:
	KadNodeId mFindId;

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgFindRsp : public KadMsg
{

public:
	KadMsgFindRsp(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsg(KadMsg::KAD_MSG_FIND_RSP, msgId, nodeId)
	{
	}


private:
	uint16_t		mQty;
	KadMsgContact	mContacts[KADEMLIA_BUCKET_SIZE];

} GCC_SPECIFIC(__attribute__((packed)));
