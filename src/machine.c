#include "machine.h"
#include "vec.h"
#include "debug.h"
#include "cstr.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum acs_type {
	VAL_NULL = 0,
	VAL_BOOL,
	VAL_NUM,
	VAL_STR,
	VAL_VAR,
	VAL_OBJ,
	VAL_FUNC,
	__VAL_MAX,
};

struct acs_var;
struct acs_object;

struct acs_value {
	enum acs_type id;
	struct acs_value *next; // used only by assignment
	union {
		int ival;
		struct str *sval;
		bool bval;
		struct acs_function *fval;
		struct acs_var *vval;
		struct acs_object *oval;
	} u;
};

struct acs_var {
	/* FIXME: consider using val.next instead */
	struct acs_var *next;
	struct acs_value val;
	char name[];
};

struct acs_object {
	struct acs_var *subs;
	int refcnt;
};

struct acs_context {
	/* TODO optimize using hash map */
	struct acs_var *vars;
	struct acs_script *script;
	bool lhs;
};

enum acs_flow {
	FL_NEXT,
	FL_RETURN,
	FL_BREAK,
	FL_CONTINUE,
};

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

static void free_vars(struct acs_var *head)
{
	for (struct acs_var *var = head, *next; var; var = next)
		next = var->next, free(var);
}

static struct acs_var *find_var(struct acs_var *head, char *name)
{
	for (struct acs_var *var = head; var; var = var->next)
		if (strcmp(var->name, name) == 0)
			return var;
	return NULL;
}

static struct acs_var *create_var(struct acs_var **head, char *name)
{
	struct acs_var *var = malloc(sizeof (*var) + strlen(name) + 1);
	if (ERR_ON(!var, "malloc() failed"))
		return NULL;

	strcpy(var->name, name);
	memset(&var->val, 0, sizeof var->val);
	var->val.id = VAL_NULL;

	var->next = *head;
	*head = var;

	return var;
}

static struct acs_value *make_value(enum acs_type type)
{
	/* TODO: optimize unisg memory pool and freelist */
	if (ERR_ON(!type >= __VAL_MAX, "invalid type %d", (int)type))
		return NULL;
	
	struct acs_value *val = calloc(1, sizeof *val);
	if (ERR_ON(!val, "calloc() failed"))
		return NULL;

	val->id = type;
	return val;
}

static void dump_value(struct acs_value *val)
{
	bool first = true;
	while (val) {
		if (!first)
			printf(", ");

		if (val->id == VAL_NULL)
			printf("null");
		else if (val->id == VAL_STR)
			printf("str:%s", val->u.sval->str);
		else if (val->id == VAL_NUM)
			printf("int:%d", val->u.ival);
		else if (val->id == VAL_BOOL)
			printf("bool:%s", val->u.bval ? "true" : "false");
		else if (val->id == VAL_VAR)
			printf("ref:%s", val->u.vval->name);

		first = false;
		val = val->next;
	}
}
static void destroy_value(struct acs_value *val)
{
	while (val) {
		struct acs_value *next = val->next;
		if (val->id == VAL_STR && val->u.sval)
			str_put(val->u.sval);
		free(val);
		val = next;
	}
}

static void clear_value(struct acs_value *val)
{
	if (val->id == VAL_STR && val->u.sval)
		str_put(val->u.sval);
	memset(&val->u, 0, sizeof val->u);
}

static void copy_value(struct acs_value *val,
	struct acs_value *old)
{
	if (old->id == VAL_STR)
		str_get(old->u.sval);
	val->u = old->u;
	val->id = old->id;
}

static void deref_value(struct acs_value *val)
{
	if (val->id != VAL_VAR)
		return;
	// FIXME: add var_put()
	copy_value(val, &val->u.vval->val);
}

/*static struct acs_value *duplicate_value(struct acs_value *old)
{
	struct acs_value *val = make_value(old->id);
	if (ERR_ON(!val, "make_value() failed"))
		return NULL;

	copy_value(val, old);

	return val;
}*/

static bool value_to_bool(struct acs_value *val)
{
	if (!val)
		return false;
	if (val->id == VAL_NULL)
		return false;
	if (val->id == VAL_BOOL)
		return val->u.bval;
	if (val->id == VAL_VAR) /* FIXME: a[0] = a; if (a[0]) ... will boom !!! */
		return value_to_bool(&val->u.vval->val);
	return true;
}

static struct acs_value *eval_assign(struct acs_context *ctx,
	struct acs_value *lhs_head, struct acs_value *rhs_head)
{
	struct acs_value *lhs, *rhs;

	for (rhs = rhs_head; rhs; rhs = rhs->next) 
		deref_value(rhs);
	printf("deref(rhs) = " );
	dump_value(rhs_head);
	printf("\n");

	for (rhs = rhs_head, lhs = lhs_head; lhs; lhs = lhs->next) {
		// FIXME: should be detected on compiler stage
		if (lhs->id != VAL_VAR) {
			ERR("invalid L-value");
			destroy_value(lhs_head);
			destroy_value(rhs_head);
			return NULL;
		}

		struct acs_value *lhs_val = &lhs->u.vval->val;
		if (!rhs) {
			clear_value(lhs_val);
			continue;
		}

		clear_value(lhs_val);
		copy_value(lhs_val, rhs);

		rhs = rhs->next;
	}

