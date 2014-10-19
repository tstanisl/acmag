#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "cstr.h"
#include "value.h"

int acs_call(struct acs_value *val, int argin, int argout);
int acs_call_by_name(char *fname, int argin, int argout);

void acs_push_num(float nval);
void acs_push_cstr(char *str);
void acs_push_str(struct str *sval);

const struct acs_value *acs_argv(int arg);
#define acs_argv_num(arg) value_to_num(acs_argv(arg))
#define acs_argv_str(arg) value_to_str(acs_argv(arg))
int acs_argc(void);

struct str *acs_pop_str(void);
float acs_pop_num(void);

struct acs_user_function {
	int (*call)(struct acs_user_function *);
	void (*cleanup)(struct acs_user_function *);
};

int acs_register_user_function(struct acs_user_function *ufunc, char *name);

#endif /* MACHINE_H */
