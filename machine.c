#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC "ACMS"

static int __info(int cond, const char *prefix, char *f_name, int line,
	const char *fmt, ...)
{
	if (!cond)
		return 0;
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "%s(%s:%d): ", prefix, f_name, line);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
	va_end(va);
	return 1;
}

#define ERR_ON(cond, ...) \
	__info((cond), "Error", __FILE__, __LINE__, __VA_ARGS__)

static int load(FILE *f)
{
}

int main()
{
	ERR_ON(1, "bye");
}
