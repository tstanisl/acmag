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

static struct ast ast_null = { .id = TOK_NULL };

static struct ast *parse_top(void)
{
	if (cur.next == TOK_NULL) {
		consume();
		return &ast_null;
	} else if (cur.next == TOK_TRUE) {
		static struct ast atrue = { .id = TOK_TRUE };
		consume();
		return &atrue;
	} else if (cur.next == TOK_FALSE) {
		static struct ast afalse = { .id = TOK_FALSE };
		consume();
		return &afalse;
	} else if (cur.next == TOK_STR || cur.next == TOK_ID) {
		struct ast *t = ast_new(cur.next);
		t->u.sval = str_create(lxr_buffer(cur.lxr));
		consume();
		return t;
	} else if (cur.next == TOK_NUM) {
		struct ast *t = ast_new(TOK_NUM);
		t->u.nval = atof(lxr_buffer(cur.lxr));
		consume();
		return t;
	} else {
		perr("unexpected token %s", token_str[cur.next]);
		return NULL;
	}
}

static struct ast *parse_expr(void);

static struct ast *parse_deref(void)
{
	struct ast *top = parse_top();
	if (!top)
		return top;
	for (;;) {
		struct ast *val = NULL;
		enum token id = TOK_ERR;
		if (cur.next == TOK_DOT) {
			consume();
			if (cur.next != TOK_ID) {
				perr("expected id after .");
				ast_free(top);
				return NULL;
			}
			val = ast_new(TOK_STR);
			val->u.sval = str_create(lxr_buffer(cur.lxr));
			id = TOK_DOT;
		} else if (cur.next == TOK_LPAR) {
			consume();
			if (cur.next != TOK_RPAR) {
				val = parse_expr();
				if (!val)
					return ast_free(top), NULL;
			}
			if (cur.next != TOK_RPAR) {
				perr("unmatched (");
				ast_free(top);
				return NULL;
			}
			id = TOK_LPAR;
		} else if (cur.next == TOK_LSQR) {
			consume();
			val = parse_expr();
			if (!val)
				return ast_free(top), NULL;
			if (cur.next != TOK_RSQR) {
				perr("unmatched ]");
				ast_free(top);
				return NULL;
			}
			id = TOK_DOT;
		} else {
			return top;
		}
		consume();
		struct ast *ntop = ast_new(id);
		ntop->u.arg[0] = top;
		ntop->u.arg[1] = val;
		top = ntop;
	}
}

static struct ast *parse_sum(void)
{
	struct ast *top = parse_deref();
	if (!top || cur.next != TOK_PLUS)
		return top;
	while (cur.next == TOK_PLUS) {
		consume();
		struct ast *val = parse_deref();
		if (!val)
			return ast_free(top), NULL;
		struct ast *newtop = ast_new(TOK_PLUS);
		newtop->u.arg[0] = top;
		newtop->u.arg[1] = val;
		top = newtop;
	}
	return top;
}

static struct ast *parse_list(void)
{
	struct ast *val = parse_sum();
	if (!val || cur.next != TOK_SEP)
		return val;
	consume();
	struct ast *ast = ast_new(TOK_SEP);
	ast->u.arg[0] = val;
	ast->u.arg[1] = parse_list();
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

static struct ast *parse_assign(void)
{
	struct ast *val = parse_list();
	if (!val || cur.next != TOK_ASSIGN)
		return val;
	consume();
	struct ast *ast = ast_new(TOK_ASSIGN);
	ast->u.arg[0] = val;
	ast->u.arg[1] = parse_assign();
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

static struct ast *parse_expr(void)
{
	return parse_assign();
}

struct ast *parse_sequence(void);

struct ast *parse_inst(void)
{
	if (cur.next == TOK_SCOLON) {
		consume();
		return &ast_null;
	} else if (cur.next == TOK_LBRA) {
		consume();
		struct ast *ast = parse_sequence();
		if (cur.next != TOK_RBRA) {
			ast_free(ast);
			perr("unmatched {");
			return NULL;
		}
		consume();
		return ast ? ast : &ast_null;
	} else if (cur.next == TOK_RBRA) {
		return NULL;
	} else {
		struct ast *ast = parse_expr();
		if (!ast)
			return NULL;

		if (cur.next != TOK_SCOLON) {
			ast_free(ast);
			perr("; expected after expression");
			return NULL;
		}

		consume();

		return ast;
	}
}

struct ast *parse_sequence(void)
{
	struct ast *t = parse_inst();
	if (!t || cur.next == TOK_RBRA || cur.next == TOK_EOF)
		return t;
	struct ast *ast = ast_new(TOK_SCOLON);
	ast->u.arg[0] = t;
	ast->u.arg[1] = parse_sequence();
	if (!ast->u.arg[1])
		return ast_free(ast), NULL;
	return ast;
}

struct ast *ast_from_file(FILE *file, char *path)
{
	cur.path = path;
	cur.next = TOK_EOF;
	cur.lxr = lxr_create(file, 256);

	if (ERR_ON(!cur.lxr, "lxr_create() failed"))
		return NULL;

	consume();

	struct ast *ret = parse_sequence();
	int ok = (cur.next != TOK_ERR);
	lxr_destroy(cur.lxr);
	if (ERR_ON(!ok, "parse_sequence() failed"))
		return NULL;

	return ret ? ret : &ast_null;
}
