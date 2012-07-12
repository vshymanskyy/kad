#include "KadNodeId.h"
#include <cryptopp/sha.h>


template<>
TKadId<32> TKadId<32>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA256 sha;
	sha.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}

template<>
TKadId<20> TKadId<20>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA1 sha;
	sha.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}

template<unsigned N>
TKadId<N> TKadId<N>::FromHash(const void* data, size_t len)
{
	union {
		TKadId	ids[32/N];
		uint8_t buff[32];
	};
	CryptoPP::SHA256 sha;
	sha.CalculateDigest((uint8_t*)buff, (uint8_t*)data, len);

	TKadId result;
	for (unsigned i = 0; i<X_COUNTOF(ids); ++i) {
		result ^= ids[i];
	}
	return result;
}

template
TKadId<1> TKadId<1>::FromHash(const void* data, size_t len);

template
TKadId<2> TKadId<2>::FromHash(const void* data, size_t len);

template
TKadId<4> TKadId<4>::FromHash(const void* data, size_t len);

template
TKadId<8> TKadId<8>::FromHash(const void* data, size_t len);
