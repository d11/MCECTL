%{

#include <string.h>
#define YY_NO_UNPUT

%}

%option noyywrap

%%

"DEFAULT_INT_BITS"		{ return DEFAULT_INT_BITS; }
"DEFAULT_SIGNED_INT_BITS"	{ return DEFAULT_SIGNED_INT_BITS; }

"bool"		{ return BOOL; }
"unsigned int"	{ return INT; }
"signed int"	{ return SGNINT; }
"int"		{ return INT; }
"struct"	{ return STRUCT; }
"enum"		{ return ENUM; }
"void"		{ return VOID; }
"define"	{ return DEFINE; }
"A"		{ return PDS_FORALL; }
"E"		{ return PDS_EXISTS; }
"init"		{ return INIT; }
"module"	{ return MODULE; }
"if"		{ return IF; }
"fi"		{ return FI; }
"else"		{ return ELSE; }
"do"		{ return DO; }
"od"		{ return OD; }
"return"	{ return RETURN; }
"goto"		{ return GOTO; }
"break"		{ return BREAK; }
"skip"		{ return SKIP; }
"true"		{ return TOK_TRUE; }
"false"		{ return TOK_FALSE; }
"undef"		{ return UNDEF; }
"fork"		{ return FORK; }
"shared"	{ return SHARED; }

[A-Za-z_][A-Za-z0-9_]* {
	
	irlval.name = wIdentCreate(irtext);
	
	/* Check if the identifier is defined as number */
	int v = irDefValue(irlval.name);
	if (v > -1) {
		irlval.integer = v;
		return NUMBER;
	}
	
	/* Check if the identifier is of type boolean */
	if (irVarType(ir_gbl, last_var, irlval.name) == IR_BOOL)
		return BVAR;
	/*if (current_module_count) {
		int i;
		IrModule m = current_module;
		for (i = current_module_count; i>0; i--, m = m->next) {
			if (irVarType(m->param, last_var, irlval.name) == IR_BOOL)
				return BVAR;
			if (irVarType(m->localvar, last_var, irlval.name) == IR_BOOL)
				return BVAR;
		}
	}*/
	if (current_module) {
		if (irVarType(current_module->param, last_var, irlval.name) == IR_BOOL)
			return BVAR;
		if (irVarType(current_module->localvar, last_var, irlval.name) == IR_BOOL)
			return BVAR;
	}
	if (called_module) {
		if (irVarType(called_module->param, last_var, irlval.name) == IR_BOOL)
			return BVAR;
	}

	/* Check if the identifier is a quantified variable */
	IrQvar qvar;
	for (qvar = ir_qvar; qvar; qvar = qvar->next) {
		if (qvar->name != irlval.name) continue;
		irlval.qvar = qvar;
		return QVAR;
	}

	/* Otherwise, it's just a normal identifier */
	return IDENT;
}

[0-9]+		{
			irlval.integer = atoi(irtext);
			return NUMBER;
		}

\"[^"]*\"	{
			irlval.string = Strdup(irtext+1);
			irlval.string[strlen(irtext)-2] = 0;
			return LABEL;
		}


"-->"		{ return ARROW; }
"->"		{ return CONDARROW; }
"::"		{ return DOUBLECOLON; }
"'"		{ return PRIME; }

"||"		{ return PDS_LGC_OR; }
"&&"		{ return PDS_LGC_AND; }
"^^"		{ return PDS_LGC_XOR; }
"!"	 	{ return PDS_LGC_NOT; }
"<=>"		{ return PDS_EQV; }

"+"		{ return PDS_PLUS; }
"*"		{ return PDS_MULT; }
"-"		{ return PDS_MINUS; }
"/"		{ return PDS_DIV; }
"<<"		{ return PDS_SHL; }
">>"		{ return PDS_SHR; }
"&"		{ return PDS_AND; }
"|"		{ return PDS_OR; }
"^"		{ return PDS_XOR; }

"<="		{ return PDS_LE; }
"=="		{ return PDS_CMPEQ; }
"!="		{ return PDS_NE; }
">="		{ return PDS_GE; }
"="			{ return PDS_EQ; }

[\{\}<>,:;\(\)]	{ last_var = 0; return irtext[0]; }
[][\.]		{ return irtext[0]; }

[\%#](.)*\n	{ irlinenumber++; }
"//"(.)*\n	{ irlinenumber++; }
[ 	]	;
\n		{ irlinenumber++; }

