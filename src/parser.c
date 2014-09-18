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

#define to_block(inst) \
	container_of(inst, struct acs_block, id)
#define to_literal(inst) \
	container_of(inst, struct acs_literal, id)

static struct acs_literal *parse_literal(struct parser *p)
{
	if (p->next == TOK_TRUE) {
		static enum acs_inst inst = ACS_TRUE;
		parse_consume(p);
		return to_literal(&inst);
	}
	if (p->next == TOK_FALSE) {
		static enum acs_inst inst = ACS_FALSE;
		parse_consume(p);
		return to_literal(&inst);
	}
	if (p->next == TOK_NULL) {
		static enum acs_inst inst = ACS_NULL;
		parse_consume(p);
		return to_literal(&inst);
	}
	return parse_err(p, "unexpected token %s", token_str[p->next]);
}

static void destroy_expr(enum acs_inst *expr)
{
}

static void dump_expr(enum acs_inst *expr, int depth)
{
	if (*expr == ACS_TRUE)
		printf("true");
	else if (*expr == ACS_FALSE)
		printf("false");
	else if (*expr == ACS_NULL)
		printf("null");
}

static enum acs_inst *parse_expr(struct parser *p)
{
	struct acs_literal *l = parse_literal(p);
	if (ERR_ON(!l, "parse_literal() failed"))
		return NULL;
	return &l->id;
}

static void destroy_inst(enum acs_inst *inst)
{
	if (*inst == ACS_BLOCK)
		destroy_block(to_block(inst));
}

static void dump_inst(enum acs_inst *inst, int depth)
{
	if (*inst == ACS_BLOCK)
		dump_block(to_block(inst), depth);
	if (*inst == ACS_NOP)
		printf(";\n");
	if (*inst >= ACS_EXPR) {
		dump_expr(inst, depth);
		printf(";\n");
	}
}

static enum acs_inst *parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON) {
		static enum acs_inst acs_nop = ACS_NOP;
		parse_consume(p);
		return &acs_nop;
	}

	if (p->next == TOK_LBRA) {
		struct acs_block *b = parse_block(p);
		if (ERR_ON(!b, "parse_block() failed"))
			return NULL;
		return &b->id;
	}

	enum acs_inst *expr = parse_expr(p);
	if (ERR_ON(!expr, "parse_expr() failed"))
		return NULL;
	if (p->next != TOK_SCOLON) {
		destroy_expr(expr);
		return parse_err(p, "expected ; after expression");
	}
	parse_consume(p); // consume ;

	return expr;
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
		enum acs_inst *inst = parse_inst(p);
		if (!inst)
			goto fail;

		if (!VEC_PUSH(block->inst, inst)) {
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
			goto fail;

		list_add_tail(&function->node, &script->functions);
	}

	// failure or not LeXeR is no longer needed
	lxr_destroy(parser.lxr);

	if (parser.next == TOK_ERR) {
		parse_err(&parser, "%s", lxr_buffer(parser.lxr));
		goto fail;
	}

	return script;

fail:
	destroy_script(script);

	return NULL;
}

