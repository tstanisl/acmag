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
	ACS_EXPR,
	ACS_TRUE = ACS_EXPR,
	ACS_FALSE,
	ACS_NULL,
	ACS_NUM,
	ACS_STR,
	ACS_ID,
	__ACS_ARG1,
	__ACS_ARG2,
	ACS_LIST,
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

struct acs_script *parse_script(FILE *file, char *path);
void destroy_script(struct acs_script *script);
void dump_script(struct acs_script *s);

#endif /* SYNTAX_H */
