#ifndef LXR_H
#define LXR_H __FILE__

#include <stdio.h>

enum token {
	TOK_ERR = 0,
	TOK_EOF,
	TOK_NUM,
	TOK_STR,
	TOK_ID,
	TOK_TRUE,
	TOK_FALSE,
	TOK_NULL,

	TOK_COLON, /* : */
	TOK_LPAR, /* ( */
	TOK_RPAR, /* ) */
	TOK_LBRA, /* { */
	TOK_RBRA, /* } */
	TOK_LSQR, /* [ */
	TOK_RSQR, /* ] */
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

	TOK_DEF, /* def */
	TOK_RETURN, /* return */
	TOK_IF, /* if */
	TOK_ELSE, /* else */
	TOK_WHILE, /* while */
	TOK_FOR, /* for */
	TOK_BREAK, /* break */
	TOK_CONTINUE, /* continue */
	TOK_THIS,
};

struct lxr;
/**
 * token_str - global array with tokens as strings
 *
 * Array is indexed by token enum so
 * token_str[TOK_BREAK] is equal to "break"
 */
extern char *token_str[];

/**
 * lxr_create() - create lxr object
 * @file - file stream with source code
 * @max_token_size - largest possible token size including 0-byte
 * Return: pointer to lxr object or NULL on failure
 */
struct lxr *lxr_create(FILE *file, int max_token_size);

/**
 * lxr_destroy() - destroy lxr object
 * @lxr - object to be destroyed
 */
void lxr_destroy(struct lxr *lxr);

/**
 * lxr_buffer() - returns a pointer with token string
 * @lxr - pointer to LXR object
 *
 * The pointer is valid until lxr_destroy() is called.
 * Content changes after after lxr_get()
 * Return: pointer to token string
 */
char *lxr_buffer(struct lxr *lxr);

/**
 * lxr_line() - returns a current line in source stream
 * @lxr - pointer to LXR object
 *
 * The pointer is valid until next lxr_get() or
 * lxr_destroy() is called
 * Return: current line starting from 1
 */
int lxr_line(struct lxr *lxr);

/**
 * lxr_get() - extract new token
 * @lxr - pointer to LXR object
 * 
 * Only token enum is returned, it order to see the
 * content of the token please use lxr_buffer()
 * Return: token id, or 0 (aka TOK_ERR) on error
 */
enum token lxr_get(struct lxr *lxr);

#endif /* LXR_H */
