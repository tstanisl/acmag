#define _BSD_SOURCE

#include "debug.h"
#include "list.h"

#include <ctype.h>
#include <endian.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* lexer part */

enum token {
        TOK_ERR,
        TOK_EOF,
        TOK_INT,
        TOK_STR,
        TOK_ID,
        TOK_COLON,
        TOK_SEP,
        TOK_HASH,
        TOK_DOLAR,
};

enum lxr_state {
	LST_NONE,
	LST_COMM,
	LST_STRB,
	__LST_ECHO,
	LST_ID = __LST_ECHO,
	LST_INT,
	LST_STR,
	__LST = 1 << 8,
};

struct lxr {
	int line;
        FILE *file;
	char *payload;
	int payload_size;
};

int lxr_get(struct lxr *lxr)
{
	int c = fgetc(lxr->file);
	if (c == '\n')
		++lxr->line;
	return c;
}

void lxr_unget(struct lxr *lxr, int c)
{
	if (c == EOF)
		return;
	if (c == '\n')
		--lxr->line;
	ungetc(c, lxr->file);
}

static enum token lxr_error(struct lxr *lxr, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vsnprintf(lxr->payload, lxr->payload_size, fmt, va);
	va_end(va);
	return TOK_ERR;
}

int lxr_action[128];

static void lxr_init(void)
{
	for (int i = 0; i < 128; ++i)
		if (isgraph(i))
			lxr_action[i] = LST_ID | __LST;
	for (int i = '0'; i <= '9'; ++i)
		lxr_action[i] = LST_INT | __LST;
	lxr_action['-'] = LST_INT | __LST;
	lxr_action[':'] = TOK_COLON;
	lxr_action[','] = TOK_SEP;
	lxr_action['#'] = TOK_HASH;
	lxr_action['$'] = TOK_DOLAR;
	lxr_action[';'] = LST_COMM | __LST;
	lxr_action['"'] = LST_STRB | __LST;
	char wspc[] = " \r\n\t";
	for (int i = 0; wspc[i]; ++i)
		lxr_action[(int)wspc[i]] = LST_NONE | __LST;
}

enum token lxr_get_token(struct lxr *lxr)
{
	enum lxr_state st = LST_NONE;
	int pos = 0;
	lxr->payload[pos] = 0;

	for (;;) {
		int c = lxr_get(lxr);
		if (st == LST_NONE) {
			if (c == EOF)
				return TOK_EOF;
			if (c < 0 || c >= 128 || !lxr_action[c])
				return lxr_error(lxr, "not ASCII character (%d)", c);
			if (~lxr_action[c] & __LST) // return 1-character token
				return lxr_action[c];
			st = lxr_action[c] & ~__LST;
		} else if (st == LST_COMM) {
			if (c == '\n')
				st = LST_NONE;
			else if (c == EOF)
				return TOK_EOF;
		} else if (st == LST_STR || st == LST_STRB) {
			if (c == '"')
				return TOK_STR;
			if (c == '\n' || c == EOF)
				return lxr_error(lxr, "unfinished string");
			st = LST_STR;
		} else if (st == LST_INT) {
			if (!isdigit(c) && !(pos == 0 && c == '-')) {
				lxr_unget(lxr, c);
				return TOK_INT;
			}
		} else if (st == LST_ID) {
			if (!isgraph(c) || c == ':' || c == ',') { // ignore lable
				lxr_unget(lxr, c);
				return TOK_ID;
			}
		}

		// store character to token payload
		if (st >= __LST_ECHO) {
			if (pos >= lxr->payload_size)
				return lxr_error(lxr, "too long sequence");
			lxr->payload[pos++] = c;
			lxr->payload[pos] = 0;
		}
	}
}

char *token_descr[] = {
        [TOK_ERR] = "#error",
        [TOK_EOF] = "#eof",
        [TOK_INT] = "#integer",
        [TOK_STR] = "#string",
        [TOK_ID] = "#identifier",
        [TOK_COLON] = ":",
        [TOK_SEP] = ",",
        [TOK_HASH] = "#",
        [TOK_DOLAR] = "$",
};

