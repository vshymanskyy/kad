#include "KadNodeId.h"
#include <cxxtest/TestSuite.h>

class KadNodeIdTS: public CxxTest::TestSuite
{
public:

	void testFromHex()
	{
		// Empty string
		TS_ASSERT(KadId<2>::Zero() == KadId<2>::FromHex(""));

		// Different length
		TS_ASSERT(KadId<2>::Zero() == KadId<2>::FromHex("0"));
		TS_ASSERT(KadId<2>::Zero() == KadId<2>::FromHex("00"));
		TS_ASSERT(KadId<2>::Zero() == KadId<2>::FromHex("000"));
		TS_ASSERT(KadId<2>::PowerOfTwo(0) == KadId<2>::FromHex("001"));
		TS_ASSERT(KadId<2>::PowerOfTwo(1) == KadId<2>::FromHex("02"));
		TS_ASSERT(KadId<2>::PowerOfTwo(2) == KadId<2>::FromHex("4"));

		// Case with more symbols than needed
		TS_ASSERT(KadId<2>::PowerOfTwo(0) == KadId<2>::FromHex("F0001"));

		// Cases with invalid symbol
		TS_ASSERT(KadId<2>::FromHex("9A") == KadId<2>::FromHex("z9A"));
		TS_ASSERT(KadId<2>::FromHex("69A") == KadId<2>::FromHex("z69A"));
	}

	void testFromHash()
	{
		TS_ASSERT(KadId<20>::FromHash("", 0) == KadId<20>::FromHex("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
		TS_ASSERT(KadId<32>::FromHash("", 0) == KadId<32>::FromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
	}

	void testOperations()
	{
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-1) == KadNodeId::PowerOfTwo(0));
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-2) == KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-3) == KadNodeId::PowerOfTwo(2));

		TS_ASSERT(~KadId<4>::FromHex("FFFFFFFF") == KadId<4>::FromHex("00000000"));
		TS_ASSERT(~KadId<4>::FromHex("12345678") == KadId<4>::FromHex("EDCBA987"));

		TS_ASSERT((KadId<4>::FromHex("12345678") | KadId<4>::FromHex("87654321")) == KadId<4>::FromHex("97755779"));
		TS_ASSERT((KadId<4>::FromHex("12345678") & KadId<4>::FromHex("87654321")) == KadId<4>::FromHex("02244220"));
		TS_ASSERT((KadId<4>::FromHex("12345678") ^ KadId<4>::FromHex("87654321")) == KadId<4>::FromHex("95511559"));

		TS_ASSERT(KadId<4>::FromHex("12345678").ShiftLeft() == KadId<4>::FromHex("2468ACF0"));
	}

	void testCompare()
	{
		TS_ASSERT(KadNodeId::Zero() < KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::PowerOfTwo(0) < KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::PowerOfTwo(2) > KadNodeId::PowerOfTwo(1));

		TS_ASSERT(KadId<2>::FromHex("4") > KadId<2>::FromHex("3"));
		TS_ASSERT(KadId<2>::FromHex("5") < KadId<2>::FromHex("6"));

		TS_ASSERT(KadId<2>::FromHex("5") != KadId<2>::FromHex("6"));
		TS_ASSERT((KadId<2>::FromHex("5") != KadId<2>::FromHex("5")) == false);

		TS_ASSERT(KadId<2>::FromHex("FA") >= KadId<2>::FromHex("F0"));
		TS_ASSERT(KadId<2>::FromHex("BB") >= KadId<2>::FromHex("BB"));

		TS_ASSERT(KadId<2>::FromHex("FF0") <= KadId<2>::FromHex("FFA"));
		TS_ASSERT(KadId<2>::FromHex("FFC") <= KadId<2>::FromHex("FFC"));
	}

	void testDistanceTo()
	{
		TS_ASSERT_EQUALS(KadId<2>::FromHex("9876").DistanceTo(KadId<2>::FromHex("9876")), 0);

		TS_ASSERT_EQUALS(KadId<2>::FromHex("0001").DistanceTo(KadId<2>::FromHex("0000")), 1);
		TS_ASSERT_EQUALS(KadId<2>::FromHex("0000").DistanceTo(KadId<2>::FromHex("0001")), 1);

		TS_ASSERT_EQUALS(KadId<2>::FromHex("0001").DistanceTo(KadId<2>::FromHex("0002")), 2);
		TS_ASSERT_EQUALS(KadId<2>::FromHex("0002").DistanceTo(KadId<2>::FromHex("0001")), 2);

		TS_ASSERT_EQUALS(KadId<2>::FromHex("0001").DistanceTo(KadId<2>::FromHex("0004")), 3);
		TS_ASSERT_EQUALS(KadId<2>::FromHex("0004").DistanceTo(KadId<2>::FromHex("0001")), 3);

		TS_ASSERT_EQUALS(KadId<2>::FromHex("0007").DistanceTo(KadId<2>::FromHex("0000")), 3);
		TS_ASSERT_EQUALS(KadId<2>::FromHex("0000").DistanceTo(KadId<2>::FromHex("0007")), 3);

		TS_ASSERT_EQUALS(KadId<2>::FromHex("FFFF").DistanceTo(KadId<2>::FromHex("0000")), 16);
		TS_ASSERT_EQUALS(KadId<2>::FromHex("0000").DistanceTo(KadId<2>::FromHex("FFFF")), 16);
	}

	void testCloser()
	{
		TS_ASSERT(KadId<2>::FromHex("FF0").Closer(KadId<2>::FromHex("F00"), KadId<2>::FromHex("000")));
	}

};
