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

	__TOK_OPERATORS = TOK_IDENT,
        TOK_LPAR, /* ( */
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
        TOK_AMP, /* & */
        TOK_SEMICOLON, /* ; */

	__TOK_KEYWORDS = SEMICOLON,
        TOK_RETURN, /* return */
        TOK_IF, /* if */
        TOK_ELSE, /* else */
        TOK_WHILE, /* while */
        TOK_FOR, /* for */
        TOK_BREAK, /* break */
        TOK_CONTINUE, /* continue */
};

struct token {
	enum token_id id;
	char *str;
} base_token[] = {
	{ TOK_ERROR, "#error" },
	{ TOK_EOS, "#eos" },
	{ TOK_NUMBER, "#number" },
	{ TOK_STRING, "#string" },
	{ TOK_IDENT, "#identifier" },
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
        { TOK_DOT, "." },
        { TOK_SCOLON, ";" },
        { TOK_OR, "||" },
        { TOK_AND, "&&" },
        { TOK_AMP, "&" },
        { TOK_EQUAL, "==" },
        { TOK_ASSIGN, "=" },
        { TOK_NEQUAL, "!=" },
        { TOK_NOT, "!" },
        { TOK_LEQ, "<=" },
        { TOK_LESS, "<" },
        { TOK_GREQ, ">=" },
        { TOK_GREAT, ">" },
        { TOK_RETURN, "return" },
        { TOK_IF, "if" },
        { TOK_ELSE, "else" },
        { TOK_WHILE, "while" },
        { TOK_FOR, "for" },
        { TOK_BREAK, "break" },
        { TOK_CONTINUE, "continue" },
};

#define PARSER_MAX_UNGET 16
struct parser {
	char unget[PARSER_MAX_UNGET];
	int ungets;
        int column;
	int line;
        FILE *f;
};

static int parset_get(struct parser *p)
{
	if (p->ungets)
		return p->unget[--p->ungets];
	return getc(p->f);
}

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
		if (c == '\n') {
			++p->line;
			p->column = 0;
		} else {
			++p->column;
		}
		if (state == ST_NONE) {
			if (c == '/') {
				state = ST_SLASH;
			} else if (!isspace(c)) {
				ungetc(c, p->f);
				return 0;
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
			if (c == '\n')
				state = ST_NONE;
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

enum lex_state {
	LST_NONE,
	LST_SLASH,
	LST_ML_COMM,
	LST_ML_STAR,
	LST_SL_COMM,
	LST_STR,
	LST_OP,
	LST_INT,
	LST_FLT,
	LST_ID,
} state;

#define LST_EMIT(token) ((token) << 16)
#define LST_TOKEN(lst) ((lst) >> 16)
#define LST_ECHO (1 << 15)

#define LEX_MAX_SIZE 4096

struct token_desc {
	enum token_id id;
	
} char2token[128];

struct token *lexer_get_token(struct lexer *l)
{
	enum lex_state state = LST_NONE;
	static char buffer[LEX_MAX_SIZE];
	int size = 0;

	for (;;) {
		int c = fgetc(l->file);
		if (c == '\n') {
			++l->line;
			l->column = 0;
		} else {
			++l->column;
		}
		if (state == LST_NONE) {
			if (c == '(')
				return &base_token[TOK_LPAR];
			if (c == ')')
				return &base_token[TOK_RPAR];
			if (c == '{')
				return &base_token[TOK_LBRA];
			if (c == '}')
				return &base_token[TOK_RBRA];
			if (c == ',')
				return &base_token[TOK_SEP];
			if (c == '-')
				return &base_token[TOK_MINUS];
			if (c == '+')
				return &base_token[TOK_PLUS];
			if (c == '*')
				return &base_token[TOK_MUL];
			if (c == '%')
				return &base_token[TOK_MOD];
			if (c == '.')
				return &base_token[TOK_DOT];
			if (c == ';')
				return &base_token[TOK_SCOLON];
			if (c == EOF)
				return &base_token[TOK_EOS];
			buffer[size++] = c;
			if (c == '/')
				lst = LST_SLASH;
			else if (c == '"')
				lst = LST_STR;
			else if (c == '|' || c == '&' || c == '='
			         c == '!' || c == '<' || c == '>')
				lst = LST_OP;
			else if (isdigit(c))
				lst = LST_INT;
			else if (isalpha(c) || c == '_')
				lst = LST_ID;
			else if (!isspace(c))
				return &base_token[TOK_ERROR];
		} else if (lst == LST_SLASH) {
			if (c == '/') {
				lst = LST_SL_COMM;
			} else if (c == '*') {
				lst = LST_ML_COMM;
			} else {
				ungetc(c, l->f);
				return &base_token[TOK_DIV];
			}
		} else if (lst == LST_SL_COMM) {
			if (c == EOF)
				return &base_token[TOK_EOS];
			if (c == '\n')
				lst = LST_NONE;
		} else if (lst == LST_ML_COMM) {
			if (c == EOF)
				return &base_token[TOK_EOS];
			if (c == '*')
				lst = LST_ML_STAR;
		} else if (lst == LST_ML_STAR) {
			if (c == EOF)
				return &base_token[TOK_EOS];
			if (c == '/')
				lst = LST_NONE;
		} else if (lst == LST_OP) {
			if (c == 
		}
	}
}

struct tree *do_parser(FILE *f)
{
        struct parser parser;
        parser.line = 0;
        parser.column = 0;
	parser.ungets = 0;
        parser.f = f;

	return NULL;
}

int main()
{
	do_parser(stdin);
        return 0;
}


