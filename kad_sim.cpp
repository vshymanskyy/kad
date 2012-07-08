#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include "KadOpMgr.h"
#include "KadContact.h"

//template <typename T>
//const T& ()

int simBasePort = 3000;

XList<KadOpMgr*> allNodes;

static
int SaveDot(int argc, char* argv[])
{
	if (argc < 2) {
		return 1;
	}
	if (FILE* f=fopen(argv[1],"w")) {
		fprintf(f,
			"digraph Kad {\n"
			"  graph [overlap=false, overlap_scaling=2, sep=\"+20\", splines=true];\n"
			"  node [shape=record, height=.1];\n"
		);
		for (XList<KadOpMgr*>::It it=allNodes.First(); it!=allNodes.End(); ++it) {
			XList<const KadContact*> contacts = allNodes[it]->GetContacts();
			for (XList<const KadContact*>::It it2=contacts.First(); it2!=contacts.End(); ++it2) {
				fprintf(f, "  \"%s\" -> \"%s\";\n", (char*)(allNodes[it]->LocalId().ToString()), (char*)(contacts[it2]->mId.ToString()));
			}
		}
		fprintf(f, "}\n");
		fclose(f);
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

	int qty = (argc >= 2) ? atoi(argv[1]) : 10;
	simBasePort = (argc >= 3) ? atoi(argv[2]) : simBasePort;


	// Join simulation nodes
	for (int i=0; i<qty; i++) {
		XList<XSockAddr> bspAddr;

		if (allNodes.Count()) {
			int bsp = RandRange(0, allNodes.Count()-1);
			for (XList<KadOpMgr*>::It it=allNodes.First(); it!=allNodes.End(); ++it) {
				if (!bsp--) {
					bspAddr.Append(allNodes[it]->BindAddr());
				}
			}
		}

		KadOpMgr* mgr = new KadOpMgr(KadId::Random(), XSockAddr(XString::Format("127.0.0.1:%d", simBasePort++)));
		mgr->Join(bspAddr);
		allNodes.Append(mgr);
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
	allNodes.Append(mgrNode);

	return 0;
}

static
int LeaveCli(int argc, char* argv[])
{
	if (!mgrNode) return 1;

	//allNodes.Remove(allNodes.FindAfter(allNodes.First(),mgrNode));
	//mgrNode->Leave();
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

	sh.RegisterCommand("j", &JoinCli);
	sh.RegisterCommand("l", &LeaveCli);

	sh.RegisterCommand("rt", &PrintRt);
	sh.RegisterCommand("dot", &SaveDot);

	//sh.RegisterCommand("fn", &FindNodeCli);
	//sh.RegisterCommand("fv", &FindValueCli);

	//sh.RegisterCommand("s", &StoreCli);
	//sh.RegisterCommand("rm", &RemoveCli);

	sh.RegisterCommand("drop", &SetDropRateCli);

	sh.Run();

	return 0;
}
