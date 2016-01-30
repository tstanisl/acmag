#ifndef STR_H
#define STR_H __FILE__

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
struct str *str_get(struct str *s);
void str_put(struct str *s);

#endif
