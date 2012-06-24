#include "KadNodeId.h"
#include "SHA256.h"
#include "SHA1.h"

template<>
KadId<32> KadId<32>::FromHash(const void* data, size_t len)
{
	KadId result;
	SHA256 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}

template<>
KadId<20> KadId<20>::FromHash(const void* data, size_t len)
{
	KadId result;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}
