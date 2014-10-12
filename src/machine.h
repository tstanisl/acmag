#ifndef MACHINE_H
#define MACHINE_H __FILE__

#include "syntax.h"
#include "value.h"

struct acs_stack;

int acs_push_int(struct acs_stack *st, int value);
int acs_push_str(struct acs_stack *st, char *str);

typedef int (*acs_user_function_cb)(struct acs_stack *st, void *priv);
int machine_add_function(char *name, int n_args, acs_user_function_cb *fptr);

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st);

struct acs_user_function {
	struct acs_value *(*call)(struct acs_user_function *, struct acs_value *);
};

int register_user_function(struct acs_user_function *ufunc, char *name);

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

#endif /* MACHINE_H */
