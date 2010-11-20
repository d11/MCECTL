%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "alloc.h"
#include "irInt.h"

int irlinenumber = 1;
int int_bits = 0;
int sgnint_bits = 0;
IrModule current_module = NULL;
IrModule called_module = NULL;
wIdent last_var = 0;
int shared_stmt = 0;
int else_cnt = 0;

int irlex();
void irerror (char*);

%}

%union
{
	char *string;
	int integer;
	wIdent name;
	IrDim y_dim;
	IrQvar qvar;
	IrVar y_var;
	IrEnum y_enum;
	IrStruct y_struct;
	IrVarList y_varlist;
	IrName y_name;
	IrFnode y_fnode;
	IrStmt y_stmt;
	IrRule y_rule;
	IrIfdo y_ifdo;
	IrExprList y_exprlist;
	IrModule y_module;
}

%token DEFINE
%token <name> IDENT BVAR
%token <integer> NUMBER
%token DEFAULT_INT_BITS DEFAULT_SIGNED_INT_BITS
%token BOOL INT SGNINT ENUM STRUCT
%token INIT
%token ARROW
%token IF FI ELSE DO OD
%token DOUBLECOLON CONDARROW
%token PDS_LGC_AND PDS_LGC_OR PDS_EQV PDS_LGC_XOR PDS_LGC_NOT
%token PDS_FORALL PDS_EXISTS
%token TOK_TRUE TOK_FALSE
%token PDS_PLUS PDS_MINUS PDS_MULT PDS_DIV
%token PDS_SHL PDS_SHR PDS_AND PDS_OR PDS_XOR
%token <qvar> QVAR
%token PDS_LE PDS_CMPEQ
%token PDS_NE PDS_GE
%token PDS_EQ
%token PRIME
%token GOTO BREAK RETURN SKIP
%token MODULE VOID
%token <string> LABEL
%token UNDEF
%token FORK
%token SHARED

%type <y_varlist> vardecl
%type <y_enum> enumname
%type <y_struct> structname
%type <y_varlist> varlist
%type <y_var> varident
%type <name> optident
%type <y_dim> optdim
%type <y_dim> dim
%type <integer> optbit
%type <y_name> enumlist
%type <y_varlist> structtypelist
%type <y_name> identlist
%type <y_fnode> optboolexpr
%type <y_stmt> stmtlist
%type <y_stmt> lblstmt
%type <y_stmt> stmt
%type <y_rule> rawtrans
%type <string> label
%type <y_ifdo> ifstmt
%type <y_ifdo> dostmt
%type <y_ifdo> clnstmtlist
%type <y_ifdo> clnstmt
%type <y_fnode> boolexpr
%type <y_fnode> quant
%type <y_fnode> intcomp
%type <y_fnode> intexpr
%type <y_fnode> structvaroptidx
%type <y_fnode> structbvaroptidx
%type <y_fnode> varoptidx
%type <y_fnode> varoptprime
%type <y_fnode> bvaroptidx
%type <y_fnode> bvaroptprime
%type <integer> cnstexpr
%type <y_fnode> expr
%type <y_stmt> action
%type <y_exprlist> args
%type <y_exprlist> argslist
%type <y_fnode> exproptundef
%type <y_stmt> callstmt
%type <y_stmt> retstmt
%type <y_exprlist> optassgnlist
%type <y_exprlist> assgnlist
%type <y_exprlist> assgn
%type <y_stmt> module
%type <y_module> modulehead
%type <y_var> rettype
%type <y_varlist> params
%type <y_varlist> paramlist
%type <y_var> param

%left PDS_LGC_OR
%left PDS_LGC_XOR
%left PDS_LGC_AND
%left PDS_OR
%left PDS_XOR
%left PDS_AND
%left PDS_EQV PDS_CMPEQ PDS_NE
%left PDS_LE PDS_GE
%left PDS_SHL PDS_SHR
%left PDS_PLUS PDS_MINUS
%left PDS_MULT PDS_DIV
%right PDS_LGC_NOT PDS_FORALL PDS_EXISTS

%%

automaton: defines gbldecls initconfig stmtlist {
		ir_stmt = $4;
		irLabelUpdate(ir_stmt);
	}
	;

defines: /* empty */
	| defines DEFINE IDENT cnstexpr {
		IrDef def = irDefCreate($3, $4, ir_def);
		ir_def = def;
	}
	| defines DEFINE DEFAULT_INT_BITS cnstexpr {
		int_bits = $4;
	}
	| defines DEFINE DEFAULT_SIGNED_INT_BITS cnstexpr {
		sgnint_bits = $4;
	}
  ;

gbldecls: /* empty */ 
	| gbldecls vardecl {
		IrVarList vl;
		for (vl = $2; vl->next; vl = vl->next) {}
		vl->next = ir_gbl;
		ir_gbl = $2;
	}
	| gbldecls SHARED vardecl {
		IrGbl vl;
		for (vl = $3; vl->next; vl = vl->next) {}
		vl->next = ir_sgbl;
		ir_sgbl = $3;
		
		/* TODO: Insert list of shared global variables (ir_sgbl) 
		to the end of list of global variables (ir_gbl). */
		if (ir_gbl)
			for (vl = ir_gbl; vl->next; vl = vl->next) {}
		else
			ir_gbl = ir_sgbl;
	}
	| gbldecls typedecl
	| gbldecls moduledecl
	;
	
moduledecl: MODULE rettype IDENT '(' params ')' ';' {
		IrModule module = irModuleCreate($3, $2, $5);
		module->next = ir_module;
		ir_module = module;
	}
	;

