#include "include/config.h"
#include "include/date.h"
#include "include/debug.h"
#include "include/file.h"
#include "include/http_def.h"
#include "include/http_error.h"
#include "include/mime.h"
#include "include/server.h"
#include "include/str.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#define MAX_FILE_PATH_LEN 8192
#define MAX_REQUEST_BACKLOG 255

int port;
int max_request_content_size;
int max_response_header_size;

config_file cfg;

int read_from_socket(int socket, char *buffer, int max_size)
{
	if (recv(socket, buffer, max_size, 0) < 0)
	{
		printf("READ FROM SOCK FAILED\n");
		return 0;
	}

	if (!buffer)
	{
		printf("REQUEST NULL\n");
		return 0;
	}

	return 1;
}

void get_ip_from_socket(struct sockaddr *sock_addr, char *ip_str)
{
	struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	inet_ntop(AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN);
}

request_info parse_reqline(char *reqline)
{
	if (!reqline)
	{
		request_info result;
		result.valid = 0;
		return result;
	}

	DEBUG_PRINT("%s\n", reqline);

	char *edit = calloc(strlen(reqline) + 1, sizeof(char));
	strcpy(edit, reqline); //, strlen(reqline));

	request_info request;
	request.valid = 1;

	request.request_type = string_to_request_type(strtok(edit, " "));

	char *path = strtok(NULL, " ");
	char *http_version = strtok(NULL, " ");

	request.path = strbeforefirst(path, "?");
	request.query = strafterfirst(path, "?");

	request.http_version = strdup(http_version);

	if (!request.path || strcmp(request.path, "/") == 0)
	{
		free(request.path);
		request.path = strdup("/index.html");
	}

	free(edit);
	return request;
}

response_info generate_response(request_info request, char *file_path)
{
	response_info response = {};

	if (request.request_type == UNDEFINED)
	{
		DEBUG_PRINT("returning 501\n");
		response = http_code_to_response(501);
	}
	else
	{
		if (file_exists(file_path))
		{
			DEBUG_PRINT("returning 200, reading file\n");
			response.content = read_file(file_path);

			response.http_code = 200;
			response.content_length = response.content.size;
			if (request.request_type == HEAD)
				response.content.size = 0;
		}
		else
		{
			DEBUG_PRINT("returning 404\n");
			response = http_code_to_response(404);
		}
	}

	DEBUG_PRINT("generating date\n");
	response.date = generate_date_string();

	return response;
}

void free_request_info(request_info info)
{
	free(info.http_version);
	free(info.path);
	free(info.query);
}

void free_response_info(response_info info)
{
	free(info.date);
	if (info.http_code == 200)
		free(info.content.content);
}

