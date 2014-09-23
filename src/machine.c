#include "machine.h"
#include "vec.h"
#include "debug.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum acs_type {
	VAL_VOID,
	VAL_NULL,
	__VAL_DYNAMIC,
	VAL_BOOL = __VAL_DYNAMIC,
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

struct acs_var;

struct acs_value {
	enum acs_type id;
	struct acs_value *next;
	union {
		int ival;
		char *sval;
		bool bval;
		struct acs_function *fval;
		struct acs_var *rval;
	} u;
};

struct acs_var {
	struct acs_var *next;
	struct acs_value val;
	char *name;
	int refcnt;
};

struct acs_context {
	/* TODO optimize using hash map */
	struct acs_var *head;
	struct acs_script *script;
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
	if (!val || val->id < __VAL_DYNAMIC)
		return;
	free(val);
}

static struct acs_var *find_id(struct acs_context *ctx, char *name)
{
	for (struct acs_var *var = ctx->head; var; var = var->next)
		if (strcmp(var->name, name) == 0)
			return var;
	return NULL;
}

static bool acs_is_true(struct acs_value *val)
{
	if (!val || val->id == VAL_NULL)
		return false;
	return true;
}

static struct acs_value *eval_expr(struct acs_context *ctx, enum acs_id *id)
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
			return destroy_value(val), NULL;
		return val;
	} else if (*id == ACS_ID) {
		struct acs_literal *l = to_literal(id);
		struct acs_var *var = find_id(ctx, l->payload);
		if (ERR_ON(!var, "undefined identifier %s", l->payload))
			return NULL;
		struct acs_value *val = make_value(VAL_REF);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		val->u.rval = var;
		return val;
	} else {
		ERR("acs_id = %s is not supported", (int)*id);
		return NULL;
	}
}

static struct acs_value *to_rhs(struct acs_value *head)
{
	return NULL;
}

static struct acs_value *eval(struct acs_context *ctx,
	enum acs_id *id, enum acs_flow *flow)
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
			val = eval(ctx, b->inst[i], flow);
			if (*flow != FL_NEXT)
				return val;
			destroy_value(val);
		}
		return &void_value;
	} else if (*id == ACS_IF) {
		struct acs_if *i = to_if(id);
		val = eval_expr(ctx, i->expr);
		bool cond = acs_is_true(val);

		destroy_value(val);
		val = eval(ctx, cond ? i->true_inst : i->false_inst, flow);
		return val;
	} else if (*id == ACS_WHILE) {
		struct acs_while *w = to_while(id);
		for (;;) {
			val = eval_expr(ctx, w->expr);
			bool cond = acs_is_true(val);
			destroy_value(val);
			if (!cond)
				break;

			val = eval(ctx, w->inst, flow);
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
		return eval_expr(ctx, r->expr);
	} else if (*id >= __ACS_EXPR) {
		*flow = FL_NEXT;
		return eval_expr(ctx, id);
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
	/* TODO: head should be arguments */
	struct acs_context ctx = { .head = NULL, .script = s };
	enum acs_flow flow;
	struct acs_value *value = eval(&ctx, &f->block->id, &flow);
	if (ERR_ON(!value, "calling %s failed", fname))
		return -1;

	// - clear stack
	// - push results on stack
	return 0;
}
