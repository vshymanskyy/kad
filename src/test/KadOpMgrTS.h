#include "KadOpMgr.h"
#include <cxxtest/TestSuite.h>

class KadOpMgrTS : public CxxTest::TestSuite
{
public:

	void testMsgSizes() {
		LOG(NULL, "Req: " << sizeof(KadMsg));
		LOG(NULL, "Rsp: " << sizeof(KadMsgRsp));

		LOG(NULL, "Ping: " << sizeof(KadMsgPing));
		LOG(NULL, "Pong: " << sizeof(KadMsgPong));

		LOG(NULL, "Addr: " << sizeof(KadMsgAddr));
		LOG(NULL, "Contact: " << sizeof(KadMsgContact));

		LOG(NULL, "FindReq: " << sizeof(KadMsgFindReq));
		LOG(NULL, "FindRsp: " << sizeof(KadMsgFindRsp));

	}

	class PingTester {
	public:
		PingTester()
			: mgr1(KadId::Random(), XSockAddr("127.0.0.1:3001"))
			, mgr2(KadId::Random(), XSockAddr("127.0.0.1:3002"))
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
			: mgrBSP(KadId::Random(), XSockAddr("127.0.0.1:3000"))
			, mgr1(KadId::Random(), XSockAddr("127.0.0.1:3001"))
			//, mgr2(KadNodeId::Random(), XSockAddr("127.0.0.1:3002"))
			//, mgr3(KadNodeId::Random(), XSockAddr("127.0.0.1:3003"))
		{
			XList<KadContact> bootstrap(KadContact(mgrBSP.LocalId(), mgrBSP.BindAddr()), 1);

			//TODO: mgr1.Join(bootstrap);
			//mgr2.Join(bootstrap);
			//mgr3.Join(bootstrap);

			XThread::SleepMs(100);
		}

		void test() {

		}

	private:
		KadOpMgr mgrBSP, mgr1;//, mgr2, mgr3;
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
