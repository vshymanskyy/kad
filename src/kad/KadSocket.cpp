#include "KadSocket.h"
#include <string.h>
#include <stdlib.h>


#if defined TARGET_OS_UNIX
#	include <netdb.h>
#	include <ifaddrs.h>
#	include <sys/ioctl.h>
#	define closesocket close
#elif defined TARGET_OS_WINDOWS
#	include <WinSock2.h>
#	pragma comment(lib,"Ws2_32.lib")
#endif

KadSocket::KadSocket()
		: mSocket(-1)
{

}

KadSocket::~KadSocket()
{
	if (mSocket >= 0) {
		closesocket(mSocket);
	}
}

void KadSocket::Bind(uint16_t port)
{
	addrinfo hints;
	memset(&hints, 0, sizeof hints);
	// AF_UNSPEC - auto IPv4/IPv6
	// AF_INET   - force IPv4
	// AF_INET6  - force IPv6
	hints.ai_family = AF_UNSPEC;
	// UDP
	hints.ai_socktype = SOCK_DGRAM;
	// Use INADDR_ANY or in6addr_any
	hints.ai_flags = AI_PASSIVE;

	addrinfo* servinfo;
	char portStr[8];
	snprintf(portStr, 8, "%d", port);
	int rv = getaddrinfo(NULL, portStr, &hints, &servinfo);
	if (rv != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	// loop through all the results and bind to the first we can
	for (addrinfo* p = servinfo; p != NULL; p = p->ai_next) {
		if ((mSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(mSocket, p->ai_addr, p->ai_addrlen) == -1) {
			closesocket(mSocket);
			perror("listener: bind");
			continue;
		} else {
			break;
		}
	}
	freeaddrinfo(servinfo);
}

void KadSocket::GetBindAddr(KadAddr* addr) const
{
	socklen_t addrLen = sizeof(KadAddr);
	getsockname(mSocket, &(addr->sa), &addrLen);
}

/*int KadSocket::GetAvaliableBytes() const
{
	int result = 0;
	ioctl(mSocket, FIONREAD, &result);
	return result;
}*/

ssize_t KadSocket::SendTo(const void* data, size_t len, const KadAddr& addr)
{
	return sendto(mSocket, (char*)data, len, 0, &(addr.sa), sizeof(KadAddr));
}

ssize_t KadSocket::RecvFrom(void* data, size_t len, KadAddr* addr)
{
	socklen_t fromLen = sizeof(KadAddr);
	return recvfrom(mSocket, (char*)data, len, 0, &(addr->sa), &fromLen);
}

const XLog::Stream& operator <<(const XLog::Stream& str, const KadAddr& addr)
{
	char hostName[NI_MAXHOST];
	char servName[NI_MAXSERV];
	getnameinfo(&addr.sa, sizeof(KadAddr), hostName, NI_MAXHOST, servName, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

	return str << hostName << ":" << servName;
}


void KadSocket::PrintInterfaces()
{
	/*ifaddrs* iflist;

	if (getifaddrs(&iflist) < 0)
		perror("getifaddrs:");

	for (ifaddrs* i = iflist; i != NULL; i = i->ifa_next) {
		printf("%s ", i->ifa_name);
		//KadAddr(i->ifa_addr, i->ifa)
	}

	freeifaddrs(iflist);*/
}
