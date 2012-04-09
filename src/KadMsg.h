#ifndef KAD_PROTOCOL_H_
#define KAD_PROTOCOL_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include "KadSocket.h"

typedef uint32_t KadMsgId;

class KadMsg
{
public:
	enum Type
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
	KadMsg(Type type, KadMsgId msgId, const KadNodeId& nodeId)
		: mMsgType	(type)
		, mMsgId	(htonl(msgId))
		, mNodeId	(nodeId)
	{}

	//uint16_t Version() const { return ntohs(mVersion); }
	uint32_t MsgId() const { return ntohl(mMsgId); }
	Type MsgType() const { return Type(mMsgType); }
	const KadNodeId& NodeId() const { return mNodeId; }

private:
	//uint16_t mVersion;
	uint8_t mMsgType;
	KadMsgId mMsgId;
	KadNodeId mNodeId;

} GCC_SPECIFIC(__attribute__((packed)));


#endif /* KAD_PROTOCOL_H_ */
