#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "syntax.h"
#include "value.h"

struct acs_stack;

int acs_push_int(struct acs_stack *st, int value);
int acs_push_str(struct acs_stack *st, char *str);

typedef int (*acs_user_function_cb)(struct acs_stack *st, void *priv);
int machine_add_function(char *name, int n_args, acs_user_function_cb *fptr);

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st);

struct acs_user_function {
	struct acs_value *(*call)(struct acs_user_function *, struct acs_value *);
};

int register_user_function(struct acs_user_function *ufunc, char *name);

#endif /* MACHINE_H */
