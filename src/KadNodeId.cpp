#include "KadNodeId.h"
#include "SHA256.h"
#include "SHA1.h"

template<>
TKadId<32> TKadId<32>::FromHash(const void* data, size_t len)
{
	TKadId result;
	SHA256 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}

template<>
TKadId<20> TKadId<20>::FromHash(const void* data, size_t len)
{
	TKadId result;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(result.mData);
	return result;
}

template<>
TKadId<8> TKadId<8>::FromHash(const void* data, size_t len)
{
	union {
		struct {
			TKadId res[4];
		};
		uint8_t buff[32];
	} u;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(u.buff);
	return u.res[0] ^ u.res[1] ^ u.res[2] ^ u.res[3];
}

template<>
TKadId<2> TKadId<2>::FromHash(const void* data, size_t len)
{
	union {
		struct {
			TKadId res[4];
		};
		uint8_t buff[32];
	} u;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(u.buff);
	return u.res[0];
}

template<>
TKadId<1> TKadId<1>::FromHash(const void* data, size_t len)
{
	union {
		struct {
			TKadId res[4];
		};
		uint8_t buff[32];
	} u;
	SHA1 sha;
	sha.Update(data, len);
	sha.Finalize(u.buff);
	return u.res[0];
}
