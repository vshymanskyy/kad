#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"
#include "XFileSystem.h"

#include "KadOpMgr.h"
#include "KadContact.h"
#include "KadRSA.h"

KadOpMgr* gMgr;

static
int Peers(int argc, char* argv[])
{
	if (!gMgr) {
		return 1;
	}
	printf("Contacts: %d\n", gMgr->GetContacts().Count());
	return 0;
}

static
int FindNode(int argc, char* argv[])
{
	return 0;
}

static
int Update(int argc, char* argv[])
{
	return 0;
}

static
int Ping(int argc, char* argv[])
{
	if (!gMgr || argc != 2) {
		return 1;
	}
	gMgr->Ping(XSockAddr::Lookup(argv[1]));
	return 0;
}

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

XList<XSockAddr> LoadBspFromFile(const char* fn)
{
	XList<XSockAddr> bsps;
	string sLine = "";
	ifstream infile;
	infile.open(fn);
	if (infile.is_open()) {
		while (!infile.eof()) {
			getline(infile, sLine);
			if (sLine[0] != ';') {
				bsps.Append(XSockAddr::Lookup(sLine.c_str()));
			}
		}
		infile.close();
	}
	return bsps;
}

void SaveBspToFile(const char* fn, const XList<XSockAddr>& bsps)
{
	ofstream outfile;
	outfile.open(fn);
	if (outfile.is_open()) {
		for (XList<XSockAddr>::It it = bsps.First(); it != bsps.End(); ++it) {
			XString name = bsps[it].ResolveName();
			if (name.Length()) {
				outfile << name << ":" << bsps[it].Port() << endl;
			} else {
				outfile << bsps[it].ToString() << endl;
			}
		}
		outfile.close();
	}
}

KadRSA* TryLoadKeys() {
	if (FileExists("key.priv") && FileExists("key.pub")) {
		KadRSA* rsa = new KadRSA("key.priv", "key.pub");
		printf("RSA keys loaded\n");

		if (rsa->ValidateKeys()) {
			return rsa;
		} else {
			FileDelete("key.priv");
			FileDelete("key.pub");
			printf("Invalid RSA keys deleted\n");
			delete rsa;
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	/************************************************
	 * General initialization
	 */
	//XStackTrace();
	RandInit();
	//XLogManager::Get().SetDefaultLogger(new XFileLogger("log.txt"));

	/************************************************
	 * Handle command line
	 */
	char* bindaddr = "[::]:7777";
	bool temporary = false;
	if (argc == 2) {
		bindaddr = argv[1];
	}

	if (argc == 3 && !strcmp(argv[2], "-temp")) {
		temporary = true;
	} else if (argc == 2 && !strcmp(argv[1], "-temp")) {
		temporary = true;
		bindaddr = new char[64];
		sprintf(bindaddr, "[::]:%d", RandRange(1024, 32000));
	}

	/************************************************
	 * Generate/Load Local identification data
	 */
	KadRSA* rsa = NULL;
	if (!temporary) {
		rsa = TryLoadKeys();
		if (!rsa) {
			printf("Generating new RSA keys\n");
			rsa = new KadRSA();
			if (!rsa->ValidateKeys()) {
				X_FATAL("Invalid RSA keys generated!");
			}

			rsa->SaveKeys("key.priv", "key.pub");
			printf("RSA keys saved\n");
		}
	} else {
		printf("Generating temporary RSA keys\n");
		rsa = new KadRSA();
		if (!rsa->ValidateKeys()) {
			X_FATAL("Invalid RSA keys generated!");
		}
	}
	std::string pubKeyStr = KadRSA::PubEncode(rsa->GetPublicKey());
	KadId localId = KadId::FromHash(pubKeyStr.c_str(), pubKeyStr.size());

	printf ("LocalID: %s\n", (char*)localId.ToString());
	gMgr = new KadOpMgr(localId, XSockAddr(bindaddr));
	printf ("Address: %s\n", (char*)gMgr->BindAddr().ToString());

	/************************************************
	 * Load bootstrap contacts (bsp.txt)
	 */
	XList<XSockAddr> bspLst = LoadBspFromFile("bsp.cfg");

	// No bsp's in the file
	if (!bspLst.Count()) {
		printf ("Please enter bootstrap node address: ");
		char buff[256];
		scanf("%s", buff);
		bspLst.Append(XSockAddr(buff));
		printf ("BSP: %s", (char*)XSockAddr(buff).ToString());
	}

	/************************************************
	 * Join the network
	 */
	gMgr->Join(bspLst);

	/************************************************
	 * Start the console
	 */
	XShell sh("kad");
	sh.RegisterCommand("peers", &Peers);
	sh.RegisterCommand("find", &FindNode);
	sh.RegisterCommand("update", &Update);
	sh.RegisterCommand("ping", &Ping);
	sh.Run();

	/************************************************
	 * Leave the network
	 */
	printf ("Leaving the network...");
	bspLst = gMgr->Leave();

	/************************************************
	 * Save bootstrap contacts (bsp.txt)
	 */
	if (bspLst.Count()) {	// Not to produce empty file!
		SaveBspToFile("bsp.cfg", bspLst);
	}

	return 0;
}
