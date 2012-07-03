#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_

#include "KadConfig.h"
#include "KadNodeId.h"
#include "KadContact.h"
#include "KadStats.h"

#include "XThread.h"
#include "XList.h"
#include "XArray.h"
#include "XLog.h"

template <unsigned K, unsigned ID, typename ADDR>
class TKadRtNode
{
	typedef TKadContact<ID,ADDR> Contact;
	typedef KadId<ID> Id;

	typedef XList<Contact> ContactList;
	typedef XList<Contact> CacheList;

	struct Bucket
	{
		ContactList mContacts;
	#if defined (KADEMLIA_CACHE_SIZE)
		CacheList mCache;
	#endif
	};

	unsigned mDepth;
	TKadRtNode* m0;
	TKadRtNode* m1;
	Bucket* mBucket;
	KadId<ID> mIndex;

	TKadRtNode* Closest(const Id& d) const {
		return d.GetBit(mDepth) ? m1 : m0;
	}

	TKadRtNode* Farthest(const Id& d) const {
		return d.GetBit(mDepth) ? m0 : m1;
	}

	bool IsSplittable() {
		return (mDepth < KADEMLIA_ID_BITS && mIndex < Id::PowerOfTwo(mDepth % 2));
	}

	bool Split(const Id& localId);

	TKadRtNode(unsigned depth, const Id& index)
		: mDepth(depth), m0(NULL), m1(NULL), mBucket(new Bucket()), mIndex(index)
	{
	}

public:

	TKadRtNode()
		: mDepth(0), m0(NULL), m1(NULL), mBucket(new Bucket()), mIndex(Id::Zero())
	{
	}

	~TKadRtNode()
	{
		if (mBucket)
			delete mBucket;
		if (m0)
			delete m0;
		if (m1)
			delete m1;
	}

	int CountContacts()
	{
		if (mBucket) {
			return mBucket->mContacts.Count();
		} else {
			return m0->CountContacts() + m1->CountContacts();
		}
	}

#if defined (KADEMLIA_CACHE_SIZE)
	int CountCachedContacts()
	{
		if (mBucket) {
			return mBucket->mCache.Count();
		} else {
			return m0->CountCachedContacts() + m1->CountCachedContacts();
		}
	}
#endif



	bool AddNode(const Contact& newNode, const Id& d);

	bool RemoveNode(const Id& id, const Id& d);

	XList<const Contact*> FindClosest(const Id& id, const Id& d, int qty) const;

	void Print() const
	{
		if (mBucket) {
			LOG(NULL, FMT("%*s", mDepth, "") << "depth: " << mDepth << "index: " << mIndex <<  ", contacts: " << mBucket->mContacts.Count());

			for (typename ContactList::It it = mBucket->mContacts.First(); it != mBucket->mContacts.End(); ++it) {
				LOG(NULL, FMT("%*s", mDepth, "") << mBucket->mContacts[it]);
			}
#if defined (KADEMLIA_CACHE_SIZE)
			if (mBucket->mCache.Count()) {
				LOG(NULL, FMT("%*s", mDepth, "") << "cached: " << mBucket->mCache.Count());

				for (XList<KadContact>::It it = mBucket->mCache.First(); it != mBucket->mCache.End(); ++it) {
					LOG(NULL, FMT("%*s", mDepth, "") << mBucket->mCache[it]);
				}
			}
#endif
		} else {
			LOG(NULL, FMT("%*s", mDepth, "") << "0:");
			m0->Print();
			LOG(NULL, FMT("%*s", mDepth, "") << "1:");
			m1->Print();
		}
	}


	XList<const Contact*> GetContacts() const {
		XList<const Contact*> result;
		if (mBucket) {
			for (typename ContactList::It it = mBucket->mContacts.First(); it != mBucket->mContacts.End(); ++it) {
				result.Append(&mBucket->mContacts[it]);
			}
		} else {
			result.Append(m0->GetContacts());
			result.Append(m1->GetContacts());
		}
		return result;
	}

	/*typename ContactList::It FindNodeById(const ContactList& lst, const Id& id)
	{
		for (typename ContactList::It it = lst.Last(); it != lst.End(); --it) {
			if (lst[it].mId == id) {
				return it;
			}
		}
		return lst.End();
	}*/

	typename CacheList::It FindNodeById(const CacheList& lst, const Id& id)
	{
		for (typename CacheList::It it = lst.Last(); it != lst.End(); --it) {
			if (lst[it].mId == id) {
				return it;
			}
		}
		return lst.End();
	}

};

typedef TKadRtNode<KADEMLIA_BUCKET_SIZE, KADEMLIA_ID_SIZE, XSockAddr> KadRtNode;