#if 0
static char *lxr_keywords[] = {
	[TOK_EXPORT - __TOK_KEYWORD] = "export",
	[TOK_PUSH - __TOK_KEYWORD] = "push",
	[TOK_PUSHN - __TOK_KEYWORD] = "pushn",
	[TOK_POP - __TOK_KEYWORD] = "pop",
	[TOK_CALL - __TOK_KEYWORD] = "call",
	[TOK_CALLB - __TOK_KEYWORD] = "callb",
	[TOK_CALLG - __TOK_KEYWORD] = "callg",
	[TOK_RET - __TOK_KEYWORD] = "ret",
	[TOK_JZ - __TOK_KEYWORD] = "jz",
	[TOK_JNZ - __TOK_KEYWORD] = "jnz",
	[TOK_JMP - __TOK_KEYWORD] = "jmp",
};
#endif

#define MAGIC "ACSC"
#define MAGIC_LEN 4
#define MAX_OFFSETS (1 << 12)

enum acsa_cmd {
	ACSA_PUSHR,
	ACSA_PUSHI,
	ACSA_PUSHL,
	ACSA_PUSHS,
	ACSA_PUSHN,
	ACSA_POPR,
	ACSA_POPN,
	ACSA_CALL,
	ACSA_CALLB,
	ACSA_CALLG,
	ACSA_RET,
	ACSA_JZ,
	ACSA_JNZ,
	ACSA_JMP,
};

char *acsa_cmd_str[] = {
	[ACSA_PUSHR] = "pushr",
	[ACSA_PUSHI] = "pushi",
	[ACSA_PUSHL] = "pushl",
	[ACSA_PUSHS] = "pushs",
	[ACSA_PUSHN] = "pushn",
	[ACSA_POPR] = "popr",
	[ACSA_POPN] = "popn",
	[ACSA_CALL] = "call",
	[ACSA_CALLB] = "callb",
	[ACSA_CALLG] = "callg",
	[ACSA_RET] = "ret",
	[ACSA_JZ] = "jz",
	[ACSA_JNZ] = "jnz",
	[ACSA_JMP] = "jmp",
};

#if 0
struct vector {
	int capacity;
	int used;
	int item_size;
	char data[];
};

void *vector_create(int item_size, int capacity)
{
	struct vector *v = calloc(1, sizeof (*v) + item_size * capacity);
	if (!v)
		return NULL;
	v->capacity = capacity;
	v->item_size = item_size;
	return v->data;
}
#endif

struct acsa_ref {
	int offset;
	struct list node;
	char data[];
};

struct acsa {
	char *path;
	struct lxr lxr;
	enum token next;
	char payload[256];
	struct list exports;
	struct list consts;
	struct list labels;
	int n_consts;
	int pc;
	int capacity;
	uint16_t *program;
};

struct acsa_ref *acsa_ref_create(char *name, int offset)
{
	int length = strlen(name);
	struct acsa_ref *ref = malloc(sizeof(ref) + length + 1);

	if (ERR_ON(!ref, "out of memory"))
		return NULL;

	memcpy(ref->data, name, length + 1);
	ref->offset = offset;

	return ref;
}

#define acsa_ref_destroy free

static inline uint16_t asca_make_cmd(enum acsa_cmd cmd, int arg)
{
	return (cmd << 12) | (arg & 4095); 
}

static int acsa_emit(struct acsa *a, enum acsa_cmd cmd, int arg)
{
	if (a->pc >= a->capacity) {
		int new_capacity = a->capacity < 16 ? 16 : 2 * a->capacity;
		uint16_t *ptr = realloc(a->program,
			new_capacity * sizeof a->program[0]);

		if (ERR_ON(!ptr, "realloc(cap=%zi) failed", new_capacity))
			return -1;

		a->program = ptr;
		a->capacity = new_capacity;
	}

	printf("%04x: %s %d\n", a->pc, acsa_cmd_str[cmd], arg);
	a->program[a->pc++] = asca_make_cmd(cmd, arg);

	return 0;
}

