#include "KadConfig.h"
#include "Kademlia.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

const int NODES_QTY = 10;
const int NODES_PORT = 3010;

KadContact cnt[NODES_QTY];

int GenerateFiles(int argc, char *argv[])
{
	mkdir("nodes", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	for (int i = 0; i < NODES_QTY; i++) {
		cnt[i].mId = KadNodeId::Random();
		cnt[i].mAddresses.Append(KadAddr::FromIPv4(INADDR_LOOPBACK, NODES_PORT+i));
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

int Simulate(int argc, char *argv[])
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

int TestRoutingTable(int argc, char *argv[])
{
	Kademlia kad(KadNodeId::Random(), 0);

	for (int i = 0; i < 10000; i++) {
		kad.Seen(Contact::Random());
	}

	kad.DumpTable();

	return 0;
}

Kademlia* node;

int NodeJoin(int argc, char *argv[])
{
	node = Kademlia::Load("nodes/kad_3010.bin", 3010);
	node->Join();
	return 0;
}

int NodeLeave(int argc, char *argv[])
{
	node->Leave();
	return 0;
}

int NodeFind(int argc, char *argv[])
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

int StartNode(int argc, char *argv[])
{
	XShell sh("node");

	sh.RegisterCommand("join", &NodeJoin);
	sh.RegisterCommand("leave", &NodeLeave);
	sh.RegisterCommand("find", &NodeFind);
	sh.Run();
	return 0;
}

int main()
{
	srand(time(NULL));

	XShell sh("kad");
	sh.RegisterCommand("sim", &Simulate);
	sh.RegisterCommand("gen", &GenerateFiles);
	sh.RegisterCommand("test_rt", &TestRoutingTable);

	sh.RegisterCommand("node", &StartNode);

	sh.Run();

	return 0;
}
