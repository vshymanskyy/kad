#include <KadConfig.h>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <msgpack.hpp>

#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

using CryptoPP::AutoSeededRandomPool;
using CryptoPP::PSS;

using namespace CryptoPP;

#define KADEMLIA_VERSION (KADEMLIA_VERSION_MAJOR*100000 + KADEMLIA_VERSION_MINOR*1000 + KADEMLIA_VERSION_PATCH)

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Files not specified\n");
		return 1;
	}
	const char* packIn = argv[1];
	const char* packSigned = argv[2];

	// Compute sha1 checksum for input file

	// Prepare msgpack header
	// [ Magic | Version | Data checksum | Random seed | Header signature ]
	msgpack::sbuffer sbuff;
	msgpack::packer<msgpack::sbuffer> pk(&sbuff);
	pk.pack_map(4);
		pk.pack_raw(3).pack_raw_body("ver", 3); pk.pack_uint64(KADEMLIA_VERSION);
		pk.pack_raw(4).pack_raw_body("seed", 4); pk.pack_raw(6).pack_raw_body("random", 6);
		pk.pack_raw(4).pack_raw_body("size", 4); pk.pack_uint32(4096);
		pk.pack_raw(4).pack_raw_body("sha1", 4); pk.pack_raw(16).pack_raw_body("ccccccccccccccccccccccccc", 16);
	AutoSeededRandomPool rng;

	{ // Generate header signature

		ByteQueue queue;
		FileSource file("master_key.priv", true);
		file.TransferTo(queue);
		queue.MessageEnd();

		RSA::PrivateKey privateKey;
		privateKey.BERDecodePrivateKey(queue, false, queue.MaxRetrievable());
		if (!privateKey.Validate(rng, 3)) {
			printf("RSA Private key not validated!\n");
			return 1;
		}

		RSASS<PSS, SHA1>::Signer signer(privateKey);
		size_t length = signer.MaxSignatureLength();
		SecByteBlock signature(length);
		signer.SignMessage(rng, (const byte*)sbuff.data(), sbuff.size(), signature);

		if (signature.size() != 4096/8) {
			printf("RSA Signature size mismatch: %ld!\n", signature.size());
			return 1;
		}
		// Sign header
		pk.pack_raw(signature.size()).pack_raw_body((char*)signature.BytePtr(), signature.size());
	}

	/*{	// Print header
		msgpack::unpacker pac;
		pac.reserve_buffer(sbuff.size());
		memcpy(pac.buffer(), sbuff.data(), sbuff.size());
		pac.buffer_consumed(sbuff.size());
		msgpack::unpacked msg;
		while (pac.next(&msg)) {
			msgpack_object_print(stdout, msg.get());
			puts("");
		}
	}*/

	// Write output file
	ofstream outfile;
	outfile.open(packSigned, iostream::binary);
	if (outfile.is_open()) {
		// Write header
		outfile.write(sbuff.data(), sbuff.size());
		// Write archive

		outfile.close();
		printf("Finished.\n");
	} else {
		printf("Could not open output file\n");
		return 1;
	}

	{	// Verify
		ByteQueue queue;
		FileSource file("master_key.pub", true);
		file.TransferTo(queue);
		queue.MessageEnd();

		RSA::PublicKey publicKey;
		publicKey.BERDecodePublicKey(queue, false, queue.MaxRetrievable());
		if (!publicKey.Validate(rng, 3)) {
			printf("RSA Public key not validated!\n");
			return 1;
		}
		// Verifier object
		RSASS<PSS, SHA1>::Verifier verifier(publicKey);

		/*{	// Verify header
			msgpack::unpacker pac;
			pac.reserve_buffer(sbuff.size());
			memcpy(pac.buffer(), sbuff.data(), sbuff.size());
			pac.buffer_consumed(sbuff.size());
			msgpack::unpacked msg;
			size_t headerSize = 50; //pac.message_size();
			if (pac.next(&msg) && msg.get().type == msgpack::type::MAP) {
				// header
				if (pac.next(&msg) && msg.get().type == msgpack::type::RAW) {
					// signature

					if (verifier.VerifyMessage((const byte*)sbuff.data(), headerSize, (const byte*)msg.get().via.raw.ptr, msg.get().via.raw.size)) {
						printf("Header verification OK.\n");
					} else {
						printf("Header verification FAILED!\n");
						return 1;
					}
				}
			}
		}*/

        msgpack_unpacked msg;
        size_t offset = 0;
        msgpack_unpacked_init(&msg);
        if (msgpack_unpack_next(&msg, sbuff.data(), sbuff.size(), &offset)) {
			// header
        	size_t headerSize = offset;
            if (msgpack_unpack_next(&msg, sbuff.data(), sbuff.size(), &offset)) {
				// signature

				if (verifier.VerifyMessage((const byte*)sbuff.data(), headerSize, (const byte*)msg.data.via.raw.ptr, msg.data.via.raw.size)) {
					printf("Header verification OK.\n");
				} else {
					printf("Header verification FAILED!\n");
					return 1;
				}
            }
        }


	}
	return 0;
}
