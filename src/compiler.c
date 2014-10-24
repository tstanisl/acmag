#include "debug.h"
#include "function.h"
#include "lxr.h"
#include "list.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

struct compiler {
	char *path;
	struct lxr *lxr;
	enum token next;
	struct list inst;
};

static void consume(struct compiler *c)
{
	c->next = lxr_get(c->lxr);
	printf("next = %s\n", token_str[c->next]);
}

static int compile_inst(struct compiler *c)
{
	return -1;
}

struct acs_finstance *acs_compile_file(FILE *file, char *path)
{
	struct compiler c = { .path = path };

	c.lxr = lxr_create(file, 256);
	if (ERR_ON(!c.lxr, "lxr_create() failed"))
		goto fail;

	consume(&c);
	while (c.next != TOK_EOF) {
		int ret = compile_inst(&c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			goto fail_lxr;
	}

	/* TODO: solve names, generate closure code etc */

	return 0;
fail_lxr:
	lxr_destroy(c.lxr);
fail:
	return NULL;
}
