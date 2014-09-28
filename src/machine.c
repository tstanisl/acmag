#include "machine.h"
#include "vec.h"
#include "debug.h"
#include "cstr.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct acs_var {
	/* FIXME: consider using val.next instead */
	struct acs_var *next;
	struct acs_value val;
	char name[];
};

/*
 * kocham Tomeczka STanislawskiego bARDZO,
 * TO MOJ MAZ, BARDZO GO KOCHAM,
 * I CHCIalabym by mi dal buzi,
 */

struct acs_context {
	struct acs_varmap local;
	struct acs_script *script;
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

#define value_to_var(value) \
	container_of(value, struct acs_var, val)

int varmap_init(struct acs_varmap *vmap)
{
	vmap->head = NULL;
	return 0;
}

static void clear_value(struct acs_value *val);

void varmap_deinit(struct acs_varmap *vmap)
{
	for (struct acs_var *var = vmap->head, *next; var; var = next) {
		next = var->next;
		clear_value(&var->val);
		free(var);
	}
}

struct acs_value *varmap_find(struct acs_varmap *vmap, char *name)
{
	for (struct acs_var *var = vmap->head; var; var = var->next)
		if (strcmp(var->name, name) == 0)
			return &var->val;
	return NULL;
}

struct acs_value *varmap_insert(struct acs_varmap *vmap, char *name)
{
	struct acs_value *val = varmap_find(vmap, name);
	if (val)
		return val;

	struct acs_var *var = calloc(1, sizeof (*var) + strlen(name) + 1);
	if (ERR_ON(!var, "malloc() failed"))
		return NULL;

	strcpy(var->name, name);
	memset(&var->val, 0, sizeof var->val);
	var->val.id = VAL_NULL;

	var->next = vmap->head;
	vmap->head = var;

	return &var->val;
}

int varmap_delete(struct acs_varmap *vmap, char *name)
{
	return -1;
}

int object_init(struct acs_object *obj, acs_object_dtor_cb dtor)
{
	int ret = varmap_init(&obj->fields);
	if (ERR_ON(ret, "varmap_init() failed"))
		return -1;
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
	varmap_deinit(&obj->fields);
	if (obj->dtor)
		obj->dtor(obj);
}

static struct acs_varmap global_vars;

static struct acs_value *var_find(struct acs_context *ctx, char *name)
{
	struct acs_value *val = varmap_find(&ctx->local, name);
	if (val)
		return val;
	return varmap_find(&global_vars, name);
}

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
		else if (val->id == VAL_OBJ)
			printf("object:%p", (void*)val->u.oval);
		else
			CRIT("unknown acs_value type %d", (int)val->id);

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
		if (val->id == VAL_OBJ && val->u.oval)
			object_put(val->u.oval);
		free(val);
		val = next;
	}
}

static void clear_value(struct acs_value *val)
{
	if (val->id == VAL_STR && val->u.sval)
		str_put(val->u.sval);
	if (val->id == VAL_OBJ && val->u.oval)
		object_put(val->u.oval);
	memset(&val->u, 0, sizeof val->u);
}

static void copy_value(struct acs_value *val,
	struct acs_value *old)
{
	if (old->id == VAL_STR)
		str_get(old->u.sval);
	if (old->id == VAL_OBJ)
		object_get(old->u.oval);
	val->u = old->u;
	val->id = old->id;
}

static bool value_to_bool(struct acs_value *val)
{
	if (!val)
		return false;
	if (val->id == VAL_NULL)
		return false;
	if (val->id == VAL_BOOL)
		return val->u.bval;
	return true;
}

static struct acs_value *make_bool_value(bool bval)
{
	struct acs_value *val = make_value(VAL_BOOL);
	if (ERR_ON(!val, "make_value() failed"))
		return NULL;
	val->u.bval = bval;
	return val;
}

static struct acs_value *eval_expr(struct acs_context *ctx, enum acs_id *id);
static struct acs_value *eval(struct acs_context *ctx,
	enum acs_id *id, enum acs_flow *flow);

