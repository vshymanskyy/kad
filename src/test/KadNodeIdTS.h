#include "KadNodeId.h"
#include <cxxtest/TestSuite.h>

class KadNodeIdTS: public CxxTest::TestSuite
{
public:
	void testOperations(void)
	{
		TS_ASSERT(KadNodeId::Zero() < KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::PowerOfTwo(0) < KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::PowerOfTwo(2) > KadNodeId::PowerOfTwo(1));

		// Todo: Fix it
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-1) == KadNodeId::PowerOfTwo(0));
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-2) == KadNodeId::PowerOfTwo(1));
		TS_ASSERT(KadNodeId::Zero().SetBit(KADEMLIA_ID_BITS-3) == KadNodeId::PowerOfTwo(2));
	}
};
