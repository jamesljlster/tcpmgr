#include <tcpmgr.h>

int foo()
{
	return 0;
}

/*
void* client_thread(void* arg)
{
	struct TCPMGR_LIST* listPtr = arg;
	sock_close(listPtr->clientSock);

	// Clean task
	pthread_cond_signal(&tcpMgr.cond);
	listPtr->closeJoin = 1;
	pthread_exit(NULL);
}
*/

int main()
{
	return 0;
}
