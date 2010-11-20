%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bp.h"

int bplex();
void yyerror(char*);
extern int line;

bp_ident_t bp_globals;
bp_ident_t bp_parms;	// sichert temporär den Inhalt von bp_decls
bp_fun_t bp_functions;
bp_tree_t bp_nametree;
bp_tree_t bp_funtree;
int bp_maxrets;
int bp_numglobals;
int bp_maxlocs;

int bp_labelcount;	// Zähler, um eindeutige Labels zu vergeben

short bp_funcount;	// zählt die Funktionen
int bp_declcount;	// zählt Anzahl Variablen in einem Deklarationsblock
char bp_doing_parms;	// zeigt an, ob gerade Parameter oder lokale Variablen
			// geparst werden
bp_ident_t bp_decls;	// Sammelt die Liste der Ids in einer Deklaration

bp_tree_t bp_labeltree;	// wird hier nicht gebraucht, erst später

bp_ident_t bp_locids;	// Liste von Programmvariablen
bp_ident_t bp_globids;	// (wird u.a. in reorder.c gebraucht)
bp_ident_t bp_allglobids;

%}

%union {
	bp_stmt_t stmt;
	bp_expr_t expr;
	bp_ident_t ident;
	bp_idref_t idref;
	bp_fun_t fun;
	char* c;
	int   i;
}

%token <c> IDENTIFIER
%token <i> INTEGER
%token NONDET
%token ASSIGN EQ_OP NE_OP IMPLIES
%token DECL
%token ENFORCE
%token AND OR
%token IF THEN ELSE ELSIF FI 
%token WHILE DO OD
%token RETURN SKIP GOTO
%token BEG END
%token BOOL VOID
%token ASSERT ASSUME PRINT CHOOSE CONSTRAIN

%type <ident> global_decls
%type <fun> fun_list function_definition
%type <i> fun_type parms
%type <idref> assign_id_list asgn_id
%type <idref> expression_list
%type <stmt> statement_list
%type <stmt> labelled_statement statement
%type <stmt> parallel_assign call assert print
%type <stmt> selection_statement elsif_list
%type <stmt> iteration_statement jump_statement
%type <expr> primary_expression unary_expression equality_expression
%type <expr> and_expression exclusive_or_expression expression
%type <expr> decider or_expression enforce choose_expression

%start program

%%

program	: pstart global_decls fun_list
	  {
		bp_globals = $2;
		bp_functions = $3;
	  } ;

pstart	: {
		bp_funcount = -1;
		bp_nametree = NULL;
		bp_funtree = NULL;
		bp_labelcount = 0;

		bp_maxlocs = 0;
		bp_maxrets = 0;
		bp_locids = NULL;
		bp_globids = NULL;
		bp_allglobids = NULL;
	  } ;

/***** decls *****/

global_decls
	: decl_start decl_list
	  {
		$$ = bp_decls;
		bp_numglobals = bp_declcount;
	  } ;

decl_start
	: {
		bp_declcount = 0;
		bp_funcount++;
		bp_decls = NULL;
		bp_doing_parms = 1;
	  } ;

decl_list
	: DECL identifier_list ';' decl_list
	| ;

identifier_list
	: decl_id
	| identifier_list ',' decl_id ;

decl_id	: IDENTIFIER			
	  {
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = wIdentCreate($1);
		newid->index = bp_funcount? bp_doing_parms?
			bp_assign_parm_index(newid->varname,bp_funcount)
		      : bp_assign_loc_index(newid->varname,bp_funcount)
		      : bp_assign_glob_index(newid->varname);
		newid->fun = bp_funcount;
		bp_insert_decl(&bp_nametree,
				newid->varname,bp_funcount,newid);
		newid->next = bp_decls;
		bp_decls = newid;
	  } ;

/***** functions *****/

fun_list
	: function_definition
	| function_definition fun_list
	  { $$ = $1; $$->next = $2; } ; 

function_definition
	: fun_type IDENTIFIER parms
	  BEG decl_list enforce statement_list END
	  {
		int i;

		$$ = alloc_bp_fun();
		$$->funname = wIdentCreate($2);
		$$->num_returns = $1;
		$$->num_args = $3;
		$$->num_locals = bp_declcount;
		$$->num_globals = bp_numglobals;
		$$->locals = bp_decls;
		$$->parms = bp_parms;
		$$->enforce = $6;
		$$->stmt = $7;
		$$->next = NULL;

		$$->returns = NULL;
		$$->callbuf = NULL;

		for (i = 0; i < $$->num_returns; i++)
		{
			char tmp[16];
			bp_ident_t newid = alloc_bp_ident();
			sprintf(tmp,"*ret%d",i+1);
			newid->varname = wIdentCreate(tmp);
			newid->index = bp_assign_ret_index(newid->varname,i);
			newid->fun = 0;
			newid->next = $$->returns;
			$$->returns = newid;
		}

		for (i = 0; i < $$->num_locals; i++)
		{
			bp_ident_t newid = alloc_bp_ident();
			newid->index = -1;
			newid->fun = 0;
			newid->next = $$->callbuf;
			
			$$->callbuf = newid;
		}

		bp_insert_decl(&bp_funtree,$$->funname,-1,(bp_ident_t)$$);
	  } ;

