#include "ast.h"
#include "common.h"
#include "debug.h"
#include "lxr.h"

#include <stdarg.h>
#include <stdio.h>

struct parser {
	char *path;
	struct lxr *lxr;
	enum token next;
};

static int perr(struct parser *p, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", p->path, lxr_line(p->lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);

	return -1;
}

static void consume(struct parser *p)
{
	p->next = lxr_get(p->lxr);
	printf("next = %s\n", token_str[p->next]);
	if (p->next == TOK_ERR)
		perr(p, "%s", lxr_buffer(p->lxr));
}

struct ast *parse_inst(struct parser *p)
{
	return NULL;
}

struct ast *parse_sequence(struct parser *p)
{
	struct ast *top = NULL;
	for (;;) {
		struct ast *inst = parse_inst(p);
		if (!inst)
			return top;
		if (!top) {
			top = inst;
		} else  {
			struct ast *ntop = ac_alloc(sizeof *ntop);
			ntop->id = ACS_SEQ;
			ntop->u.arg[0] = top;
			ntop->u.arg[1] = inst;
			top = ntop;
		}
	}
}

struct ast *parse_file(FILE *file, char *path)
{
	struct parser p = { .path = path };

	p.lxr = lxr_create(file, 256);
	if (ERR_ON(!p.lxr, "lxr_create() failed"))
		return NULL;

	consume(&p);

	struct ast *ret = parse_sequence(&p);
	lxr_destroy(p.lxr);
	if (ERR_ON(!ret, "parse_sequence() failed"))
		return NULL;

	return ret;
}
