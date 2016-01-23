#include "ast.h"
#include "common.h"
#include "debug.h"
#include "lxr.h"
#include "cstr.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static struct ast *ast_new(enum token id, struct ast *arg0, struct ast *arg1)
{
	struct ast *t = ac_alloc(sizeof *t);
	t->id = id;
	t->u.arg[0] = arg0;
	t->u.arg[1] = arg1;
	return t;
}

static struct ast ast_null = { .id = TOK_NULL };
static struct ast ast_true = { .id = TOK_TRUE };
static struct ast ast_false = { .id = TOK_FALSE };
static struct ast ast_err = { .id = TOK_ERR };

void ast_free(struct ast *t)
{
	if (!t || t == &ast_null || t == &ast_true || t == &ast_false
	    || t == &ast_err) {
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

static struct parser cur;

static void perr(char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", cur.path, lxr_line(cur.lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);
	cur.next = TOK_ERR;
}

static void consume(void)
{
	CRIT_ON(cur.next == TOK_ERR, "consuming error");
	cur.next = lxr_get(cur.lxr);
	//printf("next = %s\n", token_str[p->next]);
	if (cur.next == TOK_ERR)
		perr("%s", lxr_buffer(cur.lxr));
}

static int accept(enum token tok)
{
	if (cur.next != tok)
		return 0;
	consume();
	return 1;
}

static struct ast *parse_strid(void)
{
	enum token id = cur.next;
	if (id != TOK_STR && id != TOK_ID) {
		perr("expected id or string");
		return &ast_err;
	}
	struct ast *ast = ast_new(id, NULL, NULL);
	ast->u.sval = str_create(lxr_buffer(cur.lxr));
	consume();
	return ast;
}

static struct ast *parse_argtail(void)
{
	if (!accept(TOK_SEP))
		return NULL;
	struct ast *arg = parse_strid();
	struct ast *tail = parse_argtail();
	return ast_new(TOK_SEP, arg, tail);
}

static struct ast *parse_arglist(void)
{
	if (cur.next != TOK_ID)
		return NULL;
	struct ast *arg = parse_strid();
	struct ast *tail = parse_argtail();
	return ast_new(TOK_SEP, arg, tail);
}

static struct ast *parse_block(void);

static struct ast *parse_def(void)
{
	CRIT_ON(!accept(TOK_DEF), "expected 'def'");
	if (!accept(TOK_LPAR)) {
		perr("expected ( after 'def'");
		return &ast_err;
	}
	struct ast *args = parse_arglist();
	if (!accept(TOK_RPAR)) {
		perr("expected ')'");
		return args;
	}
	return ast_new(TOK_DEF, args, parse_block());
}

static struct ast *parse_top(void)
{
	if (accept(TOK_NULL)) {
		return &ast_null;
	} else if (accept(TOK_TRUE)) {
		return &ast_true;
	} else if (accept(TOK_FALSE)) {
		return &ast_false;
	} else if (cur.next == TOK_STR || cur.next == TOK_ID) {
		struct ast *t = ast_new(cur.next, NULL, NULL);
		t->u.sval = str_create(lxr_buffer(cur.lxr));
		consume();
		return t;
	} else if (cur.next == TOK_NUM) {
		struct ast *t = ast_new(TOK_NUM, NULL, NULL);
		t->u.nval = atof(lxr_buffer(cur.lxr));
		consume();
		return t;
	} else if (cur.next == TOK_DEF) {
		return parse_def();
	} else {
		perr("unexpected token %s", token_str[cur.next]);
		return &ast_err;
	}
}

static struct ast *parse_list(void);

static struct ast *parse_deref(void)
{
	struct ast *top = parse_top();
	if (!top)
		return top;
	for (;;) {
		struct ast *val = NULL;
		enum token id = TOK_ERR;
		if (accept(TOK_DOT)) {
			if (cur.next != TOK_ID) {
				perr("expected id after .");
				val = &ast_err;
			} else {
				val = ast_new(TOK_STR, NULL, NULL);
				val->u.sval = str_create(lxr_buffer(cur.lxr));
				consume();
			}
			id = TOK_DOT;
		} else if (accept(TOK_LPAR)) {
			if (cur.next != TOK_RPAR)
				val = parse_list();
			if (!accept(TOK_RPAR))
				perr("unmatched (");
			id = TOK_LPAR;
		} else if (accept(TOK_LSQR)) {
			val = parse_list();
			if (!accept(TOK_RSQR))
				perr("unmatched ]");
			id = TOK_DOT;
		} else {
			return top;
		}
		top = ast_new(id, top, val);
	}
}

static struct ast *parse_sum(void)
{
	struct ast *top = parse_deref();
	if (cur.next != TOK_PLUS)
		return top;
	while (cur.next == TOK_PLUS) {
		consume();
		top = ast_new(TOK_PLUS, top, parse_deref());
	}
	return top;
}

static struct ast *parse_list(void)
{
	struct ast *val = parse_sum();
	if (!accept(TOK_SEP))
		return val;
	return ast_new(TOK_SEP, val, parse_list());
}

static struct ast *parse_assign(void)
{
	struct ast *val = parse_list();
	if (!accept(TOK_ASSIGN))
		return val;
	return ast_new(TOK_ASSIGN, val, parse_assign());
}

static struct ast *parse_expr(void)
{
	return parse_assign();
}

struct ast *parse_sequence(void);

static struct ast *parse_block(void)
{
	if (!accept(TOK_LBRA)) {
		perr("expected {");
		return &ast_err;
	}
	if (accept(TOK_RBRA))
		return &ast_null;
	struct ast *ast = parse_sequence();
	if (!accept(TOK_RBRA))
		perr("unmatched {");
	return ast;
}

struct ast *parse_inst(void)
{
	if (accept(TOK_SCOLON)) {
		return &ast_null;
	} else if (cur.next == TOK_LBRA) {
		return parse_block();
	} else {
		struct ast *ast = parse_expr();
		if (!accept(TOK_SCOLON))
			perr("; expected after expression");
		return ast;
	}
}

struct ast *parse_sequence(void)
{
	struct ast *t = parse_inst();
	if (cur.next == TOK_RBRA || cur.next == TOK_ERR || cur.next == TOK_EOF)
		return t;
	return ast_new(TOK_SCOLON, t, parse_sequence());
}

struct ast *ast_from_file(FILE *file, char *path)
{
	cur.path = path;
	cur.next = TOK_EOF;
	cur.lxr = lxr_create(file, 256);

	if (ERR_ON(!cur.lxr, "lxr_create() failed"))
		return NULL;

	consume();

	struct ast *ast = parse_sequence();
	lxr_destroy(cur.lxr);
	if (cur.next == TOK_ERR) {
		ERR("parse_sequence() failed");
		ast_dump(ast);
		ast_free(ast);
		return NULL;
	}
	return ast;
}
