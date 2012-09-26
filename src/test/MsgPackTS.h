


#include "operations/KadMsg.h"
#include <cxxtest/TestSuite.h>

class MsgPackTS : public CxxTest::TestSuite
{
public:

	void testPack(void)
	{
		MsgPackRaw<16> a;

		uint8_t buff[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		uint8_t packed[17] = { 0xB0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		memcpy(&a, &buff, 16);

		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, a);

		TS_ASSERT_EQUALS(sbuf.size(), 17);
		TS_ASSERT_SAME_DATA(sbuf.data(), packed, 17);

		msgpack::unpacked msg;
		msgpack::unpack(&msg, sbuf.data(), sbuf.size());

		MsgPackRaw<16> b;
		msg.get().convert(&b);

		TS_ASSERT_SAME_DATA(&a, &b, sizeof(a));
	}

	void testCast(void)
	{
		KadMsgFindRsp a;
		memset(&a, 0, sizeof(a));

		a.mMsgType = KadMsg::KAD_MSG_JOIN_RSP;
		KadMsgContact c1;
		c1.mAddr.mPort = 4096;
		a.mContacts.push_back(c1);


        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, a);

        TS_ASSERT_EQUALS(sbuf.size(), 62);

        msgpack::unpacked msg;
        msgpack::unpack(&msg, sbuf.data(), sbuf.size());

        //msgpack_object_print(stdout, msg.get());

        KadMsgFindRsp b;
        memset(&b, 0, sizeof(b));
        msg.get().convert(&b);

        TS_ASSERT_EQUALS(a.mMsgType, msg.get().via.array.ptr[0].via.array.ptr[0].via.u64);
        TS_ASSERT_EQUALS(a.mContacts.size(), b.mContacts.size());
        TS_ASSERT_EQUALS(a.mContacts.front().mAddr.mPort, c1.mAddr.mPort);
	}

	void testSizes(void)
	{
		{	// Just header: Ping/Pong
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, KadMsg());
			TS_ASSERT_EQUALS(sbuf.size(), 20);
		}
		{	// Address
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, KadMsgAddr());
			TS_ASSERT_EQUALS(sbuf.size(), 22);
		}
		{	// Contact
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, KadMsgContact());
			TS_ASSERT_EQUALS(sbuf.size(), 38);
		}
		{	// FindReq
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, KadMsgFindReq());
			TS_ASSERT_EQUALS(sbuf.size(), 38);
		}
		{	// FindRsp with 0 contacts
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, KadMsgFindRsp());
			TS_ASSERT_EQUALS(sbuf.size(), 22);
		}
		{	// FindRsp with 16 contacts
			KadMsgFindRsp msg;
			for (int i=0; i<16; i++) {
				msg.mContacts.push_back(KadMsgContact());
			}
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, msg);
			TS_ASSERT_EQUALS(sbuf.size(), 632);
		}
	}

};
