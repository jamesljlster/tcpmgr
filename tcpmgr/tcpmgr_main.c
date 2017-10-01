#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "tcpmgr.h"
#include "tcpmgr_global.h"

#include "debug.h"

/*
void* client_thread(void* arg);
void interrupt_close(int arg);

int tcpmgr_main(int argc, char* argv[])
{
	int i, tmpIndex;
	int ret = 0;
	tcpmgr_arg_t arg;
	sock_t clientSock;
	pthread_t clientTh;

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

	// Loop for accept clients
	while(tcpMgr.stop == 0)
	{
		// Accept client
		printf("Waiting for client connection...\n");
		clientSock = accept(tcpMgr.listenSock, NULL, NULL);
		if(clientSock < 0)
		{
			printf("Accept failed!\n");
			continue;
		}

		// Search empty entry
		tmpIndex = -1;
		for(i = 0; i < tcpMgr.mgrListLen; i++)
		{
			if(tcpMgr.mgrList[i].occupied == 0)
			{
				LOG("Entity %d available for client", i);
				tmpIndex = i;
				break;
			}
		}

		// Checking
		if(tmpIndex < 0)
		{
			// Reject connection
			printf("No more entity for client available. Connection rejected!\n");
			sock_close(clientSock);
		}
		else
		{
			// Create client thread
			if(pthread_create(&clientTh, NULL, client_thread, &tcpMgr.mgrList[i]) < 0)
			{
				printf("Client thread initialization failed! Connection rejected!\n");
				sock_close(clientSock);
			}
			else
			{
				tcpMgr.mgrList[i].tHandle = clientTh;
				tcpMgr.mgrList[i].clientSock = clientSock;
				tcpMgr.mgrList[i].occupied = 1;
			}
		}
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

	LOG("exit");
	exit(0);
}
*/
