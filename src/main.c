#include "lxr.h"
#include "debug.h"
#include "list.h"
#include "common.h"
#include "syntax.h"
#include "machine.h"

#if 0
int main()
{
	struct acs_script *s = parse_script(stdin, "stdin");

	if (ERR_ON(!s, "parse_script() failed"))
		return -1;

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

	return 0;
}
#endif
