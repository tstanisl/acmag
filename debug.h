#ifndef DEBUG_H
#define DEBUG_H __FILE__

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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

#endif /* DEBUG_H */
