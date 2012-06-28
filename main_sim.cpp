#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include "KadOpMgr.h"
#include "KadContact.h"

KadContact BSP = KadContact(KadNodeId::FromHash("bsp", 3), XSockAddr("127.0.0.1:4096"));

KadOpMgr* bspMgr;

static int GEN_PORT = 5000;

int GenerateCli(int argc, char* argv[])
{
	int qty = 100;
	if (argc == 2) qty = atoi(argv[1]);

	XList<KadContact> contacts;
	for (int i=0; i<qty; i++) {
		contacts.Append(KadContact(KadNodeId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", GEN_PORT++))));
	}

	if (!bspMgr) {
		bspMgr = new KadOpMgr(BSP.mId, BSP.mAddr);
		bspMgr->Init(contacts);
	}

	for (XList<KadContact>::It it=contacts.First(); it!=contacts.End(); ++it) {
		KadOpMgr* mgr = new KadOpMgr(contacts[it].mId, contacts[it].mAddr);
		mgr->Init(contacts);
	}
	return 0;
}

int SimulateCli(int argc, char* argv[])
{
	int qty = 10;
	if (argc == 2) qty = atoi(argv[1]);

	if (!bspMgr) {
		bspMgr = new KadOpMgr(BSP.mId, BSP.mAddr);
	}
	XList<XSockAddr> bspList(BSP.mAddr);
	for (int i=0; i<qty; i++) {
		KadOpMgr* mgr = new KadOpMgr(KadNodeId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", GEN_PORT++)));
		mgr->Join(bspList);
	}
	return 0;
}

KadOpMgr* mgrNode;

int JoinCli(int argc, char* argv[])
{
	if (mgrNode) return 1;
	mgrNode = new KadOpMgr(KadNodeId::Random(), XSockAddr("127.0.0.1:2048"));

	if (argc == 2) {
		mgrNode->Join(XList<XSockAddr>(XSockAddr(argv[1])));
	} else {
		mgrNode->Join(XList<XSockAddr>(XSockAddr("127.0.0.1:4096")));
	}

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
