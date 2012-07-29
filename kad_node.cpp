#include "KadConfig.h"
#include "XLogUtils.h"
#include "XCmdShell.h"
#include "XTimeCounter.h"

#include "KadOpMgr.h"
#include "KadContact.h"
#include "KadRSA.h"

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
int Connect(int argc, char* argv[])
{

	return 0;
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

	KadRSA* rsa = NULL;
	if (true) {
		rsa = new KadRSA("key.priv", "key.pub");
		LOG(NULL, "RSA keys loaded");
	} else {
		LOG(NULL, "Generating RSA keys");
		rsa = new KadRSA();
		rsa->SaveKeys("key.priv", "key.pub");
		LOG(NULL, "RSA keys saved");
	}

	if (!rsa->ValidateKeys()) {
		X_FATAL("Invalid private/public keys!");
	}

	std::string pubKeyStr = KadRSA::PubEncode(rsa->GetPublicKey());
	KadId localId = KadId::FromHash(pubKeyStr.c_str(), pubKeyStr.size());

	printf ("LocalID: %s\n", (char*)localId.ToString());

	/************************************************
	 * Load bootstrap contacts (bsp.dat)
	 */

	/************************************************
	 * Join the network
	 */

	/************************************************
	 * Start the console
	 */

	XShell sh("kad");
	sh.RegisterCommand("peers", &Peers);
	sh.RegisterCommand("find", &FindNode);
	sh.RegisterCommand("connect", &Connect);
	sh.Run();

	/************************************************
	 * Leave the network
	 */

	return 0;
}
