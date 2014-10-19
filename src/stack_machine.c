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
	OP_PUSHR,
	OP_PUSHI,
	OP_PUSHN,
	OP_POPN,
	OP_POPR,
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
	int fp;
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

static inline struct acs_value *ntop(void)
{
	return &datast[datasp];
}

static inline struct acs_value *top(void)
{
	return &datast[datasp - 1];
}

static inline void push(struct acs_value *val)
{
	value_copy(&datast[datasp++], val);
}

static inline void pop(void)
{
	value_clear(&datast[--datasp]);
}

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

static int call_base(enum base cmd)
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

static void do_return(int argout)
{
	int src = datasp - argout;
	int dst = current()->argp;
	for (; argout--; ++src, ++dst) {
		value_clear(&datast[dst]);
		value_copy(&datast[dst], &datast[src]);
	}
	while (datasp > dst)
		pop();
	--callsp;
}

static int push_call(int argin, int argout)
{
	if (callsp >= ARRAY_SIZE(callst)) {
		ERR("call stack exceeded");
		/* TODO; add acs_stackdump() */
		return -1;
	}
	/* move call stack pointer by 1 */
	++callsp;
	struct callst *cs = current();

	cs->sp = datasp;
	cs->fp = datasp;
	cs->argin = argin;
	cs->argout = argout;

	return 0;
}

static int call(struct acs_value *val, int argin, int argout)
{
	if (ERR_ON(val->id != VAL_FUNC, "calling non-function value"))
		return -1;

	struct acs_finstance *fi = val->u.fval;

	if (push_call(argin, argout) != 0)
		return -1;

	struct callst *cs = current();

	if (fi->ufunc) {
		struct acs_user_function *ufunc = fi->u.ufunc;
		int ret = ufunc->call(ufunc);
		int real_argout = cs->sp - cs->fp;
		do_return(real_argout);
		return ret;
	}

	// TODO: construct closure values
	struct acs_function *func = fi->u.func;

	cs->code = func->code;
	cs->pc = 0;
	cs->consts = func->consts;

	return 0;
}

int execute(void)
{
	int start_datasp = datasp;
	int start_callsp = callsp;
	for (;;) {
		struct callst *cs = current();

		int code = cs->code[cs->pc];
		int op = code >> STBITS;
		int arg = code & STMASK;
		++cs->pc;

		if (op == OP_NOP) {
			/* nothing to do */
		} else if (op == OP_PUSHC) {
			push(&cs->consts[arg]);
		} else if (op == OP_PUSHN) {
			datasp += arg;
		} else if (op == OP_PUSHR) {
			push(&datast[cs->fp + arg]);
		} else if (op == OP_PUSHI) {
			ntop()->id = VAL_NUM;
			ntop()->u.nval = arg;
			++datasp;
		} else if (op == OP_POPN) {
			/* consider freeing only during rewriting */
			while (arg--)
				pop();
		} else if (op == OP_POPR) {
			value_copy(&datast[cs->fp + arg], top());
			pop();
		} else if (op == OP_BSCALL) {
			call_base(arg);
		} else if (op == OP_CALL) {
			int argin = arg & ARGMASK;
			int argout = arg >> ARGBITS;
			struct acs_value *val = &datast[cs->sp - argin];
			if (call(val, argin, argout) == 0)
				continue;
			/* FIXME: this cleanup is probably totally wrong */
			while (datasp > start_datasp)
				pop();
			callsp = start_callsp;
			return -1;
		} else if (op == OP_RET) {
			do_return(arg);
			if (callsp < start_callsp)
				return 0;
		} else if (op == OP_JMP) {
			cs->pc += arg - PC_OFFSET;
		} else if (op == OP_JNZ) {
			bool cond = value_to_bool(top());
			pop();
			if (cond)
				cs->pc += arg - PC_OFFSET;
		} else if (op == OP_JZ) {
			bool cond = value_to_bool(top());
			pop();
			if (!cond)
				cs->pc += arg - PC_OFFSET;
		}
	}
}

struct acs_varmap extern_vars;

int acs_call(struct acs_value *val, int argin, int argout)
{
	return -1;
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

int acs_register_user_function(struct acs_user_function *ufunc, char *name)
{
	struct acs_value *val = varmap_insert(&extern_vars, name);
	if (ERR_ON(!val, "varmap_insert() failed"))
		return -1;

	/* remove previous content of extern variable */
	value_clear(val);
	struct acs_finstance *fi = ac_alloc(sizeof *fi);
	fi->ufunc = true;
	fi->u.ufunc = ufunc;
	fi->refcnt = 1;

	return 0;
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
	float a = acs_argv_num(0);
	float b = acs_argv_num(1);
	acs_push_num(a + b);
	return 0;
}

static int print_call(struct acs_user_function *ufunc)
{
	int argc = acs_argc();
	for (int i = 1; i <= argc; ++i) {
		struct str *str = acs_argv_str(i);
		printf("%s", str->str);
		str_put(str);
	}
	return 0;
}

void acs_init(void)
{
	static bool initialized;
	if (initialized)
		return;
	initialized = true;

	varmap_init(&extern_vars);

	static struct acs_user_function print_ufunc = { .call = print_call };
	acs_register_user_function(&print_ufunc, "print");

	/* TODO: add test */
}
