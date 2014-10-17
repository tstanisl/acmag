#ifndef VARMAP_H
#define VARMAP_H

/* Simple interface for variable maps */
struct acs_varmap {
	struct acs_var *head;
};

int varmap_init(struct acs_varmap *vmap);
void varmap_deinit(struct acs_varmap *vmap);
struct acs_value *varmap_find(struct acs_varmap *vmap, char *name);
struct acs_value *varmap_insert(struct acs_varmap *vmap, char *name);
int varmap_delete(struct acs_varmap *vmap, char *name);


#endif /* VARMAP_H */
