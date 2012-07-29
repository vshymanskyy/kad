#ifndef KAD_OVER_IP_H_
#define KAD_OVER_IP_H_

#include <XThread.h>
#include <XDelegate.h>

#include <net/XSockAddr.h>
#include <net/XSocketUdp.h>

#include <udt/udt.h>

namespace KadNet {

	typedef XSockAddr Address;

	/*typedef struct {
		enum Type {
			UNKNOWN,
			// External addresses
			GLOBAL,			// global IP (may be dynamic)
			GLOBAL_STATIC,	// global static IP
			NAT,			// behind generic NAT
			NAT_FULL_CONE,
			NAT_ADDR_RESTR,
			NAT_PORT_RESTR,
			NAT_SYMMETRIC,	// oh no
			// Internal addresses
			LOCAL,			// some local IP
			SAME_LAN		// appears to be on same LAN
		} mType;

		XSockAddr mInt;
		XSockAddr mExt;
	} Location;*/

	class Listener
		: private XThread
	{
	public:
		typedef XDelegate< void (const void* buff, unsigned len, const Address& addr) > Handler;

	public:
		Listener(const XSockAddr& addr, Handler cbk)
			: mCbk (cbk)
		{
			if (mSocket.Bind(addr)) {
				LOG(mLog, "Bound to address " << mSocket.GetBindAddr().ToString());
			} else {
				LOG_CRIT(mLog, "Could not bind to address " << addr.ToString());
				return;
			}
			Start();
		}

		virtual ~Listener()
		{
			XThread::Stop();
			mSocket.SendTo("wake!", 6, mSocket.GetBindAddr());
			XThread::Wait();
		}

		void SendTo(const void* buff, unsigned len, const Address& addr) const {
			mSocket.SendTo(buff, len, addr);
		}

		const KadNet::Address BindAddr() const { return mSocket.GetBindAddr(); }

	private:
		virtual int Run()
		{
			XSockAddr from;
			while (!IsStopping()) {
				ssize_t len = mSocket.RecvFrom(mBuffer, sizeof(mBuffer), &from);
				if (len <= 0) continue;
				if (IsStopping()) break;

				if(mCbk) {
					mCbk(mBuffer, len, from);
				}
			}
			return 0;
		}

	private:
		Handler		mCbk;
		XSocketUdp	mSocket;
		uint8_t		mBuffer[4096];
	};

	class ConnectionMgr
		: private XThread
	{

	public:
		class Connection {
			friend class ConnectionMgr;
		public:
			typedef XDelegate< void (const void* buff, unsigned len) > Handler;

			void Send(const void* buff, unsigned len) const {
				UDT::sendmsg(mSocket, (const char*)buff, len);
			}
		private:
			UDTSOCKET	mSocket;
			Handler		mCbk;
		};

		ConnectionMgr(const XSockAddr& addr)
		{
			UDT::startup();
			mEpoll = UDT::epoll_create();

			if (UDT::INVALID_SOCK != (mListener = UDT::socket(addr.SA()->sa_family, SOCK_DGRAM, 0))) {
				if (UDT::ERROR != UDT::bind(mListener, addr.SA(), addr.SA_LEN())) {
				   if (UDT::ERROR != UDT::listen(mListener, 100)) {
					   UDT::epoll_add_usock(mEpoll, mListener);
				   } else {
					   LOG(mLog, "udt::listen: " << UDT::getlasterror().getErrorMessage());
						UDT::close(mListener);
						mListener = UDT::INVALID_SOCK;
						return;
				   }
				} else {
					LOG(mLog, "udt::bind: " << UDT::getlasterror().getErrorMessage());
					UDT::close(mListener);
					mListener = UDT::INVALID_SOCK;
					return;
				}
			} else {
				LOG(mLog, "udt::socket: " << UDT::getlasterror().getErrorMessage());
				return;
			}

			Start();
		}

		~ConnectionMgr() {
			XThread::Stop();
			UDT::epoll_release(mEpoll);
			XThread::Wait();
		}

		Connection* Connect(const XSockAddr& addr) {
			Connection* result = new Connection();
			if (UDT::INVALID_SOCK != (result->mSocket = UDT::socket(addr.SA()->sa_family, SOCK_DGRAM, 0))) {
				if (UDT::ERROR != UDT::connect(result->mSocket, addr.SA(), addr.SA_LEN())) {
					mConnections.Append(result);
					UDT::epoll_add_usock(mEpoll, result->mSocket);
					return result;
				} else {
					LOG(mLog, "connect: " << UDT::getlasterror().getErrorMessage());
				}

				UDT::close(result->mSocket);
			} else {
				LOG(mLog, "socket: " << UDT::getlasterror().getErrorMessage());
			}
			delete result;
			return NULL;
		}

	private:
		virtual int Run()
		{
			std::set<UDTSOCKET> readfds;

			while (!IsStopping()) {
				if (0 == UDT::epoll_wait(mEpoll, &readfds, NULL, 10)) continue;
				if (IsStopping()) break;

				for (std::set<UDTSOCKET>::iterator it = readfds.begin(); it != readfds.end(); ++it) {
					if (*it == mListener) {
						UDTSOCKET client;
						XSockAddr addr;
						int addrlen = sizeof(addr);
						if (UDT::INVALID_SOCK != (client = UDT::accept(mListener, addr.SA(), &addrlen)) ) {
							LOG(mLog, "Connection from: " << addr.ToString());
							UDT::epoll_add_usock(mEpoll, client);
						} else {
							LOG(mLog, "udt::accept(): " << UDT::getlasterror().getErrorMessage());
						}
					} else {
						int len = UDT::recvmsg(*it, mBuff, sizeof(mBuff));
						if (len <= 0) continue;

						for (XList<Connection*>::It it2 = mConnections.First(); it2 != mConnections.End(); ++it2) {
							if (mConnections[it2]->mSocket == *it) {
								mConnections[it2]->mCbk(mBuff, len);
							}
						}

					}
				}
			}
			return 0;
		}

	private:
		XList<Connection*> mConnections;
		UDTSOCKET	mListener;
		int			mEpoll;
		char		mBuff[4096];
	};

};

#endif /* KAD_OVER_IP_H_ */