vardecl: BOOL varlist ';' {
		$$ = $2;
		IrVarList vl;
		for (vl = $$; vl; vl = vl->next) {
			vl->var->type = IR_BOOL;
		}
	}
	| INT varlist ';' {
		$$ = $2;
		IrVarList vl;
		for (vl = $$; vl; vl = vl->next) {
			vl->var->type = IR_INT;
			if (vl->var->bits) continue;
			if (!int_bits) irerror("unspecified integer range");
			vl->var->bits = int_bits;
		}
	}
	| SGNINT varlist ';' {
		irerror("current implementation does not support signed int");
		$$ = $2;
		IrVarList vl;
		for (vl = $$; vl; vl = vl->next) {
			vl->var->type = IR_SGNINT;
			if (vl->var->bits) continue;
			if (!sgnint_bits) irerror("unspecified integer range");
			vl->var->bits = sgnint_bits;
		}
	}
	| ENUM enumname varlist ';' {
		IrVarList vl;
		for (vl = $3; vl; vl = vl->next) {
			vl->var->type = IR_ENUM;
			vl->var->type_ptr = $2;
			vl->var->bits = $2->bits;
		}
		$$ = $3;
	}
	| STRUCT structname varlist ';' {
		IrVarList vl;
		IrVarList fields;
		for (vl = $3; vl; vl = vl->next) {
			for (fields = $2->fields; fields; fields = fields->next)
				if (vl->var->dim && fields->var->dim)
					irerror("array of struct of array not allowed");
			vl->var->type = IR_STRUCT;
			vl->var->type_ptr = $2;

			irAddStructVar(vl->var);
		}
		$$ = $3;
	}
	;

enumname: optident '{' enumlist '}' {
		if (!$3) irerror("empty enum list");
		if (current_module) 
			irerror("enum declaration found inside a module");
		$$ = irCreateTypeEnum($1, $3);
	}
	| IDENT {
		IrEnum en = NULL;
		for (en = ir_enum; en; en = en->next)
			if (en->name == $1) break;
		if (!en) irerror("undefined enum type");
		$$ = en;
	}
	;

structname: optident '{' structtypelist '}' {
		if (current_module) 
			irerror("struct declaration found inside a module");
		$$ = irStructCreate($1, $3, ir_struct);
		ir_struct = $$;
	}
	| IDENT {
		IrStruct s = NULL;
		for (s = ir_struct; s; s = s->next)
			if (s->name == $1) break;
		if (!s) irerror("undefined struct type");
		$$ = s;
	}
	;

varlist: varident {
		if (irVarGetUnformatted($1->name, current_module, called_module))
			irerror("two variables have the same name");
		$$ = irVarListCreate($1, NULL);
	}
	| varlist ',' varident {
		if (irVarGetUnformatted($3->name, current_module, called_module))
			irerror("two variables have the same name");
		$$ = irVarListCreate($3, $1);
	}
	;

varident: IDENT optdim optbit {
		$$ = irVarCreate($1, $2, $3);
	}
	;

optident: /* empty */ { $$ = 0; }
	| IDENT { $$ = $1; }
	;

optdim: /* empty */ { $$ = NULL; }
	| dim { $$ = $1; }
	| dim dim {
		$1->next = $2;
		$$ = $1;
	}
	;

dim: '[' cnstexpr ']' {
		if ($2 <= 0) irerror("invalid array size");
		$$ = irDimCreate($2, 0, $2-1, NULL);
	}
	| '[' cnstexpr ',' cnstexpr ']' {
		if ($2 < 0 || $2 > $4)
			irerror("invalid array range");
		$$ = irDimCreate($4-$2+1, $2, $4, NULL)
	}
	;

optbit: /* empty */ { $$ = 0; }
	| '(' cnstexpr ')' { 
		if ($2 <= 0) irerror("positive integer expected");
		$$ = $2;
	}
	;

typedecl: ENUM optident '{' enumlist '}' ';' {
		if (!$4) irerror("empty enum list");
		irCreateTypeEnum($2, $4)
	}
	| STRUCT optident '{' structtypelist '}' ';' {
		IrStruct st = irStructCreate($2, $4, ir_struct);
		ir_struct = st;
	}
	;

enumlist: IDENT {
		$$ = irNameCreate($1, NULL);
	}
	| IDENT ',' enumlist {
		$$ = irNameCreate($1, $3);
	}
	;

structtypelist: vardecl {
		if ($1->var->type == IR_STRUCT)
			irerror("struct cannot be inside a struct");
		$$ = $1;
	}
	| structtypelist vardecl {
		if ($2->var->type == IR_STRUCT)
			irerror("struct cannot be inside a struct");
		$$ = $2;
		$$->next = $1;
	}
	;

initconfig: /* empty */ /*{
			ir_init = irInitCreate(default_state_name, NULL, 
				irNameCreate(wIdentCreate("main"), NULL), NULL);
		}*/
	/* I disabled this rule because the current data structure for bddPds
	   does not have something resembling stack_fnode. We may put it back
	   in at a later stage. - Stefan
		| INIT '(' IDENT optboolexpr '<' IDENT optboolexpr '>' ')' ';' {
				ir_init = Malloc(sizeof(struct IrInitStruct));
				ir_init->state = $3;
				ir_init->state_fnode = $4;
				ir_init->stack = $6;
				ir_init->stack_fnode = $7;
			}
	*/
  /*|*/ INIT identlist ';' {
		ir_init = irInitCreate(default_state_name, NULL, $2, NULL);
	}
	;
	
identlist: IDENT {
		$$ = irNameCreate($1, NULL);
	}
	| IDENT identlist {
		$$ = irNameCreate($1, $2);
	}
	;

optboolexpr: /* empty */ { $$ = NULL; }
	|	'(' boolexpr ')' { $$ = $2; }
	;

