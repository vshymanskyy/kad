#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_


#include "KadContact.h"

#include "XThread.h"
#include "XList.h"
#include "XArray.h"
#include "XLog.h"

#include <iostream>
#include <memory>

class TKadRtNode
{

private:
	TKadRtNode(const KadId& local, unsigned depth, unsigned index)
		: mDepth(depth), mIndex(index), mLocalId(local), m0(NULL), m1(NULL)
	{
	}

	TKadRtNode* Closest(const KadId& d) const { return d.GetBit(mDepth) ? m1 : m0; }
	TKadRtNode* Farthest(const KadId& d) const { return d.GetBit(mDepth) ? m0 : m1; }
	bool IsBucket() const { return !(m0 || m1); }

	bool IsSplittable() const {
		return (mDepth < (KADEMLIA_ID_BITS-1)) &&
				(mContacts.Count() >= KADEMLIA_BUCKET_SIZE) &&
				(mDepth < KADEMLIA_RT_BASE || KadId(mIndex) < KadId::PowerOfTwo(mDepth % KADEMLIA_B));
	}

	bool Split() {
		X_ASSERT(!m0);
		X_ASSERT(!m1);

		// Check if we can split
		if (!IsSplittable()) {
			// TODO: KAD_STATS.mSplitFailQty++;
			return false;
		}

		m0 = new TKadRtNode(mLocalId, mDepth + 1, mIndex << 1);
		m1 = new TKadRtNode(mLocalId, mDepth + 1, (mIndex << 1) + 1);
		X_ASSERT(m0);
		X_ASSERT(m1);

		for (KadContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
			Closest(mLocalId ^ mContacts[it]->mId)->mContacts.Append(mContacts[it]);
		}

		for (KadContactList::It it = mCache.First(); it != mCache.End(); ++it) {
			TKadRtNode* closest = Closest(mLocalId ^ mCache[it]->mId);
			if (closest->mContacts.Count() < KADEMLIA_BUCKET_SIZE) {
				closest->mContacts.Append(mCache[it]);
			} else if (closest->mCache.Count() < KADEMLIA_CACHE_SIZE) {
				closest->mCache.Append(mCache[it]);
			}
		}

		mContacts.Clear();
		mCache.Clear();

		// TODO: KAD_STATS.mSplitDoneQty++;
		return true;
	}

	KadContactPtr AddNode(const KadId& id, const KadNet::Address& addr, const KadId& dist) {
		if (!IsBucket()) return Closest(dist)->AddNode(id, addr, dist);

		// Bucket found
		KadContactList::It c = FindNodeById(mContacts, id);
		if (c != mContacts.End()) {
			KadContactPtr node = mContacts[c];
			// Node is already present, update the info
			// TODO: Update data
			mContacts.Append(mContacts.Remove(c));
			// TODO: KAD_STATS.mAddExistingQty++;
			return node;
		} else if (mContacts.Count() < KADEMLIA_BUCKET_SIZE) {
			//LOG(NULL, "New node: " << newNode.mId);

			// Make a copy
			KadContactPtr node(new KadContact(id, addr));
			// Bucket is not full, insert the node
			mContacts.Append(node);
			// TODO: KAD_STATS.mAddNewQty++;

			return node;
		} else if (Split()) {
			return AddNode(id, addr, dist);
		} else {
			// TODO: Check cache order
			KadContactList::It c2 = FindNodeById(mCache, id);
			if (c2 != mCache.End()) {
				KadContactPtr node = mCache[c2];
				// TODO: Update data
				mCache.Append(mCache.Remove(c2));
				return node;
			} else if (mCache.Count() < KADEMLIA_CACHE_SIZE) {
				KadContactPtr node(new KadContact(id, addr));
				mCache.Append(node);
				return node;
			} else {
				mCache.PopFront();
				KadContactPtr node(new KadContact(id, addr));
				mCache.Append(node);
				return node;
			}
		}

		return KadContactPtr();
	}

	bool RemoveNode(const KadId& id, const KadId& dist) {
		if (!IsBucket()) return Closest(dist)->RemoveNode(id, dist);

		// Bucket found, check if node is present
		KadContactList::It it = FindNodeById(mContacts, id);
		if (it != mContacts.End()) {
			// Remove it
			mContacts.Remove(it);
			// TODO: Take an item from cache
			return true;
		} else {
			// Look in mCache
			KadContactList::It it2 = FindNodeById(mCache, id);
			if (it2 != mCache.End()) {
				mCache.Remove(it2);
				return true;
			}
		}
		return false;
	}

	KadContactPtr FindNode(const KadId& id, const KadId& dist) const {
		if (!IsBucket()) {
			return Closest(dist)->FindNode(id, dist);
		} else {
			for (KadContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				if (mContacts[it]->mId == id) {
					return mContacts[it];
				}
			}
			return KadContactPtr();
		}
	}

