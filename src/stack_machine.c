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

struct function_instance {
	struct value *consts;
	struct value *closures;
	char **args;
	uint16_t *code;
};

enum valtype {
	VAL_NULL = 0,
	VAL_BOOL,
	VAL_NUM,
	VAL_STR,
	VAL_FUNC,
	VAL_USER,
	VAL_OBJ,
	__VAL_MAX,
};

struct value {
	enum valtype id;
	union {
		bool bval;
		float nval;
		struct str *sval;
		struct function *fval;
		struct object *oval;
	} u;
};

#define ARGBITS 6
#define ARGMASK ((1 << ARGBITS) - 1)
#define STBITS 12
#define STMASK ((1 << STBITS) - 1)
#define PC_OFFSET (1 << (STBITS - 1))

#define CALLST_SIZE (1 << 8)
static struct callst {
	uint16_t *code;
	int pc, maxpc;
	int sp;
	int fp;
	struct value *consts;
	int argin;
	int argout;
} callst[CALLST_SIZE];
static int callsp = 0;

#define DATAST_SIZE (64 * CSTACK_SIZE)
static struct value datast[DATAST_SIZE];
static int datasp = 0;

static int new_callst(int arg)
{
	int argin = arg & ARGMASK;
	int argout = arg >> ARGBITS;
}

int execute(/* function should be here */)
{
	struct callst *ctx = &callst[0];
	int pc = 0;
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
			value_copy(&st[sp++], &consts[arg]);
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
		} else if (op == OP_BASE) {
			execute_base(arg, st, sp);
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

void usage(void)
{
	struct acs_stack st;
	acs_stack_init(&st);
	acs_push_int(&st, 5);
	acs_push_str(&st, "hello");
	acs_execute_function("foo", &st);
	int val = acs_pop_int(&st);
	struct str *str = acs_pop_str(&st);
	printf("foo(%d, \"%s\") = %d, \"%s\"\n", 5, "hello", val, str->str);
}

