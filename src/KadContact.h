#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

struct KadAddrInfo {
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

	XSockAddr mAddr;
	Type mType;
	unsigned mRTT;

	KadAddrInfo(const XSockAddr& addr, Type type = UNKNOWN, unsigned rtt = 0) : mAddr(addr), mType(type), mRTT(rtt) {}
};

struct KadContact
{
	KadNodeId mId;
	XList<KadAddrInfo> mAddresses;

	enum State {
		ACTIVE,
		STALE
	} mState;

	static KadContact Random()
	{
		KadContact c;
		c.mId = KadNodeId::Random();
		for (int i=RandRange(1,3); i>0; i--) {
			c.mAddresses.Append(KadAddrInfo(XSockAddr::Random()));
		}
		c.mState = ACTIVE;
		return c;
	}
};

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadAddrInfo& c) {
	return str << "{type:" << c.mType << ", addr:" << c.mAddr << ", rtt:" << c.mRTT << "}";
}

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& c) {
	if (c.mState == KadContact::ACTIVE) {
		str << "Contact {id: ";
	} else {
		str << "Stale contact {id: ";
	}
	str << c.mId << " addresses: ";
	for (XList<KadAddrInfo>::It it = c.mAddresses.First(); it != c.mAddresses.End(); ++it) {
		str << c.mAddresses[it] << " ";
	}

	return str << "}";
}

#endif /* KAD_CONFIG_H_ */
