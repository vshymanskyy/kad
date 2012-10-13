#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cassert>
#include <cstdarg>
#include <ctime>

#include <msgpack.hpp>

#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;
using namespace std;

AutoSeededRandomPool rng;

inline
uint64_t FileSize(const char* fn)
{
	if (FILE* f = fopen(fn, "rb")) {
		fseek(f, 0, SEEK_END);
		uint64_t result = ftello64(f);
		fclose(f);
		return result;
	}
	return 0;
}

static void Fatal(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(1);
}

static void Info(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fprintf(stdout, "\n");
}

static bool DecodeFromFile(const char* filename, RSA::PublicKey& key)
{
	try {
		ByteQueue queue;
		FileSource file(filename, true);
		file.TransferTo(queue);
		queue.MessageEnd();
		key.BERDecodePublicKey(queue, false, queue.MaxRetrievable());
		return key.Validate(rng, 3);
	} catch (...) {
		return false;
	}
}

static bool DecodeFromFile(const char* filename, RSA::PrivateKey& key)
{
	try {
		ByteQueue queue;
		FileSource file(filename, true);
		file.TransferTo(queue);
		queue.MessageEnd();
		key.BERDecodePrivateKey(queue, false, queue.MaxRetrievable());
		return key.Validate(rng, 3);
	} catch (...) {
		return false;
	}
}

bool VerifyFile(const char* fnIn, const char* fnOut)
{
	try {
		RSA::PublicKey publicKey;
		if (!DecodeFromFile("master_key.pub", publicKey)) {
			Fatal("RSA public key not valid!");
		}
		// Verifier object
		RSASS<PSS, SHA256>::Verifier verifier(publicKey);

		size_t fileEnd = FileSize(fnIn);
		std::ifstream infile(fnIn, std::ios::binary);
		if (!infile.is_open()) {
			Fatal("Cannot open file: %s", fnIn);
		}

		char* descrBuff = new char[fileEnd];
		infile.read(descrBuff, fileEnd);

		size_t offset = 0;
		msgpack::unpacked descrPack;
		msgpack::unpacked signPack;

		msgpack::unpack(&descrPack, descrBuff, fileEnd, &offset);
		size_t descrEnd = offset;
		msgpack::unpack(&signPack, descrBuff, fileEnd, &offset);
		size_t signEnd = offset;

		if (descrEnd < 128 || signEnd != descrEnd+515 ||
			descrPack.get().type != msgpack::type::MAP ||
			signPack.get().type != msgpack::type::RAW)
		{
			Fatal("Invalid format");
		}

		// signature
		if (!verifier.VerifyMessage((const byte*) descrBuff, descrEnd,
				(const byte*) signPack.get().via.raw.ptr, signPack.get().via.raw.size))
		{
			Fatal("Descriptor verification FAIL!");
		}

		string packOutHash;
		CryptoPP::SHA256 hash;
		CryptoPP::ArraySource((const byte*)descrBuff+signEnd, fileEnd-signEnd, true,
				new CryptoPP::HashFilter(hash,
						new CryptoPP::HexEncoder(
								new CryptoPP::StringSink(packOutHash), true)));

		map<string, msgpack::object> descr;
		descrPack.get().convert(&descr);
		if (descr["sha256"].as<string>() != packOutHash) {
			Fatal("Hash verification FAIL!");
		}

		msgpack_object_print(stdout, descrPack.get());
		Info("\nVerification OK");

		if (fnOut) {
			ofstream outfile(fnOut, std::ios::binary);
			if (!outfile.is_open()) {
				Fatal("Can not open file: %s", fnOut);
			}
			outfile.write(descrBuff+signEnd, fileEnd-signEnd);
			Info("Verified file written to: %s", fnOut);
		}
		delete[] descrBuff;

	} catch (...) {
		Fatal("Unknown error");
	}
	return true;
}

bool SignFile(const char* fnIn, const char* fnOut)
{
	// Prepare msgpack header
	msgpack::sbuffer sbuff;
	msgpack::packer<msgpack::sbuffer> pk(sbuff);

	{
		// Version string
		std::ostringstream version;
		version << "0.1.0";

		time_t result = time(NULL);
	    char gmTime[64];
	    size_t gmLen = strftime(gmTime, 64, "%Z %c", std::gmtime(&result));


		// Compute sha256 checksum for input file
		string packInHash;
		CryptoPP::SHA256 hash;
		CryptoPP::FileSource(fnIn, true,
				new CryptoPP::HashFilter(hash,
						new CryptoPP::HexEncoder(
								new CryptoPP::StringSink(packInHash), true)));

		// Generate random seed
		string seed;
		CryptoPP::RandomNumberSource(rng, 16, true,
				new CryptoPP::HexEncoder(new CryptoPP::StringSink(seed), true));

		// MsgPack!
		pk.pack_map(5);
		{
			pk.pack(string("ver"));			pk.pack(version.str());
			pk.pack(string("timestamp"));	pk.pack(string(gmTime));
			pk.pack(string("seed"));		pk.pack(seed);
			pk.pack(string("size"));		pk.pack(FileSize(fnIn));
			pk.pack(string("sha256"));		pk.pack(packInHash);
		}
	}

	{ // Generate header signature
		RSA::PrivateKey privateKey;
		if (!DecodeFromFile("master_key.priv", privateKey)) {
			Fatal("RSA private key not valid!");
		}
		RSASS<PSS, SHA256>::Signer signer(privateKey);
		SecByteBlock signature(signer.MaxSignatureLength());
		signer.SignMessage(rng, (const byte*) sbuff.data(), sbuff.size(), signature);

		if (signature.size() != 512) {
			Fatal("RSA signature size mismatch: %ld!", signature.size());
		}

		pk.pack_raw(signature.SizeInBytes()).pack_raw_body(
				(char*) signature.BytePtr(), signature.SizeInBytes());
	}

	// Write output file
	ofstream outfile(fnOut, std::ios::binary);
	ifstream infile(fnIn, std::ios::binary);

	if (!outfile.is_open()) {
		Fatal("Can not open file: %s", fnOut);
	}

	if (!infile.is_open()) {
		Fatal("Can not open file: %s", fnIn);
	}

	// Write descriptor
	outfile.write(sbuff.data(), sbuff.size());
	// Append data
	outfile << infile.rdbuf();

	Info("Signed file written to: %s", fnOut);

	return true;
}

int main(int argc, char *argv[]) {
	if (argc == 4 && !strcmp("-s", argv[1])) {
		// Sign
		Info("Signing: %s", argv[2]);
		if (SignFile(argv[2], argv[3])) {
			VerifyFile(argv[3], NULL);
		}

	} else if (argc == 3 && !strcmp("-v", argv[1])) {
		// Verify
		Info("Verifying: %s", argv[2]);
		VerifyFile(argv[2], NULL);

	} else if (argc == 4 && !strcmp("-u", argv[1])) {
		// Unpack
		Info("Unpacking: %s", argv[2]);
		VerifyFile(argv[2], argv[3]);

	} else {
		Fatal("Invalid arguments");
	}
	return 0;
}
