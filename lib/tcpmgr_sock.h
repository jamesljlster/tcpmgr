#ifndef __TCPMGR_SOCK_H__
#define __TCPMGR_SOCK_H__

// Socket type and close method
#if defined(_WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET sock_t;
#define sock_close(fd)	closesocket(fd)

#elif defined(__unix__)

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define sock_close(fd)	close(fd)
typedef int sock_t;

#else
#error Unexpected building system
#endif

#endif
