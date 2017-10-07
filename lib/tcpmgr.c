#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "tcpmgr_private.h"
#include "debug.h"

void tcpmgr_set_output_stream(tcpmgr_t mgr, FILE* outStream)
{
	LOG("enter");

	mgr->stream = outStream;

	LOG("exit");
}

int tcpmgr_start(tcpmgr_t mgr, void (*client_task)(void*, int), void* arg)
{
	int ret = TCPMGR_NO_ERROR;

	LOG("enter");

	// Set client task
	mgr->client_task = client_task;
	mgr->usrData = arg;

	// Create clean task
	ret = pthread_create(&mgr->cleanTask, NULL, tcpmgr_clean_task, mgr);
	if(ret < 0)
	{
		ret = TCPMGR_SYS_FAILED;
		goto RET;
	}
	else
	{
		mgr->cleanTaskStatus = 1;
	}

	// Create accept task
	ret = pthread_create(&mgr->acceptTask, NULL, tcpmgr_accept_task, mgr);
	if(ret < 0)
	{
		ret = TCPMGR_SYS_FAILED;
		pthread_cancel(mgr->cleanTask);
		pthread_join(mgr->cleanTask, NULL);
		mgr->cleanTaskStatus = 0;
		goto RET;
	}
	else
	{
		mgr->acceptTaskStatus = 1;
	}

RET:
	LOG("exit");
	return ret;
}

void tcpmgr_stop(tcpmgr_t mgr)
{
	LOG("enter");

	// Cancel and join accept task
	if(mgr->acceptTaskStatus > 0)
	{
		LOG("Cancel and join accept task");
		pthread_cancel(mgr->acceptTask);
		pthread_join(mgr->acceptTask, NULL);
	}

	// Cancel clean task
	if(mgr->cleanTaskStatus > 0)
	{
		LOG("Cancel and join clean task");
		pthread_cancel(mgr->cleanTask);
		pthread_join(mgr->cleanTask, NULL);
	}

	// Cancel all client task
	tcpmgr_server_cleanup(mgr);

	LOG("exit");
}

void tcpmgr_delete(tcpmgr_t mgr)
{
	LOG("enter");

	// Cleanup
	if(mgr != NULL)
	{
		tcpmgr_server_cleanup(mgr);
		tcpmgr_struct_cleanup(mgr);
		free(mgr);
	}

#ifdef _WIN32
	WSACleanup();
#endif

	LOG("exit");
}

int tcpmgr_create(tcpmgr_t* mgrPtr, const char* hostIP, int hostPort, int maxClient)
{
	int ret = TCPMGR_NO_ERROR;
	tcpmgr_t tmpMgr = NULL;
	tcpmgr_arg_t arg;

#ifdef _WIN32
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(ret != 0)
	{
		ret = TCPMGR_SYS_FAILED;
		goto RET;
	}
#endif

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
	tmpMgr->stream = stdout;

	// Assign value
	*mgrPtr = tmpMgr;
	goto RET;

ERR:
	tcpmgr_server_cleanup(tmpMgr);
	tcpmgr_struct_cleanup(tmpMgr);
	free(tmpMgr);

RET:
	return ret;
}

int tcpmgr_server_init(tcpmgr_t mgrPtr, tcpmgr_arg_t* argPtr)
{
	int ret = TCPMGR_NO_ERROR;
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
		LOG("Failed to create socket for listening!");
		ret = TCPMGR_SYS_FAILED;
		goto RET;
	}

	// Bind socket
	ret = bind(tmpSocket, (struct sockaddr*)&mgrAddr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		LOG("Failed to bind on %s:%d!", argPtr->hostIP, argPtr->hostPort);
		ret = TCPMGR_BIND_FAILED;
		goto ERR;
	}

	// Listen on socket
	ret = listen(tmpSocket, argPtr->maxClient);
	if(ret < 0)
	{
		LOG("Failed to listen on %s:%d!\n", argPtr->hostIP, argPtr->hostPort);
		ret = TCPMGR_LISTEN_FAILED;
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
				LOG("Cancel %d client task", i);
				pthread_cancel(mgrPtr->mgrList[i].tHandle);
				mgrPtr->mgrList[i].closeJoin = 1;

			}

			if(mgrPtr->mgrList[i].closeJoin > 0)
			{
				LOG("Join %d client task", i);
				pthread_join(mgrPtr->mgrList[i].tHandle, NULL);
				mgrPtr->mgrList[i].occupied = 0;
				mgrPtr->mgrList[i].closeJoin = 0;
			}

			if(mgrPtr->mgrList[i].sockStatus > 0)
			{
				sock_close(mgrPtr->mgrList[i].clientSock);
			}
		}
	}

	LOG("exit");
}

void tcpmgr_struct_cleanup(tcpmgr_t mgrPtr)
{
	LOG("enter");

	if(mgrPtr->serverFlag > 0)
	{
		sock_close(mgrPtr->listenSock);
	}

	free(mgrPtr->mgrList);
	mgrPtr->mgrList = NULL;

	if(mgrPtr->mutexStatus > 0)
	{
		pthread_mutex_destroy(&mgrPtr->mutex);
	}

	if(mgrPtr->condStatus > 0)
	{
		pthread_cond_destroy(&mgrPtr->cond);
	}

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
		LOG("Memory allocation for client manage list failed!");
		ret = TCPMGR_MEM_FAILED;
		goto RET;
	}
	else
	{
		tmpMgr.mgrListLen = argPtr->maxClient;
	}

	// Create mutex and condition variable
	ret = pthread_mutex_init(&tmpMgr.mutex, NULL);
	if(ret < 0)
	{
		ret = TCPMGR_SYS_FAILED;
		goto ERR;
	}
	else
	{
		tmpMgr.mutexStatus = 1;
	}

	ret = pthread_cond_init(&tmpMgr.cond, NULL);
	if(ret < 0)
	{
		ret = TCPMGR_SYS_FAILED;
		goto ERR;
	}
	else
	{
		tmpMgr.condStatus = 1;
	}

	// Assign value
	*mgrPtr = tmpMgr;
	goto RET;

ERR:
	tcpmgr_struct_cleanup(&tmpMgr);

RET:
	LOG("exit");
	return ret;
}

