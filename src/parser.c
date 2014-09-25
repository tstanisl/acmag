#include "debug.h"
#include "list.h"
#include "lxr.h"
#include "syntax.h"
#include "vec.h"

#include <stdarg.h>
#include <stdlib.h>

struct parser {
	char *path;
	struct lxr *lxr;
	enum token next;
	int loop_depth;
};

static void *parse_err(struct parser *p, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", p->path, lxr_line(p->lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);

	return NULL;
}

static void parse_consume(struct parser *p)
{
	p->next = lxr_get(p->lxr);
	printf("next-token = %s\n", token_str[p->next]);
}

static struct acs_block *parse_block(struct parser *p);
static void destroy_block(struct acs_block *b);
static void dump_block(struct acs_block *b, int depth);

static void destroy_expr(enum acs_id *expr);
static void dump_expr(enum acs_id *expr, int depth);
static enum acs_id *parse_expr(struct parser *p);
static enum acs_id *parse_arg2_expr(struct parser *p, int level);

static enum acs_id *parse_inst(struct parser *p);
static void destroy_inst(enum acs_id *inst);

#define to_block(inst) \
	container_of(inst, struct acs_block, id)
#define to_literal(inst) \
	container_of(inst, struct acs_literal, id)
#define to_return(inst) \
	container_of(inst, struct acs_return, id)
#define to_expr(inst) \
	container_of(inst, struct acs_expr, id)
#define to_if(inst) \
	container_of(inst, struct acs_if, id)
#define to_while(inst) \
	container_of(inst, struct acs_while, id)

static enum acs_id *parse_literal(struct parser *p)
{
	if (p->next == TOK_TRUE) {
		static enum acs_id id = ACS_TRUE;
		parse_consume(p);
		return &id;
	}
	if (p->next == TOK_FALSE) {
		static enum acs_id id = ACS_FALSE;
		parse_consume(p);
		return &id;
	}
	if (p->next == TOK_NULL) {
		static enum acs_id id = ACS_NULL;
		parse_consume(p);
		return &id;
	}

	/* payloaded literals */
	if (p->next != TOK_ID && p->next != TOK_INT && p->next != TOK_STR)
		return parse_err(p, "unexpected token %s", token_str[p->next]);

	char *payload = lxr_buffer(p->lxr);
	struct acs_literal *l = calloc(1, sizeof (*l) + strlen(payload) + 1);
	if (ERR_ON(!l, "calloc() failed"))
		return NULL;

	l->id = p->next == TOK_INT ? ACS_NUM :
		p->next == TOK_ID ? ACS_ID :
		ACS_STR;
	strcat(l->payload, payload);

	parse_consume(p);
	return &l->id;
}

static enum acs_id *parse_top(struct parser *p)
{
	if (p->next != TOK_LPAR)
		return parse_literal(p);
	parse_consume(p);
	enum acs_id *expr = parse_expr(p);
	if (ERR_ON(!p, "parse_expr() failed"))
		return NULL;
	if (p->next != TOK_RPAR)
		destroy_expr(expr), parse_err(p, "missing )");
	parse_consume(p);
	return expr;
}

static enum acs_id *parse_deref(struct parser *p)
{
	enum acs_id *arg0 = parse_top(p);
	if (ERR_ON(!arg0, "parse_literal() failed"))
		return NULL;
	enum token tok = p->next;
	if (tok != TOK_LPAR && tok != TOK_LSQR && tok != TOK_DOT)
		return arg0;
	for (;;) {
		parse_consume(p);
		struct acs_expr *e = calloc(1, sizeof *e);
		if (ERR_ON(!e, "calloc() failed"))
			break;
		e->arg0 = arg0;
		arg0 = NULL;
		if (tok == TOK_LPAR) {
			e->id = ACS_CALL;
			static enum acs_id nop = ACS_NOP;
			if (p->next == TOK_RPAR) {
				e->arg1 = &nop;
			} else {
				e->arg1 = parse_expr(p);
				if (ERR_ON(!e->arg1, "parse_expr() failed"))
					break;
			}
			if (p->next != TOK_RPAR) {
				parse_err(p, "missing )");
				break;
			}
			parse_consume(p);
		} else if (tok == TOK_LSQR) {
			e->id = ACS_CALL;
			e->arg1 = parse_expr(p);
			if (ERR_ON(!e->arg1, "parse_expr() failed"))
				break;
			if (p->next != TOK_RSQR) {
				parse_err(p, "missing ]");
				break;
			}
			parse_consume(p);
		} else if (tok == TOK_DOT) {
			e->id = ACS_DOT;
			if (p->next != TOK_ID) {
				parse_err(p, "expected id after .");
				break;
			}
			e->arg1 = parse_literal(p);
			if (ERR_ON(!e->arg1, "parse_literal() failed"))
				break;
		}
		tok = p->next;
		if (tok != TOK_LPAR && tok != TOK_LSQR && tok != TOK_DOT)
			return &e->id;
		arg0 = &e->id;
	}
	if (arg0)
		destroy_expr(arg0);
	return NULL;
}