static struct acs_value *do_assign(struct acs_context *ctx,
	enum acs_id *id, struct acs_value *rhs)
{
	if (!rhs) {
		rhs = make_value(VAL_NULL);
		if (ERR_ON(!rhs, "make_value() failed"))
			return NULL;
	}

	struct acs_expr *e = to_expr(id);
	if (*id == ACS_COMMA) {
		/* FIXME: what about rhs = NULL? */
		struct acs_value *head = rhs, *tail = rhs->next;
		head->next = NULL;

		head = do_assign(ctx, e->arg0, head);
		if (ERR_ON(!head, "do_assign() failed"))
			return destroy_value(tail), NULL;

		CRIT_ON(!e->arg1, "list with no right child");
		tail = do_assign(ctx, e->arg1, tail);
		if (ERR_ON(!tail, "do_assign() failed"))
			return destroy_value(head), NULL;

		head->next = tail;

		return head;
	}

	if (*id == ACS_ID) {
		struct acs_literal *l = to_literal(id);
		struct acs_value *vval = varmap_insert(&ctx->local, l->payload);
		if (ERR_ON(!vval, "varmap_insert() failed"))
			return NULL;
		copy_value(vval, rhs);
		destroy_value(rhs->next);
		rhs->next = NULL;
		return rhs;
	}

	ERR("unexpected acs_id = %d\n", (int)*id);
	destroy_value(rhs);
	return NULL;
}

static struct acs_value *eval_assign(struct acs_context *ctx, enum acs_id *id)
{
	struct acs_expr *e = to_expr(id);

	struct acs_value *rhs = eval_expr(ctx, e->arg1);
	if (ERR_ON(!rhs, "eval_expr() for RHS failed"))
		return NULL;

	rhs = do_assign(ctx, e->arg0, rhs);
	if (ERR_ON(!rhs, "do_assign() failed"))
		return NULL;

	return rhs;
}

static struct acs_value *eval_bool(struct acs_context *ctx, enum acs_id *id, bool is_or)
{
	struct acs_expr *e = to_expr(id);

	struct acs_value *lhs = eval_expr(ctx, e->arg0);
	if (ERR_ON(!lhs, "eval_expr() for LHS failed"))
		return NULL;

	bool cond = value_to_bool(lhs);
	destroy_value(lhs);

	if (cond == is_or)
		return make_bool_value(is_or);

	struct acs_value *rhs = eval_expr(ctx, e->arg1);
	if (ERR_ON(!rhs, "eval_expr() for RHS failed"))
		return destroy_value(lhs), NULL;

	cond = value_to_bool(rhs);
	destroy_value(rhs);
	return make_bool_value(cond);
}

static int do_cmp(struct acs_value *a, struct acs_value *b)
{
	if (a->id == VAL_NULL)
		return 0;
	if (a->id == VAL_BOOL)
		return !!a->u.bval - !!b->u.bval;
	if (a->id == VAL_NUM)
		return a->u.ival - b->u.bval;
	if (a->id == VAL_STR)
		return strcmp(a->u.sval->str, b->u.sval->str);
	if (a->id == VAL_OBJ)
		return a->u.oval != b->u.oval;
	return 0;
}

static struct acs_value *eval_cmp(enum acs_id *id,
	struct acs_value *lhs, struct acs_value *rhs)
{
	if (lhs->id != rhs->id) {
		WARN("comparing not compatible types");
		destroy_value(lhs);
		destroy_value(rhs);
		return make_bool_value(false);
	}
	int cmp = do_cmp(lhs, rhs);
	cmp = cmp > 0 ? 1 : cmp;
	cmp = cmp < -1 ? -1 : cmp;

	destroy_value(lhs);
	destroy_value(rhs);

	static bool result[][3] = {
		[ACS_LESS - __ACS_CMP] = {true, false, false},
		[ACS_GREAT - __ACS_CMP] = {false, false, true},
		[ACS_EQ - __ACS_CMP] = {false, true, false},
		[ACS_NEQ - __ACS_CMP] = {true, false, true},
		[ACS_LEQ - __ACS_CMP] = {true, true, false},
		[ACS_GREQ - __ACS_CMP] = {false, true, true},
	};

	return make_bool_value(result[*id - __ACS_CMP][cmp + 1]);
}

