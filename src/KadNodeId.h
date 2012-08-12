#ifndef NODEID_H_
#define NODEID_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "KadConfig.h"
#include "XHelpers.h"
#include "XLog.h"

#define bitsof(x) (sizeof(x)*8)

template <unsigned ID_SIZE>
class TKadId
{
	/*****************************************************************
	 * Generators
	 *****************************************************************/
public:

	enum {
		SIZE = ID_SIZE,
		BIT_SIZE = (ID_SIZE*8)
	};

	TKadId () {
		memset(mData, 0, SIZE);
	}

	explicit TKadId (const char* str) {
		memset(mData, 0, SIZE);
		const char* strBuff = str+strlen(str);

	    for (int i = SIZE-1; i>=0; i--) {
	        unsigned u;
	    	if (strBuff-str >= 2 && sscanf(strBuff-2, "%2x", &u) == 1) {
	    		mData[i] = u & 0xFF;
	    		strBuff -= 2;
	    	} else if (strBuff-str >= 2 && sscanf(strBuff-1, "%1x", &u) == 1) {
	    		mData[i] = u & 0xFF;
	    		break;
	    	} else if (strBuff-str == 1 && sscanf(strBuff-1, "%1x", &u) == 1) {
	    		mData[i] = u & 0xFF;
	    		break;
	    	} else {
	    		break;
	    	}
	    }
	}

	TKadId (uint64_t num) {
		// TODO: Byte-order agnostic
		memset(mData, 0, SIZE);
		uint8_t* numBytes = (uint8_t*)&num;
		for (unsigned i=0; i<SIZE && i<sizeof(num); i++) {
			mData[SIZE-1-i] = numBytes[i];
		}
	}

	static TKadId FromHash(const void* data, size_t len);

	static TKadId FromHash(const char* str) { return FromHash(str, strlen(str)); }

	static TKadId Random() {
		XString unique = XString::Format("%d-%d", RandRange(0, 10000), RandRange(0, 10000));
		return TKadId::FromHash((char*)unique, unique.Length());
	}

	static TKadId PowerOfTwo(int power) {
		TKadId result;
		memset(result.mData, 0, SIZE);
		result.SetBit(BIT_SIZE-1 - power);
		return result;
	}

	/*****************************************************************
	 * Operators
	 *****************************************************************/
	TKadId operator ~() const {
		TKadId result;
		for (unsigned i = 0; i < SIZE; i++) {
			result.mData[i] = ~mData[i];
		}
		return result;
	}

	TKadId operator ^(const TKadId &id) const { TKadId result(*this); result ^= id; return result; }
	TKadId operator |(const TKadId &id) const { TKadId result(*this); result |= id; return result; }
	TKadId operator &(const TKadId &id) const { TKadId result(*this); result &= id; return result; }

	TKadId operator +(const TKadId &id) const { return TKadId(*this).Add(id); }
	TKadId operator -(const TKadId &id) const { return TKadId(*this).Sub(id); }
	TKadId operator *(const TKadId &id) const { return TKadId(*this).Mul(id); }
	TKadId operator /(const TKadId &id) const { return TKadId(*this).Div(id); }
	TKadId operator <<(unsigned bits) const  { return TKadId(*this).Shl(bits); }
	TKadId operator >>(unsigned bits) const  { return TKadId(*this).Shr(bits); }

	TKadId& operator ^=(const TKadId &id) { for (unsigned i = 0; i < SIZE; i++) { mData[i] ^= id.mData[i]; } return *this; }
	TKadId& operator |=(const TKadId &id) { for (unsigned i = 0; i < SIZE; i++) { mData[i] |= id.mData[i]; } return *this; }
	TKadId& operator &=(const TKadId &id) { for (unsigned i = 0; i < SIZE; i++) { mData[i] &= id.mData[i]; } return *this; }

	TKadId& operator +=(const TKadId &id) { return Add(id); }
	TKadId& operator -=(const TKadId &id) { return Sub(id); }
	TKadId& operator *=(const TKadId &id) { return Mul(id); }
	TKadId& operator /=(const TKadId &id) { return Div(id); }
	TKadId& operator <<=(unsigned bits) { return Shl(bits); }
	TKadId& operator >>=(unsigned bits) { return Shr(bits);	}

