#ifndef KAD_MSG_H_
#define KAD_MSG_H_

#include "KadConfig.h"
#include "KadContact.h"
#include "KadNodeId.h"

#include <msgpack.hpp>
#include <vector>
#include <string>

template<unsigned SIZE>
struct MsgPackRaw
{
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

struct KadMsgAddr
{
	enum Family {
		UNSPEC, IPv4, IPv6
	};

	KadMsgAddr() : mFamily(UNSPEC) {}
	KadMsgAddr(const XSockAddr& a) {
		switch (a.SA()->sa_family) {
		case AF_INET:
			mFamily = IPv4;
			mPort = a.SA4()->sin_port;
			//MemXorKey(&mPort, sizeof(mPort), "crypted");
			memcpy(&mAddr, &a.SA4()->sin_addr, sizeof(a.SA4()->sin_addr));
			//MemXorKey(&mAddr, sizeof(a.SA4()->sin_addr), "crypted");
			break;
		case AF_INET6:
			mFamily = IPv6;
			mPort = a.SA6()->sin6_port;
			//MemXorKey(&mPort, sizeof(mPort), "hidden");
			memcpy(&mAddr, &a.SA6()->sin6_addr, sizeof(a.SA6()->sin6_addr));
			//MemXorKey(&mAddr, sizeof(a.SA6()->sin6_addr), "hidden");
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
			//MemXorKey(&result.SA4()->sin_port, sizeof(result.SA4()->sin_port), "crypted");
			memcpy(&result.SA4()->sin_addr, &mAddr, sizeof(result.SA4()->sin_addr));
			//MemXorKey(&result.SA4()->sin_addr, sizeof(result.SA4()->sin_addr), "crypted");
			break;
		case IPv6:
			result.SA6()->sin6_family = AF_INET6;
			result.SA6()->sin6_port = mPort;
			//MemXorKey(&result.SA6()->sin6_port, sizeof(result.SA6()->sin6_port), "hidden");
			memcpy(&result.SA6()->sin6_addr, &mAddr, sizeof(result.SA6()->sin6_addr));
			//MemXorKey(&result.SA6()->sin6_addr, sizeof(result.SA6()->sin6_addr), "hidden");
			break;
		default:
			result.SA()->sa_family = AF_UNSPEC;
			break;
		}
		return result;
	}

	uint8_t			mFamily;
	uint16_t		mPort;
	MsgPackRaw<16>	mAddr;

	MSGPACK_DEFINE(mFamily, mPort, mAddr);
};

struct KadMsgContact
{
	KadMsgContact() {}
	KadMsgContact(const KadContact& a) : mId (a.mId), mAddr (a.mAddrExt) { }
	operator KadContact() const { return KadContact(mId, mAddr); }

	KadId	mId;
	KadMsgAddr	mAddr;

	MSGPACK_DEFINE(mId, mAddr);
};

struct KadMsg
{
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

	KadMsg() {}
	KadMsg(Type t) : mMsgType (t) {}

	uint16_t	mMsgType;
	uint64_t	mMsgId;
	KadId	mSrcId;

    MSGPACK_DEFINE(mMsgType, mMsgId, mSrcId);
};

struct KadMsgPing: public KadMsg
{
	KadMsgPing() : KadMsg(KadMsg::KAD_MSG_PING) {}

	MSGPACK_DEFINE((KadMsg&)(*this));
};

struct KadMsgPong: public KadMsg
{
	KadMsgPong() : KadMsg(KadMsg::KAD_MSG_PONG) {}

	MSGPACK_DEFINE((KadMsg&)(*this));
};

struct KadMsgFindReq: public KadMsg
{
	KadMsgFindReq() {}
	KadMsgFindReq(const KadId& id) : KadMsg(KadMsg::KAD_MSG_FIND_REQ), mTargetId(id) {}

	KadId mTargetId;

	MSGPACK_DEFINE((KadMsg&)(*this), mTargetId);
};

struct KadMsgFindRsp: public KadMsg
{
	KadMsgFindRsp() {}
	KadMsgFindRsp(const KadContactList& lst) : KadMsg(KadMsg::KAD_MSG_FIND_RSP) {
		for (KadContactList::It it = lst.First(); it != lst.End(); ++it) {
			mContacts.push_back(*lst[it]);
		}
	}

	std::vector<KadMsgContact> mContacts;

	MSGPACK_DEFINE((KadMsg&)(*this), mContacts);
};

#endif /* KAD_MSG_H_ */
