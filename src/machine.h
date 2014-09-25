#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "syntax.h"

struct acs_stack;

int acs_push_int(struct acs_stack *st, int value);
int acs_push_str(struct acs_stack *st, char *str);

typedef int (*acs_user_function_cb)(struct acs_stack *st, void *priv);
int machine_add_function(char *name, int n_args, acs_user_function_cb *fptr);

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st);

enum acs_type {
	VAL_NULL = 0,
	VAL_BOOL,
	VAL_NUM,
	VAL_STR,
	VAL_VAR,
	VAL_FUNC,
	__VAL_MAX,
};

struct acs_var;

struct acs_value {
	enum acs_type id;
	struct acs_value *next; // used only by assignment
	union {
		int ival;
		struct str *sval;
		bool bval;
		struct acs_function *fval;
		struct acs_var *vval;
	} u;
};

#endif /* MACHINE_H */
