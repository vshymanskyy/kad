#include "KadNodeId.h"
#include <cxxtest/TestSuite.h>


class KadNodeIdTS: public CxxTest::TestSuite
{
public:

	void testFromHex()
	{
		// Empty string
		TS_ASSERT(TKadId<2>() == TKadId<2>(""));

		// Different length
		TS_ASSERT(TKadId<2>() == TKadId<2>("0"));
		TS_ASSERT(TKadId<2>() == TKadId<2>("00"));
		TS_ASSERT(TKadId<2>() == TKadId<2>("000"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(0) == TKadId<2>("001"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(1) == TKadId<2>("02"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(2) == TKadId<2>("4"));

		// Case with more symbols than needed
		TS_ASSERT(TKadId<2>::PowerOfTwo(0) == TKadId<2>("F0001"));

		// Cases with invalid symbol
		TS_ASSERT(TKadId<2>("9A") == TKadId<2>("z9A"));
		TS_ASSERT(TKadId<2>("69A") == TKadId<2>("z69A"));
	}

	void testFromNum()
	{
		// Empty string
		TS_ASSERT(TKadId<2>((size_t)0) == TKadId<2>());

		// Different length
		TS_ASSERT(TKadId<2>(0x11) == TKadId<2>("11"));
		TS_ASSERT(TKadId<2>(0x123) == TKadId<2>("123"));
		TS_ASSERT(TKadId<2>(0x1234) == TKadId<2>("1234"));
		TS_ASSERT(TKadId<2>(0x123456789ABCDEF0) == TKadId<2>("123456789ABCDEF0"));
	}

	void testFromHashSHA1()
	{
		TS_ASSERT_EQUALS(TKadId<20>::FromHash(""),
				TKadId<20>("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
		TS_ASSERT_EQUALS(TKadId<20>::FromHash("abc"),
				TKadId<20>("A9993E364706816ABA3E25717850C26C9CD0D89D"));
		TS_ASSERT_EQUALS(TKadId<20>::FromHash("The quick brown fox jumps over the lazy dog"),
				TKadId<20>("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"));
		TS_ASSERT_EQUALS(TKadId<20>::FromHash("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
				TKadId<20>("84983E441C3BD26EBAAE4AA1F95129E5E54670F1"));
	}

	void testFromHashSHA256()
	{
		TS_ASSERT_EQUALS(TKadId<32>::FromHash(""),
				TKadId<32>("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
		TS_ASSERT_EQUALS(TKadId<32>::FromHash("abc"),
				TKadId<32>("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
		TS_ASSERT_EQUALS(TKadId<32>::FromHash("The quick brown fox jumps over the lazy dog"),
				TKadId<32>("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"));
		TS_ASSERT_EQUALS(TKadId<32>::FromHash("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
				TKadId<32>("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));
	}

	void testPowerOfTwo()
	{
		TS_ASSERT(KadId().SetBit(KADEMLIA_ID_BITS-1) == KadId::PowerOfTwo(0));
		TS_ASSERT(KadId().SetBit(KADEMLIA_ID_BITS-2) == KadId::PowerOfTwo(1));
		TS_ASSERT(KadId().SetBit(0) == KadId::PowerOfTwo(KADEMLIA_ID_BITS-1));
	}

	void testBinary()
	{
		TS_ASSERT((TKadId<4>("12345678") | TKadId<4>("87654321")) == TKadId<4>("97755779"));
		TS_ASSERT((TKadId<4>("12345678") & TKadId<4>("87654321")) == TKadId<4>("02244220"));
		TS_ASSERT((TKadId<4>("12345678") ^ TKadId<4>("87654321")) == TKadId<4>("95511559"));
		TS_ASSERT(~TKadId<4>("12345678") == TKadId<4>("EDCBA987"));
	}

	void testShl()
	{
		TS_ASSERT(TKadId<4>("12345678").Shl() == TKadId<4>("2468ACF0"));
		TS_ASSERT(TKadId<16>("12345678") << 7 == TKadId<16>("91A2B3C00"));
	}

	void testAdd()
	{
		TS_ASSERT(TKadId<16>("0").Add(TKadId<16>("1")) == TKadId<16>("1"));
		TS_ASSERT(TKadId<16>("1").Add(TKadId<16>("1")) == TKadId<16>("2"));
		TS_ASSERT(TKadId<16>("1").Add(TKadId<16>("2")) == TKadId<16>("3"));
		TS_ASSERT(TKadId<16>("1").Add(TKadId<16>("3")) == TKadId<16>("4"));
		TS_ASSERT(TKadId<16>("2").Add(TKadId<16>("2")) == TKadId<16>("4"));
		TS_ASSERT(TKadId<16>("3").Add(TKadId<16>("1")) == TKadId<16>("4"));

		TS_ASSERT(TKadId<4>("12345678") + TKadId<4>("89ABCDEF") == TKadId<4>("9BE02467"));
	}

	void testSub()
	{
		TS_ASSERT(TKadId<4>("89ABCDEF") - TKadId<4>("12345678") == TKadId<4>("77777777"));
		TS_ASSERT(TKadId<4>("10000") - TKadId<4>("FFF") == TKadId<4>("F001"));
	}

	void testMul()
	{
		TS_ASSERT(TKadId<8>("89ABCDEF") * TKadId<8>("12345678") == TKadId<8>("9CA39E0E242D208"));
	}


	void testCompare()
	{
		TS_ASSERT(KadId() < KadId::PowerOfTwo(1));
		TS_ASSERT(KadId::PowerOfTwo(0) < KadId::PowerOfTwo(1));
		TS_ASSERT(KadId::PowerOfTwo(2) > KadId::PowerOfTwo(1));

		TS_ASSERT(TKadId<4>(45678) > 45677);
		TS_ASSERT(TKadId<4>(45678) >= 45677);
		TS_ASSERT(TKadId<4>(45677) < 45678);
		TS_ASSERT(TKadId<4>(45677) <= 45678);

		TS_ASSERT(TKadId<4>(45678) <= 45678);
		TS_ASSERT(TKadId<4>(45678) >= 45678);

		TS_ASSERT(TKadId<2>(5) != 6);
		TS_ASSERT(!(TKadId<2>(5) != 5));

		TS_ASSERT(TKadId<2>(0xFA) >= 0xF0);
		TS_ASSERT(TKadId<2>(0xBB) >= 0xBB);

		TS_ASSERT(TKadId<2>("FF0") <= 0xFFA);
		TS_ASSERT(TKadId<2>("FFC") <= 0xFFC);
	}

	void testDistanceTo()
	{
		TS_ASSERT_EQUALS(TKadId<2>("9876").DistanceTo(TKadId<2>("9876")), 0);

		TS_ASSERT_EQUALS(TKadId<2>("0001").DistanceTo(TKadId<2>("0000")), 1);
		TS_ASSERT_EQUALS(TKadId<2>("0000").DistanceTo(TKadId<2>("0001")), 1);

		TS_ASSERT_EQUALS(TKadId<2>("0001").DistanceTo(TKadId<2>("0002")), 2);
		TS_ASSERT_EQUALS(TKadId<2>("0002").DistanceTo(TKadId<2>("0001")), 2);

		TS_ASSERT_EQUALS(TKadId<2>("0001").DistanceTo(TKadId<2>("0004")), 3);
		TS_ASSERT_EQUALS(TKadId<2>("0004").DistanceTo(TKadId<2>("0001")), 3);

		TS_ASSERT_EQUALS(TKadId<2>("0007").DistanceTo(TKadId<2>("0000")), 3);
		TS_ASSERT_EQUALS(TKadId<2>("0000").DistanceTo(TKadId<2>("0007")), 3);

		TS_ASSERT_EQUALS(TKadId<2>("FFFF").DistanceTo(TKadId<2>("0000")), 16);
		TS_ASSERT_EQUALS(TKadId<2>("0000").DistanceTo(TKadId<2>("FFFF")), 16);
	}

	void testCloser()
	{
		TS_ASSERT(TKadId<2>("FF0").Closer(TKadId<2>("F00"), TKadId<2>("000")));
	}

	/*void GenInRange1()
	{
		TKadId<8> prev;
		for (int i=0; i<10; i++) {
			TKadId<8> a = TKadId<8>::Random();
			TKadId<8> b = TKadId<8>::Random();
			for (int j=0; j<1000; j++) {
				TKadId<8> gen = TKadId<8>::GenInRange(a, b);

				TS_ASSERT_LESS_THAN_EQUALS(X_MIN(a,b), gen);
				TS_ASSERT_LESS_THAN_EQUALS(gen, X_MAX(a,b));
				TS_ASSERT_DIFFERS(gen, prev);

				prev = gen;
			}
		}
	}

	void GenInRange2()
	{
		TKadId<8> prev;
		for (int j=0; j<100000; j++) {
			TKadId<8> a = TKadId<8>::Random();
			TKadId<8> b = TKadId<8>::Random();
			TKadId<8> gen = TKadId<8>::GenInRange(a, b);

			TS_ASSERT_LESS_THAN_EQUALS(X_MIN(a,b), gen);
			TS_ASSERT_LESS_THAN_EQUALS(gen, X_MAX(a,b));
			TS_ASSERT_DIFFERS(gen, prev);

			prev = gen;
		}
	}*/

};
