#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tcpmgr.h>
#include <tcpmgr_private.h>
#include <tcpmgr_sock.h>

#define DEBUG
#include <debug.h>

void client_task(void* arg, int socket, tcpmgr_info_t cInfo);

tcpmgr_t mgr = NULL;

int main()
{
	int i;
	int ret;
	char tmpRead;
	char buf[100];

	// Initial tcpmgr
	ret = tcpmgr_create(&mgr, "0.0.0.0", 27015, 1);
	if(ret < 0)
	{
		printf("tcpmgr_init() failed with error: %d\n", ret);
		goto RET;
	}

	// Start tcpmgr
	ret = tcpmgr_start(mgr, client_task, NULL);
	if(ret < 0)
	{
		printf("tcpmgr_start() failed with error: %d\n", ret);
		goto RET;
	}

	while(1)
	{
		for(i = 0; i < 100; i++)
		{
			ret = scanf("%c", &tmpRead);
			if(ret <= 0)
			{
				continue;
			}

			if(tmpRead == '\n')
			{
				buf[i] = '\0';
				break;
			}
			else
			{
				buf[i] = tmpRead;
			}
		}

		if(strcmp(buf, "stop") == 0)
		{
			break;
		}
	}

	// Stop tcpmgr
	tcpmgr_stop(mgr);

RET:
	tcpmgr_delete(mgr);
	return ret;
}

void client_task(void* arg, int socket, tcpmgr_info_t cInfo)
{
	const char* buf = "Hello world\n";
	send(socket, buf, strlen(buf), 0);
}
