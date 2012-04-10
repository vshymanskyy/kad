#include "KadNodeId.h"
#include "SHA256.h"
#include "SHA1.h"

template<>
KadId<256> KadId<256>::FromHash(const void* data, size_t len)
{
	KadId result;
	SHA256 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}

template<>
KadId<160> KadId<160>::FromHash(const void* data, size_t len)
{
	KadId result;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}
