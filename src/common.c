#include "common.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void stackdump(void)
{
	void *buffer[32];
	int depth;

	depth = backtrace(buffer, ARRAY_SIZE(buffer));

	int fd = fileno(stderr);

	backtrace_symbols_fd(buffer, depth, fd);
}

void *ac_alloc(unsigned size)
{
	void *ptr = calloc(1, size);
	if (ptr)
		return ptr;

	ERR("calloc(%u) failed", size);

	stackdump();
	exit(-1);
}
