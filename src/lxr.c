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

static enum token lxr_get_id(struct lxr *lxr)
{
	int p, c = lxr_getc(lxr);
	for (p = 0; isalnum(c) || c == '_'; ++p, c = lxr_getc(lxr)) {
		if (p >= lxr->size)
			return lxr_error(lxr, "too long identifier");
		lxr->data[p] = c;
	}
	lxr->data[p] = 0;
	lxr_ungetc(lxr, c);

	enum token keywords[] = {
		TOK_TRUE, TOK_FALSE, TOK_NULL,
		TOK_RETURN, TOK_IF, TOK_ELSE,
		TOK_WHILE, TOK_FOR, TOK_BREAK,
		TOK_CONTINUE, TOK_EXPORT, TOK_IMPORT,
	};

	for (int i = 0; i < ARRAY_SIZE(keywords); ++i)
		if (strcmp(lxr->data, token_str[keywords[i]]) == 0)
			return keywords[i];

	return TOK_ID;
}

static enum token lxr_get_str(struct lxr *lxr)
{
	int p, c = lxr_getc(lxr);
	for (p = 0; c != '"'; ++p, c = lxr_getc(lxr)) {
		if (c == '\n' || c == EOF)
			return lxr_error(lxr, "unfinished string");
		if (c == '\\') { /* process escapes */
			c = lxr_getc(lxr);
			if (c == 'n')
				c = '\n';
			else if (c == '"' || c == '\\')
				;
			else
				return lxr_error(lxr, "invalid escape character");
		}
		if (p >= lxr->size)
			return lxr_error(lxr, "too long identifier");
		lxr->data[p] = c;
	}
	lxr->data[p] = 0;
	return TOK_STR;
}

static enum token lxr_get_int(struct lxr *lxr)
{
	int p, c = lxr_getc(lxr);
	for (p = 0; isdigit(c); ++p, c = lxr_getc(lxr)) {
		if (p >= lxr->size)
			return lxr_error(lxr, "too long identifier");
		lxr->data[p] = c;
	}
	lxr->data[p] = 0;
	lxr_ungetc(lxr, c);
	return TOK_INT;
}

static int lxr_ml_comm(struct lxr *lxr)
{
	for (int c = lxr_getc(lxr); ; c = lxr_getc(lxr)) {
		if (c == EOF)
			return -1;
		while (c == '*')
			c = lxr_getc(lxr);
		if (c == '/')
			return 0;
	}
}

/*************** EXTERNALS ***************/

enum token lxr_get(struct lxr *lxr)
{
	for (;;) {
		int c = lxr_getc(lxr);

		if (isalpha(c) || c == '_')
			return lxr_ungetc(lxr, c), lxr_get_id(lxr);
		if (isdigit(c))
			return lxr_ungetc(lxr, c), lxr_get_int(lxr);
		if (isspace(c))
			continue;

		switch (c) {
			case EOF: return TOK_EOF;
			case '(': return TOK_LPAR;
			case ')': return TOK_RPAR;
			case '{': return TOK_LBRA;
			case '}': return TOK_RBRA;
			case '[': return TOK_LSQR;
			case ']': return TOK_RSQR;
			case ',': return TOK_SEP;
			case '-': return TOK_MINUS;
			case '+': return TOK_PLUS;
			case '*': return TOK_MUL;
			case '%': return TOK_MOD;
			case ';': return TOK_SCOLON;
			case '"': return lxr_get_str(lxr);
		}

		/* parse digraphs */
		int d = lxr_getc(lxr);
		if (c == '/') {
			if (d == '/') { // single line comment
				for (; c != '\n'; c = lxr_getc(lxr))
					if (c == EOF)
						return TOK_EOF;
				continue;
			} else if (d == '*') { /* multi line comment */
				if (lxr_ml_comm(lxr) == 0)
					continue;
				return lxr_error(lxr, "unfinished comment");
			}
			return lxr_ungetc(lxr, d), TOK_DIV;
		} else if (c == '<') {
			if (d == '=')
				return TOK_LEQ;
			return lxr_ungetc(lxr, d), TOK_LESS;
		} else if (c == '>') {
			if (d == '=')
				return TOK_GREQ;
			return lxr_ungetc(lxr, d), TOK_GREAT;
		} else if (c == '=') {
			if (d == '=')
				return TOK_EQ;
			return lxr_ungetc(lxr, d), TOK_ASSIGN;
		} else if (c == '!') {
			if (d == '=')
				return TOK_NEQ;
			return lxr_ungetc(lxr, d), TOK_NOT;
		} else if (c == '.') {
			if (d == '.')
				return TOK_CONCAT;
			return lxr_ungetc(lxr, d), TOK_DOT;
		} else if (c == '|' && d == '|') {
			return TOK_OR;
		} else if (c == '&' && d == '&') {
			return TOK_AND;
		} else {
			lxr_ungetc(lxr, d);
			return lxr_error(lxr, "invalid character '%c'", c);
		}
	}
}

struct lxr *lxr_create(FILE *file, int max_token_size)
{
	struct lxr *lxr = malloc(sizeof (*lxr) + max_token_size);

	if (ERR_ON(!lxr, "malloc() failed"))
		return NULL;

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
