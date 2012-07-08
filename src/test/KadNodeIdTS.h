#include "KadNodeId.h"
#include <cxxtest/TestSuite.h>

class KadNodeIdTS: public CxxTest::TestSuite
{
public:

	void testFromHex()
	{
		// Empty string
		TS_ASSERT(TKadId<2>::Zero() == TKadId<2>::FromHex(""));

		// Different length
		TS_ASSERT(TKadId<2>::Zero() == TKadId<2>::FromHex("0"));
		TS_ASSERT(TKadId<2>::Zero() == TKadId<2>::FromHex("00"));
		TS_ASSERT(TKadId<2>::Zero() == TKadId<2>::FromHex("000"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(0) == TKadId<2>::FromHex("001"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(1) == TKadId<2>::FromHex("02"));
		TS_ASSERT(TKadId<2>::PowerOfTwo(2) == TKadId<2>::FromHex("4"));

		// Case with more symbols than needed
		TS_ASSERT(TKadId<2>::PowerOfTwo(0) == TKadId<2>::FromHex("F0001"));

		// Cases with invalid symbol
		TS_ASSERT(TKadId<2>::FromHex("9A") == TKadId<2>::FromHex("z9A"));
		TS_ASSERT(TKadId<2>::FromHex("69A") == TKadId<2>::FromHex("z69A"));
	}

	void testFromHash()
	{
		TS_ASSERT(TKadId<20>::FromHash("", 0) == TKadId<20>::FromHex("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
		TS_ASSERT(TKadId<32>::FromHash("", 0) == TKadId<32>::FromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
	}

	void testOperations()
	{
		TS_ASSERT(KadId::Zero().SetBit(KADEMLIA_ID_BITS-1) == KadId::PowerOfTwo(0));
		TS_ASSERT(KadId::Zero().SetBit(KADEMLIA_ID_BITS-2) == KadId::PowerOfTwo(1));
		TS_ASSERT(KadId::Zero().SetBit(KADEMLIA_ID_BITS-3) == KadId::PowerOfTwo(2));

		TS_ASSERT(~TKadId<4>::FromHex("FFFFFFFF") == TKadId<4>::FromHex("00000000"));
		TS_ASSERT(~TKadId<4>::FromHex("12345678") == TKadId<4>::FromHex("EDCBA987"));

		TS_ASSERT((TKadId<4>::FromHex("12345678") | TKadId<4>::FromHex("87654321")) == TKadId<4>::FromHex("97755779"));
		TS_ASSERT((TKadId<4>::FromHex("12345678") & TKadId<4>::FromHex("87654321")) == TKadId<4>::FromHex("02244220"));
		TS_ASSERT((TKadId<4>::FromHex("12345678") ^ TKadId<4>::FromHex("87654321")) == TKadId<4>::FromHex("95511559"));

		TS_ASSERT(TKadId<4>::FromHex("12345678").ShiftLeft() == TKadId<4>::FromHex("2468ACF0"));
	}

	void testCompare()
	{
		TS_ASSERT(KadId::Zero() < KadId::PowerOfTwo(1));
		TS_ASSERT(KadId::PowerOfTwo(0) < KadId::PowerOfTwo(1));
		TS_ASSERT(KadId::PowerOfTwo(2) > KadId::PowerOfTwo(1));

		TS_ASSERT(TKadId<2>::FromHex("4") > TKadId<2>::FromHex("3"));
		TS_ASSERT(TKadId<2>::FromHex("5") < TKadId<2>::FromHex("6"));

		TS_ASSERT(TKadId<2>::FromHex("5") != TKadId<2>::FromHex("6"));
		TS_ASSERT((TKadId<2>::FromHex("5") != TKadId<2>::FromHex("5")) == false);

		TS_ASSERT(TKadId<2>::FromHex("FA") >= TKadId<2>::FromHex("F0"));
		TS_ASSERT(TKadId<2>::FromHex("BB") >= TKadId<2>::FromHex("BB"));

		TS_ASSERT(TKadId<2>::FromHex("FF0") <= TKadId<2>::FromHex("FFA"));
		TS_ASSERT(TKadId<2>::FromHex("FFC") <= TKadId<2>::FromHex("FFC"));
	}

	void testDistanceTo()
	{
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("9876").DistanceTo(TKadId<2>::FromHex("9876")), 0);

		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0001").DistanceTo(TKadId<2>::FromHex("0000")), 1);
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0000").DistanceTo(TKadId<2>::FromHex("0001")), 1);

		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0001").DistanceTo(TKadId<2>::FromHex("0002")), 2);
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0002").DistanceTo(TKadId<2>::FromHex("0001")), 2);

		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0001").DistanceTo(TKadId<2>::FromHex("0004")), 3);
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0004").DistanceTo(TKadId<2>::FromHex("0001")), 3);

		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0007").DistanceTo(TKadId<2>::FromHex("0000")), 3);
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0000").DistanceTo(TKadId<2>::FromHex("0007")), 3);

		TS_ASSERT_EQUALS(TKadId<2>::FromHex("FFFF").DistanceTo(TKadId<2>::FromHex("0000")), 16);
		TS_ASSERT_EQUALS(TKadId<2>::FromHex("0000").DistanceTo(TKadId<2>::FromHex("FFFF")), 16);
	}

	void testCloser()
	{
		TS_ASSERT(TKadId<2>::FromHex("FF0").Closer(TKadId<2>::FromHex("F00"), TKadId<2>::FromHex("000")));
	}

	void testGenInRange1()
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

	void testGenInRange2()
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
	}

};
