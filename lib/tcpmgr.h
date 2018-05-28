/**
 * @author	Zheng-Ling Lai
 * @file	tcpmgr.h
**/

#ifndef __TCPMGR_H__
#define __TCPMGR_H__

#include <stdio.h>

/**
 * Return value definitions of tcpmgr library.
 */
enum TCPMGR_RETURN_VALUE
{
	/**
	 * No error.
	 */
	TCPMGR_NO_ERROR			= 0,

	/**
	 * Memory operation failed.
	 */
	TCPMGR_MEM_FAILED		= -1,

	/**
	 * System function failed.
	 */
	TCPMGR_SYS_FAILED		= -2,

	/**
	 * Failed to bind on given IP address and port.
	 */
	TCPMGR_BIND_FAILED		= -3,

	/**
	 * Failed to listen on the socket.
	 */
	TCPMGR_LISTEN_FAILED	= -4,

	/**
	 * Invalid argument(s).
	 */
	TCPMGR_INVALID_ARG		= -5
};

/**
 * Type definition of tcpmgr handle.
 */
typedef struct TCPMGR* tcpmgr_t;

/**
 * Type definition of tcpmgr information.
 */
typedef struct TCPMGR_INFO
{
	/**
	 * Client ID given by tcpmgr.
	 */
	int clientID;

	/**
	 * IP address of client connection.
	 */
	const char* ipAddr;

	/**
	 * Port of client connection.
	 */
	int port;
} tcpmgr_info_t;

#ifdef __cplusplus
extern "C" {
#endif

int tcpmgr_create(tcpmgr_t* mgrPtr, const char* hostIP, int hostPort, int maxClient);
void tcpmgr_set_output_stream(tcpmgr_t mgr, FILE* outStream);
int tcpmgr_start(tcpmgr_t mgr, void (*client_task)(void*, int, tcpmgr_info_t), void* arg);
void tcpmgr_stop(tcpmgr_t mgr);
void tcpmgr_delete(tcpmgr_t mgr);

#ifdef __cplusplus
}
#endif

#endif
