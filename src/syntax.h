#ifndef SYNTAX_H
#define SYNTAX_H

#include "list.h"
#include "lxr.h"

#include <stdbool.h>
#include <stdio.h>

enum acs_inst {
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

struct acs_inst_block {
	enum acs_inst id;
	enum acs_inst **inst;
};

struct acs_function {
	bool exported;
	struct list node;
	char **args;
	struct acs_inst_block *block;
	char name[];
};

struct acs_script *parse_script(FILE *file, char *path);

#endif /* SYNTAX_H */
