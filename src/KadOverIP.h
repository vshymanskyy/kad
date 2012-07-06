#ifndef KAD_CONTACT_H_
#define KAD_CONTACT_H_

#include <net/XSockAddr.h>
#include <net/XSocketUdp.h>

struct KadOverIp {

	struct Address {
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
		};
		Type mType;
		XSockAddr mInt;
		XSockAddr mExt;
	};

	struct Socket {
		void SendTo(const void* buff, unsigned len, const Address& addr) {

		}

		unsigned RecvFrom(void* buff, unsigned len, Address* addr) {

		}

		XSocketUdp mSocket;
	};
};



#endif /* KAD_CONFIG_H_ */
