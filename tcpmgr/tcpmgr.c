#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "debug.h"

int tcpmgr_server_init(tcpmgr_t* mgrPtr, tcpmgr_arg_t* argPtr)
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
	goto RET;

ERR:
	sock_close(tmpSocket);

RET:
	LOG("exit");
	return ret;
}

void tcpmgr_server_cleanup(tcpmgr_t* mgrPtr)
{
	int i;

	LOG("enter");

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

	sock_close(mgrPtr->listenSock);

	LOG("exit");
}

void tcpmgr_cleanup(tcpmgr_t* mgrPtr)
{
	LOG("enter");

	free(mgrPtr->mgrList);

	LOG("exit");
}

int tcpmgr_init(tcpmgr_t* mgrPtr, tcpmgr_arg_t* argPtr)
{
	int ret = 0;

	LOG("enter");

	// Zero memory
	memset(mgrPtr, 0, sizeof(tcpmgr_t));

	// Create client manage list
	mgrPtr->mgrList = calloc(argPtr->maxClient, sizeof(struct TCPMGR_LIST));
	if(mgrPtr->mgrList == NULL)
	{
		printf("Memory allocation for client manage list failed!\n");
		ret = -1;
	}
	else
	{
		mgrPtr->mgrListLen = argPtr->maxClient;
	}

	LOG("exit");
	return ret;
}

