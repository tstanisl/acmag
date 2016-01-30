#include "lxr.h"
#include "debug.h"
#include "list.h"
#include "common.h"

static void dump_tokens(void)
{
	struct lxr *lxr = lxr_create(stdin, 256);
	CRIT_ON(!lxr, "lxr_create() failed");
	enum token tok;
	do {
		tok = lxr_get(lxr);
		char *toks = token_str[tok];
		char *payload = lxr_buffer(lxr);
		if (tok == TOK_ID || tok == TOK_STR || tok == TOK_NUM
		    || tok == TOK_ERR)
			printf("%s: %s\n", toks, payload);
		else
			printf("%s\n", toks);
	} while (tok != TOK_EOF && tok != TOK_ERR);
}

int main()
{
	dump_tokens();
	return 0;
}
