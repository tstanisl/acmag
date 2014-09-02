#include "debug.h"
#include "list.h"
#include "lxr.h"
#include "syntax.h"

#include <stdarg.h>
#include <stdlib.h>

struct parser {
	char *path;
	struct lxr *lxr;
	struct acs_inst *continue_inst;
	struct acs_inst *break_inst;
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

static struct acs_inst *parse_inst(struct parser *p)
{
	struct acs_inst *inst = malloc(sizeof *inst);
	if (ERR_ON(!inst, "malloc() failed"))
		return NULL;

	if (p->next == TOK_SCOLON) {
		inst->id = ACS_NOP;
		parse_consume(p);
		return inst;
	}

	return parse_err(p, "unexpected token %s", token_str[p->next]);
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

	list_init(&script->inst);

	parse_consume(&parser);
	while (parser.next != TOK_EOF && parser.next != TOK_ERR) {
		struct acs_inst *inst;

		inst = parse_inst(&parser);
		if (!inst)
			break;

		list_add_tail(&inst->node, &script->inst);
	}

	lxr_destroy(parser.lxr);

	if (parser.next == TOK_ERR) {
		parse_err(&parser, "%s", lxr_buffer(parser.lxr));
		goto fail_script;
	}

	return script;

fail_script:
	free(script);

	return NULL;
}

