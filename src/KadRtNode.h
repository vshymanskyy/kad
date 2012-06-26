#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include "KadContact.h"
#include "KadStats.h"

#include "XThread.h"
#include "XList.h"
#include "XLog.h"


class KadRtNode
{
	struct Contact : public KadContact {
		Contact() {}
		Contact(const KadContact& c) : KadContact(c) {}

		int16_t mRTT;
		uint8_t mFailQty;
	};

	struct Bucket
	{
		XList<Contact> mContacts;
	#if defined (KADEMLIA_CACHE_SIZE)
		XList<Contact> mCache;
	#endif
	};

	uint8_t mDepth;
	KadRtNode* mNext0;
	KadRtNode* mNext1;
	Bucket* mBucket;
	KadNodeId mIndex;

	KadRtNode* Closest(const KadDistance& d) const {
		return d.GetBit(mDepth) ? mNext1 : mNext0;
	}

	KadRtNode* Farthest(const KadDistance& d) const {
		return d.GetBit(mDepth) ? mNext0 : mNext1;
	}

public:

	KadRtNode(uint8_t d, const KadNodeId& idx)
			: mDepth(d), mNext0(NULL), mNext1(NULL), mBucket(new Bucket()), mIndex(idx)
	{
	}

	~KadRtNode()
	{
		if (mBucket)
			delete mBucket;
		if (mNext0)
			delete mNext0;
		if (mNext1)
			delete mNext1;
	}

	int CountContacts()
	{
		if (mBucket) {
			return mBucket->mContacts.Count();
		} else {
			return mNext0->CountContacts() + mNext1->CountContacts();
		}
	}

#if defined (KADEMLIA_CACHE_SIZE)
	int CountCachedContacts()
	{
		if (mBucket) {
			return mBucket->mCache.Count();
		} else {
			return mNext0->CountCachedContacts() + mNext1->CountCachedContacts();
		}
	}
#endif

	bool Split(const KadNodeId& localId);

	bool AddNode(const Contact& newNode, const KadDistance& d);

	bool RemoveNode(const KadNodeId& id, const KadDistance& d);

	int GatherClosest(const KadNodeId& id, const KadDistance& d, KadContact* res, int qty, const KadNodeId& except) const;

	bool IsSplittable()
	{
		return (mDepth < KADEMLIA_ID_BITS && mIndex < KadNodeId::PowerOfTwo(mDepth % 2));
	}

	void Print() const
	{
		if (mBucket) {
			LOG(NULL, FMT("%*s", mDepth, "") << "depth: " << mDepth << "index: " << mIndex <<  ", contacts: " << mBucket->mContacts.Count());

			for (XList<Contact>::It it = mBucket->mContacts.First(); it != mBucket->mContacts.End(); ++it) {
				LOG(NULL, FMT("%*s", mDepth, "") << mBucket->mContacts[it]);
			}
#if defined (KADEMLIA_CACHE_SIZE)
			if (mBucket->mCache.Count()) {
				LOG(NULL, FMT("%*s", mDepth, "") << "cached: " << mBucket->mCache.Count());

				for (XList<Contact>::It it = mBucket->mCache.First(); it != mBucket->mCache.End(); ++it) {
					LOG(NULL, FMT("%*s", mDepth, "") << mBucket->mCache[it]);
				}
			}
#endif
		} else {
			LOG(NULL, FMT("%*s", mDepth, "") << "0:");
			mNext0->Print();
			LOG(NULL, FMT("%*s", mDepth, "") << "1:");
			mNext1->Print();
		}
	}


	XList<KadContact> GetContacts() const {
		XList<KadContact> result;
		if (mBucket) {
			for (XList<Contact>::It it = mBucket->mContacts.First(); it != mBucket->mContacts.End(); ++it) {
				result.Append(mBucket->mContacts[it]);
			}
		} else {
			result.Append(mNext0->GetContacts());
			result.Append(mNext1->GetContacts());
		}
		return result;
	}

	XList<Contact>::It FindNodeById(const XList<Contact>& lst, const KadNodeId& id)
	{
		for (XList<Contact>::It it = lst.Last(); it != lst.End(); --it) {
			if (lst[it].mId == id) {
				return it;
			}
		}
		return lst.End();
	}

};

#endif /* ROUTING_TABLE_H_ */
