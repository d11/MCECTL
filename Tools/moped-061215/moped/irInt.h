#ifndef IRINT_H
#define IRINT_H

/* Internal definitions of the IR module */

#include <wpds.h>
#include "int.h"

/* General (previously ir.h) */
typedef struct IrDefStruct *IrDef;
typedef struct IrVarStruct *IrVar;
typedef struct IrVarListStruct *IrVarList;
typedef struct IrIndVarStruct *IrIndVar;
typedef struct IrVarListStruct *IrGbl;
typedef struct IrEnumStruct *IrEnum;
typedef struct IrStructStruct *IrStruct;
typedef struct IrQvarStruct *IrQvar;
typedef struct IrInitStruct *IrInit;
typedef struct IrFnodeStruct *IrFnode;
typedef struct IrStmtStruct *IrStmt;
typedef struct IrIfdoStruct *IrIfdo;
typedef struct IrCallStruct *IrCall;
typedef struct IrExprListStruct *IrExprList;
typedef struct IrRuleStruct *IrRule;
typedef struct IrModuleStruct *IrModule;
typedef struct IrNameStruct *IrName;
typedef struct IrDimStruct *IrDim;
typedef struct IrGotoStruct *IrGoto;
typedef struct IrForkStruct *IrFork;

extern IrDef ir_def;
extern IrEnum ir_enum;
extern IrStruct ir_struct;
extern IrGbl ir_gbl;
extern IrGbl ir_sgbl;
extern IrGbl ir_ret;
extern IrQvar ir_qvar;
extern IrInit ir_init;
extern IrStmt ir_stmt;
extern IrRule ir_rule;
extern IrModule ir_module;

struct IrCallStruct {
	IrModule module;
	IrExprList params;
};

struct IrDefStruct {
	wIdent name;
	int value;
	IrDef next;
};

struct IrDimStruct {
	int len;
	int lo;
	int hi;
	IrDim next;
};

struct IrEnumStruct {
	wIdent name;
	IrName items;
	int bits;
	int size;
	IrEnum next;
};

struct IrExprListStruct {
	IrFnode expr;
	IrExprList next;
};

struct IrFnodeStruct {
	int token;
	wIdent name;
	int value;
	IrFnode left;
	IrFnode right;
};

struct IrForkStruct {
	wIdent middle;
	IrCall forked;
};

struct IrGotoStruct {
	wIdent target;
	IrExprList expr;
};

struct IrIfdoStruct {
	IrFnode cond;
	IrStmt stmt;
	IrIfdo next;
};

struct IrIndVarStruct {
	IrVar var;
	IrFnode ind;
	int scope;	//1 = global, 2 = local
	IrIndVar next;
};

struct IrInitStruct {
	wIdent state;
	IrFnode state_fnode;
	IrName stack;
	IrFnode stack_fnode;
};

struct IrModuleStruct {
	wIdent name;
	IrVar retvar;
	IrVarList param;
	IrVarList localvar;
	IrStmt first_stmt;
	IrStmt last_stmt;
	IrModule next;
};

struct IrNameStruct {
	wIdent name;
	IrName next;
};

struct IrQvarStruct {
	wIdent name;
	int lower, upper;
	IrQvar next;
};

struct IrRuleStruct {
	wIdent from_state;
	wIdent from_stack;
	wIdent to_state;
	wIdent to_stack1;
	wIdent to_stack2;
	IrFnode expr;
	char *label;
};

struct IrStmtStruct {
	int type;
	wIdent label;
	void *content;
	int shared;
	IrStmt next;
};

struct IrStructStruct {
	wIdent name;
	IrVarList fields;
	IrStruct next;
};

struct IrVarStruct {
	wIdent name;
	int type;
	void *type_ptr;
	IrDim dim;
	int bits;
	IrVar extra;	//for var type struct
};

struct IrVarListStruct {
	IrVar var;
	IrVarList next;
};

enum {
	IR_BOOL, IR_INT, IR_SGNINT, IR_ENUM, IR_STRUCT, IR_VOID
};

enum {
	IR_RAWTRANS, IR_IF, IR_DO, IR_ASSGN, IR_GOTO, IR_BREAK
	, IR_SKIP, IR_CALL, IR_RET, IR_FORK
};

extern char **type_array;
extern wIdent default_state_name;
extern wIdent default_return_name;
extern wIdent default_return_name_array;
extern wIdent default_return_name_bool;
extern wIdent default_return_name_bool_array;
extern wIdent default_quant_name;
extern wIdent q0;
extern wIdent q1;
extern wIdent q2;
extern wIdent q3;

extern void init_ir();
extern void finish_ir();
extern void reverse_ir_rule();
extern void irAddStructVar(IrVar);
extern void irFlattenDim(IrDim, int*, int*);
extern void irInsertRetVar(wIdent, int, IrDim, int);
extern IrStmt irCreateCallStmt(IrModule, IrModule, IrExprList, char**);
extern IrExprList irCreateRetExprList(int, IrModule, IrModule, IrFnode, char**);
extern IrEnum irCreateTypeEnum(wIdent, IrName);
extern int irDefValue(wIdent);
extern int irDimEquals(IrDim, IrDim);
extern IrFnode irFnodeAllAssgn(int, int, wIdent, wIdent, int, int);
extern IrFnode irFnodeAllLt(wIdent, int, int, int);
extern int irHasSameLabels(IrStmt, IrStmt);
extern int irLabelExist(IrStmt, wIdent);
extern int irIsReturnInside(IrStmt);
extern IrModule irModuleFind(wIdent);
extern int isSharedGlobal(wIdent);
extern void irLabelUpdate(IrStmt);
extern IrFnode irTwoDimIndex2Fnode(IrVar, IrFnode, IrFnode);
extern void irUpdateCallExprList(IrStmt, IrExprList);
extern IrVar irVarFind(wIdent, IrVarList);
extern IrVar irVarGet(wIdent, IrModule, IrModule);
extern IrVar irVarGetUnformatted(wIdent, IrModule, IrModule);
extern int irVarType(IrVarList, wIdent, wIdent);

