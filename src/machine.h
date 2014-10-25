#ifndef MACHINE_H
#define MACHINE_H __FILE__

enum opcode {
	OP_NOP = 0,
	OP_PUSHC,
	OP_PUSHR,
	OP_PUSHI,
	OP_PUSHN,
	OP_PUSHU,
	OP_POPN,
	OP_POPR,
	OP_POPU,
	OP_BSCALL,
	OP_CALL,
	OP_RET,
	OP_JMP,
	OP_JNZ,
	OP_JZ
};

extern char *opcode_str[];

enum bscall {
	__BS_ARITH1,
	BS_NEG,
	BS_NOT,
	__BS_ARITH1_MAX,

	__BS_ARITH2 = __BS_ARITH1_MAX,
	BS_ADD = __BS_ARITH2,
	BS_SUB,
	BS_MUL,
	BS_DIV,
	BS_MOD,
	__BS_ARITH2_MAX,

	__BS_CMP = __BS_ARITH2_MAX,
	BS_EQ = __BS_CMP,
	BS_LESS,
	BS_GREAT,
	BS_NEQ,
	BS_LEQ,
	BS_GREQ,
	__BS_CMP_MAX,

	BS_TRUE,
	BS_FALSE,
	BS_GET_GLOBAL,
	BS_SET_GLOBAL,
	BS_GET_FIELD,
	BS_SET_FIELD,
	/* TODO: add BS_ARG0, BS_ARG1, ... BS_ARG63 */
	BS_ARGC,
	BS_ARGV,
};

#define ARGBITS 6
#define ARGMASK ((1 << ARGBITS) - 1)
#define STBITS 12
#define STMASK ((1 << STBITS) - 1)
#define ARGMAX (1 << STBITS)
#define PC_OFFSET (1 << (STBITS - 1))

#endif /* MACHINE_H */
