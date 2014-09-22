#include "machine.h"
#include "vec.h"
#include "debug.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum acs_type {
	VAL_VOID,
	VAL_NULL,
	__VAL_NONCONST,
	VAL_BOOL = __VAL_NONCONST,
	VAL_NUM,
	VAL_STR,
	VAL_MAP,
	VAL_FUNC,
	VAL_REF,
	__VAL_MAX,
};

enum acs_flow {
	FL_NEXT,
	FL_RETURN,
	FL_BREAK,
	FL_CONTINUE,
};

struct acs_value {
	enum acs_type id;
	struct acs_value *next;
	union {
		int ival;
		char *sval;
		bool bval;
	} u;
};

/*struct acs_big_value {
	enum acs_value id;
	enum acs_value *next;
	union {
		int value;
		char data[];
	}
};

struct acs_str {
	enum acs_value id;
	enum acs_value *next;
};

struct acs_return {
	enum acs_value id;
	enum acs_value *value;
};

struct acs_stack {
	enum acs_value *head, *tail; 
};*/

#define to_block(inst) \
	container_of(inst, struct acs_block, id)
#define to_literal(inst) \
	container_of(inst, struct acs_literal, id)
#define to_return(inst) \
	container_of(inst, struct acs_return, id)
#define to_expr(inst) \
	container_of(inst, struct acs_expr, id)
#define to_if(inst) \
	container_of(inst, struct acs_if, id)
#define to_while(inst) \
	container_of(inst, struct acs_while, id)

static struct acs_function *script_find(struct acs_script *s, char *fname)
{
	list_foreach(l, &s->functions) {
		struct acs_function *f = list_entry(l, struct acs_function, node);
		if (strcmp(f->name, fname) == 0)
			return f;
	}
	return NULL;
}

static struct acs_value *make_value(enum acs_type type)
{
	if (ERR_ON(!type >= __VAL_MAX, "invalid type %d", (int)type))
		return NULL;
	
	struct acs_value *val = calloc(1, sizeof *val);
	if (ERR_ON(!val, "calloc() failed"))
		return NULL;

	val->id = type;
	return val;
}

static void destroy_value(struct acs_value *val)
{
	if (!val || val->id < __VAL_NONCONST)
		return;
	free(val);
}

static bool acs_is_true(struct acs_value *val)
{
	if (!val || val->id == VAL_NULL)
		return false;
}

static struct acs_value *eval_expr(enum acs_id *id)
{
	struct acs_value *val;
	if (!id) {
		static struct acs_value void_value = { .id = VAL_VOID };
		return &void_value;
	} else if (*id == ACS_NUM) {
		val = make_value(VAL_NUM);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		val->u.ival = atoi(to_literal(id)->payload);
		return val;
	} else if (*id == ACS_STR) {
		val = make_value(VAL_STR);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		val->u.sval = strdup(to_literal(id)->payload);
		if (ERR_ON(!val->u.sval, "strdup() failed"))
			return destroy_val(val), NULL;
		return val;
	} else if (*id == ACS_ID) {
		
	} else {
		ERR("acs_id = %s is not supported", (int)*id);
		return NULL;
	}
}

static acs_value *to_rhs(struct acs_value *head)
{
	
}

static struct acs_value *eval(enum acs_id *id, enum acs_flow *flow)
{
	static struct acs_value void_value = { .id = VAL_VOID };
	struct acs_value *val;

	if (!id || *id == ACS_NOP) {
		*flow = FL_NEXT;
		return &void_value;
	} else if (*id == ACS_BLOCK) {
		struct acs_block *b = to_block(id);
		*flow = FL_NEXT;
		for (int i = 0; i < vec_size(b->inst); ++i) {
			val = eval(b->inst[i], flow);
			if (*flow != FL_NEXT)
				return val;
			destroy_value(val);
		}
		return &void_value;
	} else if (*id == ACS_IF) {
		struct acs_if *i = to_if(id);
		val = eval_expr(i->expr);
		bool cond = acs_is_true(val);

		destroy_value(val);
		val = eval(cond ? i->true_inst : i->false_inst, flow);
		return val;
	} else if (*id == ACS_WHILE) {
		struct acs_if *w = to_while(id);
		for (;;) {
			val = eval_expr(w->expr);
			bool cond = acs_is_true(val);
			destroy_value(val);
			if (!cond)
				break;

			val = eval(w->inst, flow);
			if (*flow == FL_RETURN)
				return val;

			destroy_value(val);

			if (*flow == FL_CONTINUE)
				continue;
			if (*flow == FL_BREAK)
				break;
		}
		*flow = FL_NEXT;
		return &void_value;
	} else if (*id == ACS_BREAK) {
		*flow = FL_BREAK;
		return &void_value;
	} else if (*id == ACS_CONTINUE) {
		*flow = FL_CONTINUE;
		return &void_value;
	} else if (*id == ACS_RETURN) {
		struct acs_return *r = to_return(id);
		*flow = FL_RETURN;
		return eval_expr(r->expr);
	} else if (*id >= __ACS_EXPR) {
		*flow = FL_NEXT;
		return eval_expr(id);
	} else {
		ERR("id=%d is not supported yet", (int)*id);
	}

	return NULL;
}

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st)
{
	// - find function object
	struct acs_function *f = script_find(s, fname);
	if (ERR_ON(!f, "failed to find function %s", fname))
		return -1;

	// - copy stack to hash array by adding names
	// - execute function block 
	enum acs_flow flow;
	enum acs_value *value = eval(&f->block->id, &flow);
	if (ERR_ON(!value, "calling %s failed", fname))
		return -1;

	// - clear stack
	// - push results on stack
	return 0;
}
