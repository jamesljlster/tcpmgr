#include <assert.h>

#include "tcpmgr.h"

void* tcpmgr_clean_task(void* arg)
{
	int i;
	tcpmgr_t* mgrPtr = arg;

	assert(mgrPtr->mgrList != NULL);

	while(mgrPtr->stop == 0)
	{
		// Wait condition
		pthread_cond_wait(&mgrPtr->cond, &mgrPtr->mutex);

		// Join client thread
		for(i = 0; i < mgrPtr->mgrListLen; i++)
		{
			if(mgrPtr->mgrList[i].closeJoin > 0)
			{
				pthread_join(mgrPtr->mgrList[i].closeJoin, NULL);
				mgrPtr->mgrList[i].closeJoin = 0;
				mgrPtr->mgrList[i].occupied = 0;
			}
		}
	}

	pthread_exit(NULL);
	return NULL;
}
