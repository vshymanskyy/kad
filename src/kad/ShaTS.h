#include "SHA1.h"
#include "SHA256.h"

#include <cxxtest/TestSuite.h>

inline void Raw2Hex(const uint8_t* data, size_t len, char* buffer)
{
	for (unsigned i = 0; i < len; i++) {
		buffer += sprintf(buffer, "%02X", data[i]);
	}
}

#define TEST_SHA_STRING(SHA, str, dgst)			\
{												\
	SHA sha;									\
	uint8_t digest[sha.GetHashLength()];		\
	char digestStr[sha.GetHashLength()*2+2];	\
	sha.Update(str, strlen(str));				\
	sha.Finalize(digest);	\
	Raw2Hex(digest, sha.GetHashLength(), digestStr); \
	TS_ASSERT_EQUALS(0, strcasecmp(digestStr, dgst)); \
}

class ShaTS: public CxxTest::TestSuite
{
public:

	void testSha1(void)
	{
		// === Test Vectors (from FIPS PUB 180-1) ===
		TEST_SHA_STRING(SHA1, "", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
		TEST_SHA_STRING(SHA1, "abc", "A9993E364706816ABA3E25717850C26C9CD0D89D");
		TEST_SHA_STRING(SHA1, "The quick brown fox jumps over the lazy dog",
				"2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
		TEST_SHA_STRING(SHA1, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
				"84983E441C3BD26EBAAE4AA1F95129E5E54670F1");
	}

	void testSha256(void)
	{
		// === Test Vectors (from FIPS PUB 180-2) ===
		TEST_SHA_STRING(SHA256, "", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
		TEST_SHA_STRING(SHA256, "abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
		TEST_SHA_STRING(SHA256, "The quick brown fox jumps over the lazy dog",
				"d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
		TEST_SHA_STRING(SHA256, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
				"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
	}

};
