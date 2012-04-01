#ifndef NODEID_H_
#define NODEID_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "KadConfig.h"
#include "XHelpers.h"
#include "XLog.h"

class KadNodeId
{

	/*****************************************************************
	 * Generators
	 *****************************************************************/
public:
	static KadNodeId FromHash(const void* data, size_t len);

	static KadNodeId Random();

	static KadNodeId Zero();

	static KadNodeId PowerOfTwo(int power);

	/*****************************************************************
	 * Operators
	 *****************************************************************/
public:
	bool operator ==(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) == 0;
	}
	bool operator !=(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) != 0;
	}
	bool operator >=(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) >= 0;
	}
	bool operator <=(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) <= 0;
	}
	bool operator >(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) > 0;
	}
	bool operator <(const KadNodeId &id) const
	{
		return memcmp(mData, id.mData, sizeof(mData)) < 0;
	}

	KadNodeId& operator ^=(const KadNodeId &id);

	KadNodeId& operator |=(const KadNodeId &id);

	KadNodeId& operator &=(const KadNodeId &id);

	KadNodeId operator ^(const KadNodeId &id) const;

	KadNodeId operator |(const KadNodeId &id) const;

	KadNodeId operator &(const KadNodeId &id) const;

	KadNodeId operator ~() const;

	/*****************************************************************
	 * Methods
	 *****************************************************************/
	void ShiftLeft();

	bool GetBit(unsigned bit) const;

	KadNodeId& SetBit(unsigned bit, bool val = true);

	KadNodeId& SwapBit(unsigned bit);

	bool IsZero();

	// returns n in: 2^n <= distance(n1, n2) < 2^(n+1)
	// useful for finding out which bucket a node belongs to
	unsigned DistanceTo(const KadNodeId& id) const;

	// my.Closer(toA than tB)
	bool Closer(const KadNodeId& to, const KadNodeId& than) const
	{
		return ((*this) ^ to) < ((*this) ^ than);
	}

	/*****************************************************************
	 * Private data
	 *****************************************************************/
//private:
	uint8_t mData[KADEMLIA_ID_SIZE];
};

typedef KadNodeId KadKey;
typedef KadNodeId KadDistance;

inline
const XLog::Stream& operator <<(const XLog::Stream& str, const KadNodeId& v)
{

#ifdef KADEMLIA_DBG_BIN_ID
	for (unsigned i = 0; i < KADEMLIA_ID_BITS; i++) {
		str << (v.GetBit(i)?'1':'0');
	}
#else
	for (unsigned i = 0; i < sizeof(v.mData); i++) {
		str("%02x", v.mData[i]);
	}
#endif
	return str;
}
#endif /* NODEID_H_ */
