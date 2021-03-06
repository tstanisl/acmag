#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token {
        TOK_ERR,
        TOK_EOF,
        TOK_INT,
        TOK_STR,
        TOK_ID,
        TOK_FUN,

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
	LST_FUNB,
	LST_EQ,
	LST_NEQ,
	LST_LEQ,
	LST_GREQ,
	__LST_ECHO,
	LST_INT = __LST_ECHO,
	LST_STR,
	LST_FUN,
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
	lxr_action['&'] = LST_FUNB  | __LST;
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
			if (!isalnum(c) && c != '_') {
				lxr_unget(lxr, c);
				buffer[pos] = 0;
				return TOK_ID;
			}
		} else if (st == LST_FUNB) {
			if (c == '&')
				return TOK_AND;
			if (!isalpha(c) && c != '_')
				return lxr_error(buffer, size,
					"missing function name after '&'");
			st = LST_FUN;
		} else if (st == LST_FUN) {
			if (!isalnum(c) && c != '_') {
				lxr_unget(lxr, c);
				buffer[pos] = 0;
				return TOK_FUN;
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
			else if (c != '*')
				st = LST_ML_COMM;
		} else if (st == LST_OR) {
			if (c == '|')
				return TOK_OR;
			return lxr_error(buffer, size, "invalid operator '|'");
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
			if (!isalnum(c) && c != '_') {
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
	[TOK_FUN] = "#function",
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
	for (struct list *l = (h)->next; l != (h); l = l->next)

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

struct variable *variable_create(char *name)
{
	int length = strlen(name);
	struct variable *v = calloc(1, sizeof(*v) + length + 1);

	if (!v)
		return NULL;
	strcpy(v->name, name);

	return v;
}

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

#define SIZE 256
#define CONSTS 1024
#define ARGS 16
#define REGS 256

struct constant {
	struct list list;
	int offset;
	int size;
};

struct parser {
	struct lxr lxr;
	char buffer[SIZE];
	enum token next;
	struct list vars;
	struct list consts;
	int regs[REGS];
	int n_regs;
	int next_reg;
	int n_labels;
	int data_size;
	char data[CONSTS];
	int loop_break;
	int loop_continue;
	bool first_in_expr;
};

enum result_id {
	RSLT_NIL,
	RSLT_REG,
	RSLT_REF,
	RSLT_FUN,
	RSLT_STR,
	RSLT_INT,
};

enum {
	PRS_OK = 0,
	PRS_NORETURN = 1,
};

struct result {
	enum result_id id;
	bool temp;
	int value;
	char name[SIZE];
};

static int token_buffered(enum token t)
{
	return t == TOK_STR || t == TOK_INT ||
		t == TOK_ID || t == TOK_ERR;
}

int parse_get_reg(struct parser *p)
{
	if (p->next_reg == -1)
		return p->n_regs++;
	int reg = p->next_reg;
	p->next_reg = p->regs[reg];
	return reg;
}

void parse_put_reg(struct parser *p, int reg)
{
	p->regs[reg] = p->next_reg;
	p->next_reg = reg;
}

void parse_result_put(struct parser *p, struct result *r)
{
	if ((r->id != RSLT_REG && r->id != RSLT_REF) || !r->temp)
		return;
	parse_put_reg(p, r->value);
}

void parse_consume(struct parser *p)
{
	p->next = lxr_get_token(&p->lxr, p->buffer, SIZE);
	p->first_in_expr = false;
#if DEBUG_TOKENS
	if (token_buffered(p->next))
		printf(" got %s: %s\n", token_descr[p->next],
			p->buffer);
	else
		printf(" got %s\n", token_descr[p->next]);
#endif
}

void parse_emit(struct parser *p, struct result *r)
{
	if (r->id == RSLT_REF)
		printf("$%d.%s", r->value, r->name);
	else if (r->id == RSLT_STR)
		printf("\"%s\"", r->name);
	else if (r->id == RSLT_INT)
		printf("%d", r->value);
	else if (r->id == RSLT_REG)
		printf("$%d", r->value);
	else if (r->id == RSLT_FUN)
		printf("%s", r->name);
}

void parse_deref(struct parser *p, struct result *r)
{
	if (r->id != RSLT_REF)
		return;
	r->id = RSLT_REG;
	if (r->temp) {
		printf("$%d = $%d.%s\n", r->value, r->value, r->name);
		return;
	}
	int reg = parse_get_reg(p);
	printf("$%d = $%d.%s\n", reg, r->value, r->name);
	parse_put_reg(p, r->value);
	r->value = reg;
	r->temp = true;
}

int parse_constant_find(struct parser *p, char *data, int size)
{
	list_for(l, &p->consts) {
		struct constant *c = list_entry(l, struct constant, list);
		if (size != c->size) 
			continue;
		char *pdata = p->data + 4 * c->offset;
		if (strncmp(data, pdata, size) == 0)
			return c->offset;
	}
	struct constant *c = malloc(sizeof *c);
	if (!c)
		return -1;
	c->offset = p->data_size / 4;
	c->size = size;
	memcpy(p->data + p->data_size, data, size);
	size = (size + 3) & ~3; /* align to 4 bytes */
	p->data_size += size;
	list_add(&c->list, &p->consts);
	return c->offset;
}

int parse_id_expr(struct parser *p, struct result *r)
{
	struct variable *v;
	list_for(l, &p->vars) {
		v = list_entry(l, struct variable, list);
		if (strcmp(v->name, p->buffer) == 0) {
			r->id = RSLT_REG;
			r->value = v->reg;
			parse_consume(p);
			return 0;
		}
	}
	strcpy(r->name, p->buffer);
	bool first_in_expr = p->first_in_expr;
	int line = p->lxr.line;
	parse_consume(p);
	/* '^x = ...' is declaration */
	if (p->next == TOK_ASSIGN && first_in_expr) {
		v = variable_create(r->name);
		if (!v) {
			printf("error(%d): failed to create %s\n", line, r->name);
			return -1;
		}
		v->reg = parse_get_reg(p);
		list_add(&v->list, &p->vars);
		r->id = RSLT_REG;
		r->value = v->reg;
		return 0;
	}
	r->id = RSLT_FUN;
	/* x(...) is function */
	if (p->next == TOK_LPAR)
		return 0;
	/* check is symbol is defined as function */
	if (function_find(r->name))
		return 0;
	printf("error(%d): undefined id '%s'\n", line, r->name);
	return -1;
}

int parse_expr(struct parser *p, struct result *r);

int parse_top_expr(struct parser *p, struct result *r)
{
	r->temp = false;
	if (p->next == TOK_ID)
		return parse_id_expr(p, r);
	if (p->next == TOK_INT) {
		r->id = RSLT_INT;
		if (sscanf(p->buffer, "%d", &r->value) != 1) {
			printf("error(%d): invalid integer format\n", p->lxr.line);
			return -1;
		}
		parse_consume(p);
		return 0;
	}
	if (p->next == TOK_STR) {
		r->id = RSLT_STR;
		strcpy(r->name, p->buffer);
		parse_consume(p);
		return 0;
	}
	if (p->next == TOK_LPAR) {
		parse_consume(p);
		int ret = parse_expr(p, r);
		if (ret < 0)
			return -1;
		if (p->next != TOK_RPAR) {
			printf("error(%d): unbalanced '('\n", p->lxr.line);
			return -1;
		}
		parse_consume(p);
		return 0;
	}
	printf("error(%d): unexpected token '%s'\n",
		p->lxr.line, token_descr[p->next]);
	return -1;
}

int parse_dot_expr(struct parser *p, struct result *r)
{
	if (r->id != RSLT_REG && r->id != RSLT_REF) {
		printf("error(%d): dereferencing invalid object\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	if (p->next != TOK_ID) {
		printf("error(%d): id expected after '.'\n", p->lxr.line);
		return -1;
	}
	if (r->id == RSLT_REG) {
		r->id = RSLT_REF;
	} else { // RSLT_REF
		int reg = r->value;
		if (r->temp)
			parse_put_reg(p, r->value);
		r->value = parse_get_reg(p);
		printf("$%d = $%d.%s\n", r->value, reg, r->name);
		r->temp = true;
	}
	strcpy(r->name, p->buffer);
	parse_consume(p);
	return 0;
}

int parse_fun_expr(struct parser *p, struct result *r)
{
	if (r->id != RSLT_REG && r->id != RSLT_REF && r->id != RSLT_FUN) {
		printf("error(%d): calling invalid object\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	/* parse argument list */
	struct result arg;
	int n_args = 0;
	while (p->next != TOK_RPAR) {
		int ret = parse_expr(p, &arg);
		if (ret < 0)
			return -1;
		if (++n_args > ARGS) {
			printf("error(%d): more then %d arguments\n", p->lxr.line, ARGS);
			return -1;
		}
		printf("push ");
		parse_emit(p, &arg);
		parse_result_put(p, &arg);
		printf("\n");
		if (p->next == TOK_SEP) {
			parse_consume(p);
		} else if (p->next != TOK_RPAR) {
			printf("error(%d): missing ','\n", p->lxr.line);
			return -1;
		}
	}
	parse_consume(p);

	int reg = parse_get_reg(p);
	printf("$%d = call ", reg);
	parse_emit(p, r);
	printf("(#%d)\n", n_args);
	r->id = RSLT_REG;
	r->value = reg;
	r->temp = true;
	return 0;
}

int parse_ref_expr(struct parser *p, struct result *r)
{
	int ret = parse_top_expr(p, r);
	if (ret < 0)
		return -1;
	while (ret >= 0)
		if (p->next == TOK_DOT)
			ret = parse_dot_expr(p, r);
		else if (p->next == TOK_LPAR)
			ret = parse_fun_expr(p, r);
		else
			return 0;
	return -1;
}

int parse_una_expr(struct parser *p, struct result *r)
{
	int t = p->next;
	if (t == TOK_PLUS) {
		parse_consume(p); /* drop unary plus */
		return parse_una_expr(p, r);
	}
	if (t != TOK_MINUS && t != TOK_NOT)
		return parse_ref_expr(p, r);
	parse_consume(p);
	int ret = parse_una_expr(p, r);
	if (ret < 0)
		return -1;
	parse_result_put(p, r);
	int reg = parse_get_reg(p);
	printf("$%d = %s", reg, token_descr[t]);
	parse_emit(p, r);
	printf("\n");
	r->id = RSLT_REG;
	r->value = reg;
	r->temp = true;
	return 0;
}

int parse_mul_expr(struct parser *p, struct result *r)
{
	int ret = parse_una_expr(p, r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_MUL && p->next != TOK_DIV && p->next != TOK_MOD)
		return 0;
	int reg = parse_get_reg(p);
	struct result l = {0};
	while (p->next == TOK_MUL || p->next == TOK_DIV || p->next == TOK_MOD) {
		int t = p->next;
		parse_consume(p);
		ret = parse_una_expr(p, &l);
		if (ret < 0)
			return -1;
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf(" %s ", token_descr[t]);
		parse_emit(p, &l);
		printf("\n");
		parse_result_put(p, &l);
		r->id = RSLT_REG;
		r->value = reg;
		r->temp = true;
	}
	return 0;
}

int parse_sum_expr(struct parser *p, struct result *r)
{
	int ret = parse_mul_expr(p, r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_PLUS && p->next != TOK_MINUS)
		return 0;
	int reg = parse_get_reg(p);
	struct result l = {0};
	while (p->next == TOK_PLUS || p->next == TOK_MINUS) {
		int t = p->next;
		parse_consume(p);
		ret = parse_mul_expr(p, &l);
		if (ret < 0)
			return -1;
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf(t == TOK_PLUS ? " + " : " - ");
		parse_emit(p, &l);
		printf("\n");
		parse_result_put(p, &l);
		r->id = RSLT_REG;
		r->value = reg;
		r->temp = true;
	}
	return 0;
}

int parse_cmp_expr(struct parser *p, struct result *r)
{
	int ret = parse_sum_expr(p, r);
	if (ret < 0)
		return -1;
	static int cmp_tokens[] = {
		TOK_LESS, TOK_LEQ,
		TOK_GREAT, TOK_GREQ,
		TOK_EQ, TOK_NEQ
	};
	int t;
	for (t = 0; t < 6; ++t)
		if (p->next == cmp_tokens[t])
			break;
	if (t == 6)
		return 0;
	t = cmp_tokens[t];
	int reg = parse_get_reg(p);
	parse_consume(p);
	struct result l = {0};
	ret = parse_sum_expr(p, &l);
	if (ret < 0)
		return -1;
	printf("$%d = ", reg);
	parse_emit(p, r);
	printf(" %s ", token_descr[t]);
	parse_emit(p, &l);
	printf("\n");
	parse_result_put(p, &l);
	r->id = RSLT_REG;
	r->value = reg;
	r->temp = true;
	return 0;
}

int parse_and_expr(struct parser *p, struct result *r)
{
	int ret = parse_cmp_expr(p, r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_AND)
		return 0;
	int label = p->n_labels++;
	int reg;
	if (r->id == RSLT_REG && r->temp) {
		reg = r->value;
	} else {
		reg = parse_get_reg(p);
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf("\n");
		parse_result_put(p, r);
	}
	while (p->next == TOK_AND) {
		parse_consume(p);
		printf("ifz $%d goto L%d\n", reg, label);
		ret = parse_cmp_expr(p, r);
		if (ret < 0)
			return -1;
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf("\n");
		parse_result_put(p, r);
	}
	printf("L%d:\n", label);
	r->id = RSLT_REG;
	r->value = reg;
	r->temp = true;
	return 0;
}

int parse_orr_expr(struct parser *p, struct result *r)
{
	int ret = parse_and_expr(p, r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_OR)
		return 0;
	int label = p->n_labels++;
	int reg;
	if (r->id == RSLT_REG && r->temp) {
		reg = r->value;
	} else {
		reg = parse_get_reg(p);
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf("\n");
		parse_result_put(p, r);
	}
	while (p->next == TOK_OR) {
		parse_consume(p);
		printf("if $%d goto L%d\n", reg, label);
		ret = parse_and_expr(p, r);
		if (ret < 0)
			return -1;
		printf("$%d = ", reg);
		parse_emit(p, r);
		printf("\n");
		parse_result_put(p, r);
	}
	printf("L%d:\n", label);
	r->id = RSLT_REG;
	r->value = reg;
	r->temp = true;
	return 0;
}

/* TODO: conside adding IGNORE_RESULT flag */
int parse_expr(struct parser *p, struct result *r)
{
	p->first_in_expr = true;
	int ret = parse_orr_expr(p, r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_ASSIGN)
		return 0;
	if (r->id != RSLT_REF && !(r->id == RSLT_REG && !r->temp)) {
		printf("error(%d): invalid LHS\n",
			p->lxr.line);
		return -1;
	}
	/* TODO: add checking if r is temporary */
	parse_consume(p);
	struct result l = {0};
	ret = parse_expr(p, &l);
	if (ret < 0)
		return -1;
	parse_emit(p, r);
	printf(" = ");
	parse_emit(p, &l);
	printf("\n");
	parse_result_put(p, &l);
	return 0;
}

int parse_block(struct parser *p);
int parse_inst(struct parser *p);

int parse_return(struct parser *p)
{
	parse_consume(p);
	if (p->next == TOK_SCOLON) {
		parse_consume(p);
		printf("ret 0\n");
		return PRS_NORETURN;
	}
	struct result r = {0};
	int ret = parse_expr(p, &r);
	if (ret < 0)
		return -1;
	printf("ret ");
	parse_emit(p, &r);
	printf("\n");
	parse_result_put(p, &r);
	if (p->next != TOK_SCOLON) {
		printf("error(%d): expected ';' after expression\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	return PRS_NORETURN;
}

int parse_while(struct parser *p)
{
	parse_consume(p);
	if (p->next != TOK_LPAR) {
		printf("error(%d): expected '(' after while\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	int old_break = p->loop_break;
	int old_continue = p->loop_continue;
	p->loop_continue = p->n_labels++;
	p->loop_break = p->n_labels++;
	printf("L%d:\n", p->loop_continue);
	struct result r = {0};
	int ret = parse_expr(p, &r);
	if (ret < 0)
		return -1;
	if (p->next != TOK_RPAR) {
		printf("error(%d): expected ')' after expression\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	if (p->next == TOK_SCOLON) {
		parse_consume(p);
		printf("if ");
		parse_emit(p, &r);
		printf(" goto L%d\n", p->loop_continue);
		parse_result_put(p, &r);
	} else {
		printf("ifz ");
		parse_emit(p, &r);
		printf(" goto L%d\n", p->loop_break);
		parse_result_put(p, &r);
		ret = parse_inst(p);
		if (ret < 0)
			return -1;
		if (ret != PRS_NORETURN)
			printf("goto L%d\n", p->loop_continue);
		printf("L%d:\n", p->loop_break);
	}
	p->loop_break = old_break;
	p->loop_continue = old_continue;
	return 0;
}

int parse_if(struct parser *p)
{
	/* consume IF token */
	parse_consume(p);
	if (p->next != TOK_LPAR) {
		printf("error(%d): expected '(' after if\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	int label0 = p->n_labels++;

	struct result r = {0};
	int ret = parse_expr(p, &r);
	if (ret < 0)
		return -1;

	if (p->next != TOK_RPAR) {
		printf("error(%d): expected ')' after expression\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);

	parse_deref(p, &r);
	printf("ifz ");
	parse_emit(p, &r);
	printf(" goto L%d\n", label0);
	parse_result_put(p, &r);
	ret = parse_inst(p);
	if (ret < 0)
		return -1;
	if (p->next != TOK_ELSE) {
		printf("L%d:\n", label0);
		return 0;
	}
	parse_consume(p);
	if (p->next == TOK_SCOLON) {
		printf("L%d:\n", label0);
		return 0;
	}
	if (ret == PRS_NORETURN) {
		printf("L%d:\n", label0);
		ret = parse_inst(p);
		if (ret < 0)
			return -1;
		return ret;
	} else {
		int label1 = p->n_labels++;
		printf("goto L%d\n", label1);
		printf("L%d:\n", label0);
		ret = parse_inst(p);
		if (ret < 0)
			return -1;
		printf("L%d:\n", label1);
		return 0;
	}
}

int parse_break(struct parser *p)
{
	if (p->loop_break < 0) {
		printf("error(%d): 'break' outside loop\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	if (p->next != TOK_SCOLON) {
		printf("error(%d): missing ';' after 'break'\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	printf("goto L%d\n", p->loop_break);
	return PRS_NORETURN;
}

int parse_continue(struct parser *p)
{
	if (p->loop_continue < 0) {
		printf("error(%d): 'continue' outside loop\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	if (p->next != TOK_SCOLON) {
		printf("error(%d): missing ';' after 'continue'\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	printf("goto L%d\n", p->loop_continue);
	return PRS_NORETURN;
}

int parse_inst(struct parser *p)
{
	if (p->next == TOK_SCOLON) {
		parse_consume(p);
		return 0;
	}
	if (p->next == TOK_RETURN)
		return parse_return(p);
	if (p->next == TOK_LBRA)
		return parse_block(p);
	if (p->next == TOK_WHILE)
		return parse_while(p);
	if (p->next == TOK_IF)
		return parse_if(p);
	if (p->next == TOK_CONTINUE)
		return parse_continue(p);
	if (p->next == TOK_BREAK)
		return parse_break(p);
	/* trying to parse expression */
	struct result r = {0};
	int ret = parse_expr(p, &r);
	if (ret < 0)
		return -1;
	parse_result_put(p, &r);
	if (p->next != TOK_SCOLON) {
		printf("error(%d): expected ';' after expression\n", p->lxr.line);
		return -1;
	}
	parse_consume(p);
	return 0;
}

int parse_block(struct parser *p)
{
	if (p->next != TOK_LBRA) {
		printf("error(%d): expected '{'\n", p->lxr.line);
		return -1;
	}

	parse_consume(p);

	int noret = 0;
	int warned = 0;
	for (;;) {
		if (p->next == TOK_RBRA) {
			parse_consume(p);
			return noret ? PRS_NORETURN : 0;
		}
		int line = p->lxr.line;
		int ret = parse_inst(p);
		if (ret < 0)
			return -1;
		if (noret && !warned) {
			printf("warn(%d): unreachable code\n", line);
			warned = 1;
		}
		if (ret == PRS_NORETURN)
			noret = 1;
	}
}

struct function *parse_function(struct parser *p)
{
	char *str = p->buffer;
	int line = p->lxr.line;
	p->n_labels = 0;
	p->n_regs = 0;
	p->next_reg = -1;
	p->loop_break = -1;
	p->loop_continue = -1;

	printf("---- starting function %s ----\n", str);

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
		if (f->n_args > 0) {
			if (p->next != TOK_SEP) {
				printf("error(%d): expected ,\n", line);
				goto fail_args;
			}
			parse_consume(p);
		}
		if (p->next != TOK_ID) {
			printf("error(%d): expected identifier\n", line);
			goto fail_args;
		}
		struct variable *v = variable_create(p->buffer);
		if (!v) {
			printf("error(%d): expected identifier\n", line);
			goto fail_args;
		}
		v->reg = parse_get_reg(p);
		list_add(&v->list, &p->vars);
		parse_consume(p);
		f->n_args++;
	}

	int ret = parse_block(p);
	if (ret < 0)
		goto fail_args;

	f->n_regs = p->n_regs;
	if (ret != PRS_NORETURN)
		printf("ret 0\n");

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
		if (parse_function(p) == NULL)
			return -1;
	if (p->next == TOK_ERR) {
		printf("error(%d):%s\n\n", p->lxr.line, p->buffer);
		return -1;
	}
	if (p->next != TOK_EOF) {
		printf("error(%d): token '%s' outside function\n",
			p->lxr.line, token_buffered(p->next) ? p->buffer : token_descr[p->next]);
		return -1;
	}
	return 0;
}

int main()
{
	lxr_init();
	list_init(&function_head);
	struct parser p;
	if (parse_file(&p, stdin) < 0)
		printf("error: parsing failed\n");
        return 0;
}
