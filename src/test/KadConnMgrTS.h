#include "KadOverIP.h"
#include <cxxtest/TestSuite.h>

class KadConnMgrTS : public CxxTest::TestSuite
{
public:

	void testMessage(void)
	{
		KadNet::Address a("127.0.0.1:3004");
		KadNet::Address b("127.0.0.1:3005");
		KadNet::Address c("127.0.0.1:3006");

		KadNet::ConnectionMgr conna(a);
		KadNet::ConnectionMgr connb(b);
		KadNet::ConnectionMgr connc(c);

		KadNet::ConnectionMgr::Connection* socka = conna.Connect(c);
		KadNet::ConnectionMgr::Connection* sockb = connb.Connect(a);
		KadNet::ConnectionMgr::Connection* sockc = connc.Connect(b);

		socka->Send("aaaaa", 6);

		delete (socka);
		delete (sockb);
		delete (sockc);

		socka->Send("aaaaa", 6);

		XThread::SleepMs(100);
	}
};
