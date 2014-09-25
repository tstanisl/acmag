#ifndef SYNTAX_H
#define SYNTAX_H

#include "list.h"
#include "lxr.h"

#include <stdbool.h>
#include <stdio.h>

enum acs_id {
	ACS_NOP = 0,
	ACS_BLOCK,
	ACS_IF,
	ACS_WHILE,
	ACS_RETURN,
	ACS_BREAK,
	ACS_CONTINUE,
	__ACS_EXPR,
	ACS_TRUE = __ACS_EXPR,
	ACS_FALSE,
	ACS_NULL,
	ACS_NUM,
	ACS_STR,
	ACS_ID,

	__ACS_ARG1,

	ACS_MINUS = __ACS_ARG1,
	ACS_PLUS,
	ACS_NOT,

	__ACS_ARG2,

	ACS_COMMA = __ACS_ARG2,
	ACS_ASSIGN,

	ACS_OR,
	ACS_AND,
	__ACS_CMP,
	ACS_LESS = __ACS_CMP,
	ACS_GREAT,
	ACS_EQ,
	ACS_NEQ,
	ACS_LEQ,
	ACS_GREQ,
	__ACS_CMP_MAX,
	ACS_CONCAT = __ACS_CMP_MAX,
	__ACS_ARITH,
	ACS_ADD = __ACS_ARITH,
	ACS_SUB,
	ACS_MUL,
	ACS_DIV,
	ACS_MOD,
	__ACS_ARITH_MAX,

	ACS_CALL = __ACS_ARITH_MAX,
	ACS_DEREF,
	ACS_DOT,

	__ACS_MAX
};

struct acs_script {
	struct list functions;
};

struct acs_block {
	enum acs_id id;
	enum acs_id **inst;
};

struct acs_function {
	bool exported;
	struct list node;
	char **args;
	struct acs_block *block;
	char name[];
};

struct acs_literal {
	enum acs_id id;
	char payload[];
};

struct acs_expr {
	enum acs_id id;
	enum acs_id *arg0;
	enum acs_id *arg1;
};

struct acs_return {
	enum acs_id id;
	enum acs_id *expr;
};

struct acs_if {
	enum acs_id id;
	enum acs_id *expr;
	enum acs_id *true_inst;
	enum acs_id *false_inst;
};

struct acs_while {
	enum acs_id id;
	enum acs_id *expr;
	enum acs_id *inst;
};

struct acs_script *parse_script(FILE *file, char *path);
void destroy_script(struct acs_script *script);
void dump_script(struct acs_script *s);

#endif /* SYNTAX_H */
