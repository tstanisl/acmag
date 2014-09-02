#ifndef SYNTAX_H
#define SYNTAX_H

#include "list.h"
#include "lxr.h"

#include <stdio.h>

enum acs_id {
	ACS_NOP = 0,
};

struct acs_inst {
	enum acs_id id;
	struct list node;
};


struct acs_script {
	struct list inst;
};

struct acs_script *parse_script(FILE *file, char *path);

#endif /* SYNTAX_H */