stmtlist: /* empty */ { $$ = NULL; }
	| lblstmt stmtlist {
		if ($1->next) {
			IrStmt stmt;
			for (stmt = $1; stmt->next; stmt = stmt->next)
				;
			stmt->next = $2;
		}
		else {
			$1->next = $2;
		}

		// check if two labels have the same name
		if (($1->label > 0) && (irLabelExist($2, $1->label) > 0)) {
			irerror("two labels have the same name");
		}

		$$ = $1;
	}
	| module stmtlist {
		if ($1->next) {
			IrStmt stmt;
			// FIXME: error will only be reported at the end of file.
			for (stmt = $1; stmt->next; stmt = stmt->next) {
				if (stmt->label > 0 && irLabelExist($2, stmt->label) > 0)
					irerror("two labels have the same name");
			}
			if (stmt->label > 0 && irLabelExist($2, stmt->label) > 0)
				irerror("two labels have the same name");
			stmt->next = $2;
		}
		else {
			if ($1->label > 0 && irLabelExist($2, $1->label) > 0)
				irerror("two labels have the same name");
			$1->next = $2;
		}

		$$ = $1;	
	}
	;

lblstmt: IDENT ':' stmt {
		$3->label = $1;
		$$ = $3;
	}
	| stmt {
		$$ = $1;
	}
	;

stmt: rawtrans label optboolexpr {
		$1->expr = $3;
		$$ = irStmtCreate(IR_RAWTRANS, $1->from_stack, $1, NULL);
		
		called_module = NULL;
	}
	| ifstmt optsemicolon {
		$$ = irStmtCreate(IR_IF, 0, $1, NULL);
	}
	| dostmt optsemicolon {
		$$ = irStmtCreate(IR_DO, 0, $1, NULL);
	}
	| action ';' { $$ = $1; }
	| assgnlist ';' {
		$$ = irStmtCreate(IR_ASSGN, 0, $1, NULL);
		if (shared_stmt) {
			$$->shared = 1;
			shared_stmt = 0;
		}
	}
	/*| module { $$ = $1; }*/
	;

optsemicolon: /* empty */
	| ';'
	;

rawtrans: IDENT '<' IDENT '>' ARROW IDENT '<' IDENT IDENT '>' {
		$$ = irRuleCreate($1, $3, $6, $8, $9);
		
		IrModule module;
		for (module = ir_module; module; module = module->next) {
			if (module->name != $8) continue;
			called_module = module;
			break;
		}
	}
	| IDENT '<' IDENT '>' ARROW IDENT '<' IDENT '>' {
		$$ = irRuleCreate($1, $3, $6, $8, 0);
	}
	| IDENT '<' IDENT '>' ARROW IDENT '<' '>' {
		$$ = irRuleCreate($1, $3, $6, 0, 0);
	}
	;

label: /* empty */ { $$ = NULL; }
	| LABEL { $$ = $1; }
	;

ifstmt: IF clnstmtlist FI {
		if (!$2) irerror("empty if statement");
		//if (!$2->next && $2->cond->token == IR_ELSE)
		//	irerror("else without other statments");
		$$ = $2;
		else_cnt = 0;
	}
	;

dostmt: DO clnstmtlist OD {
		if (!$2) irerror("empty do statement");
		//if (!$2->next && $2->cond->token == IR_ELSE)
		//	irerror("else without other statments");
		$$ = $2;
		else_cnt = 0;
	}
	;

clnstmtlist: /* empty */ {
		$$ = NULL;
	}
	| clnstmt clnstmtlist {
		$1->next = $2;
		$$ = $1;
	}
	;

clnstmt: DOUBLECOLON boolexpr CONDARROW stmtlist {
		if (!$4) irerror("invalid statement");
		// handles the case ":: cond -> x: ...;" by adding "skip"
		if ($4->label) {
			IrStmt s = irStmtCreate(IR_SKIP, 0, NULL, NULL);
			s->next = $4;
			$4 = s;
		}
		$$ = irIfdoCreate($2, $4, NULL);
	}
	| DOUBLECOLON ELSE CONDARROW stmtlist {
		if (!$4) irerror("invalid statement");
		if (else_cnt) irerror("second else found");
		else_cnt++;
		// handles the case ":: cond -> x: ...;" by adding "skip"
		if ($4->label) {
			IrStmt s = irStmtCreate(IR_SKIP, 0, NULL, NULL);
			s->next = $4;
			$4 = s;
		}
		$$ = irIfdoCreate(irFnodeCreate(IR_ELSE, 0 ,NULL, NULL), $4, NULL);
	}
	;

boolexpr: '(' boolexpr ')' { $$ = $2; }
	| boolexpr PDS_LGC_AND boolexpr {
		$$ = irFnodeCreate(IR_LGC_AND, 0, $1, $3);
	}
	| boolexpr PDS_LGC_OR boolexpr {
		$$ = irFnodeCreate(IR_LGC_OR, 0, $1, $3);
	}
	| boolexpr PDS_LGC_XOR boolexpr {
		$$ = irFnodeCreate(IR_LGC_XOR, 0, $1, $3);
	}
	| boolexpr PDS_EQV boolexpr {
		$$ = irFnodeCreate(IR_LGC_EQV, 0, $1, $3);
	}
	| boolexpr PDS_CMPEQ boolexpr {
		$$ = irFnodeCreate(IR_CMPEQ, 0, $1, $3);
	}
	| PDS_LGC_NOT boolexpr {
		$$ = irFnodeCreate(IR_LGC_NOT, 0, NULL, $2);
	}
	| PDS_FORALL quant boolexpr {
		$$ = irFnodeCreate(IR_FORALL, 0, $2, $3);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	}
	| PDS_EXISTS quant boolexpr {
		$$ = irFnodeCreate(IR_EXISTS, 0, $2, $3);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	}
	| structbvaroptidx { $$ = $1; }
	| intcomp { $$ = $1; }
	| TOK_TRUE {
		$$ = irFnodeCreate(IR_TRUE, 0, NULL, NULL);
	}
	| TOK_FALSE {
		$$ = irFnodeCreate(IR_FALSE, 0, NULL, NULL);
	}
	;

