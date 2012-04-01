#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include "KadSocket.h"

struct KadContact
{
	KadNodeId mId;
	KadAddr mExt;

#if defined (KADEMLIA_CONTACT_INTERNAL)
	KadAddr mInt;
#endif

	static KadContact Random()
	{
		KadContact c;
		c.mId = KadNodeId::Random();
		c.mExt = KadAddr::Random();
#if defined (KADEMLIA_CONTACT_INTERNAL)
		c.mInt = KadAddr::Random();
#endif
		return c;
	}

};

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& v) {
	return str << "{ id: " << v.mId << ", ext: " << v.mExt
	#if defined (KADEMLIA_CONTACT_INTERNAL)
		<< ", int: " << v.mInt
	#endif
		<< " }";
}

#endif /* KAD_CONFIG_H_ */
