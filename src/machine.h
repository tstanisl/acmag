#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "syntax.h"

struct acs_stack;

int acs_push_int(struct acs_stack *st, int value);
int acs_push_str(struct acs_stack *st, char *str);

typedef int (*acs_user_function_cb)(struct acs_stack *st, void *priv);
int machine_add_function(char *name, int n_args, acs_user_function_cb *fptr);

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st);

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

struct acs_user_function {
	struct acs_value *(*call)(struct acs_user_function *, struct acs_value *);
};

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

int register_user_function(struct acs_user_function *ufunc, char *name);

/* Simple interface for variable maps */
struct acs_varmap {
	struct acs_var *head;
};

//struct acs_varmap *varmap_create(void);
//void varmap_destroy(struct acs_varmap *vmap);
int varmap_init(struct acs_varmap *vmap);
void varmap_deinit(struct acs_varmap *vmap);
struct acs_value *varmap_find(struct acs_varmap *vmap, char *name);
struct acs_value *varmap_insert(struct acs_varmap *vmap, char *name);
int varmap_delete(struct acs_varmap *vmap, char *name);

struct acs_object;
typedef void (*acs_object_dtor_cb)(struct acs_object *obj);
struct acs_object {
	struct acs_varmap fields;
	acs_object_dtor_cb dtor;
	int refcnt;
};

int object_init(struct acs_object *obj, acs_object_dtor_cb cb);
struct acs_object *object_get(struct acs_object *object);
void object_put(struct acs_object *object);

#endif /* MACHINE_H */
