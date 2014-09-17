#include "lxr.h"
#include "debug.h"
#include "list.h"
#include "common.h"
#include "syntax.h"

int main()
{
	struct acs_script *s = parse_script(stdin, "stdin");

	if (ERR_ON(!s, "parse_script() failed"))
		return -1;

	return 0;
}