quant: IDENT '(' cnstexpr ',' cnstexpr ')' {
		if ($3 > $5) irerror("invalid integer range");
		$$ = irNamedFnodeCreate(IR_QUANT, $1, 0
			, irFnodeCreate(IR_NUMBER, $3, NULL, NULL)
			, irFnodeCreate(IR_NUMBER, $5, NULL, NULL)
		);
		IrQvar qvar = irQvarCreate($1, $3, $5);
		qvar->next = ir_qvar;
		ir_qvar = qvar;
	}
	;

intcomp: intexpr '<' intexpr {
		$$ = irFnodeCreate(IR_LT, 0, $1, $3);
	}
	| intexpr PDS_LE intexpr {
		$$ = irFnodeCreate(IR_LE, 0, $1, $3);
	}
	| intexpr PDS_CMPEQ intexpr {
		$$ = irFnodeCreate(IR_EQ, 0, $1, $3);
	}
	| intexpr PDS_NE intexpr {
		$$ = irFnodeCreate(IR_NE, 0, $1, $3);
	}
	| intexpr PDS_GE intexpr {
		$$ = irFnodeCreate(IR_GE, 0, $1, $3);
	}
	| intexpr '>' intexpr {
		$$ = irFnodeCreate(IR_GT, 0, $1, $3);
	}
	;

intexpr: '(' intexpr ')' {
		$$ = $2;
	}
	| intexpr PDS_PLUS intexpr {
		$$ = irFnodeCreate(IR_PLUS, 0, $1, $3);
	}
	| intexpr PDS_MINUS intexpr {
		$$ = irFnodeCreate(IR_MINUS, 0, $1, $3);
	}
	| intexpr PDS_MULT intexpr {
		$$ = irFnodeCreate(IR_MULT, 0, $1, $3);
	}
	| intexpr PDS_DIV intexpr {
		$$ = irFnodeCreate(IR_DIV, 0, $1, $3);
	}
	| intexpr PDS_SHL intexpr {
		$$ = irFnodeCreate(IR_SHL, 0, $1, $3);
	}
	| intexpr PDS_SHR intexpr {
		$$ = irFnodeCreate(IR_SHR, 0, $1, $3);
	}
	| intexpr PDS_AND intexpr {
		$$ = irFnodeCreate(IR_AND, 0, $1, $3);
	}
	| intexpr PDS_OR intexpr {
		$$ = irFnodeCreate(IR_OR, 0, $1, $3);
	}
	| intexpr PDS_XOR intexpr {
		$$ = irFnodeCreate(IR_XOR, 0, $1, $3);
	}
	| structvaroptidx {
		$$ = $1;
	}
	| QVAR {
		$$ = irNamedFnodeCreate(IR_QVAR, $1->name, 0, NULL, NULL);
	}
	| NUMBER {
		$$ = irFnodeCreate(IR_NUMBER, $1, NULL, NULL);
	}
	;
	
structvaroptidx: varoptidx { 
		IrVar v = irVarGetUnformatted($1->name, current_module, called_module);
		if (!v) irerror("unknown variable");

		if ($1->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if ($1->right && (!v->dim || (v->dim && v->dim->next)))
				irerror("array dimension mismatched");
		}

		if ($1->left) 
			$1->right = irTwoDimIndex2Fnode(v, $1->left, $1->right);
		else if ($1->right)
			$1->right = irFnodeCreate(IR_INDEX, 0, NULL, $1->right);

		$1->left = NULL;
		$$ = $1;
	}
	| varoptidx '.' varoptidx {
		wIdent n = irNameStructCreate($1->name, $3->name);
		IrVar v = irVarGet(n, current_module, called_module);
		if (!v) irerror("unknown struct variable");

		if ($1->right && $3->right)
			irerror("invalid array index");

		if ($1->left || $3->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if (($1->right || $1->right) 
					&& (!v->dim || (v->dim && v->dim->next))) 
				irerror("array dimension mismatched");
		}

		IrFnode idx = NULL;
		if ($1->right && $3->right)
			irerror("invalid array index");
		if ($1->left) 
			idx = irTwoDimIndex2Fnode(v, $1->left, $1->right);
		else if ($1->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, $1->right);
		else if ($3->left) 
			idx = irTwoDimIndex2Fnode(v, $3->left, $3->right);
		else if ($3->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, $3->right);

		$$ = irNamedFnodeCreate(
			IR_IDENT, n, $3->value, NULL, NULL);
		$$->right = irFnodeCopy(idx);
		irFnodeFree($1);
		irFnodeFree($3);
	}
	;

varoptidx: varoptprime {
		$$ = $1;
	}
	| varoptprime '[' intexpr ']' {
		$1->right = $3;
		$$ = $1;
	}
	| varoptprime '[' intexpr ']' '[' intexpr ']' {
		$1->left = $3;
		$1->right = $6;
		$$ = $1;
	}
	;
	
varoptprime: IDENT {
		$$ = irNamedFnodeCreate(IR_IDENT, $1, 0, NULL, NULL);
		last_var = $1;
	}
	| IDENT PRIME {
		$$ = irNamedFnodeCreate(IR_IDENT, $1, 1, NULL, NULL);
		last_var = $1;
	}
	| IDENT PRIME PRIME {
		$$ = irNamedFnodeCreate(IR_IDENT, $1, 2, NULL, NULL);
		last_var = $1;
	}
	;
	
