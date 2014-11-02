#ifndef AST_H
#define AST_H __FILE__

enum ast_id {
	AST_NOP = 0,
	AST_FUNC,
	AST_IF,
	AST_IF_TAIL,
	AST_WHILE,
	AST_RETURN,
	AST_BREAK,
	AST_CONTINUE,
	ACS_SEQ,

	__AST_EXPR,
	AST_TRUE = __AST_EXPR,
	AST_FALSE,
	AST_NULL,
	AST_NUM,
	AST_STR,
	AST_ID,

	__AST_ARG1,

	AST_MINUS = __AST_ARG1,
	AST_PLUS,
	AST_NOT,

	__AST_ARG2,

	AST_COMMA = __AST_ARG2,
	AST_ASSIGN,

	AST_OR,
	AST_AND,
	__AST_CMP,
	AST_LESS = __AST_CMP,
	AST_GREAT,
	AST_EQ,
	AST_NEQ,
	AST_LEQ,
	AST_GREQ,
	__AST_CMP_MAX,
	AST_CONCAT = __AST_CMP_MAX,
	__AST_ARITH,
	AST_ADD = __AST_ARITH,
	AST_SUB,
	AST_MUL,
	AST_DIV,
	AST_MOD,
	__AST_ARITH_MAX,

	AST_CALL = __AST_ARITH_MAX,
	AST_DEREF,

	__AST_MAX
};

struct ast {
	enum ast_id id;
	union {
		struct str *sval;
		float nval;
		struct ast *arg[2];
	} u;
};

#endif /* AST_H */ 
