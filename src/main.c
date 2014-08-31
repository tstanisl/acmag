#include "lxr.h"
#include "debug.h"
#include "list.h"
#include "common.h"

int main()
{
	struct lxr *lxr = lxr_create(stdin, 256);

	if (ERR_ON(!lxr, "lxr_create() failed"))
		return -1;

	enum token token;
	do {
		token = lxr_get(lxr);
		printf("%s: %s\n", token_str[token], lxr_buffer(lxr));
	} while (token != TOK_EOF && token != TOK_ERR);

	lxr_destroy(lxr);

	return 0;
}