/*** Reallocator ***/
extern IrDim irDimCopy(IrDim);
extern IrExprList irExprListCopy(IrExprList);
extern IrFnode irFnodeCopy(IrFnode);
extern IrModule irModuleCopy(IrModule);
extern IrVar irVarCopy(IrVar);
extern IrVarList irVarListCopy(IrVarList);

/*** Constructor ***/
extern IrCall irCallCreate(IrModule, IrExprList);
extern IrDef irDefCreate(wIdent, int, IrDef);
extern IrDim irDimCreate(int, int, int, IrDim);
extern IrEnum irEnumCreate(wIdent, IrName, int, int, IrEnum);
extern IrExprList irExprListCreate(IrFnode, IrExprList);
extern IrFnode irFnodeCreate(int, int, IrFnode, IrFnode);
extern IrFork irForkCreate(wIdent, IrCall);
extern IrGoto irGotoCreate(wIdent, IrExprList);
extern IrIfdo irIfdoCreate(IrFnode, IrStmt, IrIfdo);
extern IrInit irInitCreate(wIdent, IrFnode, IrName, IrFnode);
extern IrModule irModuleCreate(wIdent, IrVar, IrVarList);
extern IrName irNameCreate(wIdent, IrName);
extern IrFnode irNamedFnodeCreate(int, wIdent, int, IrFnode, IrFnode);
extern IrQvar irQvarCreate(wIdent, int, int);
extern IrRule irRuleCreate(wIdent, wIdent, wIdent, wIdent, wIdent);
extern IrStmt irStmtCreate(int, wIdent, void *, IrStmt);
extern IrStruct irStructCreate(wIdent, IrVarList, IrStruct);
extern IrVar irVarCreate(wIdent, IrDim, int);
extern IrVarList irVarListCreate(IrVar, IrVarList);
extern IrVar irVarTypeCreate(int);

/*** Destructor ***/
extern void irCallFree(IrCall);
extern void irDefFree(IrDef);
extern void irDimFree(IrDim);
extern void irEnumFree(IrEnum);
extern void irExprlistFree(IrExprList);
extern void irFnodeFree(IrFnode);
extern void irGotoFree(IrGoto);
extern void irIfdoFree(IrIfdo);
extern void irIndvarFree(IrIndVar);
extern void irInitFree(IrInit);
extern void irModuleFree(IrModule);
extern void irNameFree(IrName);
extern void irRuleFree(IrRule);
extern void irStmtFree(IrStmt);
extern void irStructFree(IrStruct);
extern void irVarExtraFree(IrVar);
extern void irVarFree(IrVar);
extern void irVarlistFree(IrVarList);
extern void irFree();

extern int is_arithmetic_op(int);
extern int is_bitwise_op(int);
extern int is_relational_op(int);
extern int is_logical_op(int);
extern int is_operator(int);
extern int is_intterm_op(int);
extern int is_var(int);
extern int is_num(int);
extern IrFnode find_fnode_lhs(IrFnode, wIdent);

enum {
	IR_NUMBER = 1
	, IR_IDENT
	, IR_BVAR
	, IR_QVAR
	, IR_QUANT		//its name is qvar name
				//left, right child are lower, upper bound
	, IR_TRUE
	, IR_FALSE
	, IR_INDEX	//right child of IR_IDENT and IR_BVAR
			//its right child is index expression
							
	, IR_ELSE		//no left nor right child
	
	, IR_FORALL	//left child is IR_QUANT
	, IR_EXISTS	//right child is quantified expression
	
	//***Assignment Operators***
	, IR_EQ	
	//***Arithmetic Operators***
	, IR_PLUS	
	, IR_MINUS
	, IR_MULT
	, IR_DIV
	//***Bitwise Operators***
	, IR_AND
	, IR_OR
	, IR_XOR
	, IR_SHL
	, IR_SHR
	//***Relational Operators***
	, IR_LT
	, IR_LE
	, IR_GE
	, IR_GT
	, IR_CMPEQ
	, IR_NE
	//***Logical Operators***
	, IR_LGC_AND
	, IR_LGC_OR
	, IR_LGC_XOR
	, IR_LGC_NOT
	, IR_LGC_EQV
};

/* irCon */
extern intPds* ir2int();

/* irName */
extern wIdent irNameStackFresh();
extern wIdent irNameCreateFresh();
extern wIdent irNameStructCreate();
extern wIdent irNameRetStructCreate();

/* irParse.y */
extern void irParse (char*);

/* irPrintx */
extern void irPrintInfo();
extern void printx_formula(IrFnode);
extern void printx_exprlist(IrExprList);

#endif
