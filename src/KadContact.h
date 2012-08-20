#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include <XList.h>

#include <net/XSockAddr.h>
#include <tr1/memory>

///template <unsigned ID_SIZE>
struct KadContact
{
	KadId		mId;
	XSockAddr	mAddrExt;
	//XSockAddr	mAddrInt;
	//XSockAddr	mAddrSrv;

	unsigned	mFailQty;
	unsigned	mRTT;
	//time		mFirstSeen;
	//time		mLastSeen;
	//float		mRPM;

	KadContact(const KadId& id, const XSockAddr& extr)
		: mId		(id)
		, mAddrExt	(extr)
		, mFailQty	(0)
		, mRTT		(0)
	{
	}

	KadContact(const KadContact& c)
		: mId		(c.mId)
		, mAddrExt	(c.mAddrExt)
		, mFailQty	(c.mFailQty)
		, mRTT		(c.mRTT)
	{
	}

	bool IsStale() const { return mFailQty >= KADEMLIA_STALE; }
};

typedef std::tr1::shared_ptr<KadContact>	KadContactPtr;
typedef XList<KadContactPtr>				KadContactList;

//template <unsigned ID_SIZE>
inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadContact& c) {
	return str << (c.IsStale()?"Stale contact {id: ":"Contact {id: ") << c.mId
			<< " ext: " << c.mAddrExt.ToString()  << "}";
}

#endif /* KAD_CONFIG_H_ */
