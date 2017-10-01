#ifndef __TCPMGR_H__
#define __TCPMGR_H__

#include <pthread.h>

#include "tcpmgr_sock.h"
#include "tcpmgr_arg.h"

struct TCPMGR_LIST
{
	pthread_t tHandle;
	sock_t clientSock;

	int occupied;
	int closeJoin;
};

typedef struct TCPMGR_STRUCT
{
	int stop;
	int mgrListLen;
	struct TCPMGR_LIST* mgrList;

	int serverFlag;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

	sock_t listenSock;
} tcpmgr_t;

#ifdef __cplusplus
extern "C" {
#endif

int tcpmgr_init(tcpmgr_t* mgrPtr, tcpmgr_arg_t* argPtr);
int tcpmgr_server_init(tcpmgr_t* mgrPtr, tcpmgr_arg_t* argPtr);

void tcpmgr_server_cleanup(tcpmgr_t* mgrPtr);
void tcpmgr_cleanup(tcpmgr_t* mgrPtr);

void* tcpmgr_clean_task(void* arg);

#ifdef __cplusplus
}
#endif

#endif