static struct acs_value *eval_call(enum acs_id *id,
	struct acs_value *lhs, struct acs_value *rhs)
{
	if (lhs->id == VAL_USER) {
		struct acs_user_function *ufunc = lhs->u.uval;
		struct acs_value *ret = ufunc->call(ufunc, rhs);
		destroy_value(lhs);
		destroy_value(rhs);
		return ret;
	}

	if (lhs->id != VAL_FUNC) {
		ERR("non-function used in call statement");
		destroy_value(lhs);
		destroy_value(rhs);
		return NULL;
	}
	struct acs_function *f = lhs->u.fval;
	struct acs_context ctx = { .script = f->script };
	varmap_init(&ctx.local);

	struct acs_value *value = NULL;
	struct acs_value *n = rhs;
	for (int i = 0; i < vec_size(f->args); ++i) {
		struct acs_value *val = varmap_insert(&ctx.local, f->args[i]);
		if (ERR_ON(!val, "varmap_insert() failed"))
			goto done;
		if (n) {
			copy_value(val, n);
			n = n->next;
		} // val is VAL_NULL by default
	}

	/*printf("calling %s with args:", f->name);
	for (struct acs_var *v = ctx.vars; v; v = v->next)
		printf(" %s", v->name);
	puts("");*/

	enum acs_flow flow;
	value = eval(&ctx, &f->block->id, &flow);
	if (ERR_ON(!value, "calling %s failed", f->name))
		goto done;

	/*printf("return = ");
	dump_value(value);
	puts("");*/
	/* deref value to prevent to fix invalid adress after return a; */
	/*printf("return = ");
	dump_value(value);
	puts("");*/

done:
	varmap_deinit(&ctx.local);
	destroy_value(lhs);
	destroy_value(rhs);
	return value;
}

static struct acs_value *eval_arith(enum acs_id *id,
	struct acs_value *lhs, struct acs_value *rhs)
{
	struct acs_value *val = NULL;
	if (ERR_ON(lhs->id != VAL_NUM, "left arg is not int"))
		goto done;
	if (ERR_ON(rhs->id != VAL_NUM, "right arg is not int"))
		goto done;

	int ival = 0;
	if (*id == ACS_ADD)
		ival = lhs->u.ival + rhs->u.ival;
	else if (*id == ACS_SUB)
		ival = lhs->u.ival - rhs->u.ival;
	else if (*id == ACS_MUL)
		ival = lhs->u.ival * rhs->u.ival;
	else if (*id == ACS_DIV)
		ival = lhs->u.ival / rhs->u.ival;
	else if (*id == ACS_MOD)
		ival = lhs->u.ival % rhs->u.ival;
	else
		CRIT("unexpected acs_id = %d\n", (int)*id);

	val = make_value(VAL_NUM);
	if (ERR_ON(!val, "make_value() failed"))
		goto done;

	val->u.ival = ival;
done:
	destroy_value(lhs);
	destroy_value(rhs);
	return val;
}

static struct acs_value *eval_arg2_expr(struct acs_context *ctx, enum acs_id *id)
{
	struct acs_expr *e = to_expr(id);

	if (*id == ACS_ASSIGN)
		return eval_assign(ctx, id);
	if (*id == ACS_OR)
		return eval_bool(ctx, id, true);
	if (*id == ACS_AND)
		return eval_bool(ctx, id, false);

	struct acs_value *lhs = eval_expr(ctx, e->arg0);
	if (ERR_ON(!lhs, "eval_expr() for LHS failed"))
		return NULL;

	struct acs_value *rhs = eval_expr(ctx, e->arg1);
	if (ERR_ON(!rhs, "eval_expr() for RHS failed"))
		return destroy_value(lhs), NULL;

	destroy_value(lhs->next);
	if (*id == ACS_COMMA) {
		lhs->next = rhs;
		return lhs;
	}

	/* TODO: insert f-call here */
	if (*id == ACS_CALL)
		return eval_call(id, lhs, rhs);

	/* should not happen */
	WARN_ON(!!rhs->next, "RHS has tail!!!");
	/* TODO: drop all tail values, to be removed */
	destroy_value(rhs->next);
	rhs->next = NULL;

