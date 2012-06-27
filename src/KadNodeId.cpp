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

template<>
KadId<8> KadId<8>::FromHash(const void* data, size_t len)
{
	union {
		struct {
			KadId res[4];
		};
		uint8_t buff[32];
	} u;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(u.buff);
	return u.res[0] ^ u.res[1] ^ u.res[2] ^ u.res[3];
}
