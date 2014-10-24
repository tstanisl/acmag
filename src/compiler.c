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
};

struct inst {
	struct list node;
	enum opcode op;
	int arg;
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
	if (c->next == TOK_NULL)
		emit(c, OP_PUSHN, 1);
	else if (c->next == TOK_TRUE)
		emit(c, OP_BSCALL, BS_TRUE);
	else if (c->next == TOK_FALSE)
		emit(c, OP_BSCALL, BS_FALSE);
	else
		return perr(c, "unexpected token %s", token_str[c->next]);
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

struct acs_finstance *acs_compile_file(FILE *file, char *path)
{
	struct compiler c = { .path = path };
	list_init(&c.inst);

	c.lxr = lxr_create(file, 256);
	if (ERR_ON(!c.lxr, "lxr_create() failed"))
		goto fail;

	consume(&c);
	while (c.next != TOK_EOF) {
		int ret = compile_inst(&c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			goto fail_lxr;
	}

	/* TODO: solve names, generate closure code etc */
	static struct acs_finstance phony;
	memset(&phony, 0, sizeof phony);
	return &phony;

fail_lxr:
	lxr_destroy(c.lxr);
fail:
	return NULL;
}
