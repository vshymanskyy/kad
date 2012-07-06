#include "KadRtNode.h"
#include <cxxtest/TestSuite.h>

struct Void{};

class KademliaTS: public CxxTest::TestSuite
{
public:
	void testRouting1(void)
	{
		KadId lid = KadId::Random();
		TKadRtNode<Void> rt;

		for (int i=0; i<10000; i++) {
			KadId cid = KadId::Random();
			rt.AddNode(TKadContact<KADEMLIA_ID_SIZE, Void>(cid, Void()), cid^lid);
		}
		TS_ASSERT_DELTA(rt.GetContacts().Count(), 230, 10);
		//TS_ASSERT_DELTA(rt.CountCachedContacts(), 100, 10);
	}
};
