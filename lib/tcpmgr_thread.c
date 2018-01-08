#include <assert.h>
#include <time.h>

#include "tcpmgr.h"
#include "tcpmgr_private.h"
#include "debug.h"

#define CLEAN_ROUTINE	3	// Second

void tcpmgr_mutex_unlock(void* arg)
{
	pthread_mutex_unlock(arg);
}

void* tcpmgr_client_thread(void* arg)
{
	struct TCPMGR_LIST* listPtr = arg;

	LOG("enter, arg = %p", arg);

	// Run client task
	listPtr->client_task(listPtr->usrData, (int)listPtr->clientSock);

	// Close socket
	sock_close(listPtr->clientSock);
	listPtr->sockStatus = 0;

	// Cleanup
	listPtr->closeJoin = 1;
	pthread_mutex_lock(listPtr->mutexPtr);
	pthread_cond_signal(listPtr->condPtr);
	pthread_mutex_unlock(listPtr->mutexPtr);

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}

void* tcpmgr_accept_task(void* arg)
{
	int i, tmpIndex;
	int mutexStatus = 0;
	tcpmgr_t mgr = arg;

	pthread_t clientTh;
	sock_t clientSock;

#ifdef _WIN32
	int tmpRet;
	struct timeval timeout;
	fd_set fdSet;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif

	LOG("enter, arg = %p", arg);

	assert(mgr->mgrList != NULL);

	// Set cleanup handler
	pthread_cleanup_push(tcpmgr_mutex_unlock, &mgr->mutex);

	// Loop for accept clients
	while(mgr->stop == 0)
	{
		fprintf(mgr->stream, "Waiting for client connection...\n");

#ifdef _WIN32
#define DEFAULT_TIMEOUT 100000
SEL:
		// Create cancel point for windows
		// Set timeout
		timeout.tv_sec = 0;
		timeout.tv_usec = DEFAULT_TIMEOUT;

		// Select
		FD_ZERO(&fdSet);
		FD_SET(mgr->listenSock, &fdSet);
		tmpRet = select(mgr->listenSock + 1, &fdSet, NULL, NULL, &timeout);
		if(tmpRet == 0)
		{
			goto SEL;
		}
#endif

		// Accept client
		clientSock = accept(mgr->listenSock, NULL, NULL);
		if(clientSock < 0)
		{
			fprintf(mgr->stream, "Accept failed!\n");
			continue;
		}

		// Lock client list
		pthread_mutex_lock(&mgr->mutex);
		mutexStatus = 1;

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
			mgr->mgrList[tmpIndex].client_task = mgr->client_task;
			mgr->mgrList[tmpIndex].usrData = mgr->usrData;
			mgr->mgrList[tmpIndex].condPtr = &mgr->cond;
			mgr->mgrList[tmpIndex].mutexPtr = &mgr->mutex;

			mgr->mgrList[tmpIndex].clientSock = clientSock;
			mgr->mgrList[tmpIndex].sockStatus = 1;

			// Create client thread
			if(pthread_create(&clientTh, NULL, tcpmgr_client_thread, &mgr->mgrList[tmpIndex]) < 0)
			{
				fprintf(mgr->stream, "Client thread initialization failed! Connection rejected!\n");
				sock_close(clientSock);
				mgr->mgrList[tmpIndex].sockStatus = 0;
			}
			else
			{
				mgr->mgrList[tmpIndex].tHandle = clientTh;
				mgr->mgrList[tmpIndex].occupied = 1;
			}
		}

		// Unlock client list
		pthread_mutex_unlock(&mgr->mutex);
		mutexStatus = 0;
	}

	pthread_cleanup_pop(mutexStatus);

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}

void* tcpmgr_clean_task(void* arg)
{
	int i;
	//int ret;
	int mutexStatus;
	tcpmgr_t mgr = arg;
	//struct timespec timeout;

	LOG("enter, arg = %p", arg);

	assert(mgr->mgrList != NULL);

	// Setup cleanup handler
	pthread_cleanup_push(tcpmgr_mutex_unlock, &mgr->mutex);

	// Lock mutex
	pthread_mutex_lock(&mgr->mutex);
	mutexStatus = 1;

	while(mgr->stop == 0)
	{
		// Set timeout
		//clock_gettime(CLOCK_REALTIME, &timeout);
		//timeout.tv_sec += CLEAN_ROUTINE;

		// Wait condition
		mutexStatus = 0;
		pthread_cond_wait(&mgr->cond, &mgr->mutex);
		mutexStatus = 1;
		//pthread_cond_timedwait(&mgr->cond, &mgr->mutex, &timeout);
		/*
		ret = pthread_cond_timedwait(&mgr->cond, &mgr->mutex, &timeout);
		if(ret != 0)
		{
			printf("pthread_cond_timedwait() failed with error: %d\n", ret);

			// Try to lock mutex
			timeout.tv_sec = time(NULL) + CLEAN_ROUTINE;
			timeout.tv_nsec = 0;

			ret = pthread_mutex_timedlock(&mgr->mutex, &timeout);
			if(ret != 0)
			{
				printf("pthread_mutex_timedlock() failed with error: %d\n", ret);
				continue;
			}
			else
			{
				mutexStatus = 1;
			}
		}
		*/

		// Join client thread
		LOG("Cleaning...");
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

		// Unlock mutex
		/*
		if(mutexStatus > 0)
		{
			pthread_mutex_unlock(&mgr->mutex);
		}
		*/
	}

	pthread_cleanup_pop(mutexStatus);

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}
