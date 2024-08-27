#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>

typedef struct
{
	FILE *file;
	const char *path;
} config_file;

config_file config_open(const char *path);
void config_close(config_file config);

char *config_read_string(config_file config, const char *key);
int config_read_int(config_file config, const char *key);

#endif /* CONFIG_H_ */