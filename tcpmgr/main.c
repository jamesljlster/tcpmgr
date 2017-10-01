#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "tcpmgr.h"
#include "tcpmgr_global.h"

#include "debug.h"

void interrupt_close(int arg);

int main(int argc, char* argv[])
{
	int ret = 0;
	tcpmgr_arg_t arg;

	// Set signal handler
	signal(SIGINT, interrupt_close);

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
		goto RET;
	}

	// Create clean task
	ret = pthread_create(&cleanTask, NULL, tcpmgr_clean_task, &tcpMgr);
	if(ret < 0)
	{
		printf("Thread initialization failed!\n");
		cleanTaskStatus = 0;
		goto RET;
	}
	else
	{
		cleanTaskStatus = 1;
	}

RET:
	interrupt_close(0);
	return ret;
}

void interrupt_close(int arg)
{
	LOG("enter");

	if(cleanTaskStatus > 0)
	{
		// Cancel and join clean task thread
		pthread_cancel(cleanTask);
		pthread_join(cleanTask, NULL);
	}

	tcpmgr_server_cleanup(&tcpMgr);
	tcpmgr_cleanup(&tcpMgr);

	exit(0);
}

