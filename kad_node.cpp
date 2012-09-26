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

XList<XSockAddr> LoadBspFromFile(const char*) {
	XList<XSockAddr> bsps;
	return bsps;
}

int main(int argc, char *argv[])
{
	/************************************************
	 * General initialization
	 */
	RandInit();
	XLogManager::Get().SetDefaultLogger(new XFileLogger("log.txt"));

	/************************************************
	 * Generate/Load Local identification data
	 */
	KadRSA* rsa = new KadRSA();

	/*if (key files exist) {
		rsa = new KadRSA("key.priv", "key.pub");
		LOG(NULL, "RSA keys loaded");
	} else {
		LOG(NULL, "Generating RSA keys");
		rsa = new KadRSA();
		rsa->SaveKeys("key.priv", "key.pub");
		LOG(NULL, "RSA keys saved");
	}*/

	if (!rsa->ValidateKeys()) {
		X_FATAL("Invalid private/public keys!");
	}

	std::string pubKeyStr = KadRSA::PubEncode(rsa->GetPublicKey());
	KadId localId = KadId::FromHash(pubKeyStr.c_str(), pubKeyStr.size());

	printf ("LocalID: %s\n", (char*)localId.ToString());
	gMgr = new KadOpMgr(localId, XSockAddr("::"));
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
	sh.Run();

	/************************************************
	 * Leave the network
	 */

	return 0;
}
