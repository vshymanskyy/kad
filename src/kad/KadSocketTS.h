#include "KadSocket.h"
#include <cxxtest/TestSuite.h>

class KadSocketTS: public CxxTest::TestSuite
{
public:
	void testAddrAlignment(void)
	{
		KadAddr addr;
		memset(&addr, 0, sizeof(addr));
		addr.sa.sa_family = AF_INET;
		TS_ASSERT_EQUALS(addr.sa.sa_family, AF_INET);
		TS_ASSERT_EQUALS(addr.sa_in.sin_family, AF_INET);
		TS_ASSERT_EQUALS(addr.sa_in6.sin6_family, AF_INET);

		addr.sa.sa_family = AF_INET6;
		TS_ASSERT_EQUALS(addr.sa.sa_family, AF_INET6);
		TS_ASSERT_EQUALS(addr.sa_in.sin_family, AF_INET6);
		TS_ASSERT_EQUALS(addr.sa_in6.sin6_family, AF_INET6);
	}
};
