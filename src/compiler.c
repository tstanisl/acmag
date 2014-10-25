#include "cstr.h"
#include "debug.h"
#include "function.h"
#include "lxr.h"
#include "list.h"
#include "machine.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
	char *path;
	struct lxr *lxr;
	enum token next;
	struct list inst;
	int stsize;
	struct list consts;
};

struct inst {
	struct list node;
	enum opcode op;
	int arg;
};

struct constant {
	struct acs_value val;
	struct list node;
};

static int perr(struct compiler *c, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", c->path, lxr_line(c->lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);

	return -1;
}

static void consume(struct compiler *c)
{
	c->next = lxr_get(c->lxr);
	printf("next = %s\n", token_str[c->next]);
	if (c->next == TOK_ERR)
		perr(c, "%s", lxr_buffer(c->lxr));
}

static void emit(struct compiler *c, enum opcode op, int arg)
{
	struct inst *inst = ac_alloc(sizeof *inst);
	inst->op = op;
	inst->arg = arg;
	list_add_tail(&inst->node, &c->inst);
}

static int new_const_num(struct compiler *c, float nval)
{
	int idx = 0;
	list_foreach(l, &c->consts) {
		struct constant *cval = list_entry(l, struct constant, node);
		if (cval->val.id == VAL_NUM && cval->val.u.nval == nval)
			return idx;
		++idx;
	}
	struct constant *cval = ac_alloc(sizeof *cval);
	cval->val.id = VAL_NUM;
	cval->val.u.nval = nval;
	list_add_tail(&cval->node, &c->consts);
	return idx;
}

static int new_const_str(struct compiler *c, char *str)
{
	int idx = 0;
	list_foreach(l, &c->consts) {
		struct constant *cval = list_entry(l, struct constant, node);
		if (cval->val.id == VAL_STR &&
		    strcmp(cval->val.u.sval->str, str) == 0)
			return idx;
		++idx;
	}
	struct constant *cval = ac_alloc(sizeof *cval);
	cval->val.id = VAL_STR;
	cval->val.u.sval = str_create(str);
	list_add_tail(&cval->node, &c->consts);
	return idx;
}

static int compile_inst(struct compiler *c);

static int compile_block(struct compiler *c)
{
	CRIT_ON(c->next != TOK_LBRA, "unexpected token %s", token_str[c->next]);
	consume(c);
	while (c->next != TOK_RBRA) {
		int ret = compile_inst(c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			return -1;
	}
	consume(c);
	return 0;
}

static int compile_top(struct compiler *c)
{
	if (c->next == TOK_NULL) {
		emit(c, OP_PUSHN, 1);
	} else if (c->next == TOK_TRUE) {
		emit(c, OP_BSCALL, BS_TRUE);
	} else if (c->next == TOK_FALSE) {
		emit(c, OP_BSCALL, BS_FALSE);
	} else if (c->next == TOK_STR) {
		int idx = new_const_str(c, lxr_buffer(c->lxr));
		if (idx < 0)
			return -1;
		emit(c, OP_PUSHC, idx);
	} else if (c->next == TOK_NUM) {
		float nval = atof(lxr_buffer(c->lxr));
		int ival = (int)nval;
		if (nval == ival && ival >= 0 && ival < ARGMAX) {
			emit(c, OP_PUSHI, ival);
		} else {
			int idx = new_const_num(c, nval);
			if (idx < 0)
				return -1;
			emit(c, OP_PUSHC, idx);
		}
	} else {
		return perr(c, "unexpected token %s", token_str[c->next]);
	}
	consume(c);
	return 0;
}

static int compile_expr(struct compiler *c)
{
	c->stsize = 0;
	int ret = compile_top(c);
	return ret;
}

static int compile_inst(struct compiler *c)
{
	if (c->next == TOK_LBRA)
		return compile_block(c);
	int ret = compile_expr(c);
	if (ERR_ON(ret, "compile_expr() failed"))
		return -1;
	if (c->next != TOK_SCOLON)
		return perr(c, "missing ; after expression");
	consume(c);
	return 0;
}

static void dump_code(struct compiler *c)
{
	int pc = 0;
	list_foreach(l, &c->inst) {
		struct inst *inst = list_entry(l, struct inst, node);
		printf("%02d: %s %d\n", pc, opcode_str[inst->op], inst->arg);
		++pc;
	}
}

struct acs_finstance *acs_compile_file(FILE *file, char *path)
{
	struct compiler c = { .path = path };
	list_init(&c.inst);
	list_init(&c.consts);

	c.lxr = lxr_create(file, 256);
	if (ERR_ON(!c.lxr, "lxr_create() failed"))
		goto fail;

	consume(&c);
	while (c.next != TOK_EOF) {
		int ret = compile_inst(&c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			goto fail_lxr;
	}

	dump_code(&c);

	/* TODO: solve names, generate closure code etc */
	static struct acs_finstance phony;
	memset(&phony, 0, sizeof phony);
	return &phony;

fail_lxr:
	lxr_destroy(c.lxr);
fail:
	return NULL;
}
