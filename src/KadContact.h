#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <net/XSockAddr.h>

template <unsigned ID_SIZE, typename ADDR>
struct TKadContact
{
	TKadId<ID_SIZE> 	mId;
	ADDR		mAddr;
	unsigned	mFailQty;
	unsigned	mRTT;

	TKadContact() {}

	TKadContact(const TKadId<ID_SIZE>& id, const ADDR& extr)
		: mId		(id)
		, mAddr		(extr)
		, mFailQty	(0)
		, mRTT		(0)
	{
	}

	bool IsStale() const { return mFailQty >= KADEMLIA_STALE; }
};

typedef TKadContact<KADEMLIA_ID_SIZE, XSockAddr> KadContact;

template <unsigned ID_SIZE, typename ADDR>
inline
const XLog::Stream& operator <<(const XLog::Stream& str, const TKadContact<ID_SIZE, ADDR>& c) {
	return str << (c.IsStale()?"Stale contact {id: ":"Contact {id: ") << c.mId
			<< " ext: " << c.mAddr.ToString()  << "}";
}

#endif /* KAD_CONFIG_H_ */