static void acsa_consume(struct acsa *a)
{
	a->next = lxr_get_token(&a->lxr);
	//printf("* %s:%s\n", token_descr[a->next], a->payload);
}

static int acsa_err(struct acsa *a, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", a->path, a->lxr.line);
	vprintf(fmt, va);
	puts("");
	va_end(va);
	return -1;
}

static int acsa_export(struct acsa *a)
{
	acsa_consume(a);

	for (;;) {
		if (a->next != TOK_ID) {
			acsa_err(a, "identifier expected after export");
			return -1;
		}

		printf("add export %s\n", a->payload);

		acsa_consume(a);
		if (a->next != TOK_SEP)
			break;

		acsa_consume(a);
	}
	return 0;
}

static int acsa_push_int(struct acsa *a)
{
	acsa_consume(a);
	if (a->next != TOK_INT)
		return acsa_err(a, "expected integer after #");
	int i0 = atoi(a->payload);
	acsa_consume(a);

	if (a->next != TOK_SEP) {
		printf("emit pushi #%d\n", i0);
		return 0;
	}

	acsa_consume(a);
	if (a->next != TOK_INT)
		return acsa_err(a, "expected integer after ,");

	int i1 = atoi(a->payload);

	int ret = acsa_emit(a, ACSA_PUSHI, 8 * i0 + i1);
	if (ERR_ON(ret, "acsa_emit() failed"))
		return -1;

	acsa_consume(a);

	return 0;
}

static int acsa_pushs(struct acsa *a)
{
	struct acsa_ref *ref = acsa_ref_create(a->payload, a->n_consts);
	if (ERR_ON(!ref, "acsa_ref_create() failed"))
		return -1;

	int ret = acsa_emit(a, ACSA_PUSHS, a->n_consts);
	if (ERR_ON(ret, "acsa_emit() failed")) {
		acsa_ref_destroy(ref);
		return -1;
	}

	a->n_consts++;
	list_add(&ref->node, &a->consts);
	acsa_consume(a);

	return 0;
}

static int acsa_push(struct acsa *a)
{
	acsa_consume(a);
	if (a->next == TOK_HASH)
		return acsa_push_int(a);

	if (a->next == TOK_STR)
		return acsa_pushs(a);

	if (a->next == TOK_DOLAR) {
		acsa_consume(a);
		if (a->next != TOK_INT)
			return acsa_err(a, "expected integer after $");
		printf("emit push $%d\n", atoi(a->payload));
		acsa_consume(a);
		return 0;
	}
	acsa_err(a, "# or $ of string expected after push");
	return -1;
}

static int acsa_arg1i(struct acsa *a, enum acsa_cmd cmd)
{
	acsa_consume(a);
	if (a->next != TOK_HASH)
		return acsa_err(a, "# expected after %s", acsa_cmd_str[cmd]);

	acsa_consume(a);
	if (a->next != TOK_INT)
		return acsa_err(a, "expected integer after #");

	printf("emit %s #%d\n", acsa_cmd_str[cmd], atoi(a->payload));
	acsa_consume(a);

	return 0;
	
}

static int acsa_jump(struct acsa *a, enum acsa_cmd cmd)
{
	acsa_consume(a);

	if (a->next != TOK_ID)
		return acsa_err(a, "label expected after %s", acsa_cmd_str[cmd]);

	// check if label exists and jump is short enough
	printf("emit %s %s\n", acsa_cmd_str[cmd], a->payload);

	acsa_consume(a);

	return 0;
}

