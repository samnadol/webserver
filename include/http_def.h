#ifndef HTTP_DEF_H_
#define HTTP_DEF_H_

#include "include/file.h"

// http request types

typedef enum
{
	GET,
	HEAD,
	UNDEFINED
} REQUEST_TYPE;

// request and response info

typedef struct
{
	int http_code;
	file_content content;
	int content_length;
	char *date;
} response_info;

typedef struct
{
	REQUEST_TYPE request_type;
	char *path;
	char *http_version;
	int valid;
	char *query;
} request_info;

// FUNCTIONS

REQUEST_TYPE string_to_request_type(const char *str);
const char *request_type_to_string(REQUEST_TYPE type);
const char *http_code_to_message(int code);

#endif /* HTTP_DEF_H_ */