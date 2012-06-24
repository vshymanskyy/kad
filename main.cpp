#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#if 0

	const int NODES_QTY = 10;
	const int NODES_PORT = 3010;

	KadContact cnt[NODES_QTY];

	int GenerateFiles(int , char **)
	{
		mkdir("nodes", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		for (int i = 0; i < NODES_QTY; i++) {
			cnt[i].mId = KadNodeId::Random();
			cnt[i].mAddresses.Append(XSockAddr("::1"));
		}

		for (int i = 0; i < NODES_QTY; i++) {
			Kademlia kad(cnt[i].mId, NODES_PORT+i);
			for (int j = 0; j < NODES_QTY; j++) {
				kad.Seen(cnt[j]);
			}
			char fn[32];
			sprintf(fn, "nodes/kad_%d.bin", NODES_PORT+i);
			kad.Save(fn);
		}
		return 0;
	}

	int Simulate(int, char**)
	{
		Kademlia** nodes = new Kademlia*[NODES_QTY];
		for (int i = 0; i < NODES_QTY; i++) {
			char fn[32];
			sprintf(fn, "nodes/kad_%d.bin", NODES_PORT+i);
			nodes[i] = Kademlia::Load(fn, NODES_PORT+i);
			nodes[i]->DumpTable();
		}

		//for (int i = 0; i < NODES_QTY; i++) {
		//	nodes[i]->Start();
		//}

		getchar();

		for (int i = 0; i < NODES_QTY; i++) {
			delete  nodes[i];
		}
		delete[] nodes;

		return 0;
	}

	int TestRoutingTable(int, char**)
	{
		Kademlia kad(KadNodeId::Random(), 0);

		for (int i = 0; i < 10000; i++) {
			kad.Seen(Contact::Random());
		}

		kad.DumpTable();

		return 0;
	}

	Kademlia* node;

	int NodeJoin(int, char**)
	{
		node = Kademlia::Load("nodes/kad_3010.bin", 3010);
		node->Join();
		return 0;
	}

	int NodeLeave(int, char**)
	{
		node->Leave();
		return 0;
	}

	int NodeFind(int, char**)
	{
		Contact cnt;
		KadNodeId id = KadNodeId::Random();
		LOG(NULL, "Searching for: " << id);
		if(node->FindNode(id, &cnt)) {
			LOG(NULL, "Found: " << cnt);
		} else {
			LOG(NULL, "Not found");
		}
		return 0;
	}

	int StartNode(int, char**)
	{
		XShell sh("node");

		sh.RegisterCommand("join", &NodeJoin);
		sh.RegisterCommand("leave", &NodeLeave);
		sh.RegisterCommand("find", &NodeFind);
		sh.Run();
		return 0;
	}

//KadTransactionMgr mgr1(KadNodeId::Random(), 2001);
//KadTransactionMgr mgr2(KadNodeId::Random(), 2002);

//int TestPing(int argc, const char *argv[])
//{
//	mgr1.Ping(KadAddr::FromIPv4(INADDR_LOOPBACK, 2002));
//	return 0;
//}



	int main(int argc, char **argv)
	{
		srand(time(NULL));

		XShell sh("kad");
		sh.RegisterCommand("sim", &Simulate);
		sh.RegisterCommand("gen", &GenerateFiles);
		sh.RegisterCommand("test_rt", &TestRoutingTable);

		sh.RegisterCommand("node", &StartNode);
		//sh.RegisterCommand("ping", &TestPing);

		sh.Run();

		return 0;
	}

#elif PERF_TEST

	#include <cxxtest/RealDescriptions.h>
	#include <cxxtest/TestMain.h>
	#include <cxxtest/ErrorFormatter.h>

	int main(int argc, char **argv)
	{
		CxxTest::ErrorFormatter tmp(new CxxTest::OutputStream(), "", "" );
		if (argc < 2) {
			printf("Please specify the test suite:\n");
			const char* arg[] = { argv[0], "--help-tests" };
			return CxxTest::Main<CxxTest::ErrorFormatter>( tmp, 2, (char**)arg );
		}

		XTimeCounter t;
		for (int i=0; i < 1000; i++) {
			CxxTest::Main<CxxTest::ErrorFormatter>( tmp, argc, argv );
		}
		printf("Test finished in %d ms\n", t.Elapsed());
		return 0;
	}

#else

	#include <cxxtest/RealDescriptions.h>
	#include <cxxtest/TestMain.h>
	#include <cxxtest/StdioPrinter.h>

	int main(int argc, char **argv)
	{
		RandInit();

		CxxTest::StdioPrinter tmp;
		XTimeCounter t;
		int status = CxxTest::Main<CxxTest::StdioPrinter>( tmp, argc, argv );
		printf("Test finished in %d ms\n", t.Elapsed());
		return status;
	}

#endif
