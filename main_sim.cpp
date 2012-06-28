#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include "KadOpMgr.h"
#include "KadContact.h"

//template <typename T>
//const T& ()

int bspBasePort = 3000;
int simBasePort = 5000;

XList<KadContact> bspList;

int GenerateCli(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Generates bootstrap contacts.\n  gen <qty> <port=%d>\n", bspBasePort);
		return 1;
	}

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	bspBasePort = (argc >= 3) ? atoi(argv[2]) : bspBasePort;

	XList<KadContact>::It it=bspList.Last();
	for (int i=0; i<qty; i++) {
		bspList.Append(KadContact(KadNodeId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", bspBasePort++))));
	}

	for (++it; it!=bspList.End(); ++it) {
		KadOpMgr* mgr = new KadOpMgr(bspList[it].mId, bspList[it].mAddr);
		mgr->Init(bspList);
	}
	return 0;
}

int SimulateCli(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Joins some contacts.\n  sim <qty> <port=%d>\n", simBasePort);
		return 1;
	}

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	simBasePort = (argc >= 3) ? atoi(argv[2]) : simBasePort;

	// Join simulation nodes
	for (int i=0; i<qty; i++) {
		KadOpMgr* mgr = new KadOpMgr(KadNodeId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", simBasePort++)));
		mgr->Join(bspList);
		XThread::SleepMs(100);
	}

	return 0;
}

KadOpMgr* mgrNode;

int JoinCli(int argc, char* argv[])
{
	if (mgrNode) return 1;

	if (argc < 2) {
		printf("Joins new node to 1 bsp.\n  j <addr>\n");
		return 1;
	}

	mgrNode = new KadOpMgr(KadNodeId::Random(), XSockAddr("127.0.0.1:2048"));
	mgrNode->Join(XList<XSockAddr>(XSockAddr(argv[1])));

	return 0;
}

int LeaveCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	mgrNode->Leave();
	delete mgrNode;
	mgrNode = NULL;
	return 0;
}

int FindNodeCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

int FindValueCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

int StoreCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

int RemoveCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

int SetDropRateCli(int argc, char* argv[])
{
	if (argc >= 2) {
		int qty = atoi(argv[1]);
		KadOpMgr::DROP_RATE_RX = qty;
	}
	if (argc >= 3) {
		int qty = atoi(argv[2]);
		KadOpMgr::DROP_RATE_TX = qty;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	RandInit();

	XShell sh("sim");

	sh.RegisterCommand("sim", &SimulateCli);
	sh.RegisterCommand("gen", &GenerateCli);

	sh.RegisterCommand("j", &JoinCli);
	sh.RegisterCommand("l", &LeaveCli);

	sh.RegisterCommand("fn", &FindNodeCli);
	sh.RegisterCommand("fv", &FindValueCli);

	sh.RegisterCommand("s", &StoreCli);
	sh.RegisterCommand("rm", &RemoveCli);

	sh.RegisterCommand("drop", &SetDropRateCli);

	sh.Run();

	return 0;
}
