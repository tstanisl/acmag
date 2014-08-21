Syntax:

SCRIPT -> {FUNC}
FUNC -> ID '(' [FUNC_ARGS] ')' '{' INST '}'
FUNC_ARGS -> ID {',' ID}

INST -> ';'
INST -> EXPR ';'
INST -> 'return' [ EXPR ] ';'
INST -> 'if' '(' EXPR ')' INST ['else' INST]
INST -> 'for' '(' ID ',' EXPR ')' INST
INST -> 'while' '(' EXPR ')' INST
INST -> '{' { INST } '}'
INST -> 'break'
INST -> 'continue'

EXPR -> ORR_EXPR [ '='  EXPR ]

ORR_EXPR -> AND_EXPR { '||' AND_EXPR }
AND_EXPR -> CMP_EXPR { '&&' CMP_EXPR }
CMP_EXPR -> SUM_EXPR { '<'  SUM_EXPR }
CMP_EXPR -> SUM_EXPR { '<=' SUM_EXPR }
CMP_EXPR -> SUM_EXPR { '>'  SUM_EXPR }
CMP_EXPR -> SUM_EXPR { '>=' SUM_EXPR }
CMP_EXPR -> SUM_EXPR { '==' SUM_EXPR }
CMP_EXPR -> SUM_EXPR { '!=' SUM_EXPR }
SUM_EXPR -> MUL_EXPR { '+'  MUL_EXPR }
SUM_EXPR -> MUL_EXPR { '-'  MUL_EXPR }
MUL_EXPR -> SNG_EXPR { '*'  SNG_EXPR }
MUL_EXPR -> SNG_EXPR { '/'  SNG_EXPR }
MUL_EXPR -> SNG_EXPR { '%'  SNG_EXPR }
SNG_EXPR -> '+' SNG_EXPR
SNG_EXPR -> '-' SNG_EXPR
SNG_EXPR -> '!' SNG_EXPR
SNG_EXPR -> REF_EXPR
REF_EXPR -> TOP_EXPR { REF_TAIL }
REF_TAIL -> '(' ARGS ')'
REF_TAIL -> '.' ID
ARGS -> [EXPR {',' EXPR}]
TOP_EXPR -> ID
TOP_EXPR -> NUM
TOP_EXPR -> STR
TOP_EXPR -> '(' EXPR ')'


Example:

sword() {
	s = weapon();
	s.name = "sword";
	s.damage = "1d10";
	return s;
}

granade_boom(g)
{
	a = explosion_area(g.position, 5);
	// TODO: add owner here
	d = damage_effect("3d6", "fire");
	add_area_effect(d, a);
	delete(g);
}

granade_start(g)
{
	if (a.activated) {
		info("Granade is already activated.");
		return;
	}
	a.activated  = 1;
	// schedule boom in 10 seconds
	add_event(g, granade_boom, 10);
}

granade()
{
	g = item();
	g.name = "granade";
	g.activated = 0;
	g.on_use = granade_start;
	return g;
}

====== Machine ========
Support for commands:

movsp #n
	push previous frame pointer on stack, move sp by #n units
ret #a, #b
	pop $0 (aka FP) to fp_
	copy #b registers to fp[-#a] position
	set FP to fp_
retv #b
	pop $0 (aka FP) to fp_
	pop $-1 (aka n_args) to n_args
	copy #b registers to fp[-#n_args] position
	set FP to fp_
push $r
	push content of register $r on stack
push #n
	push integer (12bit) on stack
push ##n
	push large integer (indexed by 12bit) on stack
push "str"
	push string (indexed by 12bit) on stack
pushn #n
	push nil #n times on stack
pop $r
	pop to regiter $r
popn #n
	pop #n times and ignore results
call #n
	call global function of index #n
calli #n
	call code at offset cur+#n
callb #n
	call builin function #n
jz #n
	pop stack, if zero pc = pc + #n
jnz #n
	pop stack, if non-zero pc = pc + #n
jmp #n
	pc = pc + #n


movsp #n
ret #n, #n
retv #n
push $r
push #n
push ##n
push "str"
pushn #n
pop $r
popn #n
call #func
calli #func
callb #func
jz label
jnz label
jmp label

How it may look like:
a)
	x, x_ = 1, 0;
	while (x < 1000) {
		print(x);
		x, x_ = x + x_, x;
	}
maps to
	push #0
	push #1
	pop $0
	pop $1
L1:	push $0
	push #1000
	call <
	jz L2
	push $0
	call print
	push $0
	push $1
	call +
	push $0
	pop $1
	pop $2
	jmp L1
L2:

b)
	g.name = "granade";
==>
	push "granade"
	push "name"
	push $0 ; assume that g is in 0
	call .
	call =

Result 17 instruction == 34 bytes :)
Requirements:
- returning tuples
- functions with variable number of arguments
- 

c)
	fib(a,b) : 2;
	main() {
		x, x_ = 1, 0;
		while (x < 100)
			x, x_ = fib(x, x_);
	}
	fib(a,b) {
		return a + b, a;
	}
===>
main:	movsp #2
	push #0
	push #1
	pop $1
	pop $2
L1:	push #100
	push $1
	call <
	jz L2
	push $2
	push $1
	calli fib
	pop $1
	pop $2
	jmp L1
L2:	ret #0, #2
fib:	push $-1
	push $-2
	call +
	push $-1
	ret #2, #2

d)
	info(...) {
		for (i = 1; i <= arg[0]; ++i)
			print(arg[i]);
		return i;
	}
===>
info:	movsp #1
	push #1
	pop $1
L1:	push #0
	call arg[]
	push $1
	call <=
	ifz L2
	push $0
	call arg[]
	call print
	jmp L1
L2:	push $1
	retv #1