	KadContactList FindClosest(const KadId& id, const KadId& dist, unsigned qty) const {
		if (qty <= 0) {
			return KadContactList();
		}
		if (!IsBucket()) {
			KadContactList result = Closest(dist)->FindClosest(id, dist, qty);
			if (result.Count() < qty) {
				result.Append(Farthest(dist)->FindClosest(id, dist, qty - result.Count()));
			}
			return result;
		} else {
			KadContactList result;
			// Find closest nodes in bucket
			for (KadContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				if (!mContacts[it]->IsStale()) {
					if (result.Count() < qty) {
						result.Append(mContacts[it]);
					} else {
						for (XList<KadContactPtr>::It jt = result.First(); jt != result.End(); ++jt) {
							if (id.Closer(mContacts[it]->mId, result[jt]->mId)) {
								result[jt] = mContacts[it];
								break;
							}
						}
					}
				}
			}
			return result;
		}
	}

public:

	TKadRtNode(const KadId& local)
		: mDepth(0), mIndex(0), mLocalId(local), m0(NULL), m1(NULL)
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

	const KadId& LocalId() const { return mLocalId; }

	KadId RandomId() const {
		KadId prefix = KadId(mIndex).Shl(KADEMLIA_ID_BITS - mDepth);
		KadId rndId = KadId::Random().SetBits(0, mDepth, 0);
		return (rndId | prefix) ^ mLocalId;
	}

	/*XList<KadId> GetRefreshList() {
		XList<KadId> refreshList;
		for (int i=1; i<KADEMLIA_ID_SIZE; i++) {
			KadId prefix = KadId::PowerOfTwo(i);
			KadId rndId = KadId::Random().SetBits(0, i, 0);
			refreshList.Append((rndId | prefix) ^ mLocalId);
		}
		return refreshList;
	}*/

	KadContactPtr AddNode(const KadId& id, const KadNet::Address& addr) {
		return AddNode(id, addr, id ^ mLocalId);
	}

	bool RemoveNode(const KadId& id) {
		return RemoveNode(id, id ^ mLocalId);
	}

	KadContactList FindClosest(const KadId& id, unsigned qty) const {
		return FindClosest(id, id ^ mLocalId, qty);
	}

	KadContactPtr FindNode(const KadId& id) const {
		return FindNode(id, id ^ mLocalId);
	}

	KadContactList GetContacts() const {
		KadContactList result;
		if (IsBucket()) {
			for (KadContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				if (!mContacts[it]->IsStale()) {
					result.Append(mContacts[it]);
				}
			}
		} else {
			result.Append(m0->GetContacts());
			result.Append(m1->GetContacts());
		}
		return result;
	}

	KadContactList::It FindNodeById(const KadContactList& lst, const KadId& id)
	{
		for (KadContactList::It it = lst.Last(); it != lst.End(); --it) {
			if (lst[it]->mId == id) {
				return it;
			}
		}
		return lst.End();
	}

	void PrintDot(std::ostream& s) const {
		TKadId<KadId::SIZE+1> rangeSize = TKadId<KadId::SIZE+1>::PowerOfTwo(KadId::BIT_SIZE-mDepth);

		if (IsBucket()) {
			s << '"' << this << '"';
			s << " [shape=record, style=filled, fillcolor=skyblue, label=\"{";
			s << "{index:" << mIndex << "|depth:" << mDepth  << "}";

			s  << "|{size:" << rangeSize.ToString() << "}";
			s  << "|{[" << (rangeSize*mIndex).ToString() << "; " << (rangeSize*(mIndex+1)).ToString() << ")}";

			s << "|{Contacts (" << mContacts.Count() << ")}";
			for (KadContactList::It it = mContacts.First(); it != mContacts.End(); ++it) {
				s << "|{" << (mContacts[it]->mId).ToString() << "|" << mContacts[it]->mAddrExt.ToString() << "}";
			}
			if (mCache.Count()) {
				s << "|{Cache (" << mCache.Count() << ")}";
				for (KadContactList::It it = mCache.First(); it != mCache.End(); ++it) {
					s << "|{" << (mCache[it]->mId).ToString() << "|" << mCache[it]->mAddrExt.ToString() << "}";
				}
			}
			s << "}\"]" << std::endl;
		} else {
			s << '"' << this << '"' << " [shape=record, style=filled, fillcolor=springgreen, label=\"{";
			s << "{index:" << mIndex << "|depth:" << mDepth  << "}";

			s  << "|{local:" << mLocalId.ToString() << "}";
			s  << "|{size:" << rangeSize.ToString() << "}";
			s  << "|{[" << (rangeSize*mIndex).ToString() << "; " << (rangeSize*(mIndex+1)).ToString() << ")}";

			s << "}\"]" << std::endl;

			s << '"' << this << '"' << "-> \"" << m0 << "\" [label=0]" << std::endl;
			m0->PrintDot(s);
			s << '"' << this << '"' << "-> \"" << m1 << "\" [label=1]" << std::endl;
			m1->PrintDot(s);
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
     * Also the range: range = [mIndex*rangeSize; (mIndex+1)*rangeSize)
     **/
	unsigned	mIndex;
	const KadId	mLocalId;

	TKadRtNode* m0;
	TKadRtNode* m1;
	KadContactList mContacts;
	KadContactList mCache;
	XPlatDateTime mLastLookup;
};

typedef TKadRtNode KadRtNode;

#endif /* ROUTING_TABLE_H_ */
