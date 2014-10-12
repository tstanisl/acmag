#include "debug.h"
#include "list.h"
#include "lxr.h"
//#include "syntax.h"
#include "vec.h"
#include "cstr.h"
#include "value.h"

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
	OP_BASE,
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
	int (*call)(struct acs_user_function *, struct acs_stack *);
	void (*cleanup)(struct acs_user_function *);
};

#if 0
enum acs_type {
	VAL_NULL = 0,
	VAL_BOOL,
	VAL_NUM,
	VAL_STR,
	VAL_FUNC,
	VAL_OBJ,
	__VAL_MAX,
};

struct acs_value {
	enum acs_type id;
	union {
		bool bval;
		float nval;
		struct str *sval;
		struct acs_finstance *fval;
		struct object *oval;
	} u;
};
#endif

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
	int pc;
	int sp;
	int fp;
	struct acs_value *consts;
	int argin;
	int argout;
};

#define ARGBITS 6
#define ARGMASK ((1 << ARGBITS) - 1)
#define STBITS 12
#define STMASK ((1 << STBITS) - 1)
#define PC_OFFSET (1 << (STBITS - 1))

#define CALLST_SIZE (1 << 8)
static struct callst callst[CALLST_SIZE];
static int callsp = 0;

#define DATAST_SIZE (64 * CALLST_SIZE)
static struct acs_value datast[DATAST_SIZE];
static int datasp = 0;

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
};

static void value_clear(struct acs_value *val)
{
	if (val->id == VAL_STR && val->u.sval)
		str_put(val->u.sval);
	if (val->id == VAL_OBJ && val->u.oval)
		object_put(val->u.oval);
	/* FIXME: what about function instance */
	memset(&val->u, 0, sizeof val->u);
}

void value_convert_num(struct acs_value *val)
{
	float nval = 0;
	if (val->id == VAL_NUM)
		nval = val->u.nval;
	else if (val->id == VAL_BOOL)
		nval = (val->u.bval ? 1 : 0);
	else if (val->id == VAL_STR)
		nval = atof(val->u.sval->str);
	value_clear(val);
	val->id = VAL_NUM;
	val->u.nval = nval;
}

static bool value_to_bool(struct acs_value *val)
{
	if (!val)
		return false;
	if (val->id == VAL_NULL)
		return false;
	if (val->id == VAL_BOOL)
		return val->u.bval;
	return true;
}

static void value_convert_bool(struct acs_value *val)
{
	bool bval = value_to_bool(val);
	value_clear(val);
	val->id = VAL_BOOL;
	val->u.bval = bval;
}

static void value_copy(struct acs_value *dst,
	struct acs_value *src)
{
	WARN_ON(src->id != VAL_NULL, "copying to non-NULL");
	if (src->id == VAL_STR)
		str_get(src->u.sval);
	if (src->id == VAL_OBJ)
		object_get(src->u.oval);
	dst->u = src->u;
	dst->id = src->id;
}

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

int execute()
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
		} else if (op == OP_BASE) {
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

int acs_call(struct acs_value *val, struct acs_stack *st)
{
	if (val->id != VAL_FUNC) {
		ERR("calling non-function value");
		return -1;
	}
	struct acs_finstance *fi = val->u.fval;
	if (fi->ufunc)
		return fi->u.ufunc->call(fi->u.ufunc, st);
	// TODO: construct closure values
	struct acs_function *f = fi->u.func;
	struct callst *cst = &callst[0];
	cst->code = f->code;
	cst->pc = 0;
	cst->sp = datasp;
	cst->fp = datasp;
	cst->consts = f->consts;
	return execute();
}

int acs_call_by_name(char *fname, struct acs_stack *st)
{
	struct acs_value *val = varmap_find(&extern_vars, fname);
	if (ERR_ON(!val, "failed to find function %s", fname))
		return -1;
	if (ERR_ON(val->id != VAL_FUNC, "%s is not a function", fname))
		return -1;
	return acs_call(val, st);
}

struct acs_stack {
	int dummy;
};

int acs_stack_init(struct acs_stack *st)
{
	st->dummy = 0;
}

void acs_stack_deinit(struct acs_stack *st) { /* stub */ }

int acs_push_num(struct acs_stack *st, float nval)
{
	ST(0).id = VAL_NUM;
	ST(0).u.nval = nval;
	++datasp;
	return 0;
}

int acs_push_str(struct acs_stack *st, char *str)
{
	struct str *sval = str_create(str);
	if (ERR_ON(!sval, "str_create() failed"))
		return -1;
	ST(0).id = VAL_STR;
	ST(0).u.sval = sval;
	++datasp;
	return 0;
}

void usage(void)
{
	struct acs_stack st;
	acs_stack_init(&st);
	acs_push_int(&st, 5);
	acs_push_str(&st, "hello");
	acs_call_by_name("foo", &st);
	int val = acs_pop_int(&st);
	struct str *str = acs_pop_str(&st);
	printf("foo(%d, \"%s\") = %d, \"%s\"\n", 5, "hello", val, str->str);
	acs_stack_deinit(&st);
}

