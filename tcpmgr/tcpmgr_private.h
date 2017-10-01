#ifndef __TCPMGR_PRIVATE_H__
#define __TCPMGR_PRIVATE_H__

#include <stdio.h>
#include <pthread.h>

#include "tcpmgr.h"
#include "tcpmgr_sock.h"
#include "tcpmgr_arg.h"

struct TCPMGR_LIST
{
	pthread_t tHandle;
	pthread_cond_t* condPtr;
	sock_t clientSock;

	int occupied;
	int closeJoin;

	void* (*client_task)(void*, int);
	void* usrData;
};

struct TCPMGR
{
	int stop;
	int mgrListLen;
	struct TCPMGR_LIST* mgrList;
	FILE* stream;

	int serverFlag;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

	int cleanTaskStatus;
	pthread_t cleanTask;

	int acceptTaskStatus;
	pthread_t acceptTask;

	sock_t listenSock;

	void* (*client_task)(void*, int);
	void* usrData;
};

#ifdef __cplusplus
extern "C" {
#endif

int tcpmgr_struct_init(tcpmgr_t mgrPtr, tcpmgr_arg_t* argPtr);
int tcpmgr_server_init(tcpmgr_t mgrPtr, tcpmgr_arg_t* argPtr);

void tcpmgr_server_cleanup(tcpmgr_t mgrPtr);
void tcpmgr_struct_cleanup(tcpmgr_t mgrPtr);

void* tcpmgr_clean_task(void* arg);
void* tcpmgr_accept_task(void* arg);

#ifdef __cplusplus
}
#endif

#endif
