#ifndef KAD_OPERATION_H_
#define KAD_OPERATION_H_

#include <XTime.h>
#include "KadMsg.h"

class KadOperation {
public:
	KadOperation(KadMsgId id) : mId (id)
	{
		XPlatGetTime(&mStarted, NULL);
	}

	class SelectById {
	public:
		SelectById(KadMsgId id) : mId (id) {};
		bool operator()(const KadOperation& t) { return mId == t.mId; }
		bool operator()(const KadOperation* t) { return mId == t->mId; }
	private:
		KadMsgId mId;
	};

private:
	KadMsgId mId;
	XPlatDateTime mStarted;
};

#endif //KAD_OPERATION_H_
