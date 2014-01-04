#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token {
        TOK_ERR,
        TOK_EOF,
        TOK_INT,
        TOK_STR,
        TOK_ID,

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
        TOK_DOT, /* . */
        TOK_SCOLON, /* ; */

        TOK_LESS, /* < */
        TOK_GREAT, /* > */
        TOK_AMP, /* & */
        TOK_NOT, /* ! */
        TOK_ASSIGN, /* = */

        TOK_OR, /* || */
        TOK_AND, /* && */
        TOK_EQ, /* == */
        TOK_NEQ, /* != */
        TOK_LEQ, /* <= */
        TOK_GREQ, /* >= */

	__TOK_KEYWORD,
        TOK_RETURN = __TOK_KEYWORD, /* return */
        TOK_IF, /* if */
        TOK_ELSE, /* else */
        TOK_WHILE, /* while */
        TOK_FOR, /* for */
        TOK_BREAK, /* break */
        TOK_CONTINUE, /* continue */
};

struct lxr {
	int line;
        FILE *file;
};

enum lxr_state {
	LST_NONE,
	LST_STRB,
	LST_SLASH,
	LST_ML_COMM,
	LST_ML_STAR,
	LST_SL_COMM,
	LST_OR,
	LST_AND,
	LST_EQ,
	LST_NEQ,
	LST_LEQ,
	LST_GREQ,
	__LST_ECHO,
	LST_INT = __LST_ECHO,
	LST_STR,
	LST_ID,
	LST_KEYWORD,
	__LST = 1 << 8,
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

static enum token lxr_error(char *buffer, int max_size, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vsnprintf(buffer, max_size, fmt, va);
	va_end(va);
	return TOK_ERR;
}

static char *lxr_keywords[] = {
	[TOK_RETURN - __TOK_KEYWORD] = "return",
	[TOK_IF - __TOK_KEYWORD] = "if",
	[TOK_ELSE - __TOK_KEYWORD] = "else",
	[TOK_WHILE - __TOK_KEYWORD] = "while",
	[TOK_FOR - __TOK_KEYWORD] = "for",
	[TOK_BREAK - __TOK_KEYWORD] = "break",
	[TOK_CONTINUE - __TOK_KEYWORD] = "continue",
};

int lxr_action[128];

static void lxr_init(void)
{
	char id[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZadghjklmnopqstuvxyz_";
	for (int i = 0; id[i]; ++i)
		lxr_action[(int)id[i]] = LST_ID | __LST;
	for (int i = '0'; i <= '9'; ++i)
		lxr_action[i] = LST_INT | __LST;
	char wspc[] = " \r\n\t";
	for (int i = 0; wspc[i]; ++i)
		lxr_action[(int)wspc[i]] = LST_NONE | __LST;
	for (int  i = 0; lxr_keywords[i]; ++i)
		lxr_action[(int)lxr_keywords[i][0]] = (LST_KEYWORD + i) | __LST;
	lxr_action['/'] = LST_SLASH | __LST;
	lxr_action['"'] = LST_STRB  | __LST;
	lxr_action['<'] = LST_LEQ   | __LST;
	lxr_action['>'] = LST_GREQ  | __LST;
	lxr_action['&'] = LST_AND   | __LST;
	lxr_action['|'] = LST_OR    | __LST;
	lxr_action['!'] = LST_NEQ   | __LST;
	lxr_action['='] = LST_EQ    | __LST;
	lxr_action['('] = TOK_LPAR;
	lxr_action[')'] = TOK_RPAR;
	lxr_action['{'] = TOK_LBRA;
	lxr_action['}'] = TOK_RBRA;
	lxr_action[','] = TOK_SEP;
	lxr_action['-'] = TOK_MINUS;
	lxr_action['+'] = TOK_PLUS;
	lxr_action['*'] = TOK_MUL;
	lxr_action['%'] = TOK_MOD;
	lxr_action['.'] = TOK_DOT;
	lxr_action[';'] = TOK_SCOLON;
}

enum token lxr_get_token(struct lxr *lxr, char *buffer, int size)
{
	enum lxr_state st = LST_NONE;
	int pos = 0;

	for (;;) {
		int c = lxr_get(lxr);
		if (st == LST_NONE) {
			if (c == EOF)
				return TOK_EOF;
			int action = c < 128 ? lxr_action[c] : 0;
			if (!action)
				return lxr_error(buffer, size, "invalid char '%c'", c);
			if (~action & __LST)
				return action;
			st = action & ~__LST;
		} else if (st == LST_ID) {
			if (!isalpha(c) && c != '_') {
				lxr_unget(lxr, c);
				buffer[pos] = 0;
				return TOK_ID;
			}
		} else if (st == LST_STR || st == LST_STRB) {
			if (c == '"') {
				buffer[pos] = 0;
				return TOK_STR;
			}
			if (c == '\n' || c == EOF)
				return lxr_error(buffer, size, "broken string");
			st = LST_STR;
		} else if (st == LST_INT) {
			if (!isdigit(c)) {
				buffer[pos] = 0;
				lxr_unget(lxr, c);
				return TOK_INT;
			}
		} else if (st == LST_SLASH) {
			if (c == '/') {
				st = LST_SL_COMM;
			} else if (c == '*') {
				st = LST_ML_COMM;
			} else {
				lxr_unget(lxr, c);
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
				return lxr_error(buffer, size, "unfinished comment");
		} else if (st == LST_ML_STAR) {
			if (c == '/')
				st = LST_NONE;
			else if (c == EOF)
				return lxr_error(buffer, size, "unfinished comment");
		} else if (st == LST_OR) {
			if (c == '|')
				return TOK_OR;
			return lxr_error(buffer, size, "invalid operator '|'");
		} else if (st == LST_AND) {
			if (c == '&')
				return TOK_AND;
			lxr_unget(lxr, c);
			return TOK_AMP;
		} else if (st == LST_EQ) {
			if (c == '=')
				return TOK_EQ;
			lxr_unget(lxr, c);
			return TOK_ASSIGN;
		} else if (st == LST_NEQ) {
			if (c == '=')
				return TOK_NEQ;
			lxr_unget(lxr, c);
			return TOK_NOT;
		} else if (st == LST_LEQ) {
			if (c == '=')
				return TOK_LEQ;
			lxr_unget(lxr, c);
			return TOK_LESS;
		} else if (st == LST_GREQ) {
			if (c == '=')
				return TOK_GREQ;
			lxr_unget(lxr, c);
			return TOK_GREAT;
		} else { /* keyword */
			char *word = lxr_keywords[st - LST_KEYWORD];
			if (!isalpha(c) && c != '_') {
				buffer[pos] = 0;
				lxr_unget(lxr, c);
				if (word[pos])
					return TOK_ID;
				return st - LST_KEYWORD + __TOK_KEYWORD;
			}
			if (word[pos] != c)
				st = LST_ID;
		}

		if (st >= __LST_ECHO) {
			if (pos >= size)
				return lxr_error(buffer, size, "too long sequence");
			buffer[pos++] = c;
		}
	}
}

char *token_descr[] = {
	[TOK_ERR] = "#error",
	[TOK_EOF] = "#eof",
	[TOK_INT] = "#integer",
	[TOK_STR] = "#string",
	[TOK_ID] = "#identifier",
	[TOK_LPAR] = "(",
	[TOK_RPAR] = ")",
	[TOK_LBRA] = "{",
	[TOK_RBRA] = "}",
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
	[TOK_AMP] = "&",
	[TOK_SCOLON] = ";",
	[TOK_RETURN] = "return",
	[TOK_IF] = "if",
	[TOK_ELSE] = "else",
	[TOK_WHILE] = "while",
	[TOK_FOR] = "for",
	[TOK_BREAK] = "break",
	[TOK_CONTINUE] = "continue",
};

/* linked list */
struct list {
	struct list *prev, *next;
};

#define list_entry(l, s, m) \
	((s*)((char*)(l) - offsetof(s, m)))

static inline void list_init(struct list *h)
{
	h->next = h->prev = h;
}

static inline void list_add(struct list *n, struct list *h)
{
	h->next->prev = n;
	n->next = h->next;
	h->next = n;
	n->prev = h;
}

static inline void list_del(struct list *n)
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
}

#define list_for(l, h) \
	for (struct list l = (h)->next; l != (h); l = l->next)

#define list_empty(h) ((h)->next == (h))

struct function {
	struct list list;
	uint8_t *bcode;
	int n_args;
	int n_regs;
	char name[];
};

struct variable {
	int reg;
	struct list list;
	char name[];
};

static struct list function_head;

struct function *function_find(char *name)
{
	list_for(l, &function_head) {
		struct function *f = list_entry(l, struct function, list);
		if (strcmp(f->name, name) == 0)
			return f;
	}

	return NULL;
}

struct function *function_create(char *name)
{
	int length = strlen(name);
	struct function *f = calloc(1, sizeof(*f) + length + 1);

	if (!f)
		return NULL;
	list_add(&f->list, &function_head);
	strcpy(f->name, name);
	f->n_regs = 1;

	return f;
}

void function_destroy(struct function *f)
{
	free(f->bcode);
	list_del(&f->list);
	free(f);
}

#define SIZE 64

struct parser {
	struct lxr lxr;
	char buffer[SIZE];
	enum token next;
	struct list vars;
	int n_regs;
	int n_labels;
};

void parse_consume(struct parser *p)
{
	p->next = lxr_get_token(&p->lxr, p->buffer, SIZE);
}

int parse_expr(struct parser *p);

int parse_block(struct parser *p);

int parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON)
		return 0;
	if (p->next == TOK_RETURN) {
		parse_consume(p);
		if (p->next != TOK_SCOLON) {
			int ret = parse_expr(p);
			if (ret < 0)
				return -1;
			printf("$0 := $%d\n", ret);
		}
		printf("ret\n");
		return 0;
	}
	if (p->next == TOK_LBRA)
		return parse_block(p);
	if (p->next == TOK_WHILE) {
		parse_consume(p);
		if (p->next != TOK_LPAR) {
			printf("error(%d): expected '(' after while\n", line);
			return -1;
		}
		parse_consume(p);
		int label0 = p->n_labels++;
		int label1 = p->n_labels++;
		printf("label%d:\n", label0);
		int ret = parse_expr(p);
		if (ret < 0)
			return -1;
		if (p->next != TOK_RPAR) {
			printf("error(%d): expected ')' after expression\n", line);
			return -1;
		}
		parse_consume(p);
		printf("ifz $%d goto label%d\n", ret, label1);
		ret = parse_inst(p);
		if (ret < 0)
			return -1;
		printf("goto label%d\n", label0);
		printf("label%d:\n", label1);
		return 0;
	}
	if (p->next == TOK_IF) {
		parse_consume(p);
		if (p->next != TOK_LPAR) {
			printf("error(%d): expected '(' after while\n", line);
			return -1;
		}
		parse_consume(p);
		int label0 = p->n_labels++;

		int ret = parse_expr(p);
		if (ret < 0)
			return -1;

		if (p->next != TOK_RPAR) {
			printf("error(%d): expected ')' after expression\n", line);
			return -1;
		}
		parse_consume(p);

		printf("ifz $%d goto label%d\n", ret, label0);
		ret = parse_inst(p);
		if (ret < 0)
			return -1;
		if (p->next == TOK_ELSE) {
			parse_consume(p);
			int label1 = p->n_labels++;
			printf("goto label%d\n", label1);
			printf("label%d:\n", label0);
			ret = parse_inst(p);
			if (ret < 0)
				return -1;
			printf("label%d:\n", label1);
		} else {
			printf("label%d:\n", label0);
		}
		return 0;
	}
	/* trying to parse expression */
	return parse_expr(p);
}