	bool operator ==(const TKadId &id) const { return memcmp(mData, id.mData, SIZE) == 0; }
	bool operator !=(const TKadId &id) const { return memcmp(mData, id.mData, SIZE) != 0; }
	bool operator >=(const TKadId &id) const { return memcmp(mData, id.mData, SIZE) >= 0; }
	bool operator <=(const TKadId &id) const { return memcmp(mData, id.mData, SIZE) <= 0; }
	bool operator > (const TKadId &id) const { return memcmp(mData, id.mData, SIZE) >  0; }
	bool operator < (const TKadId &id) const { return memcmp(mData, id.mData, SIZE) <  0; }

	/*****************************************************************
	 * Methods
	 *****************************************************************/

	TKadId& Add(const TKadId& uValue)
	{
		if (uValue.IsZero())
			return *this;
		int16_t iSum = 0;
		for (int iIndex=SIZE-1; iIndex>=0; iIndex--)
		{
			iSum = iSum + mData[iIndex] + uValue.mData[iIndex];
			mData[iIndex] = (uint8_t)iSum;
			iSum >>= bitsof(mData[0]);
		}
		return *this;
	}

	TKadId& Sub(const TKadId& uValue)
	{
		if (uValue.IsZero())
			return *this;
		int16_t iSum = 0;
		for (int iIndex=SIZE-1; iIndex>=0; iIndex--)
		{
			iSum = iSum + mData[iIndex] - uValue.mData[iIndex];
			mData[iIndex] = (uint8_t)iSum;
			iSum >>= bitsof(mData[0]);
		}
		return *this;
	}

	TKadId& Mul(const TKadId& uValue)
	{
		TKadId ret;
		int16_t carry=0;
		uint8_t mat[2*SIZE+1][2*SIZE]={0};
		for(int i=SIZE-1;i>=0;i--){
			for(int j=SIZE-1;j>=0;j--){
				carry += mData[i]*uValue.mData[j];
				if(carry < 10){
					mat[i][j-(SIZE-1-i)]=carry;
					carry=0;
				}
				else{
					mat[i][j-(SIZE-1-i)]=carry % 0x100;
					carry=carry / 0x100;
				}
			}
		}
		for(int i=1;i<SIZE+1;i++){
			for(int j=SIZE-1;j>=0;j--){
				carry += mat[i][j]+mat[i-1][j];
				if(carry < 10){
					mat[i][j]=carry;
					carry=0;
				} else{
					mat[i][j]=carry % 0x100;
					carry=carry / 0x100;
				}
			}
		}
		for(int i=0;i<SIZE;i++)
			ret.mData[i]=mat[SIZE][i];

		*this = ret;
		return *this;
	}

	TKadId& Div(const TKadId& uValue)
	{
		X_FATAL("Not implemented");
	}

	TKadId& Shl(unsigned uBits = 1)
	{
		if ((uBits == 0) || IsZero()) {
			return *this;
		}

		if (uBits >= BIT_SIZE) {
			*this = TKadId();
			return *this;
		}

		TKadId uResult;

		uint16_t iShifted = 0;
		int iIndexShift = (int)uBits / bitsof(iShifted);

		for (int iIndex=SIZE-1; iIndex>=iIndexShift; iIndex--) {
			iShifted += ((uint16_t)mData[iIndex]) << (uBits % bitsof(iShifted));
			uResult.mData[iIndex-iIndexShift] = (uint8_t)iShifted;
			iShifted = iShifted >> bitsof(mData[0]);
		}
		*this = uResult;
		return *this;
	}

	TKadId& Shr(unsigned uBits = 1)
	{
		X_FATAL("Not implemented");
	}

	bool GetBit(unsigned bit) const {
		X_ASSERT(bit < BIT_SIZE);
		const int i = bit / 8;
		const int shift = 7 - (bit % 8);
		return ((mData[i] >> shift) & 1);
	}

	TKadId& SetBit(unsigned bit, bool val = true) {
		X_ASSERT(bit < BIT_SIZE);
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

	TKadId& SwapBit(unsigned bit)
	{
		X_ASSERT(bit < BIT_SIZE);
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

	bool IsZero() const
	{
		for (unsigned i = 0; i < SIZE; i++) {
			if (mData[i])
				return false;
		}
		return true;
	}

	// returns n in: 2^n <= distance(n1, n2) < 2^(n+1)
	// useful for finding out which bucket a node belongs to
	unsigned DistanceTo(const TKadId& id) const
	{
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

	XString ToString() const
	{
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