static enum acs_id *parse_arg1_expr(struct parser *p)
{
	enum acs_id id = ACS_NOP;
	if (p->next == TOK_NOT)
		id = ACS_NOT;
	else if (p->next == TOK_PLUS)
		id = ACS_PLUS;
	else if (p->next == TOK_MINUS)
		id = ACS_MINUS;
	else
		return parse_deref(p);

	parse_consume(p);

	struct acs_expr *e = calloc(1, sizeof *e);
	if (ERR_ON(!e, "malloc() failed"))
		return NULL;

	e->id = id;
	e->arg0 = parse_arg1_expr(p);
	if (ERR_ON(!e, "parse_arg1_expr() failed"))
		return free(e), NULL;
	return &e->id;
}

struct op2_desc {
	enum acs_id id;
	enum token token;
};

#define OP2(id) { ACS_ ## id, TOK_ ## id }
static struct op2_desc *op2_desc[] = {
	(struct op2_desc[]) { OP2(OR), { ACS_NOP } },
	(struct op2_desc[]) { OP2(AND), { ACS_NOP } },
	(struct op2_desc[]) { OP2(EQ), OP2(NEQ), OP2(LESS), OP2(LEQ),
	                      OP2(GREAT), OP2(GREQ), { ACS_NOP } },
	(struct op2_desc[]) { OP2(CONCAT), { ACS_NOP } },
	(struct op2_desc[]) { {ACS_ADD, TOK_PLUS}, {ACS_SUB, TOK_MINUS}, { ACS_NOP } },
	(struct op2_desc[]) { OP2(MUL), OP2(DIV), OP2(MOD),{ ACS_NOP } },
	NULL,
};

static enum acs_id token_to_id(enum token token, int level)
{
	struct op2_desc *op = op2_desc[level];
	for (int i = 0; op[i].id != ACS_NOP; ++i)
		if (op[i].token == token)
			return op[i].id;
	return ACS_NOP;
}

static enum acs_id *parse_arg2_expr(struct parser *p, int level)
{
	if (!op2_desc[level])
		return parse_arg1_expr(p);

	enum acs_id *arg0 = parse_arg2_expr(p, level + 1);
	if (ERR_ON(!arg0, "parse_arg2_expr(level=%d) failed", level + 1))
		return NULL;

	enum acs_id id = token_to_id(p->next, level);
	if (id == ACS_NOP)
		return arg0;

	for (;;) {
		parse_consume(p);
		struct acs_expr *e = calloc(1, sizeof *e);
		if (!e)
			break;
		e->id = id;
		e->arg0 = arg0;
		e->arg1 = parse_arg2_expr(p, level + 1);
		arg0 = NULL;
		if (ERR_ON(!e->arg1, "parse_arg2_expr(level=%d) failed", level + 1))
			break;
		id = token_to_id(p->next, level);
		if (id == ACS_NOP)
			return &e->id;
		arg0 = &e->id;
	}
	if (arg0)
		destroy_expr(arg0);
	return NULL;
}

static void destroy_arg_expr(enum acs_id *id)
{
	struct acs_expr *expr = to_expr(id);
	destroy_expr(expr->arg0);
	if (expr->id >= __ACS_ARG2)
		destroy_expr(expr->arg1);
	free(expr);
}

static void destroy_expr(enum acs_id *expr)
{
	if (*expr == ACS_NUM || *expr == ACS_ID || *expr == ACS_STR)
		free(to_literal(expr));
	else if (*expr >= __ACS_ARG1)
		destroy_arg_expr(expr);
	else if (*expr == ACS_NOP || *expr == ACS_NULL || *expr == ACS_TRUE || *expr == ACS_FALSE)
		;
	else
		ERR("unexpected asc_inst=%d\n", (int)*expr);
}

