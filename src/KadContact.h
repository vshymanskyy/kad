#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

struct KadContact
{
	enum Type {
		UNKNOWN,
		// External addresses
		GLOBAL,			// blobal IP (may be dynamic)
		GLOBAL_STATIC,	// blobal static IP
		NAT,			// behind generic NAT
		NAT_FULL_CONE,
		NAT_ADDR_RESTR,
		NAT_PORT_RESTR,
		NAT_SYMMETRIC,	// oh no

		// Internal addresses
		LOCAL,			// some local IP
		SAME_LAN		// appears to be on same LAN
	};

	KadNodeId	mId;
	XSockAddr	mAddrExt;
	XSockAddr	mAddrInt;
	Type		mType;
	unsigned	mFailQty;
	unsigned	mRTT;

	KadContact() {}

	KadContact(const KadNodeId& id, const XSockAddr& extr, const XSockAddr& intr = XSockAddr())
		: mId		(id)
		, mAddrExt	(extr)
		, mAddrInt	(intr)
		, mType		(UNKNOWN)
		, mFailQty	(0)
		, mRTT		(0)
	{
	}

	bool IsStale() const { return mFailQty >= KADEMLIA_STALE; }

	bool operator == (const KadContact& c) { return mId == c.mId; }

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

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& c) {
	return str << (c.IsStale()?"Stale contact {id: ":"Contact {id: ") << c.mId
			<< " ext: " << c.mAddrExt.ToString() << " int: " << c.mAddrInt.ToString()  << "}";
}

#endif /* KAD_CONFIG_H_ */
