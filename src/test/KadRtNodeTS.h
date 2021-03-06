#define private public
#include "KadRtNode.h"
#undef private

#include <cxxtest/TestSuite.h>

class KadRtNodeTS: public CxxTest::TestSuite
{
	struct Void { };
	typedef TKadRtNode TestRt;
public:

	void testInit(void) {
		TestRt rt(KadId("0"));
		TestRt::Stats s = rt.GetStats();
		TS_ASSERT_EQUALS(s.cached, 0);
		TS_ASSERT_EQUALS(s.contacts, 0);
		TS_ASSERT_EQUALS(s.spaces, 0);
		TS_ASSERT_EQUALS(s.buckets, 1);
	}

	void testRouting(void)
	{
		TestRt rt = KadId::Random();

		for (int i=0; i<10000; i++) {
			KadId cid = KadId::Random();
			rt.AddNode(cid, XSockAddr::Random());
			TestRt::Stats s = rt.GetStats();
			TS_ASSERT_EQUALS(s.spaces, s.buckets-1);
		}

		//TS_ASSERT_EQUALS(rt.CountCache(), 0);
		//TS_ASSERT_EQUALS(rt.CountContacts(), 0);

	}

	/*void Routing2(void)
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
	}*/
};