static int acsa_callg(struct acsa *a)
{
	acsa_consume(a);
	if (a->next != TOK_ID)
		return acsa_err(a, "id or string expected after callg");

	printf("emit callg %s\n", a->payload);
	acsa_consume(a);

	return 0;
}

static int acsa_callb(struct acsa *a)
{
	acsa_consume(a);
	if (a->next != TOK_ID)
		return acsa_err(a, "id or string expected after callb");

	// check if id is valid buildin
	printf("emit callb %s\n", a->payload);
	acsa_consume(a);

	return 0;
}

static int acsa_pop(struct acsa *a)
{
	acsa_consume(a);
	if (a->next != TOK_DOLAR)
		return acsa_err(a, "$ expected after pop");

	acsa_consume(a);
	if (a->next != TOK_INT)
		return acsa_err(a, "expected integer after $");

	printf("emit push $%d\n", atoi(a->payload));
	acsa_consume(a);
	return 0;
}

static int acsa_cmd(struct acsa *a)
{
	char *str = a->payload;

	if (strcmp(str, "export") == 0)
		return acsa_export(a);
	if (strcmp(str, "push") == 0)
		return acsa_push(a);
	if (strcmp(str, "ret") == 0)
		return acsa_arg1i(a, ACSA_RET);
	if (strcmp(str, "pushn") == 0)
		return acsa_arg1i(a, ACSA_PUSHN);
	if (strcmp(str, "popn") == 0)
		return acsa_arg1i(a, ACSA_POPN);
	if (strcmp(str, "jz") == 0)
		return acsa_jump(a, ACSA_JZ);
	if (strcmp(str, "jnz") == 0)
		return acsa_jump(a, ACSA_JNZ);
	if (strcmp(str, "jmp") == 0)
		return acsa_jump(a, ACSA_JMP);
	if (strcmp(str, "call") == 0)
		return acsa_jump(a, ACSA_CALL);
	if (strcmp(str, "callg") == 0)
		return acsa_callg(a);
	if (strcmp(str, "pop") == 0)
		return acsa_pop(a);
	if (strcmp(str, "callb") == 0)
		return acsa_callb(a);

	struct acsa_ref *ref = acsa_ref_create(a->payload, a->pc);
	if (ERR_ON(!ref, "acsa_ref_create() failed"))
		return -1;

	acsa_consume(a);

	if (a->next != TOK_COLON) {
		acsa_err(a, "expected ':' after label '%s'",
			ref->data);
		acsa_ref_destroy(ref);
		return -1;
	}

	acsa_consume(a);

	/* FIXME: add detection of duplicates */
	list_add(&ref->node, &a->labels);
	// add label
	return 0;
}

static int acsa_load(char *path)
{
	FILE *f = fopen(path, "r");
	if (ERR_ON(!f, "fopen(\"%s\") failed: %s", path, ERRSTR))
		return -1;

	struct acsa a;

	memset(&a, 0, sizeof a);
	a.path = path;
	a.lxr.file = f;
	a.lxr.line = 1;
	a.lxr.payload = a.payload;
	a.lxr.payload_size = ARRAY_SIZE(a.payload);
	list_init(&a.labels);
	list_init(&a.exports);
	list_init(&a.consts);

	acsa_consume(&a);

	int ret = -1;
	for (;;) {
		if (a.next == TOK_EOF) {
			ret = 0;
			break;
		}
		if (a.next != TOK_ID) {
			acsa_err(&a, "unexpected token (%s)",
				token_descr[a.next]);
			break;
		}
		if (acsa_cmd(&a) != 0)
			break;
	}
	if (a.next == TOK_ERR)
		acsa_err(&a, "%s", a.payload);

	fclose(f);

	/* do final label resolving */

	return ret;
}

int main(int argc, char *argv[])
{
	lxr_init();
	for (int i = 1; i < argc; ++i) {
		int ret = acsa_load(argv[i]);
		if (ERR_ON(ret, "while processing \"%s\"", argv[i]))
			return -1;
	}
	return 0;
}
