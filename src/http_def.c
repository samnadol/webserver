#include "include/http_def.h"
#include "include/str.h"

#include <string.h>
#include <stdlib.h>

static const struct
{
	REQUEST_TYPE request_type;
	const char *str;
} __REQUEST_TYPE_STRUCT[] = {
	{GET, "GET"},
	{HEAD, "HEAD"},
};

static const struct
{
	const int code;
	const char *message;
} __HTTP_CODE_MESSAGES[] = {
	{200, "OK"},
	{404, "Not Found"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
};

REQUEST_TYPE string_to_request_type(const char *str)
{
	for (unsigned long i = 0; i < (sizeof(__REQUEST_TYPE_STRUCT) / sizeof(__REQUEST_TYPE_STRUCT[0])); i++)
	{
		if (!strcmp(str, __REQUEST_TYPE_STRUCT[i].str))
		{
			return __REQUEST_TYPE_STRUCT[i].request_type;
		}
	}
	return UNDEFINED;
}

const char *request_type_to_string(REQUEST_TYPE type)
{
	for (unsigned long i = 0; i < (sizeof(__REQUEST_TYPE_STRUCT) / sizeof(__REQUEST_TYPE_STRUCT[0])); i++)
	{
		if (type == __REQUEST_TYPE_STRUCT[i].request_type)
		{
			return __REQUEST_TYPE_STRUCT[i].str;
		}
	}
	return "UNDEFINED";
}

const char *http_code_to_message(int code)
{
	for (unsigned long i = 0; i < (sizeof(__HTTP_CODE_MESSAGES) / sizeof(__HTTP_CODE_MESSAGES[0])); i++)
	{
		if (code == __HTTP_CODE_MESSAGES[i].code)
		{
			return __HTTP_CODE_MESSAGES[i].message;
		}
	}
	return "OK";
}