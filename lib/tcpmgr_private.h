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
	pthread_mutex_t* mutexPtr;
	int* cleanIndexPtr;

	int sockStatus;
	sock_t clientSock;

	int occupied;
	int closeJoin;

	int clientID;
	char clientAddr[INET_ADDRSTRLEN];
	int clientPort;

	void (*client_task)(void*, int, tcpmgr_info_t);
	void* usrData;
};

struct TCPMGR
{
	int stop;
	int mgrListLen;
	struct TCPMGR_LIST* mgrList;
	FILE* stream;

	int serverFlag;

	int cleanIndex;
	int mutexAttrStatus;
	pthread_mutexattr_t mutexAttr;
	int mutexStatus;
	pthread_mutex_t mutex;
	int condStatus;
	pthread_cond_t cond;

	int cleanTaskStatus;
	pthread_t cleanTask;

	int acceptTaskStatus;
	pthread_t acceptTask;

	sock_t listenSock;

	void (*client_task)(void*, int, tcpmgr_info_t);
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

void tcpmgr_mutex_unlock(void* arg);

#ifdef __cplusplus
}
#endif

#endif
