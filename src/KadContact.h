#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

struct KadContact
{
	KadNodeId mId;
	XSockAddr mExt;

#if defined (KADEMLIA_CONTACT_INTERNAL)
	XSockAddr mInt;
#endif

	static KadContact Random()
	{
		KadContact c;
		c.mId = KadNodeId::Random();
		c.mExt = XSockAddr::Random();
#if defined (KADEMLIA_CONTACT_INTERNAL)
		c.mInt = XSockAddr::Random();
#endif
		return c;
	}

};

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& v) {
	return str << "{ id: " << v.mId << ", ext: " << v.mExt.ToString()
	#if defined (KADEMLIA_CONTACT_INTERNAL)
		<< ", int: " << v.mInt
	#endif
		<< " }";
}

#endif /* KAD_CONFIG_H_ */
