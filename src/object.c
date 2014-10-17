#include "debug.h"
#include "object.h"
#include "value.h"

#include <stdlib.h>

struct acs_field {
	struct acs_value key;
	struct acs_value val;
	struct acs_field *next;
};


int object_init(struct acs_object *obj, acs_object_dtor_cb dtor)
{
	obj->fields = NULL;
	obj->dtor = dtor;
	obj->refcnt = 1;
	return 0;
}

struct acs_object *object_get(struct acs_object *obj)
{
	++obj->refcnt;
	return obj;
}

void object_put(struct acs_object *obj)
{
	if (--obj->refcnt)
		return;
	struct acs_field *fields = obj->fields;
	if (obj->dtor)
		obj->dtor(obj);
	for (struct acs_field *f = fields, *f_; f; f = f_) {
		f_ = f->next;
		value_clear(&f->key);
		value_clear(&f->val);
		free(f);
	}
}

struct acs_value *object_get_field(struct acs_object *obj,
	struct acs_value *key)
{
	for (struct acs_field *f = obj->fields; f; f = f->next)
		if (value_cmp(&f->key, key) == 0)
			return &f->val;
	struct acs_field *f = calloc(1, sizeof *f);
	if (ERR_ON(!f, "calloc() failed"))
		return NULL;
	value_copy(&f->key, key);
	f->next = obj->fields;
	obj->fields = f;
	return &f->val;
}

struct acs_value *object_try_field(struct acs_object *obj,
	struct acs_value *key)
{
	for (struct acs_field *f = obj->fields; f; f = f->next)
		if (value_cmp(&f->key, key) == 0)
			return &f->val;
	return NULL;
}