structbvaroptidx: bvaroptidx { 
		IrVar v = irVarGetUnformatted($1->name, current_module, called_module);
		if (!v) irerror("unknown variable");

		if ($1->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if ($1->right && (!v->dim || (v->dim && v->dim->next)))
				irerror("array dimension mismatched");
		}	

		if ($1->left) 
			$1->right = irTwoDimIndex2Fnode(v, $1->left, $1->right);
		else if ($1->right)
			$1->right = irFnodeCreate(IR_INDEX, 0, NULL, $1->right);

		$1->left = NULL;
		$$ = $1;
	}
	| varoptidx '.' bvaroptidx {
		wIdent n = irNameStructCreate($1->name, $3->name);
		IrVar v = irVarGet(n, current_module, called_module);
		if (!v) irerror("unknown struct variable");

		if ($1->right && $3->right)
			irerror("invalid array index");
		if ($1->left || $3->left) {
			if (!v->dim || !v->dim->next)
				irerror("array dimension mismatched");
		} else {
			if (($1->right || $1->right) 
					&& (!v->dim || (v->dim && v->dim->next))) 
				irerror("array dimension mismatched");
		}	

		IrFnode idx = NULL;
		if ($1->right && $3->right)
			irerror("invalid array index");
		if ($1->left) 
			idx = irTwoDimIndex2Fnode(v, $1->left, $1->right);
		else if ($1->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, $1->right);
		else if ($3->left) 
			idx = irTwoDimIndex2Fnode(v, $3->left, $3->right);
		else if ($3->right) 
			idx = irFnodeCreate(IR_INDEX, 0, NULL, $3->right);

		$$ = irNamedFnodeCreate(
			IR_BVAR, n, $3->value, NULL, NULL);
		$$->right = irFnodeCopy(idx);
		irFnodeFree($1);
		irFnodeFree($3);
	}
	;

bvaroptidx: bvaroptprime {
		$$ = $1;
	}
	| bvaroptprime '[' intexpr ']' {
		$1->right = $3;
		$$ = $1;
	}
	| bvaroptprime '[' intexpr ']'  '[' intexpr ']' {
		$1->left = $3;
		$1->right = $6;
		$$ = $1;
	}
	;

bvaroptprime: BVAR {
		$$ = irNamedFnodeCreate(IR_BVAR, $1, 0, NULL, NULL);
	}
	| BVAR PRIME {
		$$ = irNamedFnodeCreate(IR_BVAR, $1, 1, NULL, NULL);
	}
	| BVAR PRIME PRIME {
		$$ = irNamedFnodeCreate(IR_BVAR, $1, 2, NULL, NULL);
	}
	;

cnstexpr: cnstexpr PDS_PLUS cnstexpr { $$ = $1 + $3; }
	| cnstexpr PDS_MINUS cnstexpr { $$ = $1 - $3; }
	| cnstexpr PDS_MULT cnstexpr { $$ = $1 * $3; }
	| cnstexpr PDS_DIV cnstexpr { 
		if ($3 == 0) {
			irerror("division by zero");
		}
		$$ = $1 / $3; 
	}
	| cnstexpr PDS_SHL cnstexpr { $$ = $1 << $3; }
	| cnstexpr PDS_SHR cnstexpr { $$ = $1 >> $3; }
	| cnstexpr PDS_AND cnstexpr { $$ = $1 & $3; }
	| cnstexpr PDS_OR cnstexpr { $$ = $1 | $3; }
	| cnstexpr PDS_XOR cnstexpr { $$ = $1 ^ $3; }
	| '(' cnstexpr ')' { $$ = $2; }
	| NUMBER { $$ = $1; }
	; 

expr: boolexpr { $$ = $1; }
	| intexpr { $$ = $1; }
	;

action: callstmt optassgnlist {
		irUpdateCallExprList($1, $2);
		if (shared_stmt) {
			$1->shared = 1;
			shared_stmt = 0;
		}
		$$ = $1;
		called_module = NULL;
	}
	| structvaroptidx PDS_EQ callstmt optassgnlist {
		irUpdateCallExprList($3, $4);
		if (shared_stmt) {
			$3->shared = 1;
			shared_stmt = 0;
		}

		char *msg = 0;
		IrStmt stmt = irStmtCreate(IR_ASSGN, 0, NULL, NULL); 
		stmt->content = irCreateRetExprList(2, called_module, 
			current_module, $1, &msg);
		if (msg) irerror(msg);

		if (isSharedGlobal($1->name)) stmt->shared = 1;
		$3->next = stmt;
		$$ = $3;
		called_module = NULL;
	}
	| structbvaroptidx PDS_EQ callstmt optassgnlist {
		irUpdateCallExprList($3, $4);
		if (shared_stmt) {
			$3->shared = 1;
			shared_stmt = 0;
		}

		char *msg = 0;
		IrStmt stmt = irStmtCreate(IR_ASSGN, 0, NULL, NULL);
		stmt->content = irCreateRetExprList(2, called_module, 
			current_module, $1, &msg);
		if (msg) irerror(msg);

		if (isSharedGlobal($1->name)) stmt->shared = 1;
		$3->next = stmt;
		$$ = $3;
		called_module = NULL;
	}
	| retstmt { 
		if (!current_module) irerror("return must be in module");
		$$ = $1;
	}
	| GOTO IDENT optassgnlist {
		$$ = irStmtCreate(IR_GOTO, 0, irGotoCreate($2, $3), NULL);
		if (shared_stmt) {
			$$->shared = 1;
			shared_stmt = 0;
		}
	}
	| BREAK {
		$$ = irStmtCreate(IR_BREAK, 0, NULL, NULL);
	}
	| SKIP optboolexpr {
		$$ = irStmtCreate(IR_SKIP, 0, $2, NULL);
	}
	| FORK callstmt {
		wIdent midName = irNameStackFresh();
		IrCall forked = (IrCall) $2->content;
		
		IrExprList forkedParam = irExprListCopy(forked->params);
		IrExprList paramPtr = forkedParam;
		IrFnode param;
		for (; paramPtr; paramPtr = paramPtr->next) {
			param = paramPtr->expr;
			if ((param->token == IR_EQ) || (param->token == IR_CMPEQ)) {
				irFnodeFree(param->right);
				param->right = irNamedFnodeCreate(
					IR_IDENT, param->left->name, 0, NULL, NULL);
			} else if (param->token == IR_FORALL) {
				param->right->right->name = param->right->left->name;
			} else {
				irerror("unexpected parameter");
			}
		}
		
		IrFork irfork = irForkCreate(midName, forked);
			
		IrCall midCall = irCallCreate(forked->module, forkedParam);
		IrStmt midStmt = irStmtCreate(IR_CALL, midName, midCall, NULL);
		IrModule midModule = irModuleCopy(forked->module);
		midModule->name = midName;
		midModule->first_stmt = midStmt;
		midModule->last_stmt = midStmt;
		midModule->next = ir_module;
		ir_module = midModule;
		
		$$ = irStmtCreate(IR_FORK, 0, irfork, NULL);
	}
	;

