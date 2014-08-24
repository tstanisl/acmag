#define _BSD_SOURCE

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

static int __info(int cond, const char *prefix, char *f_name, int line,
	const char *fmt, ...)
{
	int errsv = errno;
	if (!cond)
		return 0;
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "%s(%s:%d): ", prefix, f_name, line);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
	va_end(va);
	errno = errsv;
	return 1;
}

#define ERR_ON(cond, ...) \
	__info((cond), "Error", __FILE__, __LINE__, __VA_ARGS__)
#define ERR(...) ERR_ON(1, __VA_ARGS__)
#define WARN_ON(cond, ...) \
	__info((cond), "Warning", __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) WARN_ON(1, __VA_ARGS__)
#define ERRSTR strerror(errno)

static int cur_line = 0;

static char *skipws(char *s)
{
	while (isspace(*s))
		++s;
	return s;
}

static int asm_line(char *str)
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
		return asm_export(str);
	if (strcmp(cmd, "push")
		return asm_push(str);
	if (strcmp(cmd, "pushn")
		return asm_push(str);
	if (strcmp(cmd, "callb")
		return asm_callb(str);
	if (strcmp(cmd, "call")
		return asm_call(str);
	
}

static int acs_asm_load(FILE *f)
{
	char buf[1024];
	int ret = 0;
	for (cur_line = 1; !ret && fgets(buf, 1024, f); ++cur_line)
		ret = asm_line(buf);
	return ret;
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) {
		FILE *f = fopen(argv[i], "r");
		if (ERR_ON(!f, "fopen(\"%s\") failed: %s", argv[i], ERRSTR))
			return -1;
		int ret = load(f);
		if (ERR_ON(ret, "while processing \"%s\"", argv[i]))
			return -1;
		fclose(f);
	}
	return 0;
}
