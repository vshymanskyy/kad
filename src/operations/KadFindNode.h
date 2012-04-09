#include "KadOperation.h"
#include "KadMsg.h"


class KadMsgFindNodeReq : public KadMsg
{

public:
	KadMsgFindNodeReq(KadMsgId msgId, const KadNodeId& nodeId,const KadNodeId& findId)
		: KadMsg(KadMsg::FIND_NODE_REQ, msgId, nodeId)
	{
	}

	const KadNodeId& FindId() const { return mFindId; }

private:
	KadNodeId mFindId;

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgFindNodeRsp : public KadMsg
{

public:
	KadMsgFindNodeRsp(KadMsgId msgId, const KadNodeId& nodeId)
		: KadMsg(KadMsg::FIND_NODE_RSP, msgId, nodeId)
	{
	}


private:
	KadNodeId mFindId;

} GCC_SPECIFIC(__attribute__((packed)));


typedef XDelegate< void () > KadFindNodeHandler;

class KadFindNode : public KadOperation
{
	KadFindNode(KadMsgId id, const KadAddr& addr, KadFindNodeHandler h)
		: KadOperation (id), mHandler(h)
	{

	}

	void Process(const KadMsgFindNodeRsp& rsp) {

	}

	KadFindNodeHandler mHandler;
};