callstmt: IDENT '(' args ')' {
		IrModule m;
		for (m = ir_module; m; m = m->next) 
			if (m->name == $1) break;
		if (!m) irerror("module not found");
		
		called_module = m;
		char *msg = 0;
		$$ = irCreateCallStmt(current_module, m, $3, &msg);
		if (msg) irerror(msg);
		irExprlistFree($3);
	}
	;

args: /* empty */ { $$ = NULL; }
	| argslist { $$ = $1; }
	;

argslist: exproptundef {
		$$ = irExprListCreate($1, NULL);
	}
	| exproptundef ',' argslist {
		$$ = irExprListCreate($1, $3);
	}
	; 

exproptundef: expr {
		$$ = $1;
	}
	| UNDEF {
		$$ = NULL;
	}
	;

retstmt: RETURN {
		if (current_module->retvar->type != IR_VOID)
			irerror("invalid return type");
		$$ = irStmtCreate(IR_RET, 0, NULL, NULL);
	}
	| RETURN boolexpr {
		if (current_module->retvar->type != IR_BOOL)
			irerror("invalid return type");
		$$ = irStmtCreate(IR_RET, 0, NULL, NULL);
		char *msg = 0;
		$$->content = irCreateRetExprList(1, current_module, 
			current_module, $2, &msg);
		if (msg) irerror(msg);
	}
	| RETURN intexpr {
		if (current_module->retvar->type == IR_VOID)
			irerror("invalid return type");
		$$ = irStmtCreate(IR_RET, 0, NULL, NULL);
		char *msg = 0;
		$$->content = irCreateRetExprList(1, current_module, 
			current_module, $2, &msg);
		if (msg) irerror(msg);	
	}
	;

optassgnlist: /* empty */ {
		$$ = NULL;
	}
	| ',' assgnlist {
		$$ = $2;
	}
	;
	
assgnlist: assgn {
		$$ = $1;
	}
	|  assgn ',' assgnlist {
		IrExprList exprlist;
		for (exprlist = $1; exprlist->next; exprlist = exprlist->next)
			;
		exprlist->next = $3;
		$$ = $1;
	}
	;

