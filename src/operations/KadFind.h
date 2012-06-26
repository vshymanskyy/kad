#include "KadOperation.h"
#include "KadMsg.h"

class KadMsgAddr
{
	enum Family {
		UNSPEC, IPv4, IPv6
	};
public:
	KadMsgAddr() : family(UNSPEC) {}
	KadMsgAddr(const XSockAddr& a) {
		switch (a.SA()->sa_family) {
		case AF_INET:
			family = IPv4;
			port = a.SA4()->sin_port;
			memcpy(&addr, &a.SA4()->sin_addr, sizeof(a.SA4()->sin_addr));
			break;
		case AF_INET6:
			family = IPv6;
			port = a.SA6()->sin6_port;
			memcpy(&addr, &a.SA6()->sin6_addr, sizeof(a.SA6()->sin6_addr));
			break;
		default:
			family = UNSPEC;
			break;
		}
	}
	operator XSockAddr() const {
		XSockAddr result;
		switch (family) {
		case IPv4:
			result.SA4()->sin_family = AF_INET;
			result.SA4()->sin_port = port;
			memcpy(&result.SA4()->sin_addr, &addr, sizeof(result.SA4()->sin_addr));
			break;
		case IPv6:
			result.SA6()->sin6_family = AF_INET6;
			result.SA6()->sin6_port = port;
			memcpy(&result.SA6()->sin6_addr, &addr, sizeof(result.SA6()->sin6_addr));
			break;
		default:
			result.SA()->sa_family = AF_UNSPEC;
			break;
		}
		return result;
	}
private:
	uint16_t	family;
	uint16_t	port;
	uint8_t		addr[16];
} GCC_SPECIFIC(__attribute__((packed)));

struct KadMsgContact
{
	KadNodeId	id;
	KadMsgAddr	addr;

	KadMsgContact() {}

	KadMsgContact(const KadContact& c)
		: id(c.mId)
		, addr(c.mAddr)
	{  }

	operator KadContact() const {
		return KadContact(id, XSockAddr(addr));
	}
} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgFindReq : public KadMsg
{

public:
	KadMsgFindReq(KadMsgId msgId, const KadNodeId& nodeId,const KadNodeId& findId)
		: KadMsg(KadMsg::KAD_MSG_FIND_REQ, msgId, nodeId)
		, mFindId (findId)
	{
	}

	const KadNodeId& FindId() const { return mFindId; }

private:
	KadNodeId mFindId;

} GCC_SPECIFIC(__attribute__((packed)));

class KadMsgFindRsp : public KadMsgRsp
{

public:
	KadMsgFindRsp(KadMsgId msgId, const KadNodeId& nodeId, KadMsgStatus status, const KadContact* contacts, unsigned qty)
		: KadMsgRsp(KadMsg::KAD_MSG_FIND_RSP, msgId, nodeId, status)
	{
		X_ASSERT_LE(qty, KADEMLIA_BUCKET_SIZE, "%d");
		memset(mContacts, 0, sizeof(mContacts));
		for (unsigned i=0; i<qty; i++) {
			mContacts[i]= KadMsgContact(contacts[i]);
		}
	}

//private:
	KadMsgContact	mContacts[KADEMLIA_BUCKET_SIZE];

} GCC_SPECIFIC(__attribute__((packed)));

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadMsgFindReq& v)
{
	return str << "KAD_MSG_FIND_REQ: " << v.FindId();
}

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadMsgFindRsp& v)
{
	int qty = 0;
	for (int i=0; i<KADEMLIA_BUCKET_SIZE; i++) {
		if (!v.mContacts[i].id.IsZero()) {
			qty++;
		}
	}
	return 	str << "KAD_MSG_FIND_RSP { " << qty << " }";
}
