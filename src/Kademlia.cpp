#include "Kademlia.h"
#include "KadStats.h"

bool Node::AddNode(const Contact& newNode, const KadDistance& d)
{
	if (!mBucket) {
		// This is an internal space, choose a proper subspace
		return Closest(d)->AddNode(newNode, d);
	} else {
		// Bucket found
		XList<Contact>::It c = FindNodeById(mBucket->mContacts, newNode.mId);
#if KADEMLIA_CACHE_SIZE
		XList<Contact>::It c2 = FindNodeById(mBucket->mCache, newNode.mId);
#endif
		if (c != mBucket->mContacts.End()) {
			// Node is already present, update the info
			mBucket->mContacts.Remove(c);		// TODO: Update data
			mBucket->mContacts.Append(newNode);
			KAD_STATS.mAddExistingQty++;
			return true;
		} else if (mBucket->mContacts.Count() < KADEMLIA_BUCKET_SIZE) {
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
	}
}

bool Node::Split(const KadNodeId& localId)
{
	assert(mBucket);
	assert(!mNext0);
	assert(!mNext1);

	// Check if we can split
	if (!IsSplittable()) {
		KAD_STATS.mSplitFailQty++;
		return false;
	}

	KadNodeId newIndex = mIndex;
	newIndex.ShiftLeft();

	mNext0 = new Node(mDepth + 1, newIndex);
	mNext1 = new Node(mDepth + 1, newIndex | KadNodeId::PowerOfTwo(0));

	const XList<Contact>& lst = mBucket->mContacts;
	for (XList<Contact>::It it = lst.First(); it != lst.End(); ++it) {
		Closest(localId ^ lst[it].mId)->mBucket->mContacts.Append(lst[it]);
	}

	delete mBucket;
	mBucket = NULL;

	KAD_STATS.mSplitDoneQty++;
	return true;
}

bool Node::RemoveNode(const KadNodeId& id, const KadDistance& d)
{
	if (!mBucket) {
		// This is an internal space, choose appropriate subspace
		return Closest(d)->RemoveNode(id, d);
	} else {
		// Bucket found, check if node is present
		XList<Contact>::It it = FindNodeById(mBucket->mContacts, id);
		if (it != mBucket->mContacts.End()) {
			// Remove it
			mBucket->mContacts.Remove(it);
			//TODO: Take an item from cache
			return true;
		}
#if defined (KADEMLIA_CACHE_SIZE)
		else {
			// Look in mCache
			it = FindNodeById(mBucket->mCache, id);
			if (it != mBucket->mCache.End()) {
				mBucket->mCache.Remove(it);
				return true;
			}
		}
#endif
		return false;
	}
}

int Node::GatherClosest(const KadNodeId& id, const KadDistance& d, KadContact* res, int qty) const
{
	if (qty <= 0)
		return 0;
	if (!mBucket) {
		int found = Closest(d)->GatherClosest(id, d, res, qty);
		assert(found >= 0);
		if (found < qty) {
			found += Farthest(d)->GatherClosest(id, d, res + found, qty - found);
		}
		return found;
	} else {
		const XList<Contact>& lst = mBucket->mContacts;
		// Find closest nodes in bucket
		int found = 0;
		for (XList<Contact>::It it = lst.First(); it != lst.End(); ++it) {
			if (found < qty) {
				res[found++] = lst[it];
			} else {
				for (int i = 0; i < found; i++) {
					if (id.Closer(lst[it].mId, res[i].mId)) {
						res[i] = lst[it];
						break;
					}
				}
			}
		}
		return found;
	}
}
