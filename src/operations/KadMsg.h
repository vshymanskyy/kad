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

};

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



#include <msgpack.hpp>
#include <vector>
#include <string>

template<unsigned SIZE>
struct MsgPackRaw {
	uint8_t mData[SIZE];

	void msgpack_pack(msgpack::packer<msgpack::sbuffer>& pk) const
	{
		pk.pack_raw(SIZE);
		pk.pack_raw_body((const char*)&mData, SIZE);
	}

	void msgpack_unpack(msgpack::object o)
	{
		if(o.type != msgpack::type::RAW) { throw msgpack::type_error(); }
		if(o.via.raw.size != SIZE) { return; }
		memcpy(&mData, o.via.raw.ptr, SIZE);
	}
};

template<unsigned SIZE>
struct MsgPackHeader {

	enum Type
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

	uint16_t			mMsgType;
	uint64_t			mMsgId;
	MsgPackRaw<SIZE>	mNodeId;

    MSGPACK_DEFINE(mMsgType, mMsgId, mNodeId);
};

struct MsgPackAddr
{
	enum Family {
		UNSPEC, IPv4, IPv6
	};

	uint8_t		mFamily;
	uint16_t	mPort;
	MsgPackRaw<16> mAddr;

public:
	MsgPackAddr() : mFamily(UNSPEC) {}
	MsgPackAddr(const XSockAddr& a) {
		switch (a.SA()->sa_family) {
		case AF_INET:
			mFamily = IPv4;
			mPort = a.SA4()->sin_port;
			memcpy(&mAddr, &a.SA4()->sin_addr, sizeof(a.SA4()->sin_addr));
			break;
		case AF_INET6:
			mFamily = IPv6;
			mPort = a.SA6()->sin6_port;
			memcpy(&mAddr, &a.SA6()->sin6_addr, sizeof(a.SA6()->sin6_addr));
			break;
		default:
			mFamily = UNSPEC;
			break;
		}
	}
	operator XSockAddr() const {
		XSockAddr result;
		switch (mFamily) {
		case IPv4:
			result.SA4()->sin_family = AF_INET;
			result.SA4()->sin_port = mPort;
			memcpy(&result.SA4()->sin_addr, &mAddr, sizeof(result.SA4()->sin_addr));
			break;
		case IPv6:
			result.SA6()->sin6_family = AF_INET6;
			result.SA6()->sin6_port = mPort;
			memcpy(&result.SA6()->sin6_addr, &mAddr, sizeof(result.SA6()->sin6_addr));
			break;
		default:
			result.SA()->sa_family = AF_UNSPEC;
			break;
		}
		return result;
	}

	MSGPACK_DEFINE(mFamily, mPort, mAddr);
};

template<unsigned SIZE>
struct MsgPackContact {
	MsgPackRaw<SIZE>	mId;
	MsgPackAddr		mAddr;

	MSGPACK_DEFINE(mId, mAddr);
};

template<unsigned SIZE>
struct MsgPackFindReq: public MsgPackHeader<SIZE>
{
	MsgPackRaw<SIZE> mFindId;

	MSGPACK_DEFINE((MsgPackHeader<SIZE>&)(*this), mFindId);
};

template<unsigned SIZE>
struct MsgPackFindRsp: public MsgPackHeader<SIZE>
{
	std::vector<MsgPackContact<SIZE> > mContacts;

	MSGPACK_DEFINE((MsgPackHeader<SIZE>&)(*this), mContacts);
};

#endif /* KAD_MSG_H_ */