static char *op2str[__ACS_MAX] = {
	[ACS_ASSIGN] = "=",
	[ACS_COMMA] = ",",
	[ACS_OR] = "||",
	[ACS_AND] = "&&",
	[ACS_EQ] = "==",
	[ACS_NEQ] = "!=",
	[ACS_LEQ] = "<=",
	[ACS_GREQ] = ">=",
	[ACS_LESS] = "<",
	[ACS_GREAT] = ">",
	[ACS_CONCAT] = "..",
	[ACS_SUB] = "-",
	[ACS_ADD] = "+",
	[ACS_MUL] = "*",
	[ACS_DIV] = "/",
	[ACS_MOD] = "%",
	[ACS_NOT] = "!",
	[ACS_PLUS] = "-",
	[ACS_MINUS] = "+",
	[ACS_CALL] = "(",
	[ACS_DEREF] = "[",
	[ACS_DOT] = ".",
};

static void dump_arg1_expr(enum acs_id *id, int depth)
{
	struct acs_expr *expr = to_expr(id);
	if (ERR_ON(!op2str[expr->id], "invalid asc_id = %d\n", (int)*id))
		return;
	printf("%s", op2str[expr->id]);
	dump_expr(expr->arg0, depth);
}

static void dump_arg2_expr(enum acs_id *id, int depth)
{
	struct acs_expr *expr = to_expr(id);
	if (ERR_ON(!op2str[expr->id], "invalid asc_id = %d\n", (int)*id))
		return;
	//printf("(");
	dump_expr(expr->arg0, depth);
	if (expr->id == ACS_CALL)
		printf("(");
	else if (expr->id == ACS_DEREF)
		printf("[");
	else if (expr->id == ACS_DOT)
		printf(".");
	else
		printf(" %s ", op2str[expr->id]);
	dump_expr(expr->arg1, depth);
	if (expr->id == ACS_CALL)
		printf(")");
	else if (expr->id == ACS_DEREF)
		printf("]");
	//printf(")");
}

static void dump_expr(enum acs_id *expr, int depth)
{
	if (*expr == ACS_TRUE)
		printf("true");
	else if (*expr == ACS_FALSE)
		printf("false");
	else if (*expr == ACS_NULL)
		printf("null");
	else if (*expr == ACS_STR)
		printf("\"%s\"", to_literal(expr)->payload);
	else if (*expr == ACS_NUM || *expr == ACS_ID)
		printf("%s", to_literal(expr)->payload);
	else if (*expr == ACS_NOP)
		;
	else if (*expr >= __ACS_ARG2)
		dump_arg2_expr(expr, depth);
	else if (*expr >= __ACS_ARG1)
		dump_arg1_expr(expr, depth);
	else
		ERR("unexpected asc_inst=%d\n", (int)*expr);
}

static enum acs_id *parse_list(struct parser *p)
{
	enum acs_id *lhs = parse_arg2_expr(p, 0);
	if (ERR_ON(!lhs, "parse_arg2_expr() failed"))
		return NULL;

	if (p->next != TOK_SEP)
		return lhs;
	parse_consume(p);

	/* TODO: check is lhs is really a valid LHS */
	struct acs_expr *expr = malloc(sizeof *expr);
	if (ERR_ON(!expr, "malloc() failed"))
		return destroy_expr(lhs), NULL;

	expr->id = ACS_COMMA;
	expr->arg0 = lhs;
	expr->arg1 = parse_list(p);
	if (ERR_ON(!expr->arg1, "parse_list() failed"))
		return free(expr), destroy_expr(lhs), NULL;

	return &expr->id;
}

static enum acs_id *parse_expr(struct parser *p)
{
	enum acs_id *lhs = parse_list(p);
	if (ERR_ON(!lhs, "parse_list() failed"))
		return NULL;

	if (p->next != TOK_ASSIGN)
		return lhs;
	parse_consume(p);

	/* TODO: check is lhs is really a valid LHS */
	struct acs_expr *expr = malloc(sizeof *expr);
	if (ERR_ON(!expr, "malloc() failed"))
		return destroy_expr(lhs), NULL;

	expr->id = ACS_ASSIGN;
	expr->arg0 = lhs;
	expr->arg1 = parse_expr(p);
	if (ERR_ON(!expr->arg1, "parse_expr() failed"))
		return free(expr), destroy_expr(lhs), NULL;

