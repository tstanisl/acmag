#include "acs.h"
#include "ast.h"
#include "lxr.h"
#include "debug.h"
#include "list.h"
#include "common.h"
#include "syntax.h"

int main()
{
	acs_init();
	/*extern void machine_test(void);
	machine_test();*/

	struct ast *ast = ast_from_file(stdin, "stdin");

	if (ERR_ON(!ast, "ast_from_file() failed"))
		return -1;

	ast_dump(ast);
	ast_free(ast);

#if 0
	dump_script(s);
	for (int i = 0, ok = 1; ok; ++i) {
		char buf[32];
		sprintf(buf, "main%d", i);
		printf("---- Running %s ----\n", buf);
		ok = !machine_call(s, buf, NULL);
		if (ERR_ON(!ok, "machine_call() failed"))
			break;
	}
	destroy_script(s);
#endif
	return 0;
}
