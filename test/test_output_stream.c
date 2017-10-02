#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tcpmgr.h>
#include <tcpmgr_private.h>
#include <tcpmgr_sock.h>

#define DEBUG
#include <debug.h>

#define SERVER_LOG	"server.log"

void client_task(void* arg, int socket);

tcpmgr_t mgr = NULL;

int main()
{
	int i;
	int ret;
	char tmpRead;
	char buf[100];
	FILE* outStream = NULL;

	// Open output stream file
	outStream = fopen(SERVER_LOG, "w");
	if(outStream == NULL)
	{
		printf("Failed to open %s\n", SERVER_LOG);
		ret = -1;
		goto RET;
	}

	// Initial tcpmgr
	ret = tcpmgr_create(&mgr, "0.0.0.0", 27015, 5);
	if(ret < 0)
	{
		printf("tcpmgr_init() failed with error: %d\n", ret);
		goto RET;
	}

	// Set output stream
	tcpmgr_set_output_stream(mgr, outStream);

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
	if(outStream != NULL)
	{
		fclose(outStream);
	}
	tcpmgr_delete(mgr);
	return ret;
}

void client_task(void* arg, int socket)
{
	sock_close(socket);
}
