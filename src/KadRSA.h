#ifndef KAD_RSA_H_
#define KAD_RSA_H_

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

#include <XLog.h>

using namespace CryptoPP;
using namespace std;

class KadRSA {
public:
	typedef RSA::PrivateKey PrivateKey;
	typedef RSA::PublicKey PublicKey;
	enum { KEY_SIZE = 4096 };

public:

	KadRSA() {
		CryptoPP::InvertibleRSAFunction parameters;
		try {
			parameters.GenerateRandomWithKeySize(rng(), KEY_SIZE);
		} catch(const CryptoPP::Exception &e) {
			LOG(NULL, "Failed generating keypair: " << e.what());
			return;
		}
		mPrivateKey = KadRSA::PrivateKey(parameters);
		mPublicKey = KadRSA::PublicKey(parameters);
	}

	KadRSA(const char* priv_fn, const char* pub_fn) {
		DecodeFromFile(priv_fn, mPrivateKey);
		DecodeFromFile(pub_fn, mPublicKey);

	}

	bool ValidateKeys() {
		bool result = true;
		if (!ValidateKey(mPrivateKey, 3)) {
			LOG_CRIT(NULL, "Private key not valid");
			result = false;
		}

		if (!ValidateKey(mPublicKey, 3)) {
			LOG_CRIT(NULL, "Public key not valid");
			result = false;
		}
		return result;
	}

	void SaveKeys(const char* priv_fn, const char* pub_fn) {
		EncodeToFile(mPrivateKey, priv_fn);
		EncodeToFile(mPublicKey, pub_fn);
	}

	static bool ValidateKey(const PrivateKey &private_key, unsigned level = 3) {
		  return private_key.Validate(rng(), level);
	}

	static bool ValidateKey(const PublicKey &public_key, unsigned level = 3) {
		  return public_key.Validate(rng(), level);
	}


	/**
	 * Public Key <=> String
	 */

	static std::string PubEncode(const RSA::PublicKey& key)
	{
		ByteQueue queue;
		key.DEREncodePublicKey(queue);
		std::string result;
		CryptoPP::StringSink name(result);
		queue.CopyTo(name);
		name.MessageEnd();
		return result;
	}

	static void PubDecode(const std::string str, RSA::PublicKey& key)
	{
		ByteQueue queue;
		CryptoPP::StringSource file(str, true);
		file.TransferTo(queue);
		queue.MessageEnd();
		key.BERDecodePublicKey(queue, false, queue.MaxRetrievable());
	}

	const PublicKey& GetPublicKey() const {
		return mPublicKey;
	}

	const PublicKey& GetPublicKey() {
		return mPublicKey;
	}

private:
	static CryptoPP::RandomNumberGenerator& rng() {
		static AutoSeededRandomPool res;
		return res;
	}

	/**
	 * Private Key <=> File
	 */

	static void EncodeToFile(const RSA::PrivateKey& key, const char* filename)
	{
		ByteQueue queue;
		key.DEREncodePrivateKey(queue);
		FileSink file(filename);
		queue.CopyTo(file);
		file.MessageEnd();
	}

	static void DecodeFromFile(const char* filename, RSA::PrivateKey& key)
	{
		ByteQueue queue;
		FileSource file(filename, true);
		file.TransferTo(queue);
		queue.MessageEnd();
		key.BERDecodePrivateKey(queue, false, queue.MaxRetrievable());
	}

	/**
	 * Public Key <=> File
	 */

	static void EncodeToFile(const RSA::PublicKey& key, const char* filename)
	{
		ByteQueue queue;
		key.DEREncodePublicKey(queue);
		FileSink file(filename);
		queue.CopyTo(file);
		file.MessageEnd();
	}


	static void DecodeFromFile(const char* filename, RSA::PublicKey& key)
	{
		ByteQueue queue;
		FileSource file(filename, true);
		file.TransferTo(queue);
		queue.MessageEnd();
		key.BERDecodePublicKey(queue, false, queue.MaxRetrievable());
	}

private:
	PrivateKey mPrivateKey;
	PublicKey mPublicKey;
};


#endif /* KAD_RSA_H_ */