assgn: structvaroptidx PDS_EQ intexpr {
		
		/* aray = array */
		IrVar lvar = irVarGetUnformatted($1->name, current_module, called_module);
		IrVar rvar = NULL;
		if ($3->token == IR_IDENT) 
			rvar = irVarGetUnformatted($3->name, current_module, called_module);

		if (lvar && lvar->dim && !$1->right) {

			if (!rvar || !rvar->dim || $3->right 
					|| irDimEquals(lvar->dim, rvar->dim) == 0)
				irerror("invalid array assignment");

			int i, j;
			irFlattenDim(lvar->dim, &i, &j);
			IrFnode eqnode;
			IrExprList exprlist = NULL;
			if (lvar->type == IR_STRUCT) {
				IrVar slvar = lvar->extra;
				IrVar srvar = rvar->extra;
				while(slvar) {
					int type, op;
					if (slvar->type == IR_BOOL) {
						type = IR_BVAR;
						op = IR_CMPEQ;
					} else {
						type = IR_IDENT;
						op = IR_EQ;
					}
					eqnode = irFnodeAllAssgn(type, op, slvar->name, srvar->name, i, j);
					exprlist = irExprListCreate(eqnode, exprlist);
					slvar = slvar->extra;
					srvar = srvar->extra;
				}
			} else {
				eqnode = irFnodeAllAssgn(IR_IDENT, IR_EQ, lvar->name, rvar->name, i, j);
				exprlist = irExprListCreate(eqnode, NULL);
			}
			$$ = exprlist;
		} 

		/* struct = struct */
		else if (lvar && lvar->type == IR_STRUCT && $3->token == IR_IDENT) {
			if (rvar && rvar->type == IR_STRUCT) {

				if (lvar->type_ptr != rvar->type_ptr)
					irerror("struct type mismatched");

				IrVar slvar = lvar->extra;
				IrVar srvar = rvar->extra;
				IrFnode eqnode;
				IrExprList exprlist = NULL;
				while(slvar) {
					int type, op;
					if (slvar->type == IR_BOOL) {
						type = IR_BVAR;
						op = IR_CMPEQ;
					} else {
						type = IR_IDENT;
						op = IR_EQ;
					}
					if (slvar->dim) {
						int i, j;
						irFlattenDim(slvar->dim, &i, &j);
						eqnode = irFnodeAllAssgn(type, op, slvar->name, srvar->name, i, j);
					} else {
						IrFnode lnode = irNamedFnodeCreate(type, slvar->name, 1, NULL, NULL);
						IrFnode rnode = irNamedFnodeCreate(type, srvar->name, 0, NULL, NULL);
						eqnode = irFnodeCreate(op, 0, lnode, rnode);
					}
					exprlist = irExprListCreate(eqnode, exprlist);
					slvar = slvar->extra;
					srvar = srvar->extra;
				}

				irFnodeFree($1);
				irFnodeFree($3);
				$$ = exprlist;
			}
		} else {
			$1->value = 1;
			$$ = irExprListCreate(irFnodeCreate(IR_EQ, 0, $1, $3), NULL);
			if (isSharedGlobal($1->name)) shared_stmt = 1;
		}
	}
	| structbvaroptidx PDS_EQ boolexpr {

		/* aray = array */
		IrVar lvar = irVarGetUnformatted($1->name, current_module, called_module);
		if (lvar && lvar->dim && !$1->right && $3->token == IR_BVAR) {
			IrVar rvar = irVarGetUnformatted($3->name, current_module, called_module);

			if (!rvar->dim || $3->right 
					|| irDimEquals(lvar->dim, rvar->dim) == 0)
				irerror("invalid array assignment");

			int i, j;
			irFlattenDim(lvar->dim, &i, &j);
			IrFnode eqnode = irFnodeAllAssgn(IR_BVAR, IR_CMPEQ, lvar->name, rvar->name, i, j);
			$$ = irExprListCreate(eqnode, NULL);
		} else {
			$1->value = 1;
			$$ = irExprListCreate(irFnodeCreate(IR_CMPEQ, 0, $1, $3), NULL);
			if (isSharedGlobal($1->name)) shared_stmt = 1;
		}
	}
	| structvaroptidx PDS_EQ UNDEF {
		IrVar var = irVarGetUnformatted($1->name, current_module, called_module);
		if (!var) var = irVarGet($1->name, current_module, called_module);

		/* struct = undef */
		if (var && var->type == IR_STRUCT) {
			IrVar sv;
			IrFnode vnode, eq;
			IrExprList exprlist = NULL;
			for (sv = var->extra; sv; sv = sv->extra) {
				vnode = irNamedFnodeCreate(IR_IDENT, sv->name, 1, 
					irFnodeCopy($1->left),
					irFnodeCopy($1->right));
				if (sv->type == IR_ENUM) {
					int size = ((IrEnum) sv->type_ptr)->size;
					if (sv->dim) {
						int i,j;
						irFlattenDim(sv->dim, &i, &j);
						eq = irFnodeAllLt(sv->name, i, j, size);
					} else {
						IrFnode snode = irFnodeCreate(IR_NUMBER, size, NULL, NULL);
						eq = irFnodeCreate(IR_LT, 0, vnode, snode);
					}
				} else {
					int token = (sv->type == IR_BOOL) ? IR_CMPEQ : IR_EQ;
					eq = irFnodeCreate(token, 0, vnode, NULL);
				}
				exprlist = irExprListCreate(eq, exprlist);
			}
			$$ = exprlist;
			if (isSharedGlobal($1->name)) shared_stmt = 1;
			irFnodeFree($1);
		} /* enum = undef */
		else if (var && var->type == IR_ENUM) {
			int size = ((IrEnum) var->type_ptr)->size;
			IrFnode fnode;
			if (var->dim) {
				int i,j;
				irFlattenDim(var->dim, &i, &j);
				fnode = irFnodeAllLt(var->name, i, j, size);
				irFnodeFree($1);
			} else {
				IrFnode snode = irFnodeCreate(IR_NUMBER, size, NULL, NULL);
				$1->value = 1;
				fnode = irFnodeCreate(IR_LT, 0, $1, snode);
			}
			$$ = irExprListCreate(fnode, NULL);
		} else {
			$1->value = 1;
			$$ = irExprListCreate(irFnodeCreate(IR_EQ, 0, $1, NULL), NULL);
			if (isSharedGlobal($1->name)) shared_stmt = 1;
		}
	}
	| structbvaroptidx PDS_EQ UNDEF {
		$1->value = 1;
		$$ = irExprListCreate(irFnodeCreate(IR_CMPEQ, 0, $1, NULL), NULL);
		if (isSharedGlobal($1->name)) shared_stmt = 1;
	}
	| structvaroptidx PDS_NE intexpr {
		$1->value = 1;
		$$ = irExprListCreate(irFnodeCreate(IR_NE, 0, $1, $3), NULL);
		if (isSharedGlobal($1->name)) shared_stmt = 1;
	}
	| PDS_FORALL quant assgn {
		if ($3->next) irerror("parallel assignment inside quantification");
		$$ = irExprListCreate(irFnodeCreate(IR_FORALL, 0, $2, $3->expr), NULL);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	}
	| PDS_EXISTS quant assgn {
		if ($3->next) irerror("parallel assignment inside quantification");
		$$ = irExprListCreate(irFnodeCreate(IR_EXISTS, 0, $2, $3->expr), NULL);
		IrQvar qvar = ir_qvar;
		ir_qvar = ir_qvar->next;
		Free(qvar);
	}
	;

