#ifndef KADSTATS_H_
#define KADSTATS_H_

struct KadStats
{
	int mAddNewQty;
	int mAddExistingQty;

	int mSplitDoneQty;
	int mSplitFailQty;

	void Print();
};

extern KadStats KAD_STATS;

#endif /* KADSTATS_H_ */
