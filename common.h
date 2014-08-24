#ifndef COMMON_H
#define COMMON_H __FILE__

#include <stddef.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

#ifndef container_of
#define container_of(ptr,type,member) \
	(((type)*)(((const char*)ptr) - offsetof(type,member)))
#endif

#endif /* COMMON_H */
