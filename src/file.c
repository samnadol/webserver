#include "include/file.h"
#include "include/str.h"
#include "include/mime.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int file_exists(char *path)
{
	return access(path, F_OK) == 0;
}

file_content read_file(char *path)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL)
	{
		file_content r = {
			.exists = 0,
		};
		return r;
	}

	fseek(f, 0, SEEK_END);
	long long unsigned int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	file_content r;
	char *string = calloc(fsize, sizeof(char));
	char *ext = strafterlast(path, ".");

	if (fread(string, fsize, 1, f))
	{
		r.content = string;
		r.mime_type = ext ? mime_get(ext) : "application/octet-stream";
		r.exists = 1;
		r.size = fsize;
	}
	else
	{
		r.size = 0;
		r.exists = 0;
	}
	free(ext);

	fclose(f);
	return r;
}