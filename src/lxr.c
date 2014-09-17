#include "lxr.h"
#include "debug.h"
#include "common.h"

#include <ctype.h>
#include <stdlib.h>

/*************** TYPE DECLARATIONS  ***************/

/**
 * struct lxr - LeXeR object
 * @file - input stream with source code
 * @line - current line in source code
 * @size - capacity of a buffer including 0-term
 * @data - token content
 */
struct lxr {
        FILE *file;
	int line;
	int size;
	char data[];
};


/*************** INTERNALS ***************/

static int lxr_getc(struct lxr *lxr)
{
	int c = fgetc(lxr->file);
	if (c == '\n')
		++lxr->line;
	return c;
}

static void lxr_ungetc(struct lxr *lxr, int c)
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
	vsnprintf(lxr->data, lxr->size, fmt, va);
	va_end(va);

	return TOK_ERR;
}

enum lxr_state {
	LST_NONE,
	LST_STRB,
	LST_SLASH,
	LST_ML_COMM,
	LST_ML_STAR,
	LST_SL_COMM,
	LST_OR,
	LST_EQ,
	LST_NEQ,
	LST_LEQ,
	LST_GREQ,
	LST_DOT,
	/* all states below fills token data */
	__LST_ECHO,
	LST_INT = __LST_ECHO,
	LST_STR,
	LST_ID,
	/* used to mark that lexer hidden state is used */
	__LST = 128,
};

static int lxr_action[128];

static void lxr_action_init(void)
{
	for (int i = 0; i < 128; ++i)
		if (isalpha(i) || i == '_')
			lxr_action[i] = LST_ID | __LST;
	for (int i = '0'; i <= '9'; ++i)
		lxr_action[i] = LST_INT | __LST;
	char wspc[] = " \r\n\t";
	for (int i = 0; wspc[i]; ++i)
		lxr_action[(int)wspc[i]] = LST_NONE | __LST;
	lxr_action['/'] = LST_SLASH | __LST;
	lxr_action['"'] = LST_STRB  | __LST;
	lxr_action['<'] = LST_LEQ   | __LST;
	lxr_action['>'] = LST_GREQ  | __LST;
	lxr_action['|'] = LST_OR    | __LST;
	lxr_action['!'] = LST_NEQ   | __LST;
	lxr_action['='] = LST_EQ    | __LST;
	lxr_action['.'] = LST_DOT   | __LST;
	lxr_action['('] = TOK_LPAR;
	lxr_action[')'] = TOK_RPAR;
	lxr_action['{'] = TOK_LBRA;
	lxr_action['}'] = TOK_RBRA;
	lxr_action['['] = TOK_LSQR;
	lxr_action[']'] = TOK_RSQR;
	lxr_action[','] = TOK_SEP;
	lxr_action['-'] = TOK_MINUS;
	lxr_action['+'] = TOK_PLUS;
	lxr_action['*'] = TOK_MUL;
	lxr_action['%'] = TOK_MOD;
	lxr_action[';'] = TOK_SCOLON;
}

#define LXR_HMASK ((1 << 8) - 1)
static enum token lxr_hash[LXR_HMASK + 1];

static unsigned strhash(char *str)
{
	unsigned hash = 9231;
	for (; *str; ++str)
		hash = 33 * hash + *str;
	return hash ^ (hash >> 16);
}

static void lxr_hash_insert(enum token token)
{
	unsigned hash = strhash(token_str[token]);

	for (unsigned step = 0; lxr_hash[hash & LXR_HMASK]; hash += ++step)
		if (lxr_hash[hash & LXR_HMASK] == token)
			return;

	lxr_hash[hash & LXR_HMASK] = token;
}

static enum token lxr_hash_find(char *str)
{
	unsigned hash = strhash(str);
	enum token token;

	for (unsigned step = 0; (token = lxr_hash[hash & LXR_HMASK]); hash += ++step)
		if (strcmp(str, token_str[token]) == 0)
			return token;

	return 0;
}

static void lxr_hash_init(void)
{
	enum token tokarr[] = {
		TOK_TRUE, TOK_FALSE, TOK_NULL,
		TOK_RETURN, TOK_IF, TOK_ELSE,
		TOK_WHILE, TOK_FOR, TOK_BREAK,
		TOK_CONTINUE, TOK_EXPORT, TOK_IMPORT,
	};
	for (int i = 0; i < ARRAY_SIZE(tokarr); ++i)
		lxr_hash_insert(tokarr[i]);
}

static void lxr_init(void)
{
	static int lxr_initialized = 0;

	if (lxr_initialized)
		return;

	lxr_action_init();
	lxr_hash_init();
	lxr_initialized = 1;
}

/*************** EXTERNALS ***************/

enum token lxr_get(struct lxr *lxr)
{
	enum lxr_state st = LST_NONE;
	int pos = 0;

