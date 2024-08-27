/* modified from https://gist.github.com/danieloneill/492420111421047a6b926c81feb7ced7 */

#include "include/mime.h"
#include "include/debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct s_mimetype *mimes = NULL;

void mime_init(const char *filepath)
{
	FILE *fh = fopen(filepath, "r");
	if (!fh)
	{
		printf("CRITICAL: failed to read %s\n", filepath);
		exit(1);
	}
	char *mimeraw = (char *)malloc(MIMETYPE_MAX + 1);
	size_t rin = fread(mimeraw, 1, MIMETYPE_MAX, fh);
	mimeraw[rin] = '\0';
	fclose(fh);

	bool onext = false;
	bool comment = false;
	struct s_mimetype *mime = (struct s_mimetype *)malloc(sizeof(struct s_mimetype));
	memset(mime, 0, sizeof(struct s_mimetype));

	for (unsigned long x = 0; mime && x < rin; x++)
	{
		char ch = mimeraw[x];
		if (ch == '#')
			comment = true;
		else if (ch == '\r' || ch == '\n')
		{
			if (mime->mimetype[0] == '\0')
			{
				// it's empty.
				onext = false;
				comment = false;
				continue;
			}

			// done this entry, move along now.
			mime->next = mimes;
			mimes = mime;
			mime = NULL;
			mime = (struct s_mimetype *)malloc(sizeof(struct s_mimetype));
			memset(mime, 0, sizeof(struct s_mimetype));
			onext = false;
			comment = false;
			continue;
		}
		else if (comment)
			continue;
		else if (onext)
		{
			int extnum = mime->extensioncount - 1;
			int extlen = strlen(mime->extensions[extnum]);
			if (ch == ' ' || ch == '\t')
			{
				// next one, or we're in padding still..
				if (extlen == 0)
					continue;

				if (mime->extensioncount + 1 >= MIMETYPE_EXT_MAX_COUNT)
				{
					// At our limit.
					comment = true;
					continue;
				}

				mime->extensioncount++;
				extnum++;
				mime->extensions[extnum] = (char *)malloc(MIMETYPE_EXT_MAX_LENGTH + 1);
				mime->extensions[extnum][0] = '\0';
				continue;
			}
			else if (extlen < MIMETYPE_EXT_MAX_LENGTH)
			{
				mime->extensions[extnum][extlen] = ch;
				extlen++;
				mime->extensions[extnum][extlen] = '\0';
			}
		}
		else
		{
			unsigned long mlen = strlen(mime->mimetype);
			if (mlen >= sizeof(mime->mimetype))
			{
				// At capacity!
				comment = true;
				continue;
			}
			else if (ch == ' ' || ch == '\t')
			{
				onext = true;
				int extnum = mime->extensioncount;
				mime->extensions[extnum] = (char *)malloc(MIMETYPE_EXT_MAX_LENGTH + 1);
				mime->extensions[extnum][0] = '\0';
				mime->extensioncount++;
				continue;
			}
			mime->mimetype[mlen] = ch;
			mlen++;
			mime->mimetype[mlen] = '\0';
		}
	}

	if (mime->mimetype[0] != '\0')
	{
		mimes->next = mime;
		mimes = mime;
	}
	else
		free(mime);

	free(mimeraw);
	DEBUG_PRINT("initialized from %s\n", filepath);
}

void mime_close()
{
	struct s_mimetype *mnext;
	for (struct s_mimetype *m = mimes; m;)
	{
		mnext = m->next;
		for (int x = 0; x < m->extensioncount; x++)
			free(m->extensions[x]);
		free(m);
		m = mnext;
	}
}

const char *mime_get(const char *ext)
{
	if (!ext)
		goto ret;
		
	for (struct s_mimetype *m = mimes; m; m = m->next)
	{
		for (int x = 0; x < m->extensioncount; x++)
		{
			if (strcmp(ext, m->extensions[x]) == 0)
			{
				return m->mimetype;
			}
		}
	}
ret:
	return "application/octet-stream";
}

/*
int main()
{
	int countmime = 0;

	mime_init("mime.types");
	for (struct s_mimetype *m = mimes; m; m = m->next)
	{
		printf("%s =>", m->mimetype);
		for (int x = 0; x < m->extensioncount; x++)
			printf(" %s", m->extensions[x]);

		countmime++;
		printf("\n");
	}
	printf("%d total\n", countmime);
	mime_close();
}
*/