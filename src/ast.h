#ifndef AST_H
#define AST_H __FILE__

#include "lxr.h"

struct ast {
	enum token id;
	union {
		struct str *sval;
		float nval;
		struct ast *arg[2];
	} u;
};

struct ast *ast_from_file(FILE *file, char *path);

#endif /* AST_H */ 
