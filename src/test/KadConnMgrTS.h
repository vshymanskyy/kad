#include "KadConnMgr.h"
#include <cxxtest/TestSuite.h>

class KadConnMgrTS: public CxxTest::TestSuite
{
public:

	void testPing(void)
	{
		XSockAddr a("127.0.0.1:3004");
		XSockAddr b("127.0.0.1:3005");
		XSockAddr c("127.0.0.1:3006");

		KadConnMgr conna(a);
		KadConnMgr connb(b);
		KadConnMgr connc(c);

		UDTSOCKET socka = conna.Connect(c);
		UDTSOCKET sockb = connb.Connect(a);
		UDTSOCKET sockc = connc.Connect(b);

		UDT::sendmsg(socka, "aaaaa", 6);

		UDT::close(socka);
		UDT::close(sockb);
		UDT::close(sockc);

		UDT::sendmsg(socka, "aaaaa", 6);

		XThread::SleepMs(1000);
	}
};
