#ifndef SYNTAX_H
#define SYNTAX_H

#include "list.h"
#include "lxr.h"

#include <stdbool.h>
#include <stdio.h>

enum acs_id {
	ACS_NOP = 0,
	ACS_BLOCK,
	ACS_EXPR,
	ACS_IF,
	ACS_WHILE,
	ACS_RETURN,
};

struct acs_script {
	struct list functions;
};

struct acs_inst {
	enum acs_id id;
	struct list node;
};

struct acs_inst_block {
	struct acs_inst base;
	struct list inst;
};

struct acs_function {
	bool exported;
	struct list node;
	struct list vars;
	int n_args;
	struct acs_inst_block *block;
	char name[];
};

struct acs_script *parse_script(FILE *file, char *path);

#endif /* SYNTAX_H */
