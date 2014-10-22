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

void value_clear(struct acs_value *val);
void value_copy(struct acs_value *dst, const struct acs_value *src);
int value_cmp(struct acs_value *a, struct acs_value *b);

bool value_to_bool(const struct acs_value *val);
float value_to_num(const struct acs_value *val);
char *value_to_cstr(const struct acs_value *val);
struct str *value_to_str(const struct acs_value *val);

void value_convert_bool(struct acs_value *val);
void value_convert_num(struct acs_value *val);

#endif
