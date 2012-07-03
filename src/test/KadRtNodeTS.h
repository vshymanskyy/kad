#include "KadRtNode.h"
#include <cxxtest/TestSuite.h>

class KademliaTS: public CxxTest::TestSuite
{
public:
	void testRouting1(void)
	{
		KadId<1> lid = KadId<1>::Random();
		TKadRtNode<1,1,int> rt;

		for (int i=0; i<10000; i++) {
			KadId<1> cid = KadId<1>::Random();
			rt.AddNode(TKadContact<1,int>(cid, 0), cid^lid);
		}
		TS_ASSERT_DELTA(rt.CountContacts(), 230, 10);
		TS_ASSERT_DELTA(rt.CountCachedContacts(), 100, 10);
	}
};
