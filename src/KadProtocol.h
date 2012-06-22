#ifndef KAD_PROTOCOL_H_
#define KAD_PROTOCOL_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSocketUdp.h>
/*
enum KadMsgType
{
	KADMSG_PING,
	KADMSG_PONG,
	KADMSG_JOIN_REQ,
	KADMSG_JOIN_RSP,
	KADMSG_LEAVE_REQ,
	KADMSG_LEAVE_RSP,
	KADMSG_STORE_REQ,
	KADMSG_STORE_RSP,
	KADMSG_REMOVE_REQ,
	KADMSG_REMOVE_RSP,
	KADMSG_FIND_NODE_REQ,
	KADMSG_FIND_NODE_RSP,
	KADMSG_FIND_VAL_REQ,
	KADMSG_FIND_VAL_RSP
};

class KadMsgCommon
{
private:
	uint16_t mVersion;
	uint8_t mMsgType;
	uint32_t mCookie;
	KadNodeId mMyId;

public:
	KadMsgCommon(KadMsgType type, uint32_t cookie, const KadNodeId& id)
		: mVersion	(htons(0xCAD1))
		, mMsgType	(type)
		, mCookie	(htonl(cookie))
		, mMyId		(id)
	{

	}

	uint16_t Version() const { return ntohs(mVersion); }
	uint32_t Cookie() const { return ntohl(mCookie); }
	KadMsgType Type() const { return KadMsgType(mMsgType); }

} GCC_SPECIFIC(__attribute__((packed)));


class KadMsgPing : public KadMsgCommon
{
private:

public:
	KadMsgPing(uint32_t cookie, const KadNodeId& id)
		: KadMsgCommon(KADMSG_PING, cookie, id)
	{
	}

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgPong : public KadMsgCommon
{

public:
	KadMsgPong(uint32_t cookie, const KadNodeId& id)
		: KadMsgCommon(KADMSG_PONG, cookie, id)
	{
	}

} GCC_SPECIFIC(__attribute__((packed)));
*/
#endif /* KAD_PROTOCOL_H_ */
