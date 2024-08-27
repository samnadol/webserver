
#include "include/server.h"
#include "include/debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// define server_constructor method that returns server_struct. sets variables and sets up server.address. then starts listening on port and address.
Server server_constructor(int domain, int type, int protocol, u_long iface, int port, int backlog, void (*run)(Server *server))
{
	Server server;

	server.domain = domain;
	server.type = type;
	server.protocol = protocol;

	server.iface = iface;
	server.port = port;
	server.backlog = backlog;

	server.address.sin_family = server.domain;
	server.address.sin_port = htons(server.port);
	server.address.sin_addr.s_addr = htonl(server.iface);

	server.run = run;

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		exit(1);
	}
#endif

	server.socket = socket(server.domain, server.type, server.protocol);
	if (server.socket < 0)
	{
		printf("CRITICAL: failed to create socket\n");
		exit(1);
	}

	setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, (char *)&(int){1}, sizeof(int));

	if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
	{
		printf("CRITICAL: failed to bind socket\n");
		exit(1);
	}

	if (listen(server.socket, server.backlog) < 0)
	{
		printf("CRITICAL: failed to start listening\n");
		exit(1);
	}

	return server;
}