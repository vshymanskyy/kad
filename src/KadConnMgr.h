#ifndef KADCONNMGR_H_
#define KADCONNMGR_H_

#include <net/XSockAddr.h>
#include <udt/udt.h>
#include <XThread.h>

class KadConnMgr : XThread
{

private:
	virtual int Run()
	{
		std::set<UDTSOCKET> readfds;
		char buff[2048];
		while (true) {
			if (0 == UDT::epoll_wait(mEpoll, &readfds, NULL, 10)) continue;
			if (IsStopping()) break;

			for ( std::set<UDTSOCKET>::iterator it = readfds.begin(); it != readfds.end(); ++it ) {

				if (*it == mListener) {
					UDTSOCKET client;
					XSockAddr addr;
					int addrlen = sizeof(addr);
					if (UDT::INVALID_SOCK != (client = UDT::accept(mListener, addr.SA(), &addrlen)) ) {
						LOG(mLog, "Connection from: " << addr.ToString());
						UDT::epoll_add_usock(mEpoll, client);
					} else {
						LOG(mLog, "UDT::accept(): " << UDT::getlasterror().getErrorMessage());
					}
				} else {
					int len = UDT::recvmsg(*it, buff, sizeof(buff));
					LOG(mLog, "Received " << len << "bytes");
				}
			}
		}
		return 0;
	}

public:
	KadConnMgr(const XSockAddr& addr)
	{
		UDT::startup();
		mEpoll = UDT::epoll_create();

		if (UDT::INVALID_SOCK != (mListener = UDT::socket(addr.SA()->sa_family, SOCK_DGRAM, 0))) {
			if (UDT::ERROR != UDT::bind(mListener, addr.SA(), addr.SA_LEN())) {
			   if (UDT::ERROR != UDT::listen(mListener, 100)) {
				   UDT::epoll_add_usock(mEpoll, mListener);
			   } else {
				   LOG(mLog, "listen: " << UDT::getlasterror().getErrorMessage());
					UDT::close(mListener);
					mListener = UDT::INVALID_SOCK;
					return;
			   }
			} else {
				LOG(mLog, "bind: " << UDT::getlasterror().getErrorMessage());
				UDT::close(mListener);
				mListener = UDT::INVALID_SOCK;
				return;
			}
		} else {
			LOG(mLog, "socket: " << UDT::getlasterror().getErrorMessage());
			return;
		}

		Start();
	}

	~KadConnMgr() {
		Stop();
		UDT::epoll_release(mEpoll);
	}

	UDTSOCKET Connect(const XSockAddr& addr) {
		UDTSOCKET sock;
		if (UDT::INVALID_SOCK != (sock = UDT::socket(addr.SA()->sa_family, SOCK_DGRAM, 0))) {

			if (UDT::ERROR != UDT::connect(sock, addr.SA(), addr.SA_LEN())) {
				UDT::epoll_add_usock(mEpoll, sock);
				return sock;
			} else {
				LOG(mLog, "connect: " << UDT::getlasterror().getErrorMessage());
			}

			UDT::close(sock);
		} else {
			LOG(mLog, "socket: " << UDT::getlasterror().getErrorMessage());
		}
		return UDT::INVALID_SOCK;
	}

	UDTSOCKET	mListener;
	int			mEpoll;
};

#endif /* KADCONNMGR_H_ */
