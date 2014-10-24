#include "acs.h"
#include "common.h"
#include "function.h"
#include "machine.h"

#include <stdio.h>

static int do_machine_test(uint16_t *code)
{
	struct acs_value *consts = acs_global("print");
	struct acs_function func = { .consts = consts, .code = code };
	struct acs_finstance *fi = ac_alloc(sizeof (*fi) + sizeof (fi->upvalues[0]));
	fi->u.func = &func;
	fi->n_upvalues = 1;
	struct acs_upvalue upvalue = { .refcnt = 1, .val = consts[0] };
	fi->upvalues[0] = &upvalue;
	struct acs_value fval = { .id = VAL_FUNC, .u.fval = fi };
	acs_call_head(&fval);
	acs_push_num(3);
	acs_push_num(4);
	return acs_call_tail(2, 0);
}

void machine_test(void)
{
#define CMD(op,arg) (((unsigned)(OP_ ## op) << 12) | (unsigned)(arg))
	uint16_t code1[] = {
		CMD(PUSHC, 0),
		CMD(PUSHI, 1),
		CMD(PUSHI, 4),
		CMD(CALL, 2),
		CMD(RET, 0),
	};
	puts("test1");
	do_machine_test(code1);

	uint16_t code2[] = {
		CMD(PUSHC, 0),
		CMD(PUSHI, 1),
		CMD(PUSHI, 2),
		CMD(BSCALL, BS_ADD),
		CMD(CALL, 1),
		CMD(RET, 0),
	};
	puts("test2");
	do_machine_test(code2);

	uint16_t code3[] = {
		CMD(PUSHI, 10),
		CMD(PUSHC, 0),
		CMD(PUSHR, 0),
		CMD(CALL, 1),
		CMD(PUSHR, 0),
		CMD(PUSHI, 1),
		CMD(BSCALL, BS_SUB),
		CMD(POPR, 0),
		CMD(PUSHR, 0),
		CMD(PUSHI, 0),
		CMD(BSCALL, BS_EQ),
		CMD(JZ, PC_OFFSET - 11),
		CMD(RET, 0),
	};
	puts("--- test3 ---");
	do_machine_test(code3);

#if 0
	uint16_t code4[] = {
		CMD(PUSHI, 10),
		CMD(PUSHC, 0),
		CMD(PUSHR, 0),
		CMD(CALL, 1),
		CMD(PUSHR, 0),
		CMD(PUSHI, 1),
		CMD(BSCALL, BS_ADD),
		CMD(POPR, 0),
		CMD(JMP, PC_OFFSET - 8),
		CMD(RET, 0),
	};
	puts("--- test4 ---");
	do_machine_test(code4);
#endif

	uint16_t code5[] = {
		CMD(PUSHI, 1),
		CMD(PUSHI, 0),
		CMD(PUSHI, 20), // x,x_,n = 1,0,20;
		CMD(PUSHC, 0),
		CMD(PUSHR, 0),
		CMD(CALL, 1), // print(x);
		CMD(PUSHR, 0),
		CMD(PUSHR, 1),
		CMD(BSCALL, BS_ADD),
		CMD(PUSHR, 0),
		CMD(PUSHR, 2),
		CMD(PUSHI, 1),
		CMD(BSCALL, BS_SUB),
		CMD(POPR, 2),
		CMD(POPR, 1),
		CMD(POPR, 0), // x,x_,n = x+x_, x, n - 1;
		CMD(PUSHI, 0),
		CMD(PUSHR, 2),
		CMD(BSCALL, BS_EQ),
		CMD(JZ, PC_OFFSET - 17), // while (n != 0)
		CMD(RET, 0),
	};
	puts("--- test5 ---");
	do_machine_test(code5);

	uint16_t code6[] = {
		CMD(PUSHC, 0),
		CMD(BSCALL, BS_ARGC),
		CMD(CALL, 1),
		CMD(PUSHC, 0),
		CMD(PUSHI, 0),
		CMD(BSCALL, BS_ARGV),
		CMD(PUSHI, 1),
		CMD(BSCALL, BS_ARGV),
		CMD(BSCALL, BS_ADD),
		CMD(CALL, 1),
		CMD(RET, 0),
	};
	puts("--- test6 ---");
	do_machine_test(code6);

	uint16_t code7[] = {
		CMD(PUSHU, 0),
		CMD(PUSHI, 123),
		CMD(CALL, 1),
		CMD(RET, 0),
	};
	puts("--- test7 ---");
	do_machine_test(code7);
}