fun_type
        : BOOL	{ $$ = 1; }
	| BOOL '<' INTEGER '>' { $$ = $3; }
        | VOID	{ $$ = 0; }
	;

enforce	: ENFORCE expression ';' { $$ = $2; }
	| { $$ = NULL; }
	;

parms	: decl_start '(' parameter_list ')'
	  { $$ = bp_declcount; bp_parms = bp_decls; bp_doing_parms = 0; }
	;

parameter_list
	: identifier_list
	|
	;

/***** statements *****/

statement_list
        : labelled_statement 
	| labelled_statement statement_list
	  {	$$ = $1; $$->next = $2;		}
	;

labelled_statement
        : IDENTIFIER ':' statement 
	  {
		$$ = $3; $$->no_label_given = 0; $$->label = wIdentCreate($1);
	  }
        | statement
	  {	$$ = $1; $$->no_label_given = 1; $$->label = ++bp_labelcount; }
	;

statement
	: parallel_assign ';'
        | assert ';'
        | print ';'
	| call ';'
	| selection_statement
	| iteration_statement
	| jump_statement ';' ;

parallel_assign 
       	: assign_id_list ASSIGN expression_list
	  {
		bp_idref_t tmp1, tmp2, tmp3;

		new_bp_stmt($$,BP_ASSIGN);

		for (tmp1 = $1, tmp2 = $3; tmp1 && tmp2; tmp1 = tmp1->next)
		{
			tmp1->expr = tmp2->expr;
			tmp3 = tmp2->next;
			free(tmp2);
			tmp2 = tmp3;
		}

		if (tmp1 || tmp2)
		{
			yyerror("number of assign clauses doesn't match");
			YYERROR;
		}

		$$->e.a.asgnlist = $1;
	  }
       	| assign_id_list ASSIGN call
	  { 
		$$ = $3;
		$$->e.a.asgnlist = $1;
	  } ;

assign_id_list
	: asgn_id	
	| assign_id_list ',' asgn_id
	  {	$$ = $3; $$->next = $1;	} ;

asgn_id	: IDENTIFIER			
	  {
		$$ = alloc_bp_idref();
		$$->ident = bp_find_decl(wIdentCreate($1),bp_funcount);
		$$->primed = 1;
		$$->next = NULL;
	  } ;

assert	: ASSERT '(' decider ')'
	  {
		new_bp_stmt($$,BP_ASSERT);
		$$->e.c.expr = $3;
	  }
	| ASSUME '(' decider ')'
	  {
		new_bp_stmt($$,BP_ASSUME);
		$$->e.c.expr = $3;
	  }
	| CONSTRAIN '(' decider ')'
	  {
		new_bp_stmt($$,BP_CONSTRAIN);
		$$->e.c.expr = $3;
	  } ;

print	: PRINT '(' expression_list ')'
	  {
		new_bp_stmt($$,BP_PRINT);
		$$->e.a.asgnlist = $3;
	  } ;

call	: IDENTIFIER '(' expression_list ')'
	  {
		new_bp_stmt($$,BP_CALL);
		$$->e.a.label = wIdentCreate($1);
		$$->e.a.asgnlist = NULL;
		$$->e.a.fmllist = $3;
	  }
	| IDENTIFIER '(' ')'
	  {
		new_bp_stmt($$,BP_CALL);
		$$->e.a.label = wIdentCreate($1);
		$$->e.a.asgnlist = NULL;
		$$->e.a.fmllist = NULL;
	  } ;

selection_statement
	: IF '(' decider ')' THEN statement_list elsif_list
	  {
		new_bp_stmt($$,BP_IF);
		$$->e.c.expr = $3;
		$$->e.c.thenstmt = $6;
		$$->e.c.elsestmt = $7;
	  } ;

elsif_list
	: FI
	  {	$$ = NULL;	}
	| ELSE statement_list FI
	  {	$$ = $2;	}
	| ELSIF '(' decider ')' THEN statement_list elsif_list
	  {
		new_bp_stmt($$,BP_IF);
		$$->no_label_given = 1;
		$$->label = ++bp_labelcount;
		$$->e.c.expr = $3;
		$$->e.c.thenstmt = $6;
		$$->e.c.elsestmt = $7;
	  } ;