void *handle_connection(void *vargp)
{
	clock_t start = clock();

	connection_input input = *((connection_input *)vargp);
	int conn_socket = input.conn_socket;
	struct sockaddr *sock_addr = input.sock_addr;

	DEBUG_PRINT("reading from socket\n");
	char *req = calloc(max_request_content_size, sizeof(char));
	if (!read_from_socket(conn_socket, req, max_request_content_size))
	{
		goto exit_socket;
	}

	DEBUG_PRINT("getting client IP\n");
	char ip_str[INET_ADDRSTRLEN];
	get_ip_from_socket(sock_addr, ip_str);

	DEBUG_PRINT("parsing http request\n");
	char *request_text = strreplace(strreplace(req, "\r\n", "\n", 0), "\n\n", "|", 1);
	free(req);

	DEBUG_PRINT("splitting http request\n");
	char *REQLINE = strtok(request_text, "\n");
	/*char *HEADERS = */ strtok(NULL, "|");
	char *REQBODY = strtok(NULL, "|");

	if (REQBODY == NULL)
		REQBODY = "";

	DEBUG_PRINT("parsing reqline\n");
	request_info request = parse_reqline(REQLINE);
	if (!request.valid)
	{
		printf("REQUEST CANCELLED\n");
		goto exit_reqtext;
	}

	printf("%s\t%s\t\t%s\t\t%s\n", ip_str, request_type_to_string(request.request_type), request.path, request.query);

	char *file_path = calloc(MAX_FILE_PATH_LEN, sizeof(char));
	snprintf(file_path, MAX_FILE_PATH_LEN, "%s%s", "webroot", request.path);
	response_info response = generate_response(request, file_path);
	free(file_path);

	// generate response http packet
	DEBUG_PRINT("generating response http\n");
	long long unsigned int packet_response_size = max_response_header_size + strlen(response.date) + response.content.size;
	DEBUG_PRINT("sizes %d %llu %llu total %llu\n", max_response_header_size, (unsigned long long)strlen(response.date), response.content.size, packet_response_size);

	char *packet_response = calloc(packet_response_size, sizeof(char));
	if (!packet_response)
	{
		printf("could not alloc ram for packet response, size %llu (%p)\n", packet_response_size, packet_response);
		goto exit_response;
	}

	snprintf(packet_response, packet_response_size, "HTTP/1.0 %d %s\r\nServer: c-webserver 0.1\r\nContent-Length: %d\r\nContent-Type: %s\r\nDate: %s\r\n\r\n", response.http_code, http_code_to_message(response.http_code), response.content_length, response.content.mime_type, response.date);

	// adjust size of packet_response_size to match actual header size instead of max
	DEBUG_PRINT("adjusting packet size from %llu to %llu\n", packet_response_size, strlen(packet_response) + response.content.size);
	packet_response_size = strlen(packet_response) + response.content.size;

	// copy response contents to packet_response
	DEBUG_PRINT("copying response content to packet\n");
	if (request.request_type != HEAD)
		memcpy(&packet_response[strlen(packet_response)], response.content.content, response.content.size);

	// send html and close socket
	DEBUG_PRINT("sending response\n");

	clock_t stop = clock();
	double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
	printf("%d\t\t%s\t%llu\t\t(%fms)\n", response.http_code, response.content.mime_type, packet_response_size, elapsed);

	if (send(conn_socket, packet_response, packet_response_size, 0) < 0)
	{
		printf("error while writing\n");
	}

	// free memory
	DEBUG_PRINT("freeing pointers\n");
	free(packet_response);
exit_response:
	free_response_info(response);
	free_request_info(request);
exit_reqtext:
	free(request_text);

exit_socket:
	DEBUG_PRINT("closing socket\n");
#ifdef _WIN32
	closesocket(conn_socket);
#else
	close(conn_socket);
#endif

	DEBUG_PRINT("exiting\n");
	return 0;
}

void run(Server *server)
{
	while (1)
	{
		// set variables and stuff
		struct sockaddr *sock_addr = (struct sockaddr *)&server->address;
		socklen_t address_length = (socklen_t)sizeof(server->address);

		// accept and read from socket
		int conn_socket = accept(server->socket, sock_addr, &address_length);
		if (conn_socket < 0)
			printf("Error on accept\n");

		// dispatch function to parse and repsond to request (TODO: MULTITHREADING)
		connection_input input = {
			.conn_socket = conn_socket,
			.sock_addr = sock_addr,
		};
		handle_connection((void *)&input);
	}
}

void sigintHandle()
{
	printf("\nCaught CTRL+C\n");

	// free remaining pointers
	config_close(cfg);
	mime_close();

	printf("Exiting\n");
	exit(0);
}

int main()
{
	signal(SIGINT, sigintHandle);

	DEBUG_PRINT("c-webserver 0.1 compiled for ");
#ifdef _WIN32
	DEBUG_PRINT_NOINFO("windows\n");
#else
	DEBUG_PRINT_NOINFO("linux / darwin\n");
#endif

	DEBUG_PRINT("open config file\n");
	cfg = config_open("server.conf");

	DEBUG_PRINT("load config values\n");
	port = config_read_int(cfg, "port");
	max_request_content_size = config_read_int(cfg, "max_request_content_size");
	max_response_header_size = config_read_int(cfg, "max_response_header_size");

	DEBUG_PRINT("checking valid config int values\n");
	assert(port < __INT_MAX__);
	assert(max_request_content_size < __INT_MAX__);
	assert(max_response_header_size < __INT_MAX__);

	DEBUG_PRINT("initialize mime\n");
	char *mime_loc = config_read_string(cfg, "mime_types_location");
	mime_init(mime_loc ? mime_loc : "mime.types");
	free(mime_loc);

	// create server struct with port
	DEBUG_PRINT("initialize server\n");
	Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, MAX_REQUEST_BACKLOG, run);

	// run function as defined in struct
	printf("http://127.0.0.1:%d/\n", port);
	server.run(&server);
}