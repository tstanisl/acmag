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
	if (!t || t->id == TOK_NULL || t->id == TOK_TRUE || t->id == TOK_FALSE) {
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
	p->next = TOK_ERR;
}

static void consume(struct parser *p)
{
	CRIT_ON(p->next == TOK_ERR, "consuming error");
	p->next = lxr_get(p->lxr);
	//printf("next = %s\n", token_str[p->next]);
	if (p->next == TOK_ERR)
		perr(p, "%s", lxr_buffer(p->lxr));
}

static struct ast ast_null = { .id = TOK_NULL };

static struct ast *parse_top(struct parser *p)
{
	if (p->next == TOK_NULL) {
		consume(p);
		return &ast_null;
	} else if (p->next == TOK_TRUE) {
		static struct ast atrue = { .id = TOK_TRUE };
		consume(p);
		return &atrue;
	} else if (p->next == TOK_FALSE) {
		static struct ast afalse = { .id = TOK_FALSE };
		consume(p);
		return &afalse;
	} else if (p->next == TOK_STR || p->next == TOK_ID) {
		struct ast *t = ast_new(p->next);
		t->u.sval = str_create(lxr_buffer(p->lxr));
		consume(p);
		return t;
	} else if (p->next == TOK_NUM) {
		struct ast *t = ast_new(TOK_NUM);
		t->u.nval = atof(lxr_buffer(p->lxr));
		consume(p);
		return t;
	} else {
		perr(p, "unexpected token %s", token_str[p->next]);
		return NULL;
	}
}

static struct ast *parse_list(struct parser *p)
{
	struct ast *val = parse_top(p);
	if (!val || p->next != TOK_SEP)
		return val;
	consume(p);
	struct ast *ast = ast_new(TOK_SEP);
	ast->u.arg[0] = val;
	ast->u.arg[1] = parse_list(p);
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

static struct ast *parse_assign(struct parser *p)
{
	struct ast *val = parse_list(p);
	if (!val || p->next != TOK_ASSIGN)
		return val;
	consume(p);
	struct ast *ast = ast_new(TOK_ASSIGN);
	ast->u.arg[0] = val;
	ast->u.arg[1] = parse_assign(p);
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

static struct ast *parse_expr(struct parser *p)
{
	return parse_assign(p);
}

struct ast *parse_sequence(struct parser *p);

struct ast *parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON) {
		consume(p);
		return &ast_null;
	} else if (p->next == TOK_LBRA) {
		consume(p);
		struct ast *ast = parse_sequence(p);
		if (p->next != TOK_RBRA)
			return ast_free(ast), perr(p, "unmatched {"), NULL;
		consume(p);
		return ast ? ast : &ast_null;
	} else if (p->next == TOK_RBRA) {
		return NULL;
	} else {
		struct ast *ast = parse_expr(p);
		if (!ast)
			return NULL;

		if (p->next != TOK_SCOLON) {
			ast_free(ast);
			perr(p, "; expected after expression");
			return NULL;
		}

		consume(p);

		return ast;
	}
}

struct ast *parse_sequence(struct parser *p)
{
	struct ast *t = parse_inst(p);
	if (!t || p->next == TOK_RBRA || p->next == TOK_EOF)
		return t;
	struct ast *ast = ast_new(TOK_SCOLON);
	ast->u.arg[0] = t;
	ast->u.arg[1] = parse_sequence(p);
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

struct ast *ast_from_file(FILE *file, char *path)
{
	struct parser p = { .path = path, .next = TOK_EOF };

	p.lxr = lxr_create(file, 256);
	if (ERR_ON(!p.lxr, "lxr_create() failed"))
		return NULL;

	consume(&p);

	struct ast *ret = parse_sequence(&p);
	int ok = (p.next != TOK_ERR);
	lxr_destroy(p.lxr);
	if (ERR_ON(!ok, "parse_sequence() failed"))
		return NULL;

	return ret ? ret : &ast_null;
}
