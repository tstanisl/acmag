#include "value.h"
#include "cstr.h"

void value_clear(struct acs_value *val)
{
	if (val->id == VAL_STR && val->u.sval)
		str_put(val->u.sval);
	if (val->id == VAL_OBJ && val->u.oval)
		object_put(val->u.oval);
	/* FIXME: what about function instance */
	memset(&val->u, 0, sizeof val->u);
}

void value_convert_num(struct acs_value *val)
{
	float nval = 0;
	if (val->id == VAL_NUM)
		nval = val->u.nval;
	else if (val->id == VAL_BOOL)
		nval = (val->u.bval ? 1 : 0);
	else if (val->id == VAL_STR)
		nval = atof(val->u.sval->str);
	value_clear(val);
	val->id = VAL_NUM;
	val->u.nval = nval;
}

bool value_to_bool(struct acs_value *val)
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

void value_copy(struct acs_value *dst, struct acs_value *src)
{
	WARN_ON(src->id != VAL_NULL, "copying to non-NULL");
	if (src->id == VAL_STR)
		str_get(src->u.sval);
	if (src->id == VAL_OBJ)
		object_get(src->u.oval);
	dst->u = src->u;
	dst->id = src->id;
}

