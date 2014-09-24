#ifndef CSTR_H
#define CSTR_H __FILE__

char *cstr_create(char *str);
char *cstr_reserve(int size);
char *cstr_get(char *str);
void cstr_put(char *str);

#endif
