#ifndef __TCPMGR_GLOBAL_H__
#define __TCPMGR_GLOBAL_H__

#include <pthread.h>

extern tcpmgr_t tcpMgr;
extern int cleanTaskStatus;
extern pthread_t cleanTask;

#endif
