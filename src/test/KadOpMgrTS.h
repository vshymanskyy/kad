#include "KadOpMgr.h"
#include <cxxtest/TestSuite.h>

class KadOpMgrTS : public CxxTest::TestSuite
{
public:

	class PingTester {
	public:
		PingTester()
			: mgr1(KadNodeId::Random(), XSockAddr("127.0.0.1:3001"), XList<KadContact>())
			, mgr2(KadNodeId::Random(), XSockAddr("127.0.0.1:3002"), XList<KadContact>())
		{

		}

		void test() {
			mPonged = false;
			mgr1.Ping(XSockAddr("127.0.0.1:3002"), KadOpMgr::KadPing::Handler(this, &PingTester::pong));
			XThread::SleepMs(1100);
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


	class JoinTester {
	public:
		JoinTester()
		{
			XList<KadContact> bsp1;
			XList<KadContact> bsp2;
			XList<KadContact> bsp3;

			KadNodeId id1 = KadNodeId::Random();
			KadNodeId id2 = KadNodeId::Random();
			KadNodeId id3 = KadNodeId::Random();

			XSockAddr addr1("127.0.0.1:3001");
			XSockAddr addr2("127.0.0.1:3002");
			XSockAddr addr3("127.0.0.1:3003");

			bsp2.Append(KadContact(id1, addr1));
			bsp3.Append(KadContact(id1, addr1));

			mgr1 = new KadOpMgr(id1, addr1, bsp1);
			mgr2 = new KadOpMgr(id2, addr2, bsp2);
			mgr3 = new KadOpMgr(id3, addr3, bsp3);
		}

		~JoinTester() {
			delete mgr1;
			delete mgr2;
			delete mgr3;
		}

		void test() {

		}

	private:
		void pong() {
			mFound = true;
		}
	private:
		KadOpMgr* mgr1;
		KadOpMgr* mgr2;
		KadOpMgr* mgr3;
		bool mFound;
	};


	void testPing(void)
	{
		PingTester().test();
	}


	void testJoin(void)
	{
		 JoinTester().test();
	}


/*	class FindTester {
	public:
		FindTester()
			: mgr1(KadNodeId::Random(), XSockAddr("127.0.0.1:3001"))
			, mgr2(KadNodeId::Random(), XSockAddr("127.0.0.1:3002"))
			, mgr3(KadNodeId::Random(), XSockAddr("127.0.0.1:3003"))
		{

		}

		void test() {
			mFound = false;
			mgr1.Ping(XSockAddr("127.0.0.1:3002"), KadOpMgr::KadPing::Handler(this, &FindTester::pong));
			XThread::SleepMs(1100);
			TS_ASSERT(mFound);
		}

	private:
		void pong() {
			mFound = true;
		}
	private:
		KadOpMgr mgr1, mgr2, mgr3;
		bool mFound;
	};


	void Find(void)
	{
		 FindTester().test();
	}
*/
};
