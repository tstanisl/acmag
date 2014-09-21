#include "machine.h"
#include "vec.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

enum acs_value {
	VAL_NULL,
	VAL_TRUE,
	VAL_FALSE,
	VAL_INT,
	VAL_STR,
	VAL_MAP,
	VAL_REF,
};

struct acs_int {
	enum acs_value id;
	enum acs_value *next;
	int value;
};

struct acs_str {
	enum acs_value id;
	enum acs_value *next;
	char data[];
};

struct acs_stack {
	enum acs_value *head, *tail; 
};

static struct acs_function *script_find(struct acs_script *s, char *fname)
{
	list_foreach(l, &s->functions) {
		struct acs_function *f = list_entry(l, struct acs_function, node);
		if (strcmp(f->name, fname) == 0)
			return f;
	}
	return NULL;
}

static enum acs_value *call_inst(enum acs_id *id)
{
	return NULL;
}

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st)
{
	// - find function object
	struct acs_function *f = script_find(s, fname);
	if (ERR_ON(!f, "failed to find function %s", fname))
		return -1;

	// - copy stack to hash array by adding names
	// - execute function block 
	enum acs_value *value = call_inst(&f->block->id);
	if (ERR_ON(!value, "calling %s failed", fname))
		return -1;

	// - clear stack
	// - push results on stack
	return 0;
}
