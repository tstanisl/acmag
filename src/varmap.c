#include "debug.h"
#include "value.h"
#include "varmap.h"

#include <stdlib.h>
#include <string.h>

struct acs_var {
	struct acs_var *next;
	struct acs_value val;
	char name[];
};

int varmap_init(struct acs_varmap *vmap)
{
	vmap->head = NULL;
	return 0;
}

void varmap_deinit(struct acs_varmap *vmap)
{
	for (struct acs_var *var = vmap->head, *next; var; var = next) {
		next = var->next;
		value_clear(&var->val);
		free(var);
	}
}

struct acs_value *varmap_find(struct acs_varmap *vmap, char *name)
{
	for (struct acs_var *var = vmap->head; var; var = var->next)
		if (strcmp(var->name, name) == 0)
			return &var->val;
	return NULL;
}

struct acs_value *varmap_insert(struct acs_varmap *vmap, char *name)
{
	struct acs_value *val = varmap_find(vmap, name);
	if (val)
		return val;

	struct acs_var *var = calloc(1, sizeof (*var) + strlen(name) + 1);
	if (ERR_ON(!var, "malloc() failed"))
		return NULL;

	strcpy(var->name, name);
	memset(&var->val, 0, sizeof var->val);
	var->val.id = VAL_NULL;

	var->next = vmap->head;
	vmap->head = var;

	return &var->val;
}

int varmap_delete(struct acs_varmap *vmap, char *name)
{
	return -1;
}

