#ifndef LXR_H
#define LXR_H __FILE__

#include <stdio.h>

enum token {
        TOK_ERR = 0,
        TOK_EOF,
        TOK_INT,
        TOK_STR,
        TOK_ID,

        TOK_LPAR, /* ( */
        TOK_RPAR, /* ) */
        TOK_LBRA, /* { */
        TOK_RBRA, /* } */
        TOK_SEP, /* , */
        TOK_MINUS, /* - */
        TOK_PLUS, /* + */
        TOK_MUL, /* * */
        TOK_DIV, /* / */
        TOK_MOD, /* % */
        TOK_DOT, /* . */
        TOK_CONCAT, /* .. */
        TOK_SCOLON, /* ; */

        TOK_LESS, /* < */
        TOK_GREAT, /* > */
        TOK_NOT, /* ! */
        TOK_ASSIGN, /* = */

        TOK_OR, /* || */
        TOK_AND, /* && */
        TOK_EQ, /* == */
        TOK_NEQ, /* != */
        TOK_LEQ, /* <= */
        TOK_GREQ, /* >= */

	TOK_FUNCTION, /* function */
	TOK_NULL, /* null */
	TOK_TRUE, /* true */
	TOK_FALSE, /* false */
        TOK_RETURN, /* return */
        TOK_IF, /* if */
        TOK_ELSE, /* else */
        TOK_WHILE, /* while */
        TOK_FOR, /* for */
        TOK_BREAK, /* break */
        TOK_CONTINUE, /* continue */
};

struct lxr;
extern char *token_str[];

struct lxr lxr_create(FILE *f, int bufsize);
void lxr_destroy(struct lxr *lxr);

char *lxr_buffer(struct lxr *lxr);
enum token lxr_get(struct lxr *lxr);

#endif /* LXR_H */
