#include "lxr.h"
#include "debug.h"

#include <stdlib.h>

/**
 * struct lxr - LeXeR object
 * @file - input stream with source code
 * @line - current line in source code
 * @size - capacity of a buffer including 0-term
 * @data - token content
 */
struct lxr {
        FILE *file;
	int line;
	int size;
	char data[];
};

struct lxr *lxr_create(FILE *file, int max_token_size)
{
	struct lxr *lxr = malloc(sizeof (*lxr) + max_token_size);

	if (ERR_ON(!lxr, "malloc() failed"))
		return NULL;

	lxr->line = 1;
	lxr->size = max_token_size;
	lxr->file = file;

	return lxr;
}

void lxr_destory(struct lxr *lxr)
{
	free(lxr);
}

char *lxr_buffer(struct lxr *lxr)
{
	return lxr->data;
}

int lxr_line(struct lxr *lxr)
{
	return lxr->line;
}
