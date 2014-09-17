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
}

static enum acs_inst acs_nop = ACS_NOP;

static enum acs_inst *parse_inst(struct parser *p)
{
	return &acs_nop; // stub
}

void destroy_inst_block(struct acs_inst_block *b)
{
	free(b);
}

static struct acs_inst_block *parse_inst_block(struct parser *p)
{
	if (p->next != TOK_LBRA)
		return parse_err(p, "missing { at start of block");

	parse_consume(p);

	struct acs_inst_block *block = calloc(1, sizeof *block);
	if (ERR_ON(!block, "malloc() failed"))
		return NULL;

	block->id = ACS_BLOCK;
	VEC_INIT(block->inst);
	if (ERR_ON(!block->inst, "VEC_INIT() failed"))
		goto fail;

	while (p->next != TOK_RBRA) {
		enum acs_inst *inst = parse_inst(p);
		if (!inst)
			goto fail;

		if (!VEC_PUSH(block->inst, inst)) {
			// TODO: release inst
			goto fail;
		}
	}

	parse_consume(p);
	return block;

fail:
	destroy_inst_block(block);
	return NULL;
}

/*
static struct acs_inst *parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON) {
		struct acs_inst *inst = calloc(1, sizeof *inst);
		if (ERR_ON(!inst, "malloc() failed"))
			return NULL;

		inst->id = ACS_NOP;
		parse_consume(p);
		return inst;
	}

	if (p->next == TOK_LBRA && parse_block(p, inst))
		return inst;

	return parse_err(p, "unexpected token %s", token_str[p->next]);
}
*/

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
	list_init(&f->vars);

	// extract id list
	parse_consume(p);
	while (p->next != TOK_LPAR) {
		if (p->next != TOK_ID) {
			parse_err(p, "unextepcted token");
			goto fail;
		}
		// TODO: add argument parsing
		parse_consume(p);
		if (p->next != TOK_SEP)
			continue;
		parse_consume(p);
	}

	f->block = parse_inst_block(p);
	if (ERR_ON(!f->block, "parse_inst_block() failed"))
		goto fail_args;

	return f;

fail_args:
	//free list

fail:
	free(f);

	return NULL;
}

struct acs_script *parse_script(FILE *file, char *path)
{
	struct acs_script *script = malloc(sizeof *script);
	if (ERR_ON(!script, "malloc() failed"))
		return NULL;

	struct parser parser = { .path = path };

	parser.lxr = lxr_create(file, 256);
	if (ERR_ON(!parser.lxr, "lxr_create() failed"))
		goto fail_script;

	list_init(&script->functions);

	parse_consume(&parser);
	while (parser.next != TOK_EOF && parser.next != TOK_ERR) {
		struct acs_function *function;

		function = parse_function(&parser);
		if (!function)
			break;

		list_add_tail(&function->node, &script->functions);
	}

	lxr_destroy(parser.lxr);

	if (parser.next == TOK_ERR) {
		parse_err(&parser, "%s", lxr_buffer(parser.lxr));
		goto fail_script;
	}

	return script;

	// TODO: release functions
fail_script:
	free(script);

	return NULL;
}