	lxr->data[pos] = 0;
	for (;;) {
		int c = lxr_getc(lxr);

		if (st == LST_NONE) {
			if (c == EOF)
				return TOK_EOF;
			if (c < 0 || c >= ARRAY_SIZE(lxr_action) || !lxr_action[c])
				return lxr_error(lxr, "invalid char '%c'", c);

			int action = lxr_action[c];
			if (~action & __LST)
				return action;
			st = action & ~__LST;
		} else if (st == LST_ID) {
			if (!isalnum(c) && c != '_') {
				lxr_ungetc(lxr, c);
				enum token token = lxr_hash_find(lxr->data);
				return token ? token : TOK_ID;
			}
		} else if (st == LST_STR || st == LST_STRB) {
			if (c == '"')
				return TOK_STR;
			if (c == '\n' || c == EOF)
				return lxr_error(lxr, "unfinished string");
			st = LST_STR;
		} else if (st == LST_INT) {
			if (!isdigit(c)) {
				lxr_ungetc(lxr, c);
				return TOK_INT;
			}
		} else if (st == LST_SLASH) {
			if (c == '/') {
				st = LST_SL_COMM;
			} else if (c == '*') {
				st = LST_ML_COMM;
			} else {
				lxr_ungetc(lxr, c);
				return TOK_DIV;
			}
		} else if (st == LST_SL_COMM) {
			if (c == '\n')
				st = LST_NONE;
			else if (c == EOF)
				return TOK_EOF;
		} else if (st == LST_ML_COMM) {
			if (c == '*')
				st = LST_ML_STAR;
			else if (c == EOF)
				return lxr_error(lxr, "unfinished comment");
		} else if (st == LST_ML_STAR) {
			if (c == '/')
				st = LST_NONE;
			else if (c == EOF)
				return lxr_error(lxr, "unfinished comment");
			else if (c != '*')
				st = LST_ML_COMM;
		} else if (st == LST_OR) {
			if (c == '|')
				return TOK_OR;
			return lxr_error(lxr, "invalid operator '|'");
		} else if (st == LST_EQ) {
			if (c == '=')
				return TOK_EQ;
			lxr_ungetc(lxr, c);
			return TOK_ASSIGN;
		} else if (st == LST_NEQ) {
			if (c == '=')
				return TOK_NEQ;
			lxr_ungetc(lxr, c);
			return TOK_NOT;
		} else if (st == LST_LEQ) {
			if (c == '=')
				return TOK_LEQ;
			lxr_ungetc(lxr, c);
			return TOK_LESS;
		} else if (st == LST_GREQ) {
			if (c == '=')
				return TOK_GREQ;
			lxr_ungetc(lxr, c);
			return TOK_GREAT;
		} else if (st == LST_DOT) {
			if (c == '.')
				return TOK_CONCAT;
			lxr_ungetc(lxr, c);
			return TOK_DOT;
		} else { /* not possible */
			ERR("lexer reached undefined state (%d)", st);
			exit(-1);
		}

		if (st >= __LST_ECHO) {
			if (pos >= lxr->size)
				return lxr_error(lxr, "too long sequence");
			lxr->data[pos++] = c;
			lxr->data[pos] = 0;
		}
	}
}

struct lxr *lxr_create(FILE *file, int max_token_size)
{
	struct lxr *lxr = malloc(sizeof (*lxr) + max_token_size);

	if (ERR_ON(!lxr, "malloc() failed"))
		return NULL;

	lxr_init();

	lxr->line = 1;
	lxr->size = max_token_size;
	lxr->file = file;

	return lxr;
}

void lxr_destroy(struct lxr *lxr)
{
	free(lxr);
}

char *lxr_buffer(struct lxr *lxr)
{
	return lxr->data;
}

int lxr_line(struct lxr *lxr)
{
	return lxr->line;
}

char *token_str[] = {
	[TOK_ERR] = "#error",
	[TOK_EOF] = "#eof",
	[TOK_INT] = "#integer",
	[TOK_STR] = "#string",
	[TOK_ID] = "#identifier",
	[TOK_TRUE] = "true",
	[TOK_FALSE] = "false",
	[TOK_NULL] = "null",
	[TOK_LPAR] = "(",
	[TOK_RPAR] = ")",
	[TOK_LBRA] = "{",
	[TOK_RBRA] = "}",
	[TOK_LSQR] = "[",
	[TOK_RSQR] = "]",
	[TOK_SEP] = ",",
	[TOK_MINUS] = "-",
	[TOK_PLUS] = "+",
	[TOK_MUL] = "*",
	[TOK_DIV] = "/",
	[TOK_MOD] = "%",
	[TOK_AND] = "&&",
	[TOK_OR] = "||",
	[TOK_EQ] = "==",
	[TOK_ASSIGN] = "=",
	[TOK_NEQ] = "!=",
	[TOK_LEQ] = "<=",
	[TOK_GREQ] = ">=",
	[TOK_LESS] = "<",
	[TOK_GREAT] = ">",
	[TOK_NOT] = "!",
	[TOK_DOT] = ".",
	[TOK_CONCAT] = "..",
	[TOK_SCOLON] = ";",
	[TOK_RETURN] = "return",
	[TOK_IF] = "if",
	[TOK_ELSE] = "else",
	[TOK_WHILE] = "while",
	[TOK_FOR] = "for",
	[TOK_BREAK] = "break",
	[TOK_CONTINUE] = "continue",
	[TOK_EXPORT] = "export",
	[TOK_IMPORT] = "import",
};