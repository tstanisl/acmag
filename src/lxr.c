#include "lxr.h"
#include "debug.h"

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
	/* all states below fills token data */
	__LST_ECHO,
	LST_OR = __LST_ECHO,
	LST_EQ,
	LST_NEQ,
	LST_LEQ,
	LST_GREQ,
	LST_DOT,
	LST_INT,
	LST_STR,
	LST_ID,
	/* used to mark that lexer hidden state is used */
	__LST = 128,
};

int lxr_action[128];

static void lxr_init(void)
{
	static int lxr_initialized = 0;

	if (lxr_initialized)
		return;

	char id[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZadghjklmnopqstuvxyz_";
	for (int i = 0; id[i]; ++i)
		lxr_action[(int)id[i]] = LST_ID | __LST;
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
	lxr_action[','] = TOK_SEP;
	lxr_action['-'] = TOK_MINUS;
	lxr_action['+'] = TOK_PLUS;
	lxr_action['*'] = TOK_MUL;
	lxr_action['%'] = TOK_MOD;
	lxr_action[';'] = TOK_SCOLON;
}

/*************** EXTERNALS ***************/

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

void lxr_destory(struct lxr *lxr)
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
