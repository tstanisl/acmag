#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token_id {
        TOK_ERROR,
        TOK_EOS,
        TOK_NUMBER,
        TOK_STRING,
        TOK_IDENT,

	__TOK_KEYWORDS,
        TOK_LPAR = __TOK_KEYWORDS, /* ( */
        TOK_RPAR, /* ) */
        TOK_LBRA, /* { */
        TOK_RBRA, /* } */
        TOK_SEP, /* , */
        TOK_MINUS, /* - */
        TOK_PLUS, /* + */
        TOK_MUL, /* * */
        TOK_DIV, /* / */
        TOK_MOD, /* % */
        TOK_AND, /* && */
        TOK_OR, /* || */
        TOK_EQUAL, /* == */
        TOK_ASSIGN, /* = */
        TOK_NEQUAL, /* != */
        TOK_LEQ, /* <= */
        TOK_GREQ, /* >= */
        TOK_LESS, /* < */
        TOK_GREAT, /* > */
        TOK_NOT, /* ! */
        TOK_DOT, /* . */
        TOK_SEMICOLON, /* ; */

        TOK_FUNCTION, /* function */
        TOK_RETURN, /* return */
        TOK_IF, /* if */
        TOK_ELSE, /* else */
        TOK_WHILE, /* while */
        TOK_FOR, /* for */
        TOK_BREAK, /* break */
        TOK_CONTINUE, /* continue */

        __TOK_N_BASIC,
};

struct token {
	enum token_id id;
	char *str;
} token_base[] = {
	{ TOK_ERROR, "#error" },
	{ TOK_EOS, "#eos" },
	{ TOK_NUMBER },
	{ TOK_STRING },
	{ TOK_IDENT },
	{ TOK_LPAR, "(" },
	{ TOK_RPAR, ")" },
	{ TOK_LBRA, "{" },
	{ TOK_RBRA, "}" },
        { TOK_SEP, "," },
        { TOK_MINUS, "-" },
        { TOK_PLUS, "+" },
        { TOK_MUL, "*" },
        { TOK_DIV, "/" },
        { TOK_MOD, "%" },
        { TOK_AND, "&&" },
        { TOK_OR, "||" },
        { TOK_EQUAL, "==" },
        { TOK_ASSIGN, "=" },
        { TOK_NEQUAL, "!=" },
        { TOK_LEQ, "<=" },
        { TOK_GREQ, ">=" },
        { TOK_LESS, "<" },
        { TOK_GREAT, ">" },
        { TOK_NOT, "!" },
        { TOK_DOT, "." },
        { TOK_SEMICOLON, ";" },
        { TOK_FUNCTION, "function" },
        { TOK_RETURN, "return" },
        { TOK_IF, "if" },
        { TOK_ELSE, "else" },
        { TOK_WHILE, "while" },
        { TOK_FOR, "for" },
        { TOK_BREAK, "break" },
        { TOK_CONTINUE, "continue" },
};

struct parser {
        int column;
	int line;
        FILE *f;
};

static int parser_skipws(struct parser *p)
{
	enum state {
		ST_NONE,
		ST_SLASH,
		ST_SL_COMMENT,
		ST_ML_COMMENT,
		ST_ML_STAR,
	} state = ST_NONE;
	for (;;) {
		int c = getc(p->f);
		if (c == EOF) {
			if (state == ST_NONE || state == ST_SL_COMMENT)
				return 0;
			return -1;
		}
		++p->column;
		if (state == ST_NONE) {
			if (c == '/') {
				state = ST_SLASH;
			} else if (!isspace(c)) {
				ungetc(c, p->f);
				return 0;
			}
			if (c == '\n') {
				++p->line;
				p->column = 0;
			}
		} else if (state == ST_SLASH) {
			if (c == '*') {
				state = ST_ML_COMMENT;
			} else if (c == '/') {
				state = ST_SL_COMMENT;
			} else {
				ungetc(c, p->f);
				ungetc('/', p->f);
				return 0;
			}
		} else if (state == ST_SL_COMMENT) {
			if (c == '\n') {
				++p->line;
				p->column = 0;
				state = ST_NONE;
			}
		} else if (state == ST_ML_COMMENT) {
			if (c == '*')
				state = ST_ML_STAR;
		} else if (state == ST_ML_STAR) {
			if (c == '/')
				state = ST_NONE;
			else if (c != '*')
				state = ST_ML_COMMENT;
		}
	}
}


struct tree *do_parser(FILE *f)
{
        struct parser parser;
        parser.line = 0;
        parser.column = 0;
        parser.f = f;

	parser_skipws(&parser);
	int c = fgetc(f);
	while (c != EOF) {
		putchar(c);
		parser_skipws(&parser);
		c = fgetc(f);
	}
	return NULL;
}

int main()
{
	do_parser(stdin);
        return 0;
}


