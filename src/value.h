#ifndef VALUE_H
#define VALUE_H __LINE__

enum acs_type {
	VAL_NULL = 0,
	VAL_BOOL,
	VAL_NUM,
	VAL_STR,
	VAL_FUNC,
	VAL_USER,
	VAL_OBJ,
	__VAL_MAX,
};

struct str;
struct acs_function;
struct acs_user_function;
struct acs_object;

struct acs_value {
	enum acs_type id;
	struct acs_value *next; // used only by assignment
	union {
		int ival;
		struct str *sval;
		bool bval;
		struct acs_function *fval;
		struct acs_user_function *uval;
		struct acs_object *oval;
	} u;
};

/* Simple interface for variable maps */
struct acs_varmap {
	struct acs_var *head;
};

int varmap_init(struct acs_varmap *vmap);
void varmap_deinit(struct acs_varmap *vmap);
struct acs_value *varmap_find(struct acs_varmap *vmap, char *name);
struct acs_value *varmap_insert(struct acs_varmap *vmap, char *name);
int varmap_delete(struct acs_varmap *vmap, char *name);

#endif
