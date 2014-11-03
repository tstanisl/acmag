#include "cstr.h"
#include "debug.h"
#include "function.h"
#include "lxr.h"
#include "list.h"
#include "machine.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

struct scope {
	struct scope *prev;
	struct list vars;
	struct list inst;
};

struct compiler {
	char *path;
	struct lxr *lxr;
	enum token next;
	struct list consts;
	struct scope *scope;
	struct list vars;
	int n_index;
};

struct inst {
	struct list node;
	enum opcode op;
	int arg;
};

enum var_type {
	VAR_ARG,
	VAR_LOCAL,
	VAR_UPVALUE,
	VAR_GLOBAL,
	/* special */
};

struct var {
	struct scope *scope;
	enum var_type type;
	struct var *prev;
	struct list scope_node;
	struct list map_node;
	int index;
	struct str *name;
};

struct constant {
	struct acs_value val;
	struct list node;
};

enum entry_type {
	ET_ARG, // n/a
	ET_LOCAL, // n/a
	ET_UPVALUE, // n/a
	ET_GLOBAL, // name on stack
	ET_FIELD, // object + field on stack
	ET_STACK, // n/a
};

struct entry {
	struct entry *prev;
	enum entry_type type;
	int arg;
};

static int perr(struct compiler *c, char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	printf("%s:%d: error: ", c->path, lxr_line(c->lxr));
	vprintf(fmt, va);
	puts("");
	va_end(va);

	return -1;
}

static void consume(struct compiler *c)
{
	c->next = lxr_get(c->lxr);
	printf("next = %s\n", token_str[c->next]);
	if (c->next == TOK_ERR)
		perr(c, "%s", lxr_buffer(c->lxr));
}

static void emit(struct compiler *c, enum opcode op, int arg)
{
	struct inst *inst = ac_alloc(sizeof *inst);
	inst->op = op;
	inst->arg = arg;
	list_add_tail(&inst->node, &c->scope->inst);
}

static int new_const_num(struct compiler *c, float nval)
{
	int idx = 0;
	list_foreach(l, &c->consts) {
		struct constant *cval = list_entry(l, struct constant, node);
		if (cval->val.id == VAL_NUM && cval->val.u.nval == nval)
			return idx;
		++idx;
	}
	struct constant *cval = ac_alloc(sizeof *cval);
	cval->val.id = VAL_NUM;
	cval->val.u.nval = nval;
	list_add_tail(&cval->node, &c->consts);
	return idx;
}

static int new_const_cstr(struct compiler *c, char *str)
{
	int idx = 0;
	list_foreach(l, &c->consts) {
		struct constant *cval = list_entry(l, struct constant, node);
		if (cval->val.id == VAL_STR &&
		    strcmp(cval->val.u.sval->str, str) == 0)
			return idx;
		++idx;
	}
	struct constant *cval = ac_alloc(sizeof *cval);
	cval->val.id = VAL_STR;
	cval->val.u.sval = str_create(str);
	list_add_tail(&cval->node, &c->consts);
	return idx;
}

static struct entry *entry_create(enum entry_type type, int arg, struct entry *prev)
{
	struct entry *entry = ac_alloc(sizeof *entry);
	entry->type = type;
	entry->arg = arg;
	entry->prev = prev;
	return entry;
}

static void entry_destroy(struct entry *e)
{
	while (e) {
		struct entry *to_free = e;
		e = e->prev;
		free(to_free);
	}
}

static void start_scope(struct compiler *c)
{
	struct scope *s = ac_alloc(sizeof *s);
	s->prev = c->scope;
	list_init(&s->vars);
	list_init(&s->inst);
	c->scope = s;
}

static struct var *find_var(struct compiler *c, char *name)
{
	list_foreach(l, &c->vars) {
		struct var *v = list_entry(l, struct var, map_node);
		if (strcmp(v->name->str, name) == 0) {
			if (v->scope != c->scope && v->type == VAR_LOCAL)
				v->type = VAR_UPVALUE;
			return v;
		}
	}
	struct var *v = ac_alloc(sizeof *v);
	v->name = str_create(name);
	v->type = VAR_LOCAL;
	v->scope = c->scope;
	v->index = c->n_index;
	++c->n_index;
	list_add(&v->scope_node, &c->scope->vars);
	list_add(&v->map_node, &c->vars);
	return v;
}

static int compile_inst(struct compiler *c);

