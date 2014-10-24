#ifndef FUNCTION_H
#define FUNCTION_H

#include "value.h"

#include <stdint.h>
#include <stdbool.h>

struct acs_function {
	struct acs_value *consts;
	int n_args;
	uint16_t *code;
};

struct acs_upvalue {
	int refcnt;
	struct acs_value val;
};

struct acs_finstance {
	bool ufunc;
	union {
		struct acs_function *func;
		struct acs_user_function *ufunc;
	} u;
	int refcnt;
	int n_upvalues;
	struct acs_upvalue *upvalues[];
};

#endif /* FUNCTION_H */
