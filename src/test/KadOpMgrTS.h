#include "KadOpMgr.h"
#include <cxxtest/TestSuite.h>

class KadOpMgrTS : public CxxTest::TestSuite
{
public:

	void testMsgSizes() {
		LOG(NULL, "Req: " << sizeof(KadMsg));
		LOG(NULL, "Rsp: " << sizeof(KadMsg));

		LOG(NULL, "Ping: " << sizeof(KadMsgPing));
		LOG(NULL, "Pong: " << sizeof(KadMsgPong));

		LOG(NULL, "Addr: " << sizeof(KadMsgAddr));
		LOG(NULL, "Contact: " << sizeof(KadMsgContact));

		LOG(NULL, "FindReq: " << sizeof(KadMsgFindReq));
		LOG(NULL, "FindRsp: " << sizeof(KadMsgFindRsp));

	}

/*	class PingTester {
	public:
		PingTester()
			: mgr1(KadId::Random(), XSockAddr("127.0.0.1:3001"))
			, mgr2(KadId::Random(), XSockAddr("127.0.0.1:3002"))
		{

		}

		void test() {
			mPonged1 = false;
			mPonged2 = false;
			mgr1.SendPing(XSockAddr("127.0.0.1:3002"), KadOpMgr::ReqTracker::Handler(this, &PingTester::pong1));
			mgr2.SendPing(XSockAddr("127.0.0.1:3001"), KadOpMgr::ReqTracker::Handler(this, &PingTester::pong2));
			XThread::SleepMs(10);
			TS_ASSERT(mPonged1);
			TS_ASSERT(mPonged2);
		}

	private:
		void pong1(const KadMsgRsp* rsp, KadContactPtr&) {
			if (rsp) {
				mPonged1 = true;
				LOG(NULL, "PONG1");
			} else {
				LOG(NULL, "PONG1 TIMEOUT");
			}
		}
		void pong2(const KadMsgRsp* rsp, KadContactPtr&) {
			if (rsp) {
				mPonged2 = true;
				LOG(NULL, "PONG2");
			} else {
				LOG(NULL, "PONG2 TIMEOUT");
			}
		}
	private:
		KadOpMgr mgr1;
		KadOpMgr mgr2;
		bool mPonged1;
		bool mPonged2;
	};
*/

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
		//PingTester().test();
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
