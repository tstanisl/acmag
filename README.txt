Syntax:

SCRIPT -> {FUNC}
FUNC -> ID '(' [FUNC_ARGS] ')' INST
FUNC_ARGS -> ID {',' ID}

INST -> ';'
INST -> EXPR ';'
INST -> REF_EXPR '=' EXPR ';'
INST -> REF_EXPR '=' '&' EXPR ';'
INST -> 'return' [ EXPR ] ';'
INST -> 'if' '(' EXPR ')' INST ['else' INST]
INST -> 'for' '(' ID ',' EXPR ')' INST
INST -> 'while' '(' EXPR ')' INST
INST -> '{' { INST } '}'
INST -> 'break'
INST -> 'continue'

EXPR -> ORR_EXPR
ORR_EXPR -> AND_EXPR {'||' AND_EXPR}
AND_EXPR -> CMP_EXPR {'&&' CMP_EXPR}
CMP_EXPR -> SUM_EXPR
CMP_EXPR -> SUM_EXPR '<' SUM_EXPR
CMP_EXPR -> SUM_EXPR '<=' SUM_EXPR
CMP_EXPR -> SUM_EXPR '>' SUM_EXPR
CMP_EXPR -> SUM_EXPR '>=' SUM_EXPR
CMP_EXPR -> SUM_EXPR '==' SUM_EXPR
CMP_EXPR -> SUM_EXPR '!=' SUM_EXPR
SUM_EXPR -> MUL_EXPR { '+' MUL_EXPR }
SUM_EXPR -> MUL_EXPR { '-' MUL_EXPR }
MUL_EXPR -> SNG_EXPR { '*' SNG_EXPR }
MUL_EXPR -> SNG_EXPR { '/' SNG_EXPR }
MUL_EXPR -> SNG_EXPR { '%' SNG_EXPR }
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
