#ifndef __TCPMGR_ARG_H__
#define __TCPMGR_ARG_H__

enum TCPMGR_ARG_LIST
{
	TCPMGR_ARG_HOST_IP,
	TCPMGR_ARG_HOST_PORT,
	TCPMGR_ARG_MAX_CLIENT,

	TCPMGR_ARG_AMOUNT
};

typedef struct TCPMGR_ARG
{
	const char* hostIP;
	int hostPort;
	int maxClient;
} tcpmgr_arg_t;

extern const char* tcpmgr_arg_list[];

#ifdef __cplusplus
extern "C" {
#endif

void tcpmgr_arg_set_default(tcpmgr_arg_t* argPtr);
int tcpmgr_arg_parse(tcpmgr_arg_t* argPtr, int argc, char* argv[]);
void tcpmgr_arg_print_usage();
void tcpmgr_arg_print_summary(tcpmgr_arg_t* argPtr);

#ifdef __cplusplus
}
#endif

#endif
