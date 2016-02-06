#include "debug.h"
#include "lxr.h"
#include "list.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static enum token cur;
static struct lxr *lxr;

static int perr(char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%d: error: ", lxr_line(lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);
	cur = TOK_ERR;
	return -1;
}

static void consume(void)
{
	CRIT_ON(cur == TOK_ERR, "consuming error");
	cur = lxr_get(lxr);
}

#define accept(c) ((c == cur) ? consume(), 1 : 0)

/*
 * eq = list [ '=' list ]
 * list = expr [ ',' list ]
 * expr = sum
 * sum = ref sum_tail
 * sum_tail = '+' ref sum_tail ) | '-' ref sum_tail | e
 * ref = top ref_tail
 * ref_tail = e | '[' expr ']' | '(' args ')'
 * top = id | str | num | 'true' | 'false' | 'null'
 * args = e | expr [ ',' args ]
 */

typedef uint16_t opcode_t;

struct inst {
	struct list node;
	opcode_t opcode;
};

static struct inst *inst_new(opcode_t opcode)
{
	struct inst *inst = malloc(sizeof *inst);
	inst->opcode = opcode;
	return inst;
}

enum result_id {
	RI_NULL,
	RI_STACK,
	RI_FRAME,
	RI_GLOBAL,
	RI_FIELD,
};

struct result {
	enum result_id id;
	int arg;
	struct list code;
	struct result *next;
};

void parse_test(void)
{
}
