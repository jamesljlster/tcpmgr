#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "tcpmgr_global.h"

#include "debug.h"

int main(int argc, char* argv[])
{
	int ret = 0;
	tcpmgr_arg_t arg;

	// Initial tcpmgr global variable
	memset(&tcpMgr, 0, sizeof(tcpmgr_t));

	// Set default argument
	tcpmgr_arg_set_default(&arg);

	// Parse argument
	ret = tcpmgr_arg_parse(&arg, argc, argv);
	if(ret < 0)
	{
		tcpmgr_arg_print_usage();
		goto RET;
	}
	else
	{
		tcpmgr_arg_print_summary(&arg);
	}

	// Create client manage list
	tcpMgr.mgrList = calloc(arg.maxClient, sizeof(struct TCPMGR_LIST));
	if(tcpMgr.mgrList == NULL)
	{
		printf("Memory allocation for client manage list failed!\n");
		ret = -1;
		goto RET;
	}
	else
	{
		tcpMgr.mgrListLen = arg.maxClient;
	}

	// Initial server service
	ret = tcpmgr_server_init(&tcpMgr, &arg);
	if(ret < 0)
	{
		goto RET;
	}

RET:
	LOG("exit");
	return ret;
}
