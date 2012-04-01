#include "KadNodeId.h"

#if KADEMLIA_ID_BITS == 256
#include "SHA256.h"
#elif KADEMLIA_ID_BITS == 160
#include "SHA1.h"
#elif KADEMLIA_ID_BITS == 128
#include "MD5.h"
#endif

KadNodeId KadNodeId::FromHash(const void* data, size_t len)
{
	KadNodeId result;

#if KADEMLIA_ID_BITS == 256
	SHA256 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
#elif KADEMLIA_ID_BITS == 160
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
#endif
	return result;
}

KadNodeId KadNodeId::Random()
{
	KadNodeId result;
	MemRand(result.mData, KADEMLIA_ID_SIZE);
	return result;
}

KadNodeId KadNodeId::Zero()
{
	KadNodeId result;
	memset(result.mData, 0, KADEMLIA_ID_SIZE);
	return result;
}

KadNodeId KadNodeId::PowerOfTwo(int power)
{
	KadNodeId result;
	memset(result.mData, 0, KADEMLIA_ID_SIZE);
	result.SetBit(KADEMLIA_ID_BITS - 1 - power);
	return result;
}

bool KadNodeId::IsZero()
{
	for (unsigned i = 0; i < sizeof(mData); i++) {
		if (mData[i])
			return false;
	}
	return true;
}

bool KadNodeId::GetBit(unsigned bit) const
{
	assert(bit < sizeof(mData)*8);
	const int i = bit / 8;
	const int shift = 7 - (bit % 8);
	return ((mData[i] >> shift) & 1);
}

KadNodeId& KadNodeId::SetBit(unsigned bit, bool val)
{
	assert(bit < sizeof(mData)*8);
	const int i = bit / 8;
	const int shift = 7 - (bit % 8);
	mData[i] |= (1 << shift);
	if (!val) {
		mData[i] ^= (1 << shift);
	}
	return *this;
}

KadNodeId& KadNodeId::SwapBit(unsigned bit)
{
	assert(bit < sizeof(mData)*8);
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

void KadNodeId::ShiftLeft()
{
	uint8_t* byte = mData;
	for (int size = sizeof(mData); size--; ++byte) {
		unsigned char bit = 0;
		if (size) {
			bit = byte[1] & (1 << (8 - 1)) ? 1 : 0;
		}
		byte[0] <<= 1;
		byte[0] |= bit;
	}
}

KadNodeId& KadNodeId::operator ^= (const KadNodeId &id)
{
	for (unsigned i = 0; i < sizeof(mData); i++) {
		mData[i] ^= id.mData[i];
	}
	return *this;
}

KadNodeId& KadNodeId::operator |= (const KadNodeId &id)
{
	for (unsigned i = 0; i < sizeof(mData); i++) {
		mData[i] |= id.mData[i];
	}
	return *this;
}

KadNodeId& KadNodeId::operator &= (const KadNodeId &id)
{
	for (unsigned i = 0; i < sizeof(mData); i++) {
		mData[i] &= id.mData[i];
	}
	return *this;
}

KadNodeId KadNodeId::operator ^(const KadNodeId &id) const
{
	KadNodeId result;
	for (unsigned i = 0; i < sizeof(mData); i++) {
		result.mData[i] = mData[i] ^ id.mData[i];
	}
	return result;
}

KadNodeId KadNodeId::operator |(const KadNodeId &id) const
{
	KadNodeId result;
	for (unsigned i = 0; i < sizeof(mData); i++) {
		result.mData[i] = mData[i] | id.mData[i];
	}
	return result;
}

KadNodeId KadNodeId::operator &(const KadNodeId &id) const
{
	KadNodeId result;
	for (unsigned i = 0; i < sizeof(mData); i++) {
		result.mData[i] = mData[i] | id.mData[i];
	}
	return result;
}

KadNodeId KadNodeId::operator ~() const
{
	KadNodeId result;
	for (unsigned i = 0; i < sizeof(mData); i++) {
		result.mData[i] = ~mData[i];
	}
	return result;
}

// returns n in: 2^n <= distance(n1, n2) < 2^(n+1)
// useful for finding out which bucket a node belongs to
unsigned KadNodeId::DistanceTo(const KadNodeId& id) const
{
	for (unsigned i = 0; i < sizeof(mData); ++i) {
		if (const uint8_t t = mData[i] ^ id.mData[i]) {
			// Found non-matching byte, find exact bit
			const int bit = (sizeof(mData) - 1 - i) * 8;
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
