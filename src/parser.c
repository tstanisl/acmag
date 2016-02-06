#include "debug.h"
#include "lxr.h"
#include "list.h"
#include "str.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_IDS 256
static struct str *const_str[MAX_IDS];
static int n_const_str;
static int const_num[MAX_IDS];
static int n_const_num;
static struct str *vars[MAX_IDS];
static int n_vars;

static int var_get(char *s)
{
	for (int i = 0; i < n_vars; ++i)
		if (strcmp(s, vars[i]->str) == 0)
			return str_get(vars[i]), i;
	vars[n_vars++] = str_create(s);
	return n_vars - 1;
}

static int const_str_get(char *s)
{
	for (int i = 0; i < n_const_str; ++i)
		if (strcmp(s, const_str[i]->str) == 0)
			return str_get(const_str[i]), i;
	const_str[n_const_str++] = str_create(s);
	return n_const_str - 1;
}

static int const_num_get(int v)
{
	for (int i = 0; i < n_const_num; ++i)
		if (v == const_num[i])
			return i;
	const_num[n_const_num++] = v;
	return n_const_num - 1;
}

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
	WARN_ON(cur == TOK_ERR, "consuming error");
	cur = lxr_get(lxr);
}

#define accept(c) ((c == cur) ? consume(), 1 : 0)

/*
 * assign = list [ '=' assign ]
 * list = expr [ ',' list ]
 * expr = sum
 * sum = sfx sum_tail
 * sum_tail = '+' sfx sum_tail ) | '-' sfx sum_tail | e
 * sfx = top sfx_tail
 * sfx_tail = e | '[' expr ']' | '(' args ')'
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
	lxr = lxr_create(stdin, 256);
	CRIT_ON(!lxr, "lxr_create() failed");
	consume();
}
