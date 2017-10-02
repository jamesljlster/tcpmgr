#ifndef __TCPMGR_H__
#define __TCPMGR_H__

#include <stdio.h>

enum TCPMGR_RETURN_VALUE
{
	TCPMGR_NO_ERROR			= 0,
	TCPMGR_MEM_FAILED		= -1,
	TCPMGR_SYS_FAILED		= -2,
	TCPMGR_BIND_FAILED		= -3,
	TCPMGR_LISTEN_FAILED	= -4
};

typedef struct TCPMGR* tcpmgr_t;

#ifdef __cplusplus
extern "C" {
#endif

int tcpmgr_create(tcpmgr_t* mgrPtr, const char* hostIP, int hostPort, int maxClient);
void tcpmgr_set_output_stream(tcpmgr_t mgr, FILE* outStream);
int tcpmgr_start(tcpmgr_t mgr, void (*client_task)(void*, int), void* arg);
void tcpmgr_stop(tcpmgr_t mgr);
void tcpmgr_delete(tcpmgr_t mgr);

#ifdef __cplusplus
}
#endif

#endif
