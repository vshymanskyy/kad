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

template <typename ADDR>
class TKadRtNode
{
public:
	typedef TKadContact<KADEMLIA_ID_SIZE,ADDR> Contact;
	typedef TKadId<KADEMLIA_ID_SIZE> Id;

private:
	typedef XList<Contact> ContactList;

private:
	TKadRtNode* Closest(const Id& d) const { return d.GetBit(mDepth) ? m1 : m0; }
	TKadRtNode* Farthest(const Id& d) const { return d.GetBit(mDepth) ? m0 : m1; }
	bool IsSplittable() const { return (mDepth < KADEMLIA_ID_BITS && mIndex < Id::PowerOfTwo(mDepth % 2)); }
	bool IsBucket() const { return !(m0 || m1); }

	bool Split(const Id& localId) {
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
		X_ASSERT(m0);
		X_ASSERT(m1);

		const ContactList& lst = mContacts;
		for (typename ContactList::It it = lst.First(); it != lst.End(); ++it) {
			Closest(localId ^ lst[it].mId)->mContacts.Append(lst[it]);
		}

		mContacts.Clear();

		KAD_STATS.mSplitDoneQty++;
		return true;
	}

	TKadRtNode(unsigned depth, const Id& index)
		: mDepth(depth), mIndex(index), m0(NULL), m1(NULL)
	{
	}

public:

	TKadRtNode()
		: mDepth(0), mIndex(Id::Zero()), m0(NULL), m1(NULL)
	{
	}

	~TKadRtNode() {
		if (m0) delete m0;
		if (m1) delete m1;
	}

	unsigned CountContacts() const { return IsBucket()?mContacts.Count():(m0->CountContacts() + m1->CountContacts()); }
	unsigned CountCache() const { return IsBucket()?mCache.Count():(m0->CountCache() + m1->CountCache()); }
	unsigned CountBuckets() const { return IsBucket()?1:(m0->CountBuckets() + m1->CountBuckets()); }
	unsigned CountSpaces() const { return IsBucket()?0:(m0->CountSpaces() + m1->CountSpaces() + 1); }

	Id RandomId(const Id& local) const {
		Id prefix = Id(mIndex).ShiftLeft(KADEMLIA_ID_BITS - mDepth);
		Id rndId = Id::Random().SetBits(0, mDepth, 0);
		return (rndId | prefix) ^ local;
	}

	XList<Id> GetRefreshList(const Id& local) {
		XList<Id> refreshList;
		for (int i=1; i<KADEMLIA_ID_SIZE; i++) {
			Id prefix = Id::PowerOfTwo(i);
			Id rndId = Id::Random().SetBits(0, i, 0);
			refreshList.Append((rndId | prefix) ^ local);
		}
		return refreshList;
	}

	bool AddNode(const Contact& newNode, const Id& d) {
		if (!IsBucket()) return Closest(d)->AddNode(newNode, d);

		// Bucket found
		typename ContactList::It c = FindNodeById(mContacts, newNode.mId);
		if (c != mContacts.End()) {
			// Node is already present, update the info
			mContacts.Remove(c);		// TODO: Update data
			mContacts.Append(newNode);
			KAD_STATS.mAddExistingQty++;
			return true;
		} else if (mContacts.Count() < KADEMLIA_BUCKET_SIZE) {
			//LOG(NULL, "New node: " << newNode.mId);

			// Bucket is not full, insert the node
			mContacts.Append(newNode);
			KAD_STATS.mAddNewQty++;

			return true;
		} else if (Split(newNode.mId ^ d)) {
			return AddNode(newNode, d);
		} else {
			// TODO: Check cache order
			typename ContactList::It c2 = FindNodeById(mCache, newNode.mId);
			if (c2 != mCache.End()) {
				mCache.Remove(c2);		// TODO: Update data
				mCache.Append(newNode);
				return true;
			} else if (mCache.Count() < KADEMLIA_CACHE_SIZE) {
				mCache.Append(newNode);
				return true;
			} else {
				mCache.PopFront();
				mCache.Append(newNode);
				return true;
			}
		}

		return false;
	}

	bool RemoveNode(const Id& id, const Id& d) {
		if (!IsBucket()) return Closest(d)->RemoveNode(id, d);

		// Bucket found, check if node is present
		typename ContactList::It it = FindNodeById(mContacts, id);
		if (it != mContacts.End()) {
			// Remove it
			mContacts.Remove(it);
			// TODO: Take an item from cache
			return true;
		} else {
			// Look in mCache
			typename ContactList::It it2 = FindNodeById(mCache, id);
			if (it2 != mCache.End()) {
				mCache.Remove(it2);
				return true;
			}
		}
		return false;
	}

	XList<const Contact*> FindClosest(const Id& id, const Id& d, unsigned qty) const {
		if (qty <= 0) {
			return XList<const Contact*>();
		}
		if (!IsBucket()) {
			XList<const Contact*> result = Closest(d)->FindClosest(id, d, qty);
			if (result.Count() < qty) {
				result.Append(Farthest(d)->FindClosest(id, d, qty - result.Count()));
			}
			return result;
		} else {
			XList<const Contact*> result;
			// Find closest nodes in bucket
			for (typename ContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				if (result.Count() < qty) {
					result.Append(&mContacts[it]);
				} else {
					for (typename XList<const Contact*>::It jt = result.First(); jt != result.End(); ++jt) {
						if (id.Closer(mContacts[it].mId, result[jt]->mId)) {
							result[jt] = &mContacts[it];
							break;
						}
					}
				}
			}
			return result;
		}
	}

	XList<const Contact*> GetContacts() const {
		XList<const Contact*> result;
		if (IsBucket()) {
			for (typename ContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				result.Append(&mContacts[it]);
			}
		} else {
			result.Append(m0->GetContacts());
			result.Append(m1->GetContacts());
		}
		return result;
	}

	typename ContactList::It FindNodeById(const ContactList& lst, const Id& id)
	{
		for (typename ContactList::It it = lst.Last(); it != lst.End(); --it) {
			if (lst[it].mId == id) {
				return it;
			}
		}
		return lst.End();
	}

	void Print() const {
		if (IsBucket()) {
			LOG(NULL, FMT("%*s", mDepth, "") << "depth: " << mDepth << "index: " << mIndex <<  ", contacts: " << mContacts.Count());

			for (typename ContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				LOG(NULL, FMT("%*s", mDepth, "") << mContacts[it]);
			}
			if (mCache.Count()) {
				LOG(NULL, FMT("%*s", mDepth, "") << "cached: " << mCache.Count());

				for (typename ContactList::It it = mCache.First(); it != mCache.End(); ++it) {
					LOG(NULL, FMT("%*s", mDepth, "") << mCache[it]);
				}
			}
		} else {
			LOG(NULL, FMT("%*s", mDepth, "") << "0:");
			m0->Print();
			LOG(NULL, FMT("%*s", mDepth, "") << "1:");
			m1->Print();
		}
	}

private:

	/**
     * The depth in the tree.
     * Also the number of common most significant bits.
     * Also the size of the range:  rangeSize = 2^(IDSIZE-mDepth);
     **/
	unsigned	mDepth;

	/**
     * The number of spaces between this space and the local node.
     * Also the number of common most significant bits.
     * Also the range: range = [mIndex*rangeSize; mIndex*(rangeSize+1))
     **/
	Id			mIndex;

	TKadRtNode* m0;
	TKadRtNode* m1;
	ContactList mContacts;
	ContactList mCache;
	XPlatDateTime mLastLookup;

};

typedef TKadRtNode<XSockAddr> KadRtNode;

#endif /* ROUTING_TABLE_H_ */
