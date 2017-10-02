#include <assert.h>

#include "tcpmgr.h"
#include "tcpmgr_private.h"
#include "debug.h"

void* tcpmgr_client_thread(void* arg)
{
	struct TCPMGR_LIST* listPtr = arg;

	LOG("enter, arg = %p", arg);

	// Run client task
	listPtr->client_task(listPtr->usrData, (int)listPtr->clientSock);

	// Cleanup
	pthread_cond_signal(listPtr->condPtr);
	listPtr->closeJoin = 1;

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}

void* tcpmgr_accept_task(void* arg)
{
	int i, tmpIndex;
	tcpmgr_t mgr = arg;

	pthread_t clientTh;
	sock_t clientSock;

	LOG("enter, arg = %p", arg);
	LOG("mgr->mgrList = %p", mgr->mgrList);
	LOG("mgr->mgrListLen = %d", mgr->mgrListLen);

	assert(mgr->mgrList != NULL);

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

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}

void* tcpmgr_clean_task(void* arg)
{
	int i;
	tcpmgr_t mgr = arg;

	LOG("enter, arg = %p", arg);
	LOG("mgr->mgrList = %p", mgr->mgrList);

	assert(mgr->mgrList != NULL);

	while(mgr->stop == 0)
	{
		// Wait condition
		pthread_cond_wait(&mgr->cond, &mgr->mutex);

		// Join client thread
		for(i = 0; i < mgr->mgrListLen; i++)
		{
			if(mgr->mgrList[i].closeJoin > 0)
			{
				LOG("Join %d thread", i);
				pthread_join(mgr->mgrList[i].tHandle, NULL);
				mgr->mgrList[i].closeJoin = 0;
				mgr->mgrList[i].occupied = 0;
			}
		}
	}

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}
