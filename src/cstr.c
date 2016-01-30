#include "common.h"
#include "cstr.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>

unsigned strhash(char *s)
{
	unsigned hash;
	for (hash = 0; *s; ++s)
		hash = 33 * hash + *s;
	return hash;
}

struct str *str_create(char *str)
{
	int length = strlen(str);
	struct str *s = malloc(sizeof (*s) + length + 1);
	if (ERR_ON(!s, "malloc() failed"))
		return NULL;

	s->refcnt = 1;
	s->length = length;
	s->hash = strhash(str);
	strcpy(s->str, str);

	return s;
}

struct str *str_reserve(int size)
{
	struct str *s = calloc(1, sizeof (struct str) + size);
	if (ERR_ON(!s, "malloc() failed"))
		return NULL;
	s->refcnt = 1;
	return s;
}

void str_update(struct str *s)
{
	s->length = strlen(s->str);
	s->hash = strhash(s->str);
}

struct str *str_get(struct str *s)
{
	++s->refcnt;
	return s;
}

void str_put(struct str *s)
{
	if (--s->refcnt == 0)
		free(s);
}

