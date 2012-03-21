#ifndef KADSOCKET_H_
#define KADSOCKET_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <XPlat.h>

#if defined TARGET_OS_UNIX
#	include <unistd.h>
#	include <sys/socket.h>
#	include <sys/un.h>
#	include <netinet/in.h>
#elif defined TARGET_OS_WINDOWS
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#endif

#include "XHelpers.h"
#include "XLog.h"

struct KadAddrPacked
{
	enum Type {
		IPv4,
		IPv6
	};
	uint8_t mType;
	uint8_t mAddr[16];
	uint16_t mPort;
} GCC_SPECIFIC(__attribute__((packed)));

union KadAddr
{
	struct sockaddr sa;
	struct sockaddr_in sa_in;
	struct sockaddr_in6 sa_in6;
	//struct sockaddr_storage sa_stor;

	static KadAddr Random() {
		KadAddr addr;
		MemRand(&addr, sizeof(KadAddr));
		addr.sa.sa_family = AF_INET;
		return addr;
	}

	static KadAddr FromIPv4(uint32_t addr, uint16_t port) {
		KadAddr res;
		res.sa_in.sin_family = AF_INET;
		res.sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		res.sa_in.sin_port = htons(port);
		return res;
	}

	static KadAddr FromIPv6(const in6_addr& addr, uint16_t port) {
		KadAddr res;
		res.sa_in6.sin6_family = AF_INET6;
		memcpy(&res.sa_in6.sin6_addr, &addr, sizeof(in6_addr));
		res.sa_in6.sin6_port = htons(port);
		return res;
	}

	KadAddr() {
	}

	uint16_t Port() const {
		switch(sa.sa_family) {
		case AF_INET:
			return ntohs(sa_in.sin_port);
		case AF_INET6:
			return ntohs(sa_in6.sin6_port);
		default:
			assert(false);
			return 0;
		}
	}

	void Pack(KadAddrPacked* addr) const {
		switch(sa.sa_family) {
		case AF_INET:
			addr->mType = KadAddrPacked::IPv4;
			break;
		case AF_INET6:
			addr->mType = KadAddrPacked::IPv6;
			break;
		default:
			assert(false);
			break;
		}
	}

	static KadAddr Unpack(const KadAddrPacked& addr) {
		KadAddr result;
		assert(sizeof(result.sa_in6.sin6_addr) == sizeof(addr.mAddr));
		switch(addr.mType) {
		case KadAddrPacked::IPv4:
			result.sa.sa_family = AF_INET;
			result.sa_in.sin_port = addr.mPort;
			memcpy(&result.sa_in.sin_addr, &addr.mAddr, sizeof(result.sa_in.sin_addr));
			break;
		case KadAddrPacked::IPv6:
			result.sa.sa_family = AF_INET6;
			result.sa_in6.sin6_port = addr.mPort;
			memcpy(&result.sa_in6.sin6_addr, &addr.mAddr, sizeof(result.sa_in6.sin6_addr));
			break;
		default:
			assert(false);
			break;
		}
		return result;
	}
};

class KadSocket
{

public:
	KadSocket();
	virtual ~KadSocket();

	void Bind(uint16_t port = 0);

	ssize_t SendTo(const void* data, size_t len, const KadAddr& addr);

	template <class T>
	bool SendStructTo(const T& obj, const KadAddr& addr) {
		return SendTo(&obj, sizeof(obj), addr) == sizeof(obj);
	}

	ssize_t RecvFrom(void* data, size_t len, KadAddr* addr);

	//int GetAvaliableBytes() const;

	void GetBindAddr(KadAddr* addr) const;

	void PrintInterfaces();

private:
	int mSocket;
};

const XLog::Stream& operator <<(const XLog::Stream& str, const KadAddr& addr);

#endif /* KADSOCKET_H_ */
