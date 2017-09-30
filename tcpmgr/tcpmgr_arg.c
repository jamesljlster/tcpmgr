#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr_arg.h"

int tcpmgr_arg_find_id(const char* arg);

const char* tcpmgr_arg_list[] = {
	"--host-ip",
	"--host-port",
	"--max-client"
};

int tcpmgr_arg_parse(tcpmgr_arg_t* argPtr, int argc, char* argv[])
{
	int i;
	int ret = 0;
	int argId;
	int tmp;
	char* tmpPtr;

	for(i = 1; i < argc; i++)
	{
		argId = tcpmgr_arg_find_id(argv[i]);
		switch(argId)
		{
			case TCPMGR_ARG_HOST_IP:
				if(i < argc -1)
				{
					argPtr->hostIP = argv[i + 1];
				}
				else
				{
					printf("Too few arguments for %s!\n", tcpmgr_arg_list[TCPMGR_ARG_HOST_IP]);
					ret = -1;
					goto RET;
				}
				break;

			case TCPMGR_ARG_HOST_PORT:
				if(i < argc - 1)
				{
					tmp = strtol(argv[i + 1], &tmpPtr, 10);
					if(tmpPtr == argv[i + 1])
					{
						printf("Failed to convert \'%s\' to server port!\n", tmpPtr);
						ret = -1;
						goto RET;
					}
					else
					{
						argPtr->hostPort = tmp;
					}
				}
				else
				{
					printf("Too few arguments fot %s!\n", tcpmgr_arg_list[TCPMGR_ARG_HOST_PORT]);
					ret = -1;
					goto RET;
				}
				break;

			case TCPMGR_ARG_MAX_CLIENT:
				if(i < argc - 1)
				{
					tmp = strtol(argv[i + 1], &tmpPtr, 10);
					if(tmpPtr == argv[i + 1])
					{
						printf("Failed to convert \'%s\' to max client!\n", tmpPtr);
						ret = -1;
						goto RET;
					}
					else
					{
						argPtr->maxClient = tmp;
					}
				}
				else
				{
					printf("Too few arguments fot %s!\n", tcpmgr_arg_list[TCPMGR_ARG_MAX_CLIENT]);
					ret = -1;
					goto RET;
				}
				break;
		}
	}

RET:
	return ret;
}

void tcpmgr_arg_print_usage()
{
	printf("Program arguments:\n");
	printf("    %s: Host IP Address\n", tcpmgr_arg_list[TCPMGR_ARG_HOST_IP]);
	printf("    %s: Host Port\n", tcpmgr_arg_list[TCPMGR_ARG_HOST_PORT]);
	printf("    %s: Maximum Clients", tcpmgr_arg_list[TCPMGR_ARG_MAX_CLIENT]);
}

void tcpmgr_arg_print_summary(tcpmgr_arg_t* argPtr)
{
	printf("Server Setting:\n");
	printf("    Host IP Address: %s\n", argPtr->hostIP);
	printf("    Host Port: %d\n", argPtr->hostPort);
	printf("    Maximum Clients: %d\n", argPtr->maxClient);
}

void tcpmgr_arg_set_default(tcpmgr_arg_t* argPtr)
{
	argPtr->hostIP = "0.0.0.0";
	argPtr->hostPort = 27015;
	argPtr->maxClient = 5;
}

int tcpmgr_arg_find_id(const char* arg)
{
	int i;
	int ret = -1;
	int cmpLen;

	for(i = 0; i < TCPMGR_ARG_AMOUNT; i++)
	{
		cmpLen = strlen(tcpmgr_arg_list[i]);
		if(strlen(arg) != cmpLen)
		{
			continue;
		}
		else if(strncmp(arg, tcpmgr_arg_list[i], cmpLen) == 0)
		{
			ret = i;
			break;
		}
	}

	return ret;
}