int parse_block(struct parser *p)
{
	if (p->next != TOK_LBRA) {
		printf("error(%d): expected '{'\n", line);
		return -1;
	}

	parse_consume(p);

	for (;;) {
		if (p->next == TOK_RBRA) {
			parse_consume(p);
			return 0;
		}
		if (parse_inst(p))
			return -1;
	}
}

struct function *parse_function(struct parser *p)
{
	char *str = p->buffer;
	int line = p->lxr.line;

	/* check if function is already defined */
	if (function_find(str)) {
		printf("error(%d): redefined %s\n", line, str);
		return NULL;
	}

	struct function *f = function_create(str);
	if (!f) {
		printf("error(%d): failed to create %s\n", line, str);
		return NULL;
	}

	parse_consume(p);

	if (p->next != TOK_LPAR) {
		printf("error(%d): missing '('\n", line);
		goto fail_function;
	}
	parse_consume(p);

	list_init(&p->vars);

	for (;;) {
		if (p->next == TOK_RPAR) {
			parse_consume(p);
			break;
		}
		if (p->next == TOK_SEP && f->n_args > 0)
			parse_consume(p);
		if (p->next != TOK_ID) {
			printf("error(%d): expected identifier\n", line);
			goto fail_args;
		}
		int length = strlen(p->buffer);
		struct variable *v = calloc(1, sizeof(*v) + length + 1);
		if (!v) {
			printf("error(%d): expected identifier\n", line);
			goto fail_args;
		}
		strcpy(v->name, p->buffer);
		v->reg = p->n_regs++;
		list_add(&v->list, &p->vars);
	}

	int ret = parse_block(p);
	if (ret)
		goto fail_args;

	f->n_regs = v->n_regs;

	while (!list_empty(&p->vars)) {
		struct variable *v = list_entry(p->vars.next, struct variable, list);
		list_del(&v->list);
		free(v);
	}

	return f;

fail_args:
	while (!list_empty(&p->vars)) {
		struct variable *v = list_entry(p->vars.next, struct variable, list);
		list_del(&v->list);
		free(v);
	}

fail_function:
	function_destroy(f);

	return NULL;
}

int parse_file(struct parser *p, FILE *file)
{
	p->lxr.line = 1;
	p->lxr.file = file;
	parse_consume(p);

	while (p->next == TOK_ID)
		if (parse_function(p) != 0)
			return -1;
	if (p->next == TOK_ERR) {
		printf("error(%d):%s\n\n", p->lxr.line, p->buffer);
		return -1;
	}
	if (p->next != TOK_EOF) {
		printf("error(%d): token '%s' outside function\n",
			p->lxr.line, p->buffer);
		return -1;
	}
	return 0;
}

int main()
{
	lxr_init();
	struct parser p;
	parse_file(&p, stdin);
        return 0;
}
