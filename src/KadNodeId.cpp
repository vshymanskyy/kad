#include "KadNodeId.h"
#include <cryptopp/sha.h>

#if KADEMLIA_ID_BITS == 128
#	include <cryptopp/md5.h>
#endif

template<>
TKadId<64> TKadId<64>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA512 sha;
	sha.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}

template<>
TKadId<48> TKadId<48>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA384 sha;
	sha.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}

template<>
TKadId<32> TKadId<32>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA256 sha;
	sha.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}

template<>
TKadId<28> TKadId<28>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::SHA224 sha;
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

#if KADEMLIA_ID_BITS == 128
template<>
TKadId<16> TKadId<16>::FromHash(const void* data, size_t len)
{
	TKadId result;
	CryptoPP::MD5 md5;
	md5.CalculateDigest((uint8_t*)&result.mData, (uint8_t*)data, len);
	return result;
}
#endif

template<unsigned N>
TKadId<N> TKadId<N>::FromHash(const void* data, size_t len)
{
	TKadId	ids[32/N];

	CryptoPP::SHA256 sha;
	sha.CalculateDigest((uint8_t*)ids, (uint8_t*)data, len);

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
TKadId<3> TKadId<3>::FromHash(const void* data, size_t len);

template
TKadId<4> TKadId<4>::FromHash(const void* data, size_t len);

template
TKadId<8> TKadId<8>::FromHash(const void* data, size_t len);

