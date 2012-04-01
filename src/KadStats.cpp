#include "KadStats.h"
#include <stdio.h>

KadStats KAD_STATS;

void KadStats::Print()
{
	printf("**************************************\n");
	printf(" add done:          %d\n", mAddNewQty);
	printf(" add existing:      %d\n", mAddExistingQty);

	printf(" split done:        %d\n", mSplitDoneQty);
	printf(" split fail:        %d\n", mSplitFailQty);
	printf("**************************************\n");
}
