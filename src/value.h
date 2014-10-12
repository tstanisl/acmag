#ifndef VALUE_H
#define VALUE_H __LINE__

#include <stdbool.h>

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
struct acs_finstance;

struct acs_value {
	enum acs_type id;
	union {
		float nval;
		struct str *sval;
		bool bval;
		struct acs_finstance *fval;
		struct acs_object *oval;
	} u;
};

struct acs_object;
typedef void (*acs_object_dtor_cb)(struct acs_object *obj);
struct acs_field;
struct acs_object {
	struct acs_field *fields;
	acs_object_dtor_cb dtor;
	int refcnt;
};

int object_init(struct acs_object *obj, acs_object_dtor_cb cb);
struct acs_object *object_get(struct acs_object *object);
void object_put(struct acs_object *object);
struct acs_value *object_get_field(struct acs_object *obj,
	struct acs_value *key);
struct acs_value *object_try_field(struct acs_object *obj,
	struct acs_value *key);

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