	return &expr->id;
}

static enum acs_id *parse_expr_inst(struct parser *p)
{
	enum acs_id *expr = parse_expr(p);
	if (ERR_ON(!expr, "parse_expr() failed"))
		return NULL;
	if (p->next != TOK_SCOLON) {
		destroy_expr(expr);
		return parse_err(p, "expected ; after expression");
	}
	parse_consume(p); // consume ;

	return expr;
}

static void destroy_while(enum acs_id *id)
{
	struct acs_while *inst = to_while(id);
	destroy_expr(inst->expr);
	destroy_inst(inst->inst);
	free(inst);
}

static enum acs_id *parse_while(struct parser *p)
{
	parse_consume(p);

	if (p->next != TOK_LPAR)
		return parse_err(p, "missing ( after while");
	parse_consume(p);

	enum acs_id *expr = parse_expr(p);
	if (ERR_ON(!expr, "parse_expr() failed"))
		return NULL;

	if (p->next != TOK_RPAR) {
		parse_err(p, "missing ) after while's expression");
		goto fail_expr;
	}
	parse_consume(p);

	++p->loop_depth;
	enum acs_id *true_inst = parse_inst(p);
	--p->loop_depth;

	if (ERR_ON(!true_inst, "parse_inst() failed"))
		goto fail_expr;

	struct acs_while *inst = calloc(1, sizeof *inst);
	if (ERR_ON(!inst, "calloc() failed"))
		goto fail_true_inst;

	inst->id = ACS_WHILE;
	inst->expr = expr;
	inst->inst = true_inst;

	return &inst->id;

fail_true_inst:
	destroy_inst(true_inst);
fail_expr:
	destroy_expr(expr);

	return NULL;
}

static void destroy_if(enum acs_id *id)
{
	struct acs_if *inst = to_if(id);
	destroy_expr(inst->expr);
	destroy_inst(inst->true_inst);
	if (inst->false_inst)
		destroy_inst(inst->false_inst);
	free(inst);
}

static enum acs_id *parse_if(struct parser *p)
{
	parse_consume(p);

	if (p->next != TOK_LPAR)
		return parse_err(p, "missing ( after if");
	parse_consume(p);

	enum acs_id *expr = parse_expr(p);
	if (ERR_ON(!expr, "parse_expr() failed"))
		return NULL;

	if (p->next != TOK_RPAR) {
		parse_err(p, "missing ) after if's expression");
		goto fail_expr;
	}
	parse_consume(p);

	enum acs_id *true_inst = parse_inst(p);
	if (ERR_ON(!true_inst, "parse_inst() failed"))
		goto fail_expr;

	struct acs_if *inst = calloc(1, sizeof *inst);
	if (ERR_ON(!inst, "calloc() failed"))
		goto fail_true_inst;

	inst->id = ACS_IF;
	inst->expr = expr;
	inst->true_inst = true_inst;

	if (p->next != TOK_ELSE)
		return &inst->id;
	parse_consume(p);

	inst->false_inst = parse_inst(p);
	if (ERR_ON(!inst->false_inst, "parse_inst() failed"))
		goto fail_inst;

	return &inst->id;

fail_inst:
	free(inst);
fail_true_inst:
	destroy_inst(true_inst);
fail_expr:
	destroy_expr(expr);

	return NULL;
}

static enum acs_id *parse_return(struct parser *p)
{
	parse_consume(p);
	struct acs_return *r = malloc(sizeof (*r));
	if (ERR_ON(!r, "malloc() failed"))
		return NULL;
	r->id = ACS_RETURN;
	if (p->next == TOK_SCOLON) {
		r->expr = NULL;
		parse_consume(p);
		return &r->id;
	}
	r->expr = parse_expr_inst(p);
	if (ERR_ON(!r->expr, "parse_expr_inst() failed")) {
		free(r);
		return NULL;
	}
	return &r->id;
}

static void destroy_inst(enum acs_id *inst)
{
	if (*inst == ACS_BLOCK) {
		destroy_block(to_block(inst));
	} else if (*inst >= __ACS_EXPR) {
		destroy_expr(inst);
	} else if (*inst == ACS_RETURN) {
		struct acs_return *r = to_return(inst);
		if (r->expr)
			destroy_expr(r->expr);
		free(r);
	} else if (*inst == ACS_IF) {
		destroy_if(inst);
	} else if (*inst == ACS_WHILE) {
		destroy_while(inst);
	} else if (*inst == ACS_NOP || *inst == ACS_BREAK || *inst == ACS_CONTINUE) {
	} else {
		ERR("unexpected asc_inst=%d\n", (int)*inst);
	}
}

