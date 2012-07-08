#include "KadRtNode.h"
#include <cxxtest/TestSuite.h>


class KadRtNodeTS: public CxxTest::TestSuite
{
	struct Void{};
	typedef TKadRtNode<Void> TestRt;
public:

	void testInit(void) {
		TestRt rt;
		TS_ASSERT_EQUALS(rt.CountCache(), 0);
		TS_ASSERT_EQUALS(rt.CountContacts(), 0);
		TS_ASSERT_EQUALS(rt.CountSpaces(), 0);
		TS_ASSERT_EQUALS(rt.CountBuckets(), 1);
	}

	void testRouting(void)
	{
		KadId lid = KadId::Random();
		TestRt rt;

		for (int i=0; i<10000; i++) {
			KadId cid = KadId::Random();
			rt.AddNode(TestRt::Contact(cid, Void()), cid^lid);
			TS_ASSERT_EQUALS(rt.CountSpaces(), rt.CountBuckets()-1);
		}

		//TS_ASSERT_EQUALS(rt.CountCache(), 0);
		//TS_ASSERT_EQUALS(rt.CountContacts(), 0);

	}

	void testRouting2(void)
	{
		TestRt rt;

		for (int i=0; i<KADEMLIA_ID_BITS-1; i++) {
			for (int b=0; b < KADEMLIA_BUCKET_SIZE; b++) {
				KadId id = KadId::GenInRange(KadId::PowerOfTwo(i), KadId::PowerOfTwo(i+1));
				rt.AddNode(TestRt::Contact(id, Void()), id);
				LOG(NULL, "Adding:" << id);
			}
			LOG(NULL, "NEXT BIT");
		}

		TS_ASSERT_EQUALS(rt.CountCache(), 0);
		TS_ASSERT_EQUALS(rt.CountContacts(), 0);
		TS_ASSERT_EQUALS(rt.CountSpaces(), 0);
		TS_ASSERT_EQUALS(rt.CountBuckets(), 0);
	}
};
