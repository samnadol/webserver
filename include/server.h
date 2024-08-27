#ifndef SERVER_H_
#define SERVER_H_

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif

typedef unsigned long u_long;

// define server struct with necessary data types
typedef struct Server
{
	int domain;
	int type;
	int protocol;
	int backlog;

	u_long iface;
	int port;
	struct sockaddr_in address;
	int socket;

	void (*run)(struct Server *server);
} Server;

typedef struct
{
	int conn_socket;
	struct sockaddr *sock_addr;
} connection_input;

Server server_constructor(int domain, int type, int protocol, u_long iface, int port, int backlog, void (*run)(Server *server));

#endif /* SERVER_H_ */