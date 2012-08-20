#ifndef KAD_MSG_H_
#define KAD_MSG_H_

#include "KadConfig.h"
#include "KadNodeId.h"

typedef KadId KadMsgId;

class KadMsg
{
public:
	enum KadMsgType
	{
		KAD_MSG_PING,
		KAD_MSG_PONG,
		KAD_MSG_JOIN_REQ,
		KAD_MSG_JOIN_RSP,
		KAD_MSG_FIND_REQ,
		KAD_MSG_FIND_RSP,
		KAD_MSG_STORE_REQ,
		KAD_MSG_STORE_RSP,
		KAD_MSG_REMOVE_REQ,
		KAD_MSG_REMOVE_RSP,
		KAD_MSG_LEAVE_REQ,
		KAD_MSG_LEAVE_RSP,

		KAD_MSG_RFIND_REQ,
		KAD_MSG_RFIND_RSP,

		KAD_MSG_QTY
	};

public:
	KadMsg(KadMsgType type)
		: mVersion	(1)
		, mMsgType	(type)
	{}

	//suint16_t Version() const { return ntohs(mVersion); }
	const KadMsgId& MsgId() const { return mMsgId; }
	KadMsgType MsgType() const { return KadMsgType(mMsgType); }
	const KadId& NodeId() const { return mNodeId; }

	void SetMsgId(const KadMsgId& id) { mMsgId = id; }
	void SetNodeId(const KadId& id) { mNodeId = id; }

private:
	uint8_t mVersion;
	uint8_t mMsgType;
	KadMsgId mMsgId;
	KadId mNodeId;

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgRsp : public KadMsg
{
public:
	enum KadMsgStatus
	{
		KAD_MSG_STATUS_OK,
		KAD_MSG_STATUS_FAILURE,
		KAD_MSG_STATUS_TIMEOUT,
		KAD_MSG_STATUS_NOT_FOUND,
		KAD_MSG_STATUS_EXISTS,
	};

public:
	KadMsgRsp(KadMsgType type, KadMsgStatus status)
		: KadMsg	(type)
		, mStatus	(status)
	{}

	KadMsgStatus Status() const { return KadMsgStatus(mStatus); }

private:
	uint8_t mStatus;

} GCC_SPECIFIC(__attribute__((packed)));


#endif /* KAD_MSG_H_ */
