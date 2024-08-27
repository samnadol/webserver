#ifndef FILE_H_
#define FILE_H_

typedef struct
{
	char *content;
	const char *mime_type;
	long long unsigned int size;
	int exists;
} file_content;

int file_exists(char *path);
file_content read_file(char *path);

#endif /* FILE_H_ */