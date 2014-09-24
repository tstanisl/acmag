#include "common.h"
#include "cstr.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>

#define CSTR_MAGIC 0xc7b413d6

struct cstr {
	int refcnt;
	unsigned magic;
	char str[];
};

#define to_cstr(str) \
	container_of(str, struct cstr, str)

char *cstr_create(char *str)
{
	int length = strlen(str);
	struct cstr *cstr = malloc(sizeof (*cstr) + length + 1);
	if (ERR_ON(!cstr, "malloc() failed"))
		return NULL;

	cstr->refcnt = 1;
	cstr->magic = CSTR_MAGIC;
	strcat(cstr->str, str);

	return cstr->str;
}

char *cstr_get(char *str)
{
	struct cstr *cstr = to_cstr(str);

	CRIT_ON(cstr->magic != CSTR_MAGIC, "using invalid cstr!!!");
	CRIT_ON(cstr->refcnt <= 0, "invalid refcounting");

	++cstr->refcnt;

	return str;
}

void cstr_put(char *str)
{
	struct cstr *cstr = to_cstr(str);

	CRIT_ON(cstr->magic != CSTR_MAGIC, "using invalid cstr!!!");
	CRIT_ON(cstr->refcnt <= 0, "invalid refcounting");

	if (--cstr->refcnt)
		return;

	cstr->magic = 0;
	free(cstr);
}
