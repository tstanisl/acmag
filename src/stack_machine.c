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

struct function {
	// list of constants
	// list of upvalues
	// list of arguments
	// code
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

#define CSTACK_SIZE (1 << 8)

static struct st_entry {
	uint16_t *code;
	int pc;
	int sp;
	struct value *consts;
	int argin;
	int argout;
} call_stack[CSTACK_SIZE];

int execute(struct st_entry *st_entry)
{
	int pc = st_entry->pc;
	uint16_t 
	for (int pc = 0; ; ++pc) {
		int op = code[pc] >> 12;
		int arg = code[pc] & 4095;

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
			pc += arg - 2048;
		} else if (op == OP_JNZ) {
			--sp;
			bool cond = value_to_bool(&st[sp]);
			value_clear(&st[sp]);
			if (cond)
				pc += arg - 2048;
		} else if (op == OP_JZ) {
			--sp;
			bool cond = value_to_bool(&st[sp]);
			value_clear(&st[sp]);
			if (!cond)
				pc += arg - 2048;
		}
	}
}

