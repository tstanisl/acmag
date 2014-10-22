#include "cstr.h"
#include "debug.h"
#include "object.h"
#include "value.h"

#include <string.h>
#include <stdlib.h>

void value_clear(struct acs_value *val)
{
	if (val->id == VAL_STR && val->u.sval)
		str_put(val->u.sval);
	if (val->id == VAL_OBJ && val->u.oval)
		object_put(val->u.oval);
	/* FIXME: what about function instance */
	memset(val, 0, sizeof *val);
}

float value_to_num(const struct acs_value *val)
{
	if (val->id == VAL_NUM)
		return val->u.nval;
	else if (val->id == VAL_BOOL)
		return val->u.bval ? 1 : 0;
	else if (val->id == VAL_STR)
		return atof(val->u.sval->str);
	return 0;
}

void value_convert_num(struct acs_value *val)
{
	float nval = value_to_num(val);
	value_clear(val);
	val->id = VAL_NUM;
	val->u.nval = nval;
}

bool value_to_bool(const struct acs_value *val)
{
	if (!val)
		return false;
	if (val->id == VAL_NULL)
		return false;
	if (val->id == VAL_BOOL)
		return val->u.bval;
	return true;
}

void value_convert_bool(struct acs_value *val)
{
	bool bval = value_to_bool(val);
	value_clear(val);
	val->id = VAL_BOOL;
	val->u.bval = bval;
}

char *value_to_cstr(const struct acs_value *val)
{
	static char buf[32];

	if (val->id == VAL_STR) {
		return val->u.sval->str; /* nothing to do */
	} else if (val->id == VAL_NULL) {
		strcpy(buf, "null");
	} else if (val->id == VAL_NUM) {
		sprintf(buf, "%g", val->u.nval);
	} else if (val->id == VAL_BOOL) {
		strcpy(buf, val->u.bval ? "true" : "false");
	} else if (val->id == VAL_FUNC) {
		sprintf(buf, "(func):%p", (void*)val->u.fval);
	} else if (val->id == VAL_OBJ) {
		sprintf(buf, "(obj):%p", (void*)val->u.oval);
	} else {
		CRIT("value type not supported id=%d", (int)val->id);
	}
	return buf;
}

struct str *value_to_str(const struct acs_value *val)
{
	if (val->id == VAL_STR)
		return str_get(val->u.sval);
	return str_create(value_to_cstr(val));
}

void value_copy(struct acs_value *dst, const struct acs_value *src)
{
	WARN_ON(dst->id != VAL_NULL, "copying to non-NULL");
	if (src->id == VAL_STR)
		str_get(src->u.sval);
	if (src->id == VAL_OBJ)
		object_get(src->u.oval);
	dst->u = src->u;
	dst->id = src->id;
}

static inline int fsign(float f)
{
	return f > 0.0 ? 1 : (f < 0.0 ? -1 : 0);
}

int value_cmp(struct acs_value *a, struct acs_value *b)
{
	if (a->id == VAL_NULL)
		return 0;
	if (a->id == VAL_BOOL)
		return !!a->u.bval - !!value_to_bool(b);
	if (a->id == VAL_NUM)
		return fsign(a->u.nval - value_to_num(b));
	if (a->id == VAL_STR)
		return strcmp(a->u.sval->str, value_to_cstr(b));
	return memcmp(&a->u, &b->u, sizeof a->u);
}