static void dump_inst(enum acs_id *inst, int depth)
{
	if (*inst == ACS_BLOCK) {
		dump_block(to_block(inst), depth);
	} else if (*inst == ACS_NOP) {
		printf(";\n");
	} else if (*inst == ACS_BREAK) {
		printf("break;\n");
	} else if (*inst == ACS_CONTINUE) {
		printf("continue;\n");
	} else if (*inst >= __ACS_EXPR) {
		dump_expr(inst, depth);
		printf(";\n");
	} else if (*inst == ACS_RETURN) {
		struct acs_return *r = to_return(inst);
		if (r->expr) {
			printf("return ");
			dump_expr(r->expr, depth);
			printf(";\n");
		} else {
			printf("return;\n");
		}
	} else if (*inst == ACS_IF) {
		struct acs_if *i = to_if(inst);
		printf("if (");
		dump_expr(i->expr, depth);
		if (*i->true_inst == ACS_BLOCK) {
			printf(") ");
			dump_inst(i->true_inst, depth);
			if (i->false_inst)
				printf(" ");
		} else {
			printf(")\n");
			printf("%*s", 2 * depth + 2, "");
			dump_inst(i->true_inst, depth + 1);
		}
		if (!i->false_inst)
			return;
		printf("%*s", 2 * depth, "");
		if (*i->false_inst == ACS_BLOCK) {
			printf("else ");
			dump_inst(i->false_inst, depth);
		} else {
			printf("else\n");
			printf("%*s", 2 * depth + 2, "");
			dump_inst(i->false_inst, depth + 1);
		}
	} else if (*inst == ACS_WHILE) {
		struct acs_while *w = to_while(inst);
		printf("while (");
		dump_expr(w->expr, depth);
		if (*w->inst == ACS_BLOCK) {
			printf(") ");
			dump_inst(w->inst, depth);
		} else {
			printf(")\n");
			printf("%*s", 2 * depth + 2, "");
			dump_inst(w->inst, depth + 1);
		}
	}
}

static enum acs_id *parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON) {
		static enum acs_id acs_nop = ACS_NOP;
		parse_consume(p);
		return &acs_nop;
	} else if (p->next == TOK_BREAK) {
		if (!p->loop_depth)
			return parse_err(p, "break outside loop");
		parse_consume(p);
		if (p->next != TOK_SCOLON)
			return parse_err(p, "missing ; after break");
		parse_consume(p);
		static enum acs_id acs_break = ACS_BREAK;
		return &acs_break;
	} else if (p->next == TOK_CONTINUE) {
		if (!p->loop_depth)
			return parse_err(p, "continue outside loop");
		parse_consume(p);
		if (p->next != TOK_SCOLON)
			return parse_err(p, "missing ; after continue");
		static enum acs_id acs_continue = ACS_CONTINUE;
		parse_consume(p);
		return &acs_continue;
	} else if (p->next == TOK_LBRA) {
		struct acs_block *b = parse_block(p);
		if (ERR_ON(!b, "parse_block() failed"))
			return NULL;
		return &b->id;
	} else if (p->next == TOK_RETURN) {
		return parse_return(p);
	} else if (p->next == TOK_IF) {
		return parse_if(p);
	} else if (p->next == TOK_WHILE) {
		return parse_while(p);
	} else {
		return parse_expr_inst(p);
	}
}

static void destroy_block(struct acs_block *b)
{
	for (int i = 0; i < vec_size(b->inst); ++i)
		destroy_inst(b->inst[i]);
	vec_destroy(b->inst);
	free(b);
}

static void dump_block(struct acs_block *b, int depth)
{
	printf("{\n");
	for (int i = 0; i < vec_size(b->inst); ++i) {
		printf("%*s", 2 + 2 * depth, "");
		dump_inst(b->inst[i], depth + 1);
	}
	printf("%*s}\n", 2 * depth, "");
}

static struct acs_block *parse_block(struct parser *p)
{
	if (p->next != TOK_LBRA)
		return parse_err(p, "missing { at start of block");

	parse_consume(p);

	struct acs_block *block = calloc(1, sizeof *block);
	if (ERR_ON(!block, "malloc() failed"))
		return NULL;