iteration_statement
	: WHILE '(' decider ')' DO statement_list OD
	  {
		new_bp_stmt($$,BP_WHILE);
		$$->e.c.expr = $3;
		$$->e.c.thenstmt = $6;
	  } ;

jump_statement
	: RETURN
	  {
		new_bp_stmt($$,BP_RETURN);
		$$->e.a.asgnlist = NULL;
	  }
	| RETURN expression_list
	  {
		new_bp_stmt($$,BP_RETURN);
		$$->e.a.asgnlist = $2;
	  }
	| SKIP
	  {
		new_bp_stmt($$,BP_SKIP);
	  }
	| GOTO IDENTIFIER
	  {
		new_bp_stmt($$,BP_GOTO);
		$$->e.a.label = wIdentCreate($2);
	  } ;

/***** expressions *****/

primary_expression
	: IDENTIFIER 	
	  { 
		if ((*$1 == 'T' || *$1 == 'F') && !$1[1])
		{
			new_bp_expr($$,BP_CONST,(bp_expr_t)(*$1 == 'T'),NULL);
		}
		else
		{
			bp_idref_t ref = alloc_bp_idref();
			ref->ident =
			   bp_find_var(bp_nametree,wIdentCreate($1),bp_funcount);
			ref->primed = 0;
			new_bp_expr($$,BP_IDENT,(bp_expr_t)ref,NULL);
		}
	  }
	| '\'' IDENTIFIER
	  {
		bp_idref_t ref = alloc_bp_idref();
		ref->ident =
			bp_find_var(bp_nametree,wIdentCreate($2),bp_funcount);
		ref->primed = 1;
		new_bp_expr($$,BP_IDENT,(bp_expr_t)ref,NULL);
	  }
	| INTEGER 
	  { 
		if ($1 == 1 || $1 ==0) {
			new_bp_expr($$,BP_CONST,(bp_expr_t)$1,NULL);
		} else {
		    yyerror("Boolean constants are 0 and 1\n");
		    YYERROR;
		}
	  }
	| '(' expression ')'
	  {	$$ = $2;	} ;

unary_expression
	: primary_expression	
	| unary_operator primary_expression 
	  {	new_bp_expr($$,BP_NOT,$2,NULL);		} ;

unary_operator
	: '~'
	| '!' ;

equality_expression
	: unary_expression 
	| equality_expression EQ_OP unary_expression
	  {	new_bp_expr($$,BP_EQ,$1,$3);	}
	| equality_expression NE_OP unary_expression
	  {	new_bp_expr($$,BP_NE,$1,$3);	} ;

and_expression
	: equality_expression
	| and_expression AND equality_expression
	  {	new_bp_expr($$,BP_AND,$1,$3);	} ;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	  {	new_bp_expr($$,BP_NE,$1,$3);	} ;

or_expression
	: exclusive_or_expression
	| or_expression OR exclusive_or_expression
	  {	new_bp_expr($$,BP_OR,$1,$3);	} ;

expression
	: or_expression
	| expression IMPLIES or_expression
	  {	new_bp_expr($$,BP_IMP,$1,$3);	} ;

decider
	: NONDET
	  {	new_bp_expr($$,BP_ND,NULL,NULL);	}
        | expression ;

choose_expression
	: expression
        | CHOOSE '[' expression ',' expression ']'
	  {	new_bp_expr($$,BP_CHOOSE,$3,$5);	} ;

expression_list
	: choose_expression
	  {
		$$ = alloc_bp_idref();
		$$->expr = $1;
		$$->next = NULL;
	  }
	| expression_list ',' choose_expression
	  {
		$$ = alloc_bp_idref();
		$$->expr = $3;
		$$->next = $1;
	  } ;

%%

#include "bpLex.c"

extern int column;

void yyerror(char *s)
{
	fflush(stdout);
	printf("Line %d: syntax error.\n",line);
}

bddPds* bpReadBP (char *filename)
{
	line = column = 1;
	if (!(bpin = fopen(filename,"r")))
		bpError("program file %s not found",filename);
	if (bpparse())
		bpError("parse errors encountered");
	fclose(bpin);

	/* translate bp tree into pds */
	return bp_create_pds();
}

void bpError(char* msg, ...)
{
        va_list args;
        va_start(args,msg);
        fprintf(stderr,"error: ");
        vfprintf(stderr,msg,args);
        fprintf(stderr,"\n");
        va_end(args);
        exit(1);
}
