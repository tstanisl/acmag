#include "debug.h"
#include "list.h"
#include "lxr.h"
#include "machine.h"
#include "vec.h"
#include "cstr.h"
#include "value.h"
#include "varmap.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

enum opcode {
	OP_NOP = 0,
	OP_PUSHC,
	OP_PUSHS,
	OP_PUSHI,
	OP_PUSHN,
	OP_POPN,
	OP_POPS,
	OP_BSCALL,
	OP_CALL,
	OP_RET,
	OP_JMP,
	OP_JNZ,
	OP_JZ
};

struct inst {
	enum opcode op;
	int arg;
	int addr;
	struct list node;
};

struct parser {
	char *path;
	struct lxr *lxr;
	enum token next;
	struct list code;
};

struct acs_function {
	struct acs_value *consts;
	int n_args;
	uint16_t *code;
	struct list node;
};

struct acs_stack;

struct acs_user_function {
	int (*call)(struct acs_user_function *);
	void (*cleanup)(struct acs_user_function *);
};

struct acs_finstance {
	bool ufunc;
	union {
		struct acs_function *func;
		struct acs_user_function *ufunc;
	} u;
	int refcnt;
	int n_closures;
	struct acs_value closures[];
};

struct callst {
	uint16_t *code;
	struct acs_value *consts;
	int pc;
	int sp;
	int argp;
	int argin;
	int argout;
};

#define ARGBITS 6
#define ARGMASK ((1 << ARGBITS) - 1)
#define STBITS 12
#define STMASK ((1 << STBITS) - 1)
#define PC_OFFSET (1 << (STBITS - 1))

#define CALLST_SIZE 256
static struct callst callst[CALLST_SIZE];
static int callsp = 0;

#define DATAST_SIZE (64 * CALLST_SIZE)
static struct acs_value datast[DATAST_SIZE];
static int datasp = 0;

static struct callst *current(void)
{
	return &callst[callsp - 1];
}

/*
static int new_callst(int arg)
{
	int argin = arg & ARGMASK;
	int argout = arg >> ARGBITS;
}
*/

enum base {
	BS_ADD2,
	BS_SUB2,
	BS_MUL2,
	BS_DIV2,
	BS_MOD2,
	BS_GET_GLOBAL,
	BS_SET_GLOBAL,
	BS_GET_FIELD,
	BS_SET_FIELD,
	BS_ARGC,
	BS_ARG0,
	BS_ARGN,
};

#define ST(n) datast[datasp - n]

static int execute_base(enum base cmd)
{
	switch (cmd) {
	case BS_ADD2:
		value_convert_num(&ST(1));
		value_convert_num(&ST(2));
		ST(2).u.nval += ST(1).u.nval;
		value_clear(&ST(1));
		--datasp;
		break;
	default:
		ERR("unsupported base operation %d", (int)cmd);
		return -1;
	}
	return 0;
}

int execute(void)
{
	struct callst *ctx = &callst[0];
	int pc = 0;
	int maxpc = vec_size(ctx->code);
	for (;;) {
		CRIT_ON(pc < 0 || pc >= maxpc, "invalid program counter");

		int code = ctx->code[pc];
		int op = code >> STBITS;
		int arg = code & STMASK;
		++pc;

		if (op == OP_NOP) {
			/* nothing to do */
		} else if (op == OP_PUSHC) {
			value_copy(&ST(0), &ctx->consts[arg]);
			++datasp;
		} else if (op == OP_PUSHN) {
			datasp += arg;
		} else if (op == OP_PUSHS) {
			value_copy(&ST(0), &ST(arg));
			++datasp;
		} else if (op == OP_PUSHI) {
			ST(0).id = VAL_NUM;
			ST(0).u.nval = arg;
			++datasp;
		} else if (op == OP_POPN) {
			/* consider freeing only during rewriting */
			while (arg--)
				--datasp, value_clear(&ST(0));
		} else if (op == OP_POPS) {
			--datasp;
			value_copy(&ST(arg), &ST(0)); 
			value_clear(&ST(0));
		} else if (op == OP_BSCALL) {
			execute_base(arg);
		} else if (op == OP_CALL) {
			/* TODO: nothing by now */
		} else if (op == OP_RET) {
			/* TODO: nothing by now */
		} else if (op == OP_JMP) {
			pc += arg - PC_OFFSET;
		} else if (op == OP_JNZ) {
			bool cond = value_to_bool(&ST(1));
			value_clear(&ST(1));
			--datasp;
			if (cond)
				pc += arg - PC_OFFSET;
		} else if (op == OP_JZ) {
			bool cond = value_to_bool(&ST(1));
			value_clear(&ST(1));
			--datasp;
			if (!cond)
				pc += arg - PC_OFFSET;
		}
	}
}

struct acs_varmap extern_vars;

int acs_call(struct acs_value *val, int argin, int argout)
{
	if (val->id != VAL_FUNC) {
		ERR("calling non-function value");
		return -1;
	}
	struct acs_finstance *fi = val->u.fval;
	if (fi->ufunc)
		return fi->u.ufunc->call(fi->u.ufunc);
	if (callsp >= ARRAY_SIZE(callst)) {
		ERR("call stack exceeded");
		return -1;
	}

	// TODO: construct closure values
	struct acs_function *func = fi->u.func;
	struct callst *cst = &callst[callsp];
	cst->code = func->code;
	cst->pc = 0;
	cst->sp = datasp;
	cst->argp = datasp - argin;
	cst->argin = argin;
	cst->argout = argout;
	cst->consts = func->consts;
	/* move call stack pointer by 1 */
	++callsp;
	return execute();
}

int acs_call_by_name(char *fname, int argin, int argout)
{
	struct acs_value *val = varmap_find(&extern_vars, fname);
	if (ERR_ON(!val, "failed to find function %s", fname))
		return -1;
	if (ERR_ON(val->id != VAL_FUNC, "%s is not a function", fname))
		return -1;
	return acs_call(val, argin, argout);
}

void acs_push_num(float nval)
{
	ST(0).id = VAL_NUM;
	ST(0).u.nval = nval;
	++datasp;
}

void acs_push_cstr(char *str)
{
	ST(0).id = VAL_STR;
	ST(0).u.sval = str_create(str);
	++datasp;
}

void acs_push_str(struct str *sval)
{
	ST(0).id = VAL_STR;
	ST(0).u.sval = str_get(sval);
	++datasp;
}

const struct acs_value *acs_argv(int arg)
{
	static struct acs_value null = { .id = VAL_NULL };
	struct callst *cs = current();
	if (arg >= cs->argin)
		return &null;
	return &datast[cs->argp + arg];
}

int acs_argc(void)
{
	return current()->argin;
}

struct str *acs_pop_str(void)
{
	struct str *str = value_to_str(&ST(0));
	/* TODO: add check if datasp got below fp */
	--datasp;
	return str;
}

float acs_pop_num(void)
{
	float num = value_to_num(&ST(0));
	--datasp;
	return num;
}

void usage_embed(void)
{
	acs_push_num(5);
	acs_push_cstr("hello");
	acs_call_by_name("foo", 2, 2);
	float val = acs_pop_num();
	struct str *str = acs_pop_str();
	printf("foo(%d, \"%s\") = %g, \"%s\"\n", 5, "hello", val, str->str);
	str_put(str);
}

int usage_extend(struct acs_user_function *unused)
{
	float a = acs_arg_num(0);
	float b = acs_arg_num(1);
	acs_push_num(a + b);
	return 0;
}