static int compile_block(struct compiler *c)
{
	CRIT_ON(c->next != TOK_LBRA, "unexpected token %s", token_str[c->next]);
	consume(c);
	while (c->next != TOK_RBRA) {
		int ret = compile_inst(c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			return -1;
	}
	consume(c);
	return 0;
}

static enum entry_type var_to_entry(enum var_type vt)
{
	switch (vt) {
	case VAR_ARG: return ET_ARG;
	case VAR_LOCAL: return ET_LOCAL;
	case VAR_UPVALUE: return ET_UPVALUE;
	case VAR_GLOBAL: return ET_GLOBAL;
	}
	CRIT("invalid var_type");
}

static struct entry *compile_top(struct compiler *c, bool need_value, struct entry *prev)
{
	if (!need_value) {
		// TODO: add warning here
		/* FIXME: possible undetected syntax errors */
		if (c->next != TOK_ID)
			return prev;
		char *name = lxr_buffer(c->lxr);
		struct var *v = find_var(c, lxr_buffer(c->lxr));
		if (!v)
			return perr(c, "undefined identifier %s", name), NULL;
		consume(c);
		return entry_create(var_to_entry(v->type), v->index, prev);
	}

	if (c->next == TOK_NULL) {
		emit(c, OP_PUSHN, 1);
	} else if (c->next == TOK_TRUE) {
		emit(c, OP_BSCALL, BS_TRUE);
	} else if (c->next == TOK_FALSE) {
		emit(c, OP_BSCALL, BS_FALSE);
	} else if (c->next == TOK_STR) {
		int idx = new_const_cstr(c, lxr_buffer(c->lxr));
		emit(c, OP_PUSHC, idx);
	} else if (c->next == TOK_NUM) {
		float nval = atof(lxr_buffer(c->lxr));
		int ival = (int)nval;
		if (nval == ival && ival >= 0 && ival < ARGMAX) {
			emit(c, OP_PUSHI, ival);
		} else {
			int idx = new_const_num(c, nval);
			emit(c, OP_PUSHC, idx);
		}
	} else if (c->next == TOK_ID) {
		char *name = lxr_buffer(c->lxr);
		struct var *v = find_var(c, name);
		if (!v)
			return perr(c, "undefined identifier %s", name), NULL;
		emit(c, OP_PUSHR, v->index);
		consume(c);
		return entry_create(var_to_entry(v->type), v->index, prev);
	} else {
		return perr(c, "unexpected token %s", token_str[c->next]), NULL;
	}
	consume(c);
	return entry_create(ET_STACK, 0, prev);
}

static struct entry *compile_list(struct compiler *c, bool need_value)
{
	struct entry *e = compile_top(c, need_value, NULL);
	while (c->next == TOK_SEP) {
		consume(c);
		struct entry *next = compile_top(c, need_value, e);
		if (!next) {
			entry_destroy(e);
			return NULL;
		}
		e = next;
	}
	return e;
}

static struct entry *compile_expr(struct compiler *c)
{
	return compile_list(c, true);
}

static int compile_inst(struct compiler *c)
{
	if (c->next == TOK_LBRA)
		return compile_block(c);
	struct entry *entry = compile_expr(c);
	entry_destroy(entry);
	if (ERR_ON(!entry, "compile_expr() failed"))
		return -1;
	if (c->next != TOK_SCOLON)
		return perr(c, "missing ; after expression");
	consume(c);
	return 0;
}

static void dump_code(struct compiler *c)
{
	int pc = 0;
	list_foreach(l, &c->scope->inst) {
		struct inst *inst = list_entry(l, struct inst, node);
		printf("%02d: %s %d\n", pc, opcode_str[inst->op], inst->arg);
		++pc;
	}
}

struct acs_finstance *acs_compile_file(FILE *file, char *path)
{
	struct compiler c = { .path = path };
	start_scope(&c);
	list_init(&c.consts);
	list_init(&c.vars);

	c.lxr = lxr_create(file, 256);
	if (ERR_ON(!c.lxr, "lxr_create() failed"))
		goto fail;

	consume(&c);
	while (c.next != TOK_EOF) {
		int ret = compile_inst(&c);
		if (ERR_ON(ret < 0, "compile_inst() failed"))
			goto fail_lxr;
	}

	dump_code(&c);

	/* TODO: solve names, generate closure code etc */
	static struct acs_finstance phony;
	memset(&phony, 0, sizeof phony);
	return &phony;

fail_lxr:
	lxr_destroy(c.lxr);
fail:
	return NULL;
}