	if (*id >= __ACS_CMP && *id < __ACS_CMP_MAX)
		return eval_cmp(id, lhs, rhs);

	if (*id >= __ACS_ARITH && *id < __ACS_ARITH_MAX)
		return eval_arith(id, lhs, rhs);

	destroy_value(lhs);
	destroy_value(rhs);

	ERR("acs_id = %d is not supported by evaluator", id ? (int)*id : -1);

	return NULL;
}

static struct acs_value *eval_expr(struct acs_context *ctx, enum acs_id *id)
{
	struct acs_value *val;
	//printf("processing id=%d\n", id ? *id : -1);
	if (!id || *id == ACS_NULL || *id == ACS_NOP) {
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
		struct acs_value *rval = var_find(ctx, l->payload);
		if (rval) {
			struct acs_value *val = make_value(VAL_NULL);
			if (ERR_ON(!val, "make_value() failed"))
				return NULL;
			copy_value(val, rval);
			return val;
		}

		struct acs_function *func = script_find(ctx->script, l->payload);
		if (func) {
			struct acs_value *val = make_value(VAL_FUNC);
			if (ERR_ON(!val, "make_value() failed"))
				return NULL;
			val->u.fval = func;
			return val;
		}

		ERR("undefined identifier %s", l->payload);
		return NULL;
	} else if (*id >= __ACS_ARG2) {
		return eval_arg2_expr(ctx, id);
	} else {
		ERR("acs_id = %d is not supported", id ? (int)*id : -1);
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
			//dump_value(val); puts("");
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

static struct acs_value *call_print(struct acs_user_function *ufunc,
	struct acs_value *args)
{
	for (struct acs_value *val = args; val; val = val->next)
		if (val->id == VAL_STR)
			printf("%s", val->u.sval->str);
		else if (val->id == VAL_NUM)
			printf("%d", val->u.ival);
		else if (val->id == VAL_BOOL)
			printf("%s", val->u.bval ? "true" : "false");
		else
			printf("(invalid)");
	return make_value(VAL_NULL);
}

static struct acs_value *call_obj(struct acs_user_function *ufunc,
	struct acs_value *args)
{
	struct acs_object *obj = calloc(1, sizeof *obj);
	if (ERR_ON(!obj, "malloc() failed"))
		return NULL;
	object_init(obj, (acs_object_dtor_cb)free);

	struct acs_value *ret = make_value(VAL_OBJ);
	if (ERR_ON(!obj, "malloc() failed"))
		return object_put(obj), NULL;

	ret->u.oval = obj;
	return ret;
}

static void machine_deinit(void)
{
	varmap_deinit(&global_vars);
}

static void machine_init(void)
{
	static bool initialized = false;
	if (initialized)
		return;
	varmap_init(&global_vars);
	initialized = true;

	atexit(machine_deinit);

	static struct acs_user_function print = { .call = call_print };
	register_user_function(&print, "print");
	static struct acs_user_function obj = { .call = call_obj };
	register_user_function(&obj, "obj");
}

int machine_call(struct acs_script *s, char *fname, struct acs_stack *st)
{
	machine_init();
	// - find function object
	struct acs_function *f = script_find(s, fname);
	if (ERR_ON(!f, "failed to find function %s", fname))
		return -1;

	// - copy stack to hash array by adding names
	// - execute function block 
	/* TODO: head should be arguments */
	struct acs_context ctx = { .script = s };
	varmap_init(&ctx.local);

	enum acs_flow flow;
	struct acs_value *value = eval(&ctx, &f->block->id, &flow);
	if (ERR_ON(!value, "calling %s failed", fname))
		return -1;

	dump_value(value);
	puts("");
	destroy_value(value);

	varmap_deinit(&ctx.local);

	// - clear stack
	// - push results on stack
	return 0;
}

int register_user_function(struct acs_user_function *ufunc, char *name)
{
	struct acs_value *val = varmap_insert(&global_vars, name);
	if (ERR_ON(!val, "varmap_insert() failed"))
		return -1;
	clear_value(val);
	val->id = VAL_USER;
	val->u.uval = ufunc;
	return 0;
}
