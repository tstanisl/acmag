#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "syntax.h"

struct acs_stack;

int acs_push_int(struct acs_stack *st, int value);
int acs_push_str(struct acs_stack *st, char *str);

typedef int (*acs_user_function_cb)(struct acs_stack *st, void *priv);
int machine_add_function(char *name, int n_args, acs_user_function_cb *fptr);

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st);

#endif /* MACHINE_H */
