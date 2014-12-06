#include "ast.h"
#include "common.h"
#include "debug.h"
#include "lxr.h"
#include "cstr.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static struct ast *ast_new(enum token id)
{
	struct ast *t = ac_alloc(sizeof *t);
	t->id = id;
	return t;
}

void ast_free(struct ast *t)
{
	if (!t || t->id == TOK_NULL) {
		return;
	} else if (t->id == TOK_STR || t->id == TOK_ID) {
		str_put(t->u.sval);
	} else if (t->id == TOK_NUM) {
		;
	} else {
		ast_free(t->u.arg[0]);
		ast_free(t->u.arg[1]);
	}
	free(t);
}

static void do_ast_dump(struct ast *t, int level)
{
	if (!t) {
		return;
	} else if (t->id == TOK_STR || t->id == TOK_ID) {
		printf("%*s%s:%s\n", 2 * level, "",
			token_str[t->id], t->u.sval->str);
	} else if (t->id == TOK_NUM) {
		printf("%*s%s:%f\n", 2 * level, "",
			token_str[t->id], t->u.nval);
	} else {
		do_ast_dump(t->u.arg[1], level + 1);
		printf("%*s%s\n", 2 * level, "", token_str[t->id]);
		do_ast_dump(t->u.arg[0], level + 1);
	}
}

void ast_dump(struct ast *t)
{
	do_ast_dump(t, 0);
}

struct parser {
	char *path;
	struct lxr *lxr;
	enum token next;
};

static void perr(struct parser *p, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", p->path, lxr_line(p->lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);
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
	if (p->next == TOK_SCOLON) {
		consume(p);
		static struct ast null = { .id = TOK_NULL };
		return &null;
	}
	return NULL;
}

struct ast *parse_sequence(struct parser *p)
{
	struct ast *t = ast_new(TOK_SCOLON);
	t->u.arg[0] = parse_inst(p);
	if (!t->u.arg[0])
		return ast_free(t), NULL;
	t->u.arg[1] = parse_sequence(p);
	return t;
}

struct ast *ast_from_file(FILE *file, char *path)
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
