#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "debug.h"

int tcpmgr_start(tcpmgr_t mgr, void* (*client_task)(void*, int), void* arg)
{
	int ret = TCPMGR_NO_ERROR;

	return ret;
}

int tcpmgr_join(tcpmgr_t mgr)
{
	int ret = TCPMGR_NO_ERROR;

	return ret;
}

int tcpmgr_init(tcpmgr_t* mgrPtr, const char* hostIP, int hostPort, int maxClient)
{
	int ret = TCPMGR_NO_ERROR;
	tcpmgr_t tmpMgr = NULL;
	tcpmgr_arg_t arg;

	// Memory allocation for manage structure
	tmpMgr = calloc(1, sizeof(struct TCPMGR));
	if(tmpMgr == NULL)
	{
		ret = TCPMGR_MEM_FAILED;
		goto RET;
	}

	// Set argument
	arg.hostIP = hostIP;
	arg.hostPort = hostPort;
	arg.maxClient = maxClient;

	// Initial tcpmgr struct
	ret = tcpmgr_struct_init(tmpMgr, &arg);
	if(ret < 0)
	{
		goto ERR;
	}

	// Initial server service
	ret = tcpmgr_server_init(tmpMgr, &arg);
	if(ret < 0)
	{
		goto ERR;
	}

	// Set default output stream
	tmpMgr.stream = stdout;

	// Assign value
	*mgrPtr = tmpMgr;

ERR:
	tcpmgr_server_cleanup(tmpMgr);
	tcpmgr_struct_cleanup(tmpMgr);
	free(tmpMgr);

RET:
	return ret;
}

int tcpmgr_server_init(tcpmgr_t mgrPtr, tcpmgr_arg_t* argPtr)
{
	int ret = 0;
	sock_t tmpSocket;
	struct sockaddr_in mgrAddr;

	LOG("enter");

	// Create socket
	mgrAddr.sin_family = AF_INET;
	mgrAddr.sin_addr.s_addr = inet_addr(argPtr->hostIP);
	mgrAddr.sin_port = htons(argPtr->hostPort);
    tmpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(tmpSocket < 0)
	{
		printf("Failed to create socket for listening!\n");
		ret = -1;
		goto RET;
	}

	// Bind socket
	ret = bind(tmpSocket, (struct sockaddr*)&mgrAddr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		printf("Failed to bind on %s:%d!\n", argPtr->hostIP, argPtr->hostPort);
		goto ERR;
	}

	// Listen on socket
	ret = listen(tmpSocket, argPtr->maxClient);
	if(ret < 0)
	{
		printf("Failed to listen on %s:%d!\n", argPtr->hostIP, argPtr->hostPort);
		goto ERR;
	}

	// Assign value
	mgrPtr->listenSock = tmpSocket;
	mgrPtr->serverFlag = 1;
	goto RET;

ERR:
	sock_close(tmpSocket);

RET:
	LOG("exit");
	return ret;
}

void tcpmgr_server_cleanup(tcpmgr_t mgrPtr)
{
	int i;

	LOG("enter");

	if(mgrPtr->mgrList != NULL)
	{
		for(i = 0; i < mgrPtr->mgrListLen; i++)
		{
			if(mgrPtr->mgrList[i].occupied > 0)
			{
				pthread_cancel(mgrPtr->mgrList[i].tHandle);
				mgrPtr->mgrList[i].occupied = 0;
			}

			if(mgrPtr->mgrList[i].closeJoin > 0)
			{
				pthread_join(mgrPtr->mgrList[i].closeJoin, NULL);
				mgrPtr->mgrList[i].closeJoin = 0;
			}
		}
	}

	if(mgrPtr->serverFlag > 0)
	{
		sock_close(mgrPtr->listenSock);
	}

	LOG("exit");
}

void tcpmgr_struct_cleanup(tcpmgr_t mgrPtr)
{
	LOG("enter");

	free(mgrPtr->mgrList);
	pthread_mutex_destroy(&mgrPtr->mutex);
	pthread_cond_destroy(&mgrPtr->cond);

	LOG("exit");
}

int tcpmgr_struct_init(tcpmgr_t mgrPtr, tcpmgr_arg_t* argPtr)
{
	int ret = 0;
	struct TCPMGR tmpMgr;

	LOG("enter");

	// Zero memory
	memset(&tmpMgr, 0, sizeof(struct TCPMGR));

	// Create client manage list
	tmpMgr.mgrList = calloc(argPtr->maxClient, sizeof(struct TCPMGR_LIST));
	if(tmpMgr.mgrList == NULL)
	{
		printf("Memory allocation for client manage list failed!\n");
		ret = -1;
	}
	else
	{
		tmpMgr.mgrListLen = argPtr->maxClient;
	}

	// Create mutex and condition variable
	ret = pthread_mutex_init(&tmpMgr.mutex, NULL);
	if(ret < 0)
	{
		goto ERR;
	}

	ret = pthread_cond_init(&tmpMgr.cond, NULL);
	if(ret < 0)
	{
		goto ERR;
	}

	// Assign value
	*mgrPtr = tmpMgr;
	goto RET;

ERR:
	tcpmgr_cleanup(&tmpMgr);

RET:
	LOG("exit");
	return ret;
}

