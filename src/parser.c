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
			return i;
	vars[n_vars++] = str_create(s);
	return n_vars - 1;
}

static int const_str_get(char *s)
{
	for (int i = 0; i < n_const_str; ++i)
		if (strcmp(s, const_str[i]->str) == 0)
			return i;
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
 * top = var | str | num | 'true' | 'false' | 'null'
 * var = [ '$' | '.' ] id
 * args = e | expr [ ',' args ]
 */

typedef uint16_t opcode_t;

struct inst {
	struct list node;
	char str[];
};

enum result_id {
	RI_NULL,
	RI_STACK,
	RI_FRAME,
	RI_GLOBAL,
	//RI_CONST,
	RI_FIELD,
	//RI_CALL,
};

struct result {
	enum result_id id;
	int arg;
	struct list code;
	struct result *next;
};

static struct result *new_result(void)
{
	struct result *res = malloc(sizeof *res);
	res->next = NULL;
	list_init(&res->code);
	return res;
}

static void dump_result(struct result *res)
{
	char *id_to_str[] = {
		[RI_NULL] = "null",
		[RI_STACK] = "stack",
		[RI_FRAME] = "frame",
		[RI_GLOBAL] = "global",
		[RI_FIELD] = "field",
	};
	printf("%s arg=%d next=%p\n", id_to_str[res->id], res->arg,
		(void*)res->next);
	list_foreach(l, &res->code) {
		struct inst *inst = list_entry(l, struct inst, node);
		printf("\t%s\n", inst->str);
	}
}

static void dump_result_rec(struct result *res, int depth)
{
	if (res) {
		printf("%*s", 2 * depth, "");
		dump_result(res);
		dump_result_rec(res->next, depth + 1);
	}
}

static void emit(struct result *res, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);

	size_t size = vsnprintf(NULL, 0, fmt, va);
	va_end(va);

	struct inst *inst = malloc(size + 1 + sizeof *inst);
	CRIT_ON(!inst, "OOM!");

	va_start(va, fmt);
	vsnprintf(inst->str, size + 1, fmt, va);
	va_end(va);

	list_add_tail(&inst->node, &res->code);
	//printf("added '%s'\n", inst->str);
}

static void push(struct result *res)
{
	if (res->id == RI_FRAME) {
		emit(res, "pushf #%d", res->arg);
	} else if (res->id == RI_GLOBAL) {
		emit(res, "call @getglobal");
	} else if (res->id == RI_FIELD) {
		emit(res, "call @getfield");
	} else if (res->id == RI_NULL) {
		emit(res, "pushn #1");
	}
}

static void pop(struct result *res)
{
	if (res->id == RI_FRAME) {
		emit(res, "popf #%d", res->arg);
	} else if (res->id == RI_GLOBAL) {
		emit(res, "call @setglobal");
	} else if (res->id == RI_FIELD) {
		emit(res, "call @setfield");
	} else {
		CRIT("storing to LHS value");
	}
}

static void drop(struct result *res)
{
	if (res->id == RI_GLOBAL || res->id == RI_STACK) {
		emit(res, "popn #1");
	} else if (res->id == RI_FIELD) {
		emit(res, "popn #2");
	}
}

static void flatten(struct result *head, struct result *res, int expects)
{
	if (!res) {
		if (expects > 0)
			emit(head, "pushn #%d", expects);
		return;
	}
	printf("flatten(res=%p)\n", (void*)res);
	dump_result(res);
	if (expects > 0)
		push(res);
	else
		drop(res);
	list_splice_tail(&res->code, &head->code);
	flatten(head, res->next, expects - 1);
}

static void arg_flatten(struct result *head, struct result *res)
{
	if (!res)
		return;
	dump_result(res);
	push(res);
	list_splice(&res->code, &head->code);
	arg_flatten(head, res->next);
}

static int length(struct result *head)
{
	int len = 0;
	for (; head; head = head->next)
		++len;
	return len;
}

static void parse_expr(struct result *res, int expects);
static void parse_scalar(struct result *res);

