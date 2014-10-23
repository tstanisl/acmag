#include "acs.h"
#include "debug.h"
#include "function.h"
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

char *opcode_str[] = {
	[OP_NOP] = "NOP",
	[OP_PUSHC] = "PUSHC",
	[OP_PUSHR] = "PUSHR",
	[OP_PUSHI] = "PUSHI",
	[OP_PUSHN] = "PUSHN",
	[OP_PUSHU] = "PUSHU",
	[OP_POPN] = "POPN",
	[OP_POPR] = "POPR",
	[OP_POPU] = "POPU",
	[OP_BSCALL] = "BSCALL",
	[OP_CALL] = "CALL",
	[OP_RET] = "RET",
	[OP_JMP] = "JMP",
	[OP_JNZ] = "JNZ",
	[OP_JZ] = "JZ",
};

struct callst {
	uint16_t *code;
	struct acs_value *consts;
	struct acs_value *upvalues;
	int pc;
	int sp;
	int fp;
	int argp;
	int argin;
	int argout;
};

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

#define TOP(n) (&datast[datasp - (n)])
#define PUSH(val) value_copy(&datast[datasp++], (val))
#define POP() value_clear(&datast[--datasp])

static void bscall_arith2(enum bscall cmd)
{
	float b = value_to_num(TOP(1));
	POP();
	float a = value_to_num(TOP(1));
	POP();
	++datasp;
	TOP(1)->id = VAL_NUM;

	if (cmd == BS_ADD)
		TOP(1)->u.nval = a + b;
	else if (cmd == BS_SUB)
		TOP(1)->u.nval = a - b;
	else if (cmd == BS_MUL)
		TOP(1)->u.nval = a * b;
	else if (cmd == BS_DIV)
		TOP(1)->u.nval = a / b;
	else if (cmd == BS_MOD)
		TOP(1)->u.nval = (int)a % (int)b;
	else
		CRIT("not supported bscall = %d", (int)cmd);
}

static void bscall_cmp(enum bscall cmd)
{
	int cmp = value_cmp(&datast[datasp - 2], &datast[datasp - 1]);
	/* compute sign() */
	cmp = cmp > 0 ? 1 : (cmp < 0 ? -1 : 0);

	POP(); POP();

	static bool result[][3] = {
		[BS_LESS - __BS_CMP] = {true, false, false},
		[BS_GREAT - __BS_CMP] = {false, false, true},
		[BS_EQ - __BS_CMP] = {false, true, false},
		[BS_NEQ - __BS_CMP] = {true, false, true},
		[BS_LEQ - __BS_CMP] = {true, true, false},
		[BS_GREQ - __BS_CMP] = {false, true, true},
	};

	++datasp;
	TOP(1)->id = VAL_BOOL;
	TOP(1)->u.bval = result[cmd - __BS_CMP][cmp + 1];
}

static void bscall(enum bscall cmd)
{
	if (cmd >= __BS_ARITH2 && cmd < __BS_ARITH2_MAX) {
		bscall_arith2(cmd);
	} else if (cmd >= __BS_CMP && cmd < __BS_CMP_MAX) {
		bscall_cmp(cmd);
	} else if (cmd == BS_ARGV) {
		int arg = value_to_num(TOP(1));
		POP();
		PUSH(acs_argv(arg));
	} else if (cmd == BS_ARGC) {
		++datasp;
		TOP(1)->id = VAL_NUM;
		TOP(1)->u.nval = acs_argc();
	} else {
		CRIT("not supported bscall %d", (int)cmd);
	}
}

static void do_return(int argout)
{
	int src = datasp - argout;
	int dst = current()->argp - 1;
	for (; argout--; ++src, ++dst) {
		value_clear(&datast[dst]);
		value_copy(&datast[dst], &datast[src]);
	}
	while (datasp > dst)
		POP();
	--callsp;
}

