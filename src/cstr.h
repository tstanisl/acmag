#ifndef CSTR_H
#define CSTR_H __FILE__

char *cstr_create(char *str);
char *cstr_reserve(int size);
char *cstr_get(char *str);
void cstr_put(char *str);

unsigned strhash(char *s);

struct str {
	unsigned refcnt;
	unsigned length;
	unsigned hash;
	char str[];
};

struct str *str_create(char *str);
struct str *str_reserve(int size);
void str_update(struct str *s);

static inline struct str *str_get(struct str *s)
{
	++s->refcnt;
	return s;
}

static inline void str_put(struct str *s)
{
	void free(void *);
	if (--s->refcnt == 0)
		free(s);
}

#endif