	/* right expression is no longer needed */
	destroy_value(rhs_head);

	return lhs_head;
}

static struct acs_value *eval_expr(struct acs_context *ctx, enum acs_id *id);

static struct acs_value *eval_arg2_expr(struct acs_context *ctx, enum acs_id *id)
{
	struct acs_expr *e = to_expr(id);

	bool old_lhs = ctx->lhs;
	if (*id == ACS_ASSIGN)
		ctx->lhs = true;

	struct acs_value *lhs = eval_expr(ctx, e->arg0);
	if (ERR_ON(!lhs, "eval_expr() for LHS failed"))
		return NULL;

	ctx->lhs = false;

	struct acs_value *rhs = eval_expr(ctx, e->arg1);
	if (ERR_ON(!lhs, "eval_expr() for RHS failed"))
		return destroy_value(lhs), NULL;

	if (*id == ACS_ASSIGN) {
		dump_value(lhs);
		printf("  :=  ");
		dump_value(rhs);
		printf("\n");
		ctx->lhs = old_lhs;
		struct acs_value *val = eval_assign(ctx, lhs, rhs);
		printf("result = ");
		dump_value(val);
		printf("\n");
		return val;
	}

	destroy_value(lhs->next);

	if (*id == ACS_COMMA) {
		lhs->next = rhs;
		return lhs;
	}

	ERR("acs_id = %d is not supported by evaluator", id ? (int)*id : -1);

	return NULL;
}

static struct acs_value *eval_expr(struct acs_context *ctx, enum acs_id *id)
{
	struct acs_value *val;
	//printf("processing id=%d\n", id ? *id : -1);
	if (!id || *id == ACS_NULL) {
		val = make_value(VAL_NULL);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		return val;
	} else if (*id == ACS_TRUE || *id == ACS_FALSE) {
		val = make_value(VAL_BOOL);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		val->u.bval = (*id == ACS_TRUE);
		return val;
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
		val->u.sval = str_create(to_literal(id)->payload);
		if (ERR_ON(!val->u.sval, "str_create() failed"))
			return destroy_value(val), NULL;
		return val;
	} else if (*id == ACS_ID) {
		struct acs_literal *l = to_literal(id);
		struct acs_var *var = find_var(ctx->vars, l->payload);
		if (ctx->lhs) {
			if (!var)
				var = create_var(&ctx->vars, l->payload);
			if (ERR_ON(!var, "create_var() failed"))
				return NULL;
		} else {
			if (ERR_ON(!var, "undefined identifier %s", l->payload))
				return NULL;
		}
		struct acs_value *val = make_value(VAL_VAR);
		if (ERR_ON(!val, "make_value() failed"))
			return NULL;
		val->u.vval = var;
		return val;
	} else if (*id >= __ACS_ARG2) {
		return eval_arg2_expr(ctx, id);
	} else {
		ERR("acs_id = %s is not supported", id ? (int)*id : -1);
		return NULL;
	}
}

static struct acs_value *eval(struct acs_context *ctx,
	enum acs_id *id, enum acs_flow *flow)
{
	struct acs_value *val;

	if (!id || *id == ACS_NOP) {
		*flow = FL_NEXT;
		return make_value(VAL_NULL);
	} else if (*id == ACS_BLOCK) {
		struct acs_block *b = to_block(id);
		*flow = FL_NEXT;
		for (int i = 0; i < vec_size(b->inst); ++i) {
			val = eval(ctx, b->inst[i], flow);
			if (*flow != FL_NEXT)
				return val;
			dump_value(val); puts("");
			destroy_value(val);
		}
		return make_value(VAL_NULL);
	} else if (*id == ACS_IF) {
		struct acs_if *i = to_if(id);
		val = eval_expr(ctx, i->expr);
		bool cond = value_to_bool(val);

		destroy_value(val);
		val = eval(ctx, cond ? i->true_inst : i->false_inst, flow);
		if (*flow == FL_RETURN)
			return val;
		destroy_value(val);
		return make_value(VAL_NULL);
	} else if (*id == ACS_WHILE) {
		struct acs_while *w = to_while(id);
		for (;;) {
			val = eval_expr(ctx, w->expr);
			bool cond = value_to_bool(val);
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
		return make_value(VAL_NULL);
	} else if (*id == ACS_BREAK) {
		*flow = FL_BREAK;
		return make_value(VAL_NULL);
	} else if (*id == ACS_CONTINUE) {
		*flow = FL_CONTINUE;
		return make_value(VAL_NULL);
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
	struct acs_context ctx = { .vars = NULL, .script = s };
	enum acs_flow flow;
	struct acs_value *value = eval(&ctx, &f->block->id, &flow);
	if (ERR_ON(!value, "calling %s failed", fname))
		return -1;

	for (struct acs_value *v = value; v; v = v->next)
		deref_value(v);

	dump_value(value);
	puts("");
	destroy_value(value);

	free_vars(ctx.vars);

	// - clear stack
	// - push results on stack
	return 0;
}