module: modulehead '{' localdecls stmtlist '}' {
		if ($4) {
			if ($4->label && ($4->label != $1->name)) {
				$4 = irStmtCreate(IR_SKIP, 0, NULL, $4);
			}
			$4->label = $1->name;
			$1->first_stmt = $4;
			IrStmt last_stmt;
			for (last_stmt = $4; last_stmt->next; last_stmt = last_stmt->next)
				;
			
			/* implicit return */
			if ((current_module->retvar->type == IR_VOID)
					&& (last_stmt->type != IR_RET)) {
				IrStmt ret_stmt = irStmtCreate(IR_RET, 0, NULL, NULL);
				last_stmt->next = ret_stmt;
				last_stmt = ret_stmt;
			}
			$1->last_stmt = last_stmt;

			/* if the last statement is not a return statment*/
			if (!irIsReturnInside(last_stmt)) {
				irerror("return statement expected");
			}
		} else if (current_module->retvar->type == IR_VOID) {
			$4 = irStmtCreate(IR_RET, $1->name, NULL, NULL);
			$1->first_stmt = $4;
			$1->last_stmt = $4;
		}
		$$ = $4;
		
		current_module = NULL; //current_module->next;
	}
	;

modulehead: MODULE rettype IDENT '(' params ')' {
		IrModule module;
		
		for (module = ir_module; module; module = module->next) {
			if (module->name == $3)
				break;
		}
		
		if (!module) {
			module = irModuleCreate($3, $2, $5);
			module->next = ir_module;
			ir_module = module;
		}
		$$ = module;
		
		current_module = module;
	}
	;

localdecls: /* empty */
	| localdecls vardecl {
		IrVarList vl;
		for (vl = $2; vl->next; vl = vl->next) 
			;	
		vl->next = current_module->localvar;
		current_module->localvar = $2;
	  }
	/* Local type declaration is not possible (for now) */
/*	| localdecls typedecl*/
	;

rettype: VOID { 
		$$ = irVarTypeCreate(IR_VOID);
	}
	| BOOL optdim { 
		$$ = irVarTypeCreate(IR_BOOL);
		$$->dim = $2;
	}
	| INT optdim optbit { 
		$$ = irVarTypeCreate(IR_INT);
		$$->dim = $2;
		if ($3) $$->bits = $3;
		else if (int_bits) $$->bits = int_bits;
		else irerror("unspecified integer range");
	}
	| SGNINT optdim optbit { 
		irerror("current implementation does not support signed int");
		$$ = irVarTypeCreate(IR_SGNINT);
		$$->dim = $2;
		if ($3) $$->bits = $3; 
		else if (sgnint_bits) $$->bits = sgnint_bits;
		else irerror("unspecified integer range");
	}
	| ENUM IDENT optdim { 
		$$ = irVarTypeCreate(IR_ENUM);
		IrEnum e;
		for (e = ir_enum; e; e = e->next)
			if (e->name == $2)
				break;
		if (!e)
			irerror("invalid enum type");
		
		$$->type_ptr = e;
		$$->bits = e->bits;
		$$->dim = $3;
	}
	| STRUCT IDENT optdim { 
		$$ = irVarTypeCreate(IR_STRUCT);
		IrStruct s;
		for (s = ir_struct; s; s = s->next)
			if (s->name == $2)
				break;
		if (s)
			$$->type_ptr = s;
		else
			irerror("invalid struct type");

		if ($3) {
			IrVarList f = s->fields;
			for (; f; f = f->next)
				if (f->var->dim)
					irerror("array of struct of array not allowed");
			$$->dim = $3;
		}
	}
	;

params: /* empty */ { $$ = NULL; }
	| paramlist { $$ = $1; }
	;

paramlist: param {
		$$ = irVarListCreate($1, NULL);
	}
	| param ',' paramlist {
		$$ = irVarListCreate($1, $3);
	}
	;

param: BOOL varident {
		if (irVarGetUnformatted($2->name, NULL, NULL))
			irerror("two variables have the same name");
		$2->type = IR_BOOL;
		$$ = $2;
	}
	| INT varident {
		if (irVarGetUnformatted($2->name, NULL, NULL))
			irerror("two variables have the same name");
		$2->type = IR_INT;
		if (!$2->bits) {
			if (!int_bits)
				irerror("unspecified integer range");
			else
				$2->bits = int_bits;
		}
		$$ = $2;
	}
	| SGNINT varident {
		irerror("current implementation does not support signed int");
		if (irVarGetUnformatted($2->name, NULL, NULL))
			irerror("two variables have the same name");
		$2->type = IR_SGNINT;
		if (!$2->bits) {
			if (!sgnint_bits)
				irerror("unspecified integer range");
			else
				$2->bits = sgnint_bits;
		}
		$$ = $2;
	}
	| ENUM IDENT varident {
		if (irVarGetUnformatted($3->name, NULL, NULL))
			irerror("two variables have the same name");
		$3->type = IR_ENUM;
		IrEnum e;
		for (e = ir_enum; e; e = e->next)
			if (e->name == $2)
				break;
		if (!e)
			irerror("invalid enum type");
		else
			$3->type_ptr = e;
		$$ = $3;
	}
	| STRUCT IDENT varident {
		if (irVarGetUnformatted($3->name, NULL, NULL))
			irerror("two variables have the same name");
		$3->type = IR_STRUCT;
		IrStruct s;
		for (s = ir_struct; s; s = s->next)
			if (s->name == $2)
				break;
		if (!s)
			irerror("invalid struct type");
		else
			$3->type_ptr = s;
		
		irAddStructVar($3);
		$$ = $3;
	}
	;

%%

#include "irLex.c"

void irerror (char *info) { 
	printf("\nsyntax error in Remopla: %s, line %d\n",
		info, irlinenumber);
	irFree();
	finish_ir();
	exit(1);
}

void irParse (char *filename) {
	if (!(irin = fopen(filename,"r"))) {
		fprintf(stderr,"modelfile %s not found\n", filename);
		exit(1);
	}

	if (irparse())
		printf("parse errors encountered while reading input\n");

	fclose(irin);
}