static void parse_top(struct result *res)
{
	if (cur == TOK_NULL) {
		res->id = RI_STACK;
		emit(res, "pushn #1");
	} else if (cur == TOK_ID) {
		res->id = RI_FRAME;
		char *name = lxr_buffer(lxr);
		//printf("name=%s\n", name);
		res->arg = var_get(name);
		//printf("name=%s id=%d\n", name, res->arg);
		//emit(res, "pushf #%d", res->arg);
		consume();
	} else if (cur == TOK_NUM) {
		res->id = RI_STACK;
		char *value = lxr_buffer(lxr);
		//printf("value=%s\n", value);
		emit(res, "pushi #%s", value);
		consume();
	} else if (cur == TOK_STR) {
		res->id = RI_STACK;
		char *value = lxr_buffer(lxr);
		//printf("value=%s\n", value);
		emit(res, "pushs \"%s\"", value);
		consume();
	} else if (accept(TOK_LPAR)) {
		parse_expr(res, 1);
		if (!accept(TOK_RPAR))
			CRIT("missing )");
	} else if (accept(TOK_LBRA)) {
		parse_expr(res, 0);
		if (!accept(TOK_RBRA))
			CRIT("missing }");
		res->id = RI_NULL;
	} else if (accept(TOK_COLON)) {
		CRIT_ON(cur != TOK_ID, "id expected after ':'");
		char *value = lxr_buffer(lxr);
		emit(res, "pushs \"%s\"", value);
		res->id = RI_GLOBAL;
		consume();
	} else {
		CRIT("unexpected token '%s'", token_str[cur]);
	}
}

static struct result *parse_list(void);

static void parse_sfx_tail(struct result *res)
{
	if (accept(TOK_DOT)) {
		push(res);
		CRIT_ON(cur != TOK_ID, "ID expected after '.'");
		char *value = lxr_buffer(lxr);
		emit(res, "pushs \"%s\"", value);
		consume();
		res->id = RI_FIELD;
		parse_sfx_tail(res);
	} else if (accept(TOK_LPAR)) { // function call
		struct result *args = NULL;
		if (cur != TOK_RPAR)
			args = parse_list();
		int n_args = length(args);
		if (!accept(TOK_RPAR))
			CRIT(") expected");
		push(res);
		arg_flatten(res, args);
		emit(res, "call #%d, #1", n_args);
		res->id = RI_STACK;
		parse_sfx_tail(res);
	}
}

static void parse_sfx(struct result *res)
{
	parse_top(res);
	parse_sfx_tail(res);
}

static void parse_sum_tail(struct result *res)
{
	int add;
	if (accept(TOK_PLUS)) {
		add = 1;
	} else if (accept(TOK_MINUS)) {
		add = 0;
	} else {
		return;
	}
	push(res);
	parse_sfx(res);
	push(res);
	emit(res, "call @%s", add ? "add" : "sub");
	res->id = RI_STACK;
	parse_sum_tail(res);
}

static void parse_sum(struct result *res)
{
	parse_sfx(res);
	parse_sum_tail(res);
}

static void parse_scalar(struct result *res)
{
	parse_sum(res);
}

static void parse_list_tail(struct result *res)
{
	if (!accept(TOK_SEP))
		return;
	res->next = new_result();
	parse_scalar(res->next);
	parse_list_tail(res->next);
}

static struct result *parse_list(void)
{
	struct result *res = new_result();
	parse_scalar(res);
	parse_list_tail(res);
	return res;
}

// TODO: use 'left =  expects - depth' as argument
static void assign_rec(struct result *head, struct result *dst, int expects, int depth)
{
	if (!dst)
		return;

	assign_rec(head, dst->next, expects, depth + 1);

	if (depth + 1 == expects)
		emit(head, "call @dup%d", expects);

	pop(dst);
	list_splice_tail(&dst->code, &head->code);
}

static void parse_assign(struct result *res, int expects)
{
	struct result *dst = parse_list();

	if (!accept(TOK_ASSIGN)) {
		flatten(res, dst, expects);
		return;
	}

	int len = length(dst);
	parse_assign(res, len);
	assign_rec(res, dst, expects, 0);
	if (expects > len)
		emit(res, "pushn #%d", expects - len);
}

static void parse_expr(struct result *res, int expects)
{
	parse_assign(res, expects);
}

void parse_test(void)
{
	lxr = lxr_create(stdin, 256);
	CRIT_ON(!lxr, "lxr_create() failed");
	consume();
	struct result res;
	list_init(&res.code);
	while (cur != TOK_EOF) {
		parse_assign(&res, 0);
	}

	printf("-------- FINAL -----------\n");
	dump_result(&res);
}
