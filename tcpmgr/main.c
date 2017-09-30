#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "tcpmgr_global.h"
#include "tcpmgr_arg.h"

#include "debug.h"

int main(int argc, char* argv[])
{
	int iResult;
	int ret = 0;
	tcpmgr_arg_t arg;

	// Initial tcpmgr global variable
	memset(&tcpMgr, 0, sizeof(tcpmgr_t));

	// Set default argument
	tcpmgr_arg_set_default(&arg);

	// Parse argument
	iResult = tcpmgr_arg_parse(&arg, argc, argv);
	if(iResult < 0)
	{
		tcpmgr_arg_print_usage();
		ret = -1;
		goto RET;
	}
	else
	{
		tcpmgr_arg_print_summary(&arg);
	}

RET:
	LOG("exit");
	return ret;
}
