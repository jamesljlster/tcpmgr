#include <assert.h>

#include "tcpmgr.h"
#include "debug.h"

void* tcpmgr_client_thread(void* arg)
{
	struct TCPMGR_LIST* listPtr = arg;

	// Run client task
	clientArg->client_task(listPtr->usrData, (int)listPtr->clientSock);

	// Cleanup
	pthread_cond_signal(listPtr->condPtr);
	listPtr->closeJoin = 1;

	pthread_exit(NULL);
	return NULL;
}

void* tcpmgr_accept_task(void* arg)
{
	int tmpIndex;
	tcpmgr_t mgr = arg;

	pthread_t clientTh;
	sock_t clientSock;

	// Loop for accept clients
	while(mgr->stop == 0)
	{
		// Accept client
		fprintf(mgr->stream, "Waiting for client connection...\n");
		clientSock = accept(mgr->listenSock, NULL, NULL);
		if(clientSock < 0)
		{
			fprintf(mgr->stream, "Accept failed!\n");
			continue;
		}

		// Search empty entry
		tmpIndex = -1;
		for(i = 0; i < mgr->mgrListLen; i++)
		{
			if(mgr->mgrList[i].occupied == 0)
			{
				LOG("Entity %d available for client", i);
				tmpIndex = i;
				break;
			}
		}

		// Checking
		if(tmpIndex < 0)
		{
			// Reject connection
			fprintf(mgr->stream, "No more entity for client available. Connection rejected!\n");
			sock_close(clientSock);
		}
		else
		{
			// Set argument
			mgr->mgrList[i].client_task = mgr->client_task;
			mgr->mgrList[i].usrData = mgr->usrData;
			mgr->mgrList[i].condPtr = &mgr->cond;

			// Create client thread
			if(pthread_create(&clientTh, NULL, tcpmgr_client_thread, &mgr->mgrList[i]) < 0)
			{
				fprintf(mgr->stream, "Client thread initialization failed! Connection rejected!\n");
				sock_close(clientSock);
			}
			else
			{
				mgr->mgrList[i].tHandle = clientTh;
				mgr->mgrList[i].clientSock = clientSock;
				mgr->mgrList[i].occupied = 1;
			}
		}
	}

	pthread_exit(NULL);
	return NULL;
}

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
				LOG("Join %d thread", i);
				pthread_join(mgrPtr->mgrList[i].tHandle, NULL);
				mgrPtr->mgrList[i].closeJoin = 0;
				mgrPtr->mgrList[i].occupied = 0;
			}
		}
	}

	pthread_exit(NULL);
	return NULL;
}
