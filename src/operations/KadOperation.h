#ifndef KAD_OPERATION_H_
#define KAD_OPERATION_H_

#include <XTime.h>
#include "KadMsg.h"

class KadOpMgr;

class KadOperation {
public:
	KadOperation(KadOpMgr* mgr, KadMsgId id)
		: mId	(id)
		, mMgr	(mgr)
	{
		XPlatGetTime(&mStarted, NULL);
	}

	virtual ~KadOperation() {}

	virtual void Process(const KadMsgRsp* rsp) = 0;

	class SelectById {
	public:
		SelectById(const KadMsgId& id) : mId (id) {};
		bool operator()(const KadOperation& t) { return mId == t.mId; }
		bool operator()(const KadOperation* t) { return mId == t->mId; }
	private:
		const KadMsgId& mId;
	};

protected:
	KadMsgId		mId;
	KadOpMgr*		mMgr;
	XPlatDateTime	mStarted;
};

#endif //KAD_OPERATION_H_