	block->id = ACS_BLOCK;
	VEC_INIT(block->inst);
	if (ERR_ON(!block->inst, "VEC_INIT() failed"))
		goto fail;

	for (;;) {
		if (p->next == TOK_RBRA)
			break;
		enum acs_id *inst = parse_inst(p);
		if (ERR_ON(!inst, "parse_inst() failed"))
			goto fail;

		if (!VEC_PUSH(block->inst, inst)) {
			ERR("VEC_PUSH() failed");
			destroy_inst(inst);
			goto fail;
		}
	}

	parse_consume(p);
	return block;

fail:
	destroy_block(block);
	return NULL;
}

static void destroy_function(struct acs_function *f)
{
	for (int i = 0; i < vec_size(f->args); ++i)
		free(f->args[i]);
	vec_destroy(f->args);
	if (f->block)
		destroy_block(f->block);
	free(f);
}

static void dump_function(struct acs_function *f)
{
	if (f->exported)
		printf("export ");
	printf("%s(", f->name);
	for (int i = 0; i < vec_size(f->args); ++i)
		printf("%s%s", i ? ", " : "", f->args[i]);
	printf(") ");
	dump_block(f->block, 0);
	puts("");
}

static struct acs_function *parse_function(struct parser *p)
{
	bool exported = false;
	if (p->next == TOK_EXPORT) {
		exported = true;
		parse_consume(p);
	}

	if (p->next != TOK_ID)
		return parse_err(p, "function name expected");

	char *name = lxr_buffer(p->lxr);
	struct acs_function *f = malloc(sizeof (*f) + strlen(name) + 1);
	if (ERR_ON(!f, "malloc() failed"))
		return NULL;

	f->exported = exported;
	strcpy(f->name, name);
	f->block = NULL;

	// extract id list
	parse_consume(p);

	VEC_INIT(f->args);
	if (ERR_ON(!f->args, "VEC_INIT() failed"))
		goto fail;

	if (p->next != TOK_LPAR) {
		parse_err(p, "expected ( after function id");
		goto fail;
	}
	parse_consume(p); // consume (

	while (p->next != TOK_RPAR) {
		if (p->next != TOK_ID) {
			parse_err(p, "unextepcted token");
			goto fail;
		}
		char *str = strdup(lxr_buffer(p->lxr));
		if (ERR_ON(!str, "strdup() failed\n"))
			goto fail;

		if (!VEC_PUSH(f->args, str)) {
			ERR("VEC_PUSH() failed");
			goto fail;
		}

		// TODO: add argument parsing
		parse_consume(p);
		if (p->next != TOK_SEP)
			continue;
		parse_consume(p);
	}

	parse_consume(p);

	f->block = parse_block(p);
	if (ERR_ON(!f->block, "parse_block() failed"))
		goto fail;

	return f;

fail:
	destroy_function(f);

	return NULL;
}

void dump_script(struct acs_script *s)
{
	list_foreach(l, &s->functions) {
		struct acs_function *f = list_entry(l, struct acs_function, node);
		dump_function(f);
		puts("");
	}
}

void destroy_script(struct acs_script *s)
{
	list_foreach(l, &s->functions) {
		struct acs_function *f = list_entry(l, struct acs_function, node);
		list_del(&f->node);
		destroy_function(f);
	}
	free(s);
}

struct acs_script *parse_script(FILE *file, char *path)
{
	struct acs_script *script = malloc(sizeof *script);
	if (ERR_ON(!script, "malloc() failed"))
		return NULL;

	list_init(&script->functions);

	struct parser parser = { .path = path };

	parser.lxr = lxr_create(file, 256);
	if (ERR_ON(!parser.lxr, "lxr_create() failed"))
		goto fail;

	parse_consume(&parser);
	while (parser.next != TOK_EOF && parser.next != TOK_ERR) {
		struct acs_function *function;

		function = parse_function(&parser);
		if (ERR_ON(!function, "parse_function() failed"))
			goto fail_lxr;

		list_add_tail(&function->node, &script->functions);
		function->script = script;
	}

	// failure or not LeXeR is no longer needed
	lxr_destroy(parser.lxr);

	return script;

fail_lxr:
	if (parser.next == TOK_ERR)
		parse_err(&parser, "%s", lxr_buffer(parser.lxr));

	lxr_destroy(parser.lxr);
fail:
	destroy_script(script);

	return NULL;
}

