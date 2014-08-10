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

static uint32_t get_uint(FILE *f)
{
	uint32_t value;
	fread(&value, sizeof(value), 1, f);
	return le32toh(value);
}

static int load(FILE *f)
{
	int ret;
	char buf[MAGIC_LEN];

	ret = fread(buf, MAGIC_LEN, 1, f);
	if (ERR_ON(ret != MAGIC_LEN, "failed to load magic"))
		return -EINVAL;

	ret = strncmp(buf, MAGIC, MAGIC_LEN);
	if (ERR_ON(ret, "not a compiled Acmag Script"))
		return -EINVAL;

	long pos = ftell(f);
	uint32_t header_size = get_uint(f);
	if (ERR_ON(ferror(f), "no header size at %ld", pos))
		return -EINVAL;

	WARN_ON(header_size < 4 || header_size > 1024,
		"a strange header size (%u)", (unsigned)header_size);

	// skip header
	ret = fseek(f, header_size - 4, SEEK_CUR);
	if (ERR_ON(ret, "fseek() failed: %s\n", ERRSTR))
		return -EIO;

	return 0;
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
	}
	return 0;
}
