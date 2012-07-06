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

static
int GenBspCli(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Generates bootstrap contacts.\n  gen <qty> <port=%d>\n", bspBasePort);
		return 1;
	}

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	bspBasePort = (argc >= 3) ? atoi(argv[2]) : bspBasePort;

	XList<KadContact>::It it=bspList.Last();
	for (int i=0; i<qty; i++) {
		bspList.Append(KadContact(KadId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", bspBasePort++))));
	}

	for (++it; it!=bspList.End(); ++it) {
		KadOpMgr* mgr = new KadOpMgr(bspList[it].mId, bspList[it].mAddr);
		mgr->Init(bspList);
		//mgr->DumpTable();
	}
	return 0;
}

static
int AddBspCli(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Adds bootstrap contacts.\n  add <qty> <port=%d>\n", bspBasePort);
		return 1;
	}

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	bspBasePort = (argc >= 3) ? atoi(argv[2]) : bspBasePort;

	XList<KadContact>::It it=bspList.Last();
	for (int i=0; i<qty; i++) {
		bspList.Append(KadContact(KadId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", bspBasePort++))));
	}

	return 0;
}

static
int SimulateCli(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Joins some contacts.\n  sim <qty> <port=%d>\n", simBasePort);
		return 1;
	}

	if (bspList.Count() == 0) {
		printf("Need to generate some bootstrap contacts first.\n");
		return 1;
	}

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	simBasePort = (argc >= 3) ? atoi(argv[2]) : simBasePort;

	XList<XSockAddr> bspAddr;
	for (XList<KadContact>::It it=bspList.First(); it!=bspList.End(); ++it) {
		bspAddr.Append(bspList[it].mAddr);
	}
	// Join simulation nodes
	for (int i=0; i<qty; i++) {
		KadOpMgr* mgr = new KadOpMgr(KadId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", simBasePort++)));
		mgr->Join(bspAddr);
	}

	return 0;
}

KadOpMgr* mgrNode;

static
int JoinCli(int argc, char* argv[])
{
	if (mgrNode) return 1;

	if (argc < 2) {
		printf("Joins new node to 1 bsp.\n  j <addr>\n");
		return 1;
	}

	mgrNode = new KadOpMgr(KadId::Random(), XSockAddr("0.0.0.0"));
	XList<XSockAddr> bspList;
	bspList.Append(XSockAddr(argv[1]));
	mgrNode->Join(bspList);

	return 0;
}

static
int LeaveCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	mgrNode->Leave();
	delete mgrNode;
	mgrNode = NULL;
	return 0;
}

static
int PrintRt(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	mgrNode->DumpTable();
	return 0;
}


static
int FindNodeCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

static
int FindValueCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

static
int StoreCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

static
int RemoveCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	return 0;
}

static
int SetDropRateCli(int argc, char* argv[])
{
	if (argc <= 1) {
		printf("Set global drop rate.\n  drop <rx=%d> <tx=%d>\n", KadOpMgr::DROP_RATE_RX, KadOpMgr::DROP_RATE_TX);
		return 1;
	}
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

	XLogManager::Get().SetDefaultLogger(new XFileLogger("log.txt"));

	XShell sh("sim");

	sh.RegisterCommand("sim", &SimulateCli);
	sh.RegisterCommand("gen", &GenBspCli);
	sh.RegisterCommand("add", &AddBspCli);

	sh.RegisterCommand("j", &JoinCli);
	sh.RegisterCommand("l", &LeaveCli);

	sh.RegisterCommand("rt", &PrintRt);

	//sh.RegisterCommand("fn", &FindNodeCli);
	//sh.RegisterCommand("fv", &FindValueCli);

	//sh.RegisterCommand("s", &StoreCli);
	//sh.RegisterCommand("rm", &RemoveCli);

	sh.RegisterCommand("drop", &SetDropRateCli);

	sh.Run();

	return 0;
}
