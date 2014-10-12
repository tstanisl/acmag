#include "debug.h"
#include "list.h"
#include "lxr.h"
#include "syntax.h"
#include "vec.h"

#include <stdarg.h>
#include <stdlib.h>

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

struct opcode {
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
	struct value *consts;
	int n_args;
	uint16_t *code;
	struct list node;
};

struct acs_user_function {
	int (*call)(struct acs_user_function *, struct acs_stack *);
	void (*cleanup)(struct acs_user_function *);
};

struct acs_finstance;

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
	struct value *consts;
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

#define DATAST_SIZE (64 * CSTACK_SIZE)
static struct value datast[DATAST_SIZE];
static int datasp = 0;

static int new_callst(int arg)
{
	int argin = arg & ARGMASK;
	int argout = arg >> ARGBITS;
}

enum base {
	BS_ADD2,
};

static int execute_base(enum base cmd)
{
#define ST(n) datast[datasp - n]
	switch (cmd) {
	case BS_ADD2:
		value_convert_num(ST(1));
		value_convert_num(ST(2));
		ST(2).u.nval += ST(1).u.nval;
		value_clear(&ST(1));
		--datasp;
		break;
	}
#undef ST
}

int execute()
{
	struct callst *ctx = &callst[0];
	int pc = 0;
	int maxpc = vec_size(ctx->code);
	struct value *st = data_stack;
	for (;;) {
		CRIT_ON(pc < 0 || pc >= maxpc, "invalid program counter");

		int code = ctx->code[pc];
		int op = code >> STBITS;
		int arg = code & STMASK;
		++pc;

		if (op == OP_NOP) {
			/* nothing to do */
		} else if (op == OP_PUSHC) {
			value_copy(&st[sp++], &ctx->consts[arg]);
		} else if (op == OP_PUSHN) {
			sp += arg;
		} else if (op == OP_PUSHS) {
			value_copy(&st[sp], &st[sp - arg]);
			++sp;
		} else if (op == OP_PUSHI) {
			st[sp].id = VAL_NUM;
			st[sp].u.nval = arg;
			++sp;
		} else if (op == OP_POPN) {
			/* consider freeing only during rewriting */
			while (arg--)
				value_clear(&st[--sp]);
		} else if (op == OP_POPS) {
			--sp;
			value_copy(&st[sp - arg], &st[sp]); 
			value_clear(&st[sp]);
		} else if (op == OP_BASE) {
			execute_base(arg);
		} else if (op == OP_CALL) {
			/* TODO: nothing by now */
		} else if (op == OP_RET) {
			/* TODO: nothing by now */
		} else if (op == OP_JMP) {
			pc += arg - PC_OFFSET;
		} else if (op == OP_JNZ) {
			--sp;
			bool cond = value_to_bool(&st[sp]);
			value_clear(&st[sp]);
			if (cond)
				pc += arg - PC_OFFSET;
		} else if (op == OP_JZ) {
			--sp;
			bool cond = value_to_bool(&st[sp]);
			value_clear(&st[sp]);
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
	struct acs_finstance *fi = val->fval;
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
	execute();
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

int acs_stack_init(struct acs_stact *st)
{
	st->dummy = 0;
}

void acs_stack_deinit(struct acs_stack *st) { /* stub */ }

int acs_push_num(struct acs_stack *st, float nval)
{
	datast[datasp].id = VAL_INT;
	datast[datasp].u.nval = nval;
	++datasp;
	return 0;
}

int acs_push_str(struct acs_stack *st, char *str)
{
	struct str *sval = str_create(str);
	if (ERR_ON(!sval, "str_create() failed"))
		return -1;
	datast[datasp].id = VAL_STR;
	datast[datasp].u.sval = sval;
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

