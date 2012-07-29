#ifndef KAD_OVER_SIM_H_
#define KAD_OVER_SIM_H_

#include <XDelegate.h>

namespace KadNet {

	typedef unsigned Address;

	class Connection {
	public:
		typedef XDelegate< void (const void* buff, unsigned len) > Handler;

		void Send(void* buff, unsigned len) {

		}
	private:
	};

	class Listener {
	public:
		typedef XDelegate< void (const void* buff, unsigned len, const Address& addr) > Handler;

		void SendTo(const void* buff, unsigned len, const Address& addr) {

		}
	private:
	};
};


#endif /* KAD_OVER_SIM_H_ */
