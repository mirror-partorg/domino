#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED



#ifdef WINSOCK
#include	<winsock2.h>
typedef SOCKET socket_t;
typedef SOCKADDR_IN sockaddr_in_t;
typedef int socklen_t;
typedef SOCKADDR sockaddr_t;
#define SOCKERROR_SUFFIX "%d", WSAGetLastError()
#define iserr_socket(a) (a == (SOCKET_ERROR))
#define invalid_socket(a) (a == INVALID_SOCKET)
#define	close_socket(sd)	closesocket(sd)
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
typedef int socket_t;
typedef struct sockaddr_in sockaddr_in_t;
typedef socklen_t socklen_t;
typedef struct sockaddr sockaddr_t;
#define SOCKERROR_SUFFIX "%s", strerror(errno)
#define iserr_socket(a) (a < 0)
#define invalid_socket(a) (a <= 0)
#define INVALID_SOCKET 0
#define	close_socket(sd) close(sd)
#endif


#endif // SOCKET_H_INCLUDED
