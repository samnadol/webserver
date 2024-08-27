#ifndef MIME_H_
#define MIME_H_

#define MIMETYPE_MAX 32768 * 4
#define MIMETYPE_EXT_MAX_LENGTH 16
#define MIMETYPE_EXT_MAX_COUNT 16

struct s_mimetype
{
	char mimetype[64];
	int extensioncount;
	char *extensions[MIMETYPE_EXT_MAX_COUNT];

	struct s_mimetype *next;
};

void mime_init(const char *filepath);
void mime_close();
const char *mime_get(const char *ext);

#endif /* MIME_H_ */