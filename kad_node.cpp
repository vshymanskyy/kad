#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include "KadOpMgr.h"
#include "KadContact.h"
#include "KadRSA.h"

KadOpMgr* gMgr;

static
int Peers(int argc, char* argv[])
{
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
			bsps.Append(XSockAddr(sLine.c_str()));
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
			//XString name = bsps[it].ResolveName();
			//if (name.Length()) {
			//	outfile << name << ":" << bsps[it].Port() << endl;
			//} else {
				outfile << bsps[it].ToString() << endl;
			//}
		}
		outfile.close();
	}
}

#include <sys/stat.h>
#include <unistd.h>

bool FileExists(const char* fn) {
	struct stat sts;
	if (stat(fn, &sts) == -1)
	{
		if (errno == ENOENT) {
			return false;
		}
		X_FATAL("stat failed: %d (%s)", errno, strerror(errno));
	}
	return true;
}

bool FileDelete(const char* fn) {
	if (unlink(fn) == -1)
	{
		if (errno != ENOENT) {
			return false;
		}
	}
	return true;
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
	const char* bindaddr = "[::]:7777";
	if (argc == 2) {
		bindaddr = argv[1];
	}

	/************************************************
	 * General initialization
	 */
	XStackTrace();
	RandInit();
	//XLogManager::Get().SetDefaultLogger(new XFileLogger("log.txt"));

	/************************************************
	 * Generate/Load Local identification data
	 */
	KadRSA* rsa = NULL; //TryLoadKeys();
	if (!rsa) {
		printf("Generating new RSA keys\n");
		rsa = new KadRSA();

		if (!rsa->ValidateKeys()) {
			X_FATAL("Invalid RSA keys generated!");
		}

		rsa->SaveKeys("key.priv", "key.pub");
		printf("RSA keys saved\n");
	}

	std::string pubKeyStr = KadRSA::PubEncode(rsa->GetPublicKey());
	KadId localId = KadId::FromHash(pubKeyStr.c_str(), pubKeyStr.size());

	printf ("LocalID: %s\n", (char*)localId.ToString());
	gMgr = new KadOpMgr(localId, XSockAddr(bindaddr));
	printf ("Address: %s\n", (char*)gMgr->BindAddr().ToString());

	/************************************************
	 * Load bootstrap contacts (bsp.txt)
	 */
	XList<XSockAddr> bspLst = LoadBspFromFile("bsp.txt");

	// No bsp's in file case
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
	sh.Run();

	/************************************************
	 * Leave the network
	 */
	printf ("Leaving the network...");
	bspLst = gMgr->Leave();

	/************************************************
	 * Save bootstrap contacts (bsp.txt)
	 */
	SaveBspToFile("bsp.txt", bspLst);

	return 0;
}
