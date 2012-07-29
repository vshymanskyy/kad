#ifndef NODEID_H_
#define NODEID_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "KadConfig.h"
#include "XHelpers.h"
#include "XLog.h"

template <unsigned SIZE>
class TKadId
{
	/*****************************************************************
	 * Generators
	 *****************************************************************/
public:

	enum { ID_SIZE = SIZE };

	static TKadId FromHex(const char* hexStr) {
		TKadId result;
		memset(result.mData, 0, SIZE);
		const char* strBuff = hexStr+strlen(hexStr);

	    for (int i = SIZE-1; i>=0; i--) {
	        unsigned u;
	    	if (strBuff-hexStr >= 2 && sscanf(strBuff-2, "%2x", &u) == 1) {
	    		result.mData[i] = u & 0xFF;
	    		strBuff -= 2;
	    	} else if (strBuff-hexStr >= 2 && sscanf(strBuff-1, "%1x", &u) == 1) {
	    		result.mData[i] = u & 0xFF;
	    		break;
	    	} else if (strBuff-hexStr == 1 && sscanf(strBuff-1, "%1x", &u) == 1) {
	    		result.mData[i] = u & 0xFF;
	    		break;
	    	} else {
	    		break;
	    	}
	    }

		return result;
	}

	static TKadId FromHash(const void* data, size_t len);

	static TKadId FromHash(const char* str) { return FromHash(str, strlen(str)); }

	static TKadId Random() {
		XString unique = XString::Format("%d-%d", RandRange(0, 10000), RandRange(0, 10000));
		return TKadId::FromHash((char*)unique, unique.Length());
	}

	static TKadId Zero() {
		TKadId result;
		memset(result.mData, 0, SIZE);
		return result;
	}

	static TKadId PowerOfTwo(int power) {
		TKadId result;
		memset(result.mData, 0, SIZE);
		result.SetBit(SIZE*8 - 1 - power);
		return result;
	}

	/*****************************************************************
	 * Operators
	 *****************************************************************/
public:
	bool operator ==(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) == 0;
	}
	bool operator !=(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) != 0;
	}
	bool operator >=(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) >= 0;
	}
	bool operator <=(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) <= 0;
	}
	bool operator >(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) > 0;
	}
	bool operator <(const TKadId &id) const
	{
		return memcmp(mData, id.mData, SIZE) < 0;
	}

	TKadId& operator ^=(const TKadId &id) {
		for (unsigned i = 0; i < SIZE; i++) {
			mData[i] ^= id.mData[i];
		}
		return *this;
	}

	TKadId& operator |=(const TKadId &id) {
		for (unsigned i = 0; i < SIZE; i++) {
			mData[i] |= id.mData[i];
		}
		return *this;
	}

	TKadId& operator &=(const TKadId &id) {
		for (unsigned i = 0; i < SIZE; i++) {
			mData[i] &= id.mData[i];
		}
		return *this;
	}

	TKadId operator ^(const TKadId &id) const {
		TKadId result;
		for (unsigned i = 0; i < SIZE; i++) {
			result.mData[i] = mData[i] ^ id.mData[i];
		}
		return result;
	}

	TKadId operator |(const TKadId &id) const {
		TKadId result;
		for (unsigned i = 0; i < SIZE; i++) {
			result.mData[i] = mData[i] | id.mData[i];
		}
		return result;
	}

	TKadId operator &(const TKadId &id) const {
		TKadId result;
		for (unsigned i = 0; i < SIZE; i++) {
			result.mData[i] = mData[i] & id.mData[i];
		}
		return result;
	}

	TKadId operator ~() const {
		TKadId result;
		for (unsigned i = 0; i < SIZE; i++) {
			result.mData[i] = ~mData[i];
		}
		return result;
	}

	/*****************************************************************
	 * Methods
	 *****************************************************************/
	TKadId& ShiftLeft() {
		uint8_t* byte = mData;
		for (int size = SIZE; size--; ++byte) {
			unsigned char bit = 0;
			if (size) {
				bit = byte[1] & (1 << (8 - 1)) ? 1 : 0;
			}
			byte[0] <<= 1;
			byte[0] |= bit;
		}
		return *this;
	}

	TKadId& ShiftLeft(unsigned uBits)
	{
		// TODO: Optimize
		for (int i=0; i<uBits; ++i) {
			ShiftLeft();
		}
		return *this;
	}


	bool GetBit(unsigned bit) const {
		X_ASSERT(bit < SIZE*8);
		const int i = bit / 8;
		const int shift = 7 - (bit % 8);
		return ((mData[i] >> shift) & 1);
	}

	TKadId& SetBit(unsigned bit, bool val = true) {
		X_ASSERT(bit < SIZE*8);
		const int i = bit / 8;
		const int shift = 7 - (bit % 8);
		mData[i] |= (1 << shift);
		if (!val) {
			mData[i] ^= (1 << shift);
		}
		return *this;
	}

	TKadId& SetBits(unsigned bit, unsigned qty, bool val = true) {
		for (int i=bit; i<bit+qty; i++) {
			SetBit(i, val);
		}
		return *this;
	}

	TKadId& SwapBit(unsigned bit) {
		X_ASSERT(bit < SIZE*8);
		int i = bit / 8;
		int shift = 7 - (bit % 8);
		if ((mData[i] >> shift) & 1) {
			mData[i] |= (1 << shift);
			mData[i] ^= (1 << shift);
		} else {
			mData[i] |= (1 << shift);
		}
		return *this;
	}

	bool IsZero() const {
		for (unsigned i = 0; i < SIZE; i++) {
			if (mData[i])
				return false;
		}
		return true;
	}

	// returns n in: 2^n <= distance(n1, n2) < 2^(n+1)
	// useful for finding out which bucket a node belongs to
	unsigned DistanceTo(const TKadId& id) const {
		for (unsigned i = 0; i < SIZE; ++i) {
			if (const uint8_t t = mData[i] ^ id.mData[i]) {
				// Found non-matching byte, find exact bit
				const unsigned bit = (SIZE - 1 - i) * 8;
				for (int b = 7; b >= 0; --b) {
					if (t >= (1 << b)) {
						return bit + b + 1;
					}
				}
				return bit;
			}
		}
		return 0;
	}

	// my.Closer(toA than toB)
	bool Closer(const TKadId& to, const TKadId& than) const
	{
		return ((*this) ^ to) < ((*this) ^ than);
	}

	const uint8_t* Data() const { return mData; }
	uint8_t* Data() { return mData; }

	XString ToString() const {
		XString result;
#ifdef KADEMLIA_DBG_BIN_ID
		for (unsigned i = 0; i < SIZE*8; i++) {
			result += GetBit(i)?"1":"0";
		}
#else
		for (unsigned i = 0; i < SIZE; i++) {
			result += XString::Format("%02x", mData[i]);
		}
#endif
		return result;
	}

	/*****************************************************************
	 * Private data
	 *****************************************************************/
private:
	uint8_t mData[SIZE];
};

template <unsigned SIZE>
inline
const XLog::Stream& operator <<(const XLog::Stream& str, const TKadId<SIZE>& v)
{

#ifdef KADEMLIA_DBG_BIN_ID
	for (unsigned i = 0; i < SIZE*8; i++) {
		str << (v.GetBit(i)?'1':'0');
	}
#else
	for (unsigned i = 0; i < SIZE; i++) {
		str("%02x", v.Data()[i]);
	}
#endif
	return str;
}

typedef TKadId<KADEMLIA_ID_SIZE> KadId;

#endif /* NODEID_H_ */
