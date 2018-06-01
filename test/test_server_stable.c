#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include <tcpmgr_sock.h>

#define CONNECTION 10000

sock_t sockSvc;

int main(int argc, char* argv[])
{
	int i;
	int iResult;
	int serverPort;
	char* serverIP, *tmpPtr;
	struct sockaddr_in addrInfo;

#ifdef _WIN32
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != 0)
	{
		printf("Failed to initial WinSock!\n");
		return -1;
	}
#endif

	// Checking
	if(argc < 3)
	{
		printf("test_connection <server_ip> <server_port>\n");
		return -1;
	}
	else
	{
		serverIP = argv[1];
		serverPort = strtol(argv[2], &tmpPtr, 10);
		if(tmpPtr == argv[2])
		{
			printf("Failed to convert %s to serverPort", argv[2]);
			return -1;
		}

		printf("Using serverIP: %s, serverPort: %d\n", argv[1], serverPort);
	}

	for(i = 0; i < CONNECTION; i++)
	{
		// Create socket
		sockSvc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockSvc < 0)
		{
			printf("Failed to create socket\n");
			return -1;
		}

		// Connect to server
		addrInfo.sin_family = AF_INET;
		addrInfo.sin_addr.s_addr = inet_addr(serverIP);
		addrInfo.sin_port = htons(serverPort);
		iResult = connect(sockSvc, (struct sockaddr*)&addrInfo, sizeof(struct sockaddr));
		if(iResult < 0)
		{
			printf("Failed to connect %s:%d\n", serverIP, serverPort);
			return -1;
		}
	}

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

