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
			if (!isdigit(c)) {
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
	ACSA_POP,
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
	[ACSA_POP] = "pop",
	[ACSA_CALL] = "call",
	[ACSA_CALLB] = "callb",
	[ACSA_CALLG] = "callg",
	[ACSA_RET] = "ret",
	[ACSA_JZ] = "jz",
	[ACSA_JNZ] = "jnz",
	[ACSA_JMP] = "jmp",
};

#if 0
struct function {
	bool exported;
	char *name;
	struct list node;
};

static int cur_line = 0;
static DEFINE_LIST(function_head);

static struct function *function_create(char *name, bool exported)
{
	struct function *f = malloc(sizeof *f);

	if (ERR_ON(!f, "malloc() failed"))
		return NULL;

	f->name = strdup(name);
	if (ERR_ON(!f->name, "strdup() failed")) {
		free(f);
		return NULL;
	}

	f->exported = exported;

	return f;
}

static char *skipws(char *s)
{
	while (isspace(*s))
		++s;
	return s;
}

static int acsa_export(char *str)
{
	char name[64];
	int ret = sscanf(str, "%63[a-zA-Z0-9_]", name);
	if (ERR_ON(ret != 1, "export: invalid function name"))
		return -1;
	
	struct function *f = function_create(name, true);

	if (ERR_ON(!f, "function_create(\"%s\") failed", str))
		return -1;

	list_add(&function_head, &f->node);

	INFO("add %sfunction \"%s\"", f->exported ? "exported " : "", name);

	return 0;
}

static int acsa_label(char *str)
{
	char name[64];
	int ret = sscanf(str, "%63[a-zA-Z0-9_]", name);
	if (ERR_ON(ret != 1, "export: invalid label name at (%s)", str))
		return -1;

	INFO("add label \"%s\"", name);

	return 0;
}

static int asm_push(char *str)
{
	return 0;
}

static int acsa_line(char *str)
{
	INFO("line %3d: %s", cur_line, str);
	str = skipws(str);
	if (*str == ';') // skip comment line
		return 0;
	char cmd[16];
	int ret, shift = 0;
	ret = sscanf(str, "%15[a-zA-Z_0-9]%n", cmd, &shift);
	if (ret == 0 || cmd[0] == 0) // no command
		return 0;
	//INFO("cmd='%s'", cmd);
	str = skipws(str + shift);
	if (strcmp(cmd, "export") == 0)
		return acsa_export(str);
	if (str[0] == ':') {
		ret = acsa_label(cmd);
		if (ERR_ON(ret, "failed to add label \"%s\"", cmd))
			return ret;
		return acsa_line(str + 1);
	}
	if (strcmp(cmd, "push") == 0)
		return asm_push(str);

	ERR("unknown keyword '%s'", cmd);
	return -1;
	/*	if (strcmp(cmd, "pushn")
		return asm_push(str);
	if (strcmp(cmd, "callb")
		return asm_callb(str);
	if (strcmp(cmd, "call")
		return asm_call(str);*/
}
#endif

struct acsa {
	char *path;
	struct lxr lxr;
	enum token next;
	char payload[256];
};

static void acsa_consume(struct acsa *a)
{
	a->next = lxr_get_token(&a->lxr);
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

static int acsa_push(struct acsa *a)
{
	acsa_consume(a);
	if (a->next == TOK_HASH) {
		acsa_consume(a);
		if (a->next != TOK_INT)
			return acsa_err(a, "expected integer after #");
		printf("emit push #%d\n", atoi(a->payload));
		acsa_consume(a);
		return 0;
	}
	if (a->next == TOK_STR) {
		printf("emit push \"%s\"\n", a->payload);
		acsa_consume(a);
		return 0;
	}
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
	if (strcmp(str, "jz") == 0)
		return acsa_jump(a, ACSA_JZ);
	if (strcmp(str, "jnz") == 0)
		return acsa_jump(a, ACSA_JNZ);
	if (strcmp(str, "jmp") == 0)
		return acsa_jump(a, ACSA_JMP);
#if 0
	if (strcmp(str, "call") == 0)
		return acsa_call(a);
	if (strcmp(str, "callb") == 0)
		return acsa_callb(a);
	if (strcmp(str, "callg") == 0)
		return acsa_callg(a);
	if (strcmp(str, "pop") == 0)
		return acsa_pop(a);
#endif

	char *label = strdup(a->payload);

	acsa_consume(a);

	if (a->next != TOK_COLON) {
		acsa_err(a, "expected ':' after label '%s'",
			label);
		free(label);
		return -1;	
	}

	acsa_consume(a);

	printf("got label %s\n", label);
	// add label
	return 0;
}

static int acsa_load(char *path)
{
	FILE *f = fopen(path, "r");
	if (ERR_ON(!f, "fopen(\"%s\") failed: %s", path, ERRSTR))
		return -1;

	struct acsa a;

	a.path = path;
	a.lxr.file = f;
	a.lxr.line = 1;
	a.lxr.payload = a.payload;
	a.lxr.payload_size = ARRAY_SIZE(a.payload);

	acsa_consume(&a);

	int ret = -1;
	for (;;) {
		if (a.next == TOK_EOF) {
			ret = 0;
			break;
		}
		if (a.next == TOK_ERR) {
			acsa_err(&a, "%s", a.payload);
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