template <unsigned K, unsigned ID, typename ADDR>
bool TKadRtNode<K,ID,ADDR>::AddNode(const Contact& newNode, const Id& d)
{
	if (!mBucket) {
		// This is an internal space, choose a proper subspace
		return Closest(d)->AddNode(newNode, d);
	} else {
		// Bucket found
		typename ContactList::It c = FindNodeById(mBucket->mContacts, newNode.mId);
#if KADEMLIA_CACHE_SIZE
		typename CacheList::It c2 = FindNodeById(mBucket->mCache, newNode.mId);
#endif
		if (c != mBucket->mContacts.End()) {
			// Node is already present, update the info
			mBucket->mContacts.Remove(c);		// TODO: Update data
			mBucket->mContacts.Append(newNode);
			KAD_STATS.mAddExistingQty++;
			return true;
		} else if (mBucket->mContacts.Count() < KADEMLIA_BUCKET_SIZE) {
			//LOG(NULL, "New node: " << newNode.mId);

			// Bucket is not full, insert the node
			mBucket->mContacts.Append(newNode);
			KAD_STATS.mAddNewQty++;

			return true;
		} else if (Split(newNode.mId ^ d)) {
			return AddNode(newNode, d);
#if defined (KADEMLIA_CACHE_SIZE) // Todo: Check cache order
		} else if (c2 != mBucket->mCache.End()) {
			mBucket->mCache.Remove(c2);		// TODO: Update data
			mBucket->mCache.Append(newNode);
			return true;
		} else if (mBucket->mCache.Count() < KADEMLIA_CACHE_SIZE) {
			mBucket->mCache.Append(newNode);
			return true;
		} else {
			mBucket->mCache.PopFront();
			mBucket->mCache.Append(newNode);
			return true;
#endif
		}
		return false;
	}
}

template <unsigned K, unsigned ID, typename ADDR>
bool TKadRtNode<K,ID,ADDR>::Split(const KadId<ID>& localId)
{
	X_ASSERT(mBucket);
	X_ASSERT(!m0);
	X_ASSERT(!m1);

	// Check if we can split
	if (!IsSplittable()) {
		KAD_STATS.mSplitFailQty++;
		return false;
	}

	Id newIndex = mIndex;
	newIndex.ShiftLeft();

	m0 = new TKadRtNode(mDepth + 1, newIndex);
	m1 = new TKadRtNode(mDepth + 1, newIndex | Id::PowerOfTwo(0));

	const ContactList& lst = mBucket->mContacts;
	for (typename ContactList::It it = lst.First(); it != lst.End(); ++it) {
		Closest(localId ^ lst[it].mId)->mBucket->mContacts.Append(lst[it]);
	}

	delete mBucket;
	mBucket = NULL;

	KAD_STATS.mSplitDoneQty++;
	return true;
}

template <unsigned K, unsigned ID, typename ADDR>
bool TKadRtNode<K,ID,ADDR>::RemoveNode(const KadId<ID>& id, const KadId<ID>& d)
{
	if (!mBucket) {
		// This is an internal space, choose appropriate subspace
		return Closest(d)->RemoveNode(id, d);
	} else {
		// Bucket found, check if node is present
		typename ContactList::It it = FindNodeById(mBucket->mContacts, id);
		if (it != mBucket->mContacts.End()) {
			// Remove it
			mBucket->mContacts.Remove(it);
			//TODO: Take an item from cache
			return true;
		}
#if defined (KADEMLIA_CACHE_SIZE)
		else {
			// Look in mCache
			typename CacheList::It it = FindNodeById(mBucket->mCache, id);
			if (it != mBucket->mCache.End()) {
				mBucket->mCache.Remove(it);
				return true;
			}
		}
#endif
		return false;
	}
}

template <unsigned K, unsigned ID, typename ADDR>
XList<const TKadContact<ID,ADDR>*>
TKadRtNode<K,ID,ADDR>::FindClosest(const Id& id, const Id& d, int qty) const
{
	if (qty <= 0) {
		return XList<const Contact*>();
	}
	if (!mBucket) {
		XList<const Contact*> result = Closest(d)->FindClosest(id, d, qty);
		if (result.Count() < qty) {
			result.Append(Farthest(d)->FindClosest(id, d, qty - result.Count()));
		}
		return result;
	} else {
		XList<const Contact*> result;
		// Find closest nodes in bucket
		for (typename ContactList::It it = mBucket->mContacts.First(); it != mBucket->mContacts.End(); ++it) {
			if (result.Count() < qty) {
				result.Append(&mBucket->mContacts[it]);
			} else {
				for (typename XList<const Contact*>::It jt = result.First(); jt != result.End(); ++jt) {
					if (id.Closer(mBucket->mContacts[it].mId, result[jt]->mId)) {
						result[jt] = &mBucket->mContacts[it];
						break;
					}
				}
			}
		}
		return result;
	}
}


#endif /* ROUTING_TABLE_H_ */
