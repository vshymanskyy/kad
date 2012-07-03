#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

template <unsigned ID, typename ADDR>
struct TKadContact
{
	KadId<ID>	mId;
	ADDR		mAddr;
	unsigned	mFailQty;
	unsigned	mRTT;

	TKadContact() {}

	TKadContact(const KadId<ID>& id, const ADDR& extr)
		: mId		(id)
		, mAddr		(extr)
		, mFailQty	(0)
		, mRTT		(0)
	{
	}

	bool IsStale() const { return mFailQty >= KADEMLIA_STALE; }

	bool operator == (const TKadContact& c) { return mId == c.mId; }

/*	static KadContact Random()
	{
		KadContact c;
		c.mId = KadNodeId::Random();
		for (int i=RandRange(1,3); i>0; i--) {
			c.mAddrInt = (KadAddrInfo(XSockAddr::Random()));
		}
		c.mState = ACTIVE;
		return c;
	}*/
};

struct Address {
	enum Type {
		UNKNOWN,
		// External addresses
		GLOBAL,			// global IP (may be dynamic)
		GLOBAL_STATIC,	// global static IP
		NAT,			// behind generic NAT
		NAT_FULL_CONE,
		NAT_ADDR_RESTR,
		NAT_PORT_RESTR,
		NAT_SYMMETRIC,	// oh no

		// Internal addresses
		LOCAL,			// some local IP
		SAME_LAN		// appears to be on same LAN
	};
	Type mType;
	XSockAddr mInt;
	XSockAddr mExt;
};

typedef TKadContact<KADEMLIA_ID_SIZE, XSockAddr> KadContact;

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& c) {
	return str << (c.IsStale()?"Stale contact {id: ":"Contact {id: ") << c.mId
			<< " ext: " << c.mAddr.ToString()  << "}";
}

#endif /* KAD_CONFIG_H_ */
