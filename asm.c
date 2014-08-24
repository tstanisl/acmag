#define _BSD_SOURCE

#include "debug.h"
#include "list.h"

#include <ctype.h>
#include <endian.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC "ACSC"
#define MAGIC_LEN 4
#define MAX_OFFSETS (1 << 12)

struct function {
	bool exported;
	char *name;
	struct list node;
};

static int cur_line = 0;
static DEFINE_LIST(function_head);

static struct function *function_create(char *name, bool exported)
{
	struct function *f = malloc(sizeof *f);

	if (ERR_ON(!f, "malloc() failed"))
		return NULL;

	f->name = strdup(name);
	if (ERR_ON(!f->name, "strdup() failed")) {
		free(f);
		return NULL;
	}

	f->exported = exported;

	return f;
}

static char *skipws(char *s)
{
	while (isspace(*s))
		++s;
	return s;
}

static int acsa_export(char *str)
{
	struct function *f = function_create(str, true);

	if (ERR_ON(!f, "function_create(\"%s\") failed", str))
		return -1;

	list_add(&function_head, &f->node);

	return 0;
}

static int acsa_line(char *str)
{
	str = skipws(str);
	if (*str == ';') // skip comment line
		return 0;
	char cmd[16];
	int ret, shift = 0;
	ret = sscanf(str, "%15s%n", cmd, &shift);
	if (ret == 0) // no command
		return 0;
	str += shift;
	if (strcmp(cmd, "export") == 0)
		return acsa_export(str);
	ERR("unknown keyword '%s'", cmd);
	return -1;
	/*if (strcmp(cmd, "push")
		return asm_push(str);
	if (strcmp(cmd, "pushn")
		return asm_push(str);
	if (strcmp(cmd, "callb")
		return asm_callb(str);
	if (strcmp(cmd, "call")
		return asm_call(str);*/
}

static int acsa_load(FILE *f)
{
	char buf[1024];
	int ret = 0;
	for (cur_line = 1; !ret && fgets(buf, 1024, f); ++cur_line)
		ret = acsa_line(buf);
	return ret;
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) {
		FILE *f = fopen(argv[i], "r");
		if (ERR_ON(!f, "fopen(\"%s\") failed: %s", argv[i], ERRSTR))
			return -1;
		int ret = acsa_load(f);
		if (ERR_ON(ret, "while processing \"%s\"", argv[i]))
			return -1;
		fclose(f);
	}
	return 0;
}
