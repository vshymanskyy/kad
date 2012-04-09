#include "KadOpMgr.h"
#include <cxxtest/TestSuite.h>

class KadTransactionMgrTS: public CxxTest::TestSuite
{
public:

	class PingTester {
	public:
		PingTester()
			: mgr1(KadNodeId::Random(), 3001)
			, mgr2(KadNodeId::Random(), 3002)
		{

		}

		void test() {
			mPonged = false;
			mgr1.Ping(KadAddr::FromIPv4(INADDR_LOOPBACK, 3002), KadPingHandler(this, &PingTester::pong));
			XThread::SleepMs(5);
			TS_ASSERT(mPonged);
		}

	private:
		void pong() {
			mPonged = true;
		}
	private:
		KadOpMgr mgr1;
		KadOpMgr mgr2;
		bool mPonged;
	};


	void testPing(void)
	{
		PingTester t;
		t.test();
		t.test();
	}
};
