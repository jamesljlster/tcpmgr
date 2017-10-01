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

	// Initial tcpmgr
	ret = tcpmgr_init(&tcpMgr, &arg);
	if(ret < 0)
	{
		goto RET;
	}

	// Initial server service
	ret = tcpmgr_server_init(&tcpMgr, &arg);
	if(ret < 0)
	{
		goto CLEANUP;
	}

CLEANUP:
	tcpmgr_server_cleanup(&tcpMgr);

RET:
	tcpmgr_cleanup(&tcpMgr);

	LOG("exit");
	return ret;
}