static int call(struct acs_value *val, int argin, int argout)
{
	if (ERR_ON(val->id != VAL_FUNC, "calling non-function value"))
		return -1;

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
	cs->argp = datasp - argin;

	struct acs_finstance *fi = val->u.fval;
	if (fi->ufunc) {
		struct acs_user_function *ufunc = fi->u.ufunc;
		int ret = ufunc->call(ufunc);
		int real_argout = cs->sp - cs->fp;
		do_return(real_argout);
		return ret;
	}

	struct acs_function *func = fi->u.func;

	cs->code = func->code;
	cs->pc = 0;
	cs->consts = func->consts;
	cs->upvalues = fi->upvalues;

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
#if 0
		printf("stack=");
		for (int i = 0; i < datasp; ++i)
			printf(" %s", value_to_cstr(&datast[i]));
		puts("");
		printf("%04x: %s %d\n", cs->pc, opcode_str[op], arg);
#endif
		++cs->pc;
		cs->sp = datasp;

		if (op == OP_NOP) {
			/* nothing to do */
		} else if (op == OP_PUSHC) {
			PUSH(&cs->consts[arg]);
		} else if (op == OP_PUSHN) {
			datasp += arg;
		} else if (op == OP_PUSHR) {
			PUSH(&datast[cs->fp + arg]);
		} else if (op == OP_PUSHI) {
			TOP(0)->id = VAL_NUM;
			TOP(0)->u.nval = arg;
			++datasp;
		} else if (op == OP_PUSHU) {
			PUSH(&cs->upvalues[arg]);
		} else if (op == OP_POPN) {
			/* consider freeing only during rewriting */
			while (arg--)
				POP();
		} else if (op == OP_POPR) {
			value_clear(&datast[cs->fp + arg]);
			value_copy(&datast[cs->fp + arg], TOP(1));
			POP();
		} else if (op == OP_POPU) {
			value_clear(&cs->upvalues[arg]);
			value_copy(&cs->upvalues[arg], TOP(1));
			POP();
		} else if (op == OP_BSCALL) {
			bscall(arg);
		} else if (op == OP_CALL) {
			int argin = arg & ARGMASK;
			int argout = arg >> ARGBITS;
			struct acs_value *val = &datast[cs->sp - argin - 1];
			if (call(val, argin, argout) == 0)
				continue;
			/* FIXME: this cleanup is probably totally wrong */
			while (datasp > start_datasp)
				POP();
			callsp = start_callsp;
			return -1;
		} else if (op == OP_RET) {
			do_return(arg);
			if (callsp < start_callsp)
				return 0;
		} else if (op == OP_JMP) {
			cs->pc += arg - PC_OFFSET;
		} else if (op == OP_JNZ) {
			bool cond = value_to_bool(TOP(1));
			POP();
			if (cond)
				cs->pc += arg - PC_OFFSET;
		} else if (op == OP_JZ) {
			bool cond = value_to_bool(TOP(1));
			POP();
			if (!cond)
				cs->pc += arg - PC_OFFSET;
		}
	}
}

struct acs_varmap global_vars;

void acs_call_head(struct acs_value *val)
{
	WARN_ON(val->id != VAL_FUNC, "value is not a function");
	PUSH(val);
}

int acs_call_tail(int argin, int argout)
{
	call(TOP(argin + 1), argin, argout);
	return execute();
}

int acs_call_head_by_name(char *fname)
{
	struct acs_value *val = varmap_find(&global_vars, fname);
	if (ERR_ON(!val, "failed to find function %s", fname))
		return -1;
	acs_call_head(val);
	return 0;
}

void acs_push_num(float nval)
{
	TOP(0)->id = VAL_NUM;
	TOP(0)->u.nval = nval;
	++datasp;
}

void acs_push_cstr(char *str)
{
	TOP(0)->id = VAL_STR;
	TOP(0)->u.sval = str_create(str);
	++datasp;
}

void acs_push_str(struct str *sval)
{
	TOP(0)->id = VAL_STR;
	TOP(0)->u.sval = str_get(sval);
	++datasp;
}

const struct acs_value *acs_argv(int arg)
{
	static struct acs_value null = { .id = VAL_NULL };
	struct callst *cs = current();
	if (arg < 0 || arg >= cs->argin)
		return &null;
	return &datast[cs->argp + arg];
}

int acs_argc(void)
{
	return current()->argin;
}

struct str *acs_pop_str(void)
{
	struct str *str = value_to_str(TOP(1));
	/* TODO: add check if datasp got below fp */
	POP();
	return str;
}

float acs_pop_num(void)
{
	float num = value_to_num(TOP(1));
	POP();
	return num;
}

struct acs_value *acs_global(char *name)
{
	return varmap_find(&global_vars, name);
}

int acs_register_user_function(struct acs_user_function *ufunc, char *name)
{
	struct acs_value *val = varmap_insert(&global_vars, name);
	if (ERR_ON(!val, "varmap_insert() failed"))
		return -1;

	/* remove previous content of global variable */
	value_clear(val);
	struct acs_finstance *fi = ac_alloc(sizeof *fi);
	fi->ufunc = true;
	fi->u.ufunc = ufunc;
	fi->refcnt = 1;

	val->id = VAL_FUNC;
	val->u.fval = fi;

	return 0;
}

static int print_call(struct acs_user_function *ufunc)
{
	int argc = acs_argc();
	for (int i = 0; i < argc; ++i) {
		char *str = value_to_cstr(acs_argv(i));
		printf("%s", str);
	}
	puts("");
	return 0;
}

static void machine_deinit(void)
{
	varmap_deinit(&global_vars);
}

void acs_init(void)
{
	static bool initialized;
	if (initialized)
		return;
	initialized = true;

	varmap_init(&global_vars);

	atexit(machine_deinit);

	static struct acs_user_function print_ufunc = { .call = print_call };
	acs_register_user_function(&print_ufunc, "print");
}
