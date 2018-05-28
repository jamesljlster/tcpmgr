#include <assert.h>
#include <string.h>
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
	tcpmgr_info_t clientInfo;

	LOG("enter, arg = %p", arg);

	// Set client information
	clientInfo.clientID = listPtr->clientID;
	clientInfo.ipAddr = (const char*)listPtr->clientAddr;
	clientInfo.port = listPtr->clientPort;

	// Run client task
	listPtr->client_task(listPtr->usrData, (int)listPtr->clientSock, clientInfo);

	// Close socket
	sock_close(listPtr->clientSock);
	listPtr->sockStatus = 0;

	// Cleanup
	listPtr->closeJoin = 1;
	pthread_mutex_lock(listPtr->mutexPtr);
	*listPtr->cleanIndexPtr = -1; // Not able to get current client index yet;
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

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;

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
		clientAddrLen = sizeof(struct sockaddr_in);
		clientSock = accept(mgr->listenSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
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
			mgr->mgrList[tmpIndex].cleanIndexPtr = &mgr->cleanIndex;

			mgr->mgrList[tmpIndex].clientSock = clientSock;
			mgr->mgrList[tmpIndex].sockStatus = 1;

			// Set client information
			mgr->mgrList[tmpIndex].clientID = tmpIndex;
			strcpy(mgr->mgrList[tmpIndex].clientAddr, inet_ntoa(clientAddr.sin_addr));
			mgr->mgrList[tmpIndex].clientPort = ntohs(clientAddr.sin_port);

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

				fprintf(mgr->stream, "Client %s:%d connected\n",
						mgr->mgrList[tmpIndex].clientAddr, mgr->mgrList[tmpIndex].clientPort);
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
	int mutexStatus;
	tcpmgr_t mgr = arg;

	LOG("enter, arg = %p", arg);

	assert(mgr->mgrList != NULL);

	// Setup cleanup handler
	pthread_cleanup_push(tcpmgr_mutex_unlock, &mgr->mutex);

	// Lock mutex
	pthread_mutex_lock(&mgr->mutex);
	mutexStatus = 1;

	while(mgr->stop == 0)
	{
		// Wait condition
		mutexStatus = 0;
		pthread_cond_wait(&mgr->cond, &mgr->mutex);
		mutexStatus = 1;

		// Join client thread
		LOG("Cleaning...");
		if(mgr->cleanIndex >= 0)
		{
			// Cleanup given client
			i = mgr->cleanIndex;

			LOG("Join %d thread", i);
			pthread_join(mgr->mgrList[i].tHandle, NULL);
			mgr->mgrList[i].closeJoin = 0;
			mgr->mgrList[i].occupied = 0;
		}
		else
		{
			// Search client to cleanup
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
	}

	pthread_cleanup_pop(mutexStatus);

	LOG("exit");
	pthread_exit(NULL);
	return NULL;
}
