#ifndef KAD_MSG_H_
#define KAD_MSG_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

typedef uint32_t KadMsgId;

class KadMsg
{
public:
	enum KadMsgType
	{
		PING,
		PONG,
		JOIN_REQ,
		JOIN_RSP,
		//LEAVE_REQ,
		//LEAVE_RSP,
		//STORE_REQ,
		//STORE_RSP,
		//REMOVE_REQ,
		//REMOVE_RSP,
		//RFIND_NODE_REQ,
		//RFIND_NODE_RSP,
		FIND_NODE_REQ,
		FIND_NODE_RSP,

		//FIND_VAL_REQ,
		//FIND_VAL_RSP
	};

public:
	KadMsg(KadMsgType type, KadMsgId msgId, const KadNodeId& nodeId)
		: mMsgType	(type)
		, mMsgId	(htonl(msgId))
		, mNodeId	(nodeId)
	{}

	//uint16_t Version() const { return ntohs(mVersion); }
	uint32_t MsgId() const { return ntohl(mMsgId); }
	KadMsgType MsgType() const { return KadMsgType(mMsgType); }
	const KadNodeId& NodeId() const { return mNodeId; }

private:
	//uint16_t mVersion;
	uint8_t mMsgType;
	KadMsgId mMsgId;
	KadNodeId mNodeId;

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgRsp : public KadMsg
{
public:
	enum KadMsgStatus
	{
		STATUS_OK,
		STATUS_FAILURE,
		STATUS_TIMEOUT,
		STATUS_NOT_FOUND,
		STATUS_EXISTS,
	};

public:
	KadMsgRsp(KadMsgType type, KadMsgId msgId, const KadNodeId& nodeId, KadMsgStatus status)
		: KadMsg	(type, msgId, nodeId)
		, mStatus	(status)
	{}

	KadMsgStatus Status() const { return KadMsgStatus(mStatus); }

private:
	uint8_t mStatus;

} GCC_SPECIFIC(__attribute__((packed)));


#endif /* KAD_MSG_H_ */
