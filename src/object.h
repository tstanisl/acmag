#ifndef OBJECT_H
#define OBJECT_H

struct acs_value;
struct acs_field;
struct acs_object;

typedef void (*acs_object_dtor_cb)(struct acs_object *obj);

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

#endif /* OBJECT_H */
