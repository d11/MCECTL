#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"
#include "int.h"
#include "irInt.h"
#include "ir.h"

intVariable *globals_t;
intModule *modules_t;

intModule *current_module;
intModule *called_module;
intModule *next_module;

struct QntListStruct {
	intQuantifier *q;
	struct QntListStruct *next;
} *current_qnt = NULL, *copy_qnt = NULL;

struct IfdoListStruct {
	IrStmt stmt;
	wIdent end;
	int loop;	//if = 0, do = 1
	struct IfdoListStruct *next;
} *current_ifdo = NULL;

struct VarRefList {
	intVarRef *vref;
	struct VarRefList *next;
} *usedvar;

intExpr* irFnode2intExpr(IrFnode);
void intpds_transition_insert(intPds*, IrStmt, intExpr*); 

/************************************************************************/

intQuantifier *qnt_list_insert(struct QntListStruct **qnt_list,
	wIdent name, int lower, int upper) {
	
	struct QntListStruct *qnt = Malloc(sizeof(struct QntListStruct));
	qnt->q = intCreateQuantifier(name, lower, upper);
	qnt->next = *qnt_list;
	*qnt_list = qnt;
	
	return qnt->q;
}

void qnt_list_remove(struct QntListStruct **qnt_list) {
	
	struct QntListStruct *tmp =*qnt_list;
	*qnt_list = (*qnt_list)->next;
	Free(tmp);
	
	return;
}

void current_ifdo_insert(IrStmt stmt, wIdent end, int loop) {
	
	struct IfdoListStruct *ifdolist = Malloc(sizeof(struct IfdoListStruct));
	ifdolist->stmt = stmt;
	ifdolist->end = end;
	ifdolist->loop = loop;
	ifdolist->next = current_ifdo;
	current_ifdo = ifdolist;
	
	return;
}

void current_ifdo_remove() {
	
	struct IfdoListStruct *tmp = current_ifdo;
	current_ifdo = current_ifdo->next;
	Free(tmp);
	
	return;
}

struct VarRefList *varreflist_create(intVarRef *vref, struct VarRefList *next) {

	struct VarRefList *vrlist = Malloc(sizeof(struct VarRefList));
	vrlist->vref = vref;
	vrlist->next = next;
	
	return vrlist;
}

void varreflist_free(struct VarRefList *varref) {
	
	if (!varref) return;
	
	if (varref->next) varreflist_free(varref->next);
	Free(varref);
	
	return;
}

/************************************************************************/

intExpr* intexpr_copy(intExpr *expr) {
	
	if (!expr) return NULL;
	
	intExpr *result = Malloc(sizeof(struct intExpr));
	
	result->type = expr->type;
	switch(expr->type) {
		case EXPR_OR:
		case EXPR_AND:
		case EXPR_EQV:
		case EXPR_XOR:
		case EXPR_LT:
		case EXPR_LE:
		case EXPR_EQ:
		case EXPR_NE:
		case EXPR_GE:
		case EXPR_GT:
		case EXPR_PLUS:
		case EXPR_MINUS:
		case EXPR_MULT:
		case EXPR_DIV:
		case EXPR_SHL:
			result->left = intexpr_copy(expr->left);
			result->right = intexpr_copy(expr->right);
			break;
		case EXPR_NOT:
			result->left = intexpr_copy(expr->left);
			break;
		case EXPR_FORALL:
		case EXPR_EXISTS:
			result->q = qnt_list_insert(&copy_qnt, expr->q->name
				, expr->q->lower, expr->q->upper);
			result->left = intexpr_copy(expr->left);
			qnt_list_remove(&copy_qnt);
			break;
		case EXPR_VAR:
			Free(result);
			result = intExprVarRef(expr->vref->var, expr->vref->primes
				, expr->vref->index);
			break;
		case EXPR_QVAR: {
			/* Why did I do this??? All we have to do is to set
			   result->q, which we can get from expr->q ...*/
			/*struct QntListStruct *qnt;
			for (qnt = copy_qnt; qnt; qnt = qnt->next)
				if (qnt->q->name == expr->q->name)
					break;
			if (!qnt)
				return NULL;
			result->q = qnt->q;*/
			result->q = expr->q;
			break;
		}
		case EXPR_CONST:
			result->value = expr->value;
			break;
		default:
			fprintf(stderr, "unknown token in intexpr_copy (%d)\n"
				, expr->type);
			exit(1);
	}
	
	return result;
}

void intexpr_print(intExpr *expr) {
	
	if (!expr) return;
	
	switch(expr->type) {
		case EXPR_NOT:
			printf("!(");
			intexpr_print(expr->left);
			printf(")");
			return;
		case EXPR_FORALL:
			printf("(A %s (%d,%d) (", wIdentString(expr->q->name),
				expr->q->lower, expr->q->upper);
			intexpr_print(expr->left);
			printf("))");
			return;
		case EXPR_EXISTS:
			printf("(E %s (%d,%d) (", wIdentString(expr->q->name),
				expr->q->lower, expr->q->upper);
			intexpr_print(expr->left);
			printf("))");
			return;
		case EXPR_VAR:
			printf("%s", wIdentString(expr->vref->var->name));
			if (expr->vref->primes == 1) printf("'");
			if (expr->vref->primes == 2) printf("''");
			if (expr->vref->index) {
				printf("[");
				intexpr_print(expr->vref->index);
				printf("]");
			}
			return;
		case EXPR_QVAR:
			printf("%s", wIdentString(expr->q->name));
			return;
		case EXPR_CONST:
			printf("%d", expr->value);
			return;
		default:
			break;
	}
	
	int parenthesis = 0;
	
	switch (expr->type) {
		case EXPR_EQV:
		case EXPR_OR:
			printf("(");
			parenthesis = 1;
			break;
		default:
			break;
	}
	
	intexpr_print(expr->left);
	
	switch (expr->type) {
		case EXPR_OR:		printf(" | ");	break;
		case EXPR_AND:	printf(" & ");	break;
		case EXPR_EQV:	printf("==");		break;
		case EXPR_LT:	printf("<");	break;
		case EXPR_LE:	printf("<=");	break;
		case EXPR_EQ:	printf("=");	break;
		case EXPR_NE:	printf("!=");	break;
		case EXPR_GE:	printf(">=");	break;
		case EXPR_GT:	printf(">");	break;
		case EXPR_PLUS:		printf("+");	break;
		case EXPR_MINUS:	printf("-");	break;
		case EXPR_MULT:		printf("*");	break;
		case EXPR_DIV:		printf("/");	break;
		case EXPR_SHL:		printf("<<");	break;
		default:
			printf("XXX");
	}
	
	intexpr_print(expr->right);
	
	if (parenthesis)
			printf(")");
	
	return;
}

/************************************************************************/

intVariable* irVar2intVar(IrVar var) {
	
	if (!var) return NULL;
	
	intVariable *intvar = Malloc(sizeof(struct intVariable));
	intvar->name = var->name;
	intvar->type = (var->type) ? var->bits : 0;
	if (var->dim) {
		if (var->dim->next) {
			intvar->dim = var->dim->len * var->dim->next->len;
			intvar->low_index = 0;
		} else {
			intvar->dim = var->dim->len;
			intvar->low_index = var->dim->lo;
		}
	} else {
		intvar->dim = 0;
		intvar->low_index = 0;
	}
	intvar->next = NULL;
	
	return intvar;
}

intVariable *irVarlist2intVar(IrVarList varlist) {
	
	if (!varlist) return NULL;
	
	intVariable *rintvar = NULL, *intvar;
	for (; varlist; varlist = varlist->next) {
		
		if (varlist->var->type == IR_STRUCT) {
			IrVar svar = varlist->var->extra;
			for (; svar; svar = svar->extra) {
				intvar = irVar2intVar(svar);
				intvar->next = rintvar;
				rintvar = intvar;
			}
		}
		else {
			intvar = irVar2intVar(varlist->var);
			intvar->next = rintvar;
			rintvar = intvar;
		}
	}
	
	return rintvar;
}

intVariable* irVarlists2intVar(IrVarList varlist1
	, IrVarList varlist2) {
	
	intVariable *intvar;
	if (varlist1) {
		intvar = irVarlist2intVar(varlist1);
		if (varlist2) {
			intVariable *last_intvar = intvar;
			for (; last_intvar->next; last_intvar = last_intvar->next )
				;
			last_intvar->next = irVarlist2intVar(varlist2);
		}
	}
	else {
		intvar = irVarlist2intVar(varlist2);
	}
	
	return intvar;
}

/************************************************************************/

void all_labels_update(wSet **wset, IrStmt stmt) {
	
	if (!stmt) return;
	
	for (; stmt; stmt = stmt->next) {
		
		if (!wSetFind(*wset, stmt->label)) {
			wSetInsert(wset, stmt->label);
		}
		if ((stmt->type == IR_IF) || (stmt->type == IR_DO)) {
			IrIfdo ifdo = (IrIfdo) stmt->content;
			for (; ifdo; ifdo = ifdo->next) {
				all_labels_update(wset, ifdo->stmt);
			}
		}
	}
}

void labels_update(wSet **wset, IrStmt leftstmt, IrStmt rightstmt) {
	
	if (!leftstmt) return;
	
	unsigned char equal = (leftstmt == rightstmt);
	while (1) {
		
		if (leftstmt->label) {
			wSetInsert(wset, leftstmt->label);
			//printf("%s inserted\n", wIdentString(leftstmt->label));
		}
		
		if ((leftstmt->type == IR_IF) || (leftstmt->type == IR_DO)) {
			IrIfdo ifdo = (IrIfdo) leftstmt->content;
			for (; ifdo; ifdo = ifdo->next)
				labels_update(wset, ifdo->stmt, NULL);
		}
		
		leftstmt = leftstmt->next;
		if (!leftstmt || equal)
			break;
		
		equal = (leftstmt == rightstmt);
	}
}

intModule* irModule2intModule(IrModule irmodule) {
	
	if (!irmodule)
		return NULL;
	
	intModule *modules = NULL, *intmodule;
	for (; irmodule; irmodule = irmodule->next) {
		//if ((irmodule->param == NULL) && (irmodule->localvar == NULL))
		//	continue;
		//printf("Module: %s\n", wIdentString(irmodule->name));
		intmodule = Malloc(sizeof(struct intModule));
		intmodule->name = irmodule->name;
		intmodule->labels = wSetCreate();
		labels_update(&(intmodule->labels)
			, irmodule->first_stmt
			, irmodule->last_stmt);
		intmodule->vars = irVarlists2intVar(irmodule->param
			, irmodule->localvar);
		intmodule->next = modules;
		modules = intmodule;
	}
	
	if (DPN) {
		intmodule = Malloc(sizeof(struct intModule));
		intmodule->name = default_state_name;
		intmodule->labels = wSetCreate();
		wSetInsert(&(intmodule->labels), default_state_name);
		intmodule->vars = NULL;
		intmodule->next = modules;
		modules = intmodule;
	}
	
	return modules;
}

/************************************************************************/

intVariable* intvar_get(intVariable *var, wIdent var_name) {
	
	if (!var) return NULL;
	
	for (; var; var = var->next) {
		if (var->name == var_name)
			return var;
	}
	
	return NULL;
}

intVariable* intvar_find(wIdent var_name, int prime) {
	
	intVariable *var;
	
	if ((var = intvar_get(globals_t, var_name)))
			return var;
	if (prime == 0) {
		if (current_module) {
			if ((var = intvar_get(current_module->vars, var_name)))
				return var;
		}
	} else if (prime == 1) {
		if (called_module) {
			if ((var = intvar_get(called_module->vars, var_name)))
				return var;
		}
		if (next_module) {
			if ((var = intvar_get(next_module->vars, var_name)))
				return var;
		}
	} else if (prime == 2) {
		if (next_module) {
			if ((var = intvar_get(next_module->vars, var_name)))
				return var;
		}
	}
	
	fprintf(stderr, "unexpected variable: %s (prime=%d) in ",
		wIdentString(var_name), prime);
	if (current_module)
		fprintf(stderr, "current_module=%s\n", wIdentString(current_module->name));
	if (called_module)
		fprintf(stderr, "called_module=%s\n", wIdentString(called_module->name));
	if (next_module)
		fprintf(stderr, " next_module=%s\n", wIdentString(next_module->name));
	exit(1);
}

intExpr* irFnode2intExprVarRef(IrFnode fnode) {
	
	intExpr *index;
	if (fnode->right && fnode->right->token == IR_INDEX)
		index = irFnode2intExpr(fnode->right->right);
	else
		index = NULL;

	return intExprVarRef(intvar_find(fnode->name, fnode->value),
				(char) fnode->value,index);
}

intExpr* irFnode2intExprQuantified(IrFnode fnode) {
	
	if (!fnode) return NULL;
	
	intQuantifier *q = qnt_list_insert(&current_qnt
		, fnode->left->name, fnode->left->left->value
		, fnode->left->right->value);
	
	intExpr *expr = intExprQuantified(
		(fnode->token == IR_FORALL) ? EXPR_FORALL : EXPR_EXISTS
		, q, irFnode2intExpr(fnode->right));
	
	qnt_list_remove(&current_qnt);
	
	return expr;
}

intExpr* intexpr_else_create() {
	
	intExpr *expr = NULL, *condexpr;
	IrIfdo ifdo = (IrIfdo) current_ifdo->stmt->content;
	for (; ifdo; ifdo = ifdo->next) {
		
		if (ifdo->cond->token == IR_ELSE) continue;
		
		condexpr = irFnode2intExpr(ifdo->cond);
		expr = (expr) ? intExprBinary(EXPR_OR, expr, condexpr) : condexpr;
	}
	
	return (expr) ? intExprBinary(EXPR_NOT, expr, NULL) : intExprConst(1);
}

intExpr* irFnode2intExpr(IrFnode fnode) {
	
	if (!fnode)
		return NULL;
	
	//Two undef cases
	if (((fnode->token == IR_EQ) && !fnode->right)
			|| ((fnode->token == IR_CMPEQ) && !fnode->right)) {
		intExpr *expr = irFnode2intExprVarRef(fnode->left);
		usedvar = varreflist_create(expr->vref, usedvar);
		return intExprConst(1);
	}
	
	//true and false nodes
	if (fnode->token == IR_CMPEQ) {
		if (fnode->right->token == IR_FALSE) {
			return intExprBinary(EXPR_NOT,
					irFnode2intExpr(fnode->left),NULL);
		}
		if (fnode->right->token == IR_TRUE)
			return irFnode2intExpr(fnode->left);
	}
	
	int token_type = -1;
	switch (fnode->token) {
		case IR_NUMBER:
			return intExprConst(fnode->value);
		case IR_IDENT:
		case IR_BVAR: {
			intExpr *expr = irFnode2intExprVarRef(fnode);
			if (((!called_module) && (fnode->value == 1))
					|| ((called_module) && (fnode->value == 2)))
				usedvar = varreflist_create(expr->vref, usedvar);
			return expr;
		}
		case IR_QVAR: {
			struct QntListStruct *qnt;
			for (qnt = current_qnt; qnt; qnt = qnt->next)
				if (qnt->q->name == fnode->name)
					break;
			return intExprQvar(qnt->q);
		}
		case IR_TRUE:
			return intExprConst(1);
		case IR_FALSE:
			return intExprConst(0);
		case IR_EQ:		token_type = EXPR_EQ;		break;
		case IR_PLUS:		token_type = EXPR_PLUS; 	break;
		case IR_MINUS:		token_type = EXPR_MINUS; 	break;
		case IR_MULT: 		token_type = EXPR_MULT; 	break;
		case IR_DIV: 		token_type = EXPR_DIV; 		break;
		case IR_SHL:		token_type = EXPR_SHL;		break;
		case IR_SHR:		token_type = EXPR_SHR;      break;
		case IR_AND:		token_type = EXPR_BIT_AND;  break;
		case IR_OR:		token_type = EXPR_BIT_OR;   break;
		case IR_XOR:		token_type = EXPR_BIT_XOR;  break;
		case IR_LT:		token_type = EXPR_LT;		break;
		case IR_LE:		token_type = EXPR_LE;		break;
		case IR_CMPEQ:		token_type = EXPR_EQV;		break;
		case IR_NE:		token_type = EXPR_NE;		break;
		case IR_GE:		token_type = EXPR_GE;		break;
		case IR_GT:		token_type = EXPR_GT;		break;
		case IR_LGC_AND:	token_type = EXPR_AND;		break;
		case IR_LGC_OR:		token_type = EXPR_OR;		break;
		case IR_LGC_XOR:	token_type = EXPR_XOR;		break;
		case IR_LGC_NOT:
			return intExprBinary(EXPR_NOT,
				irFnode2intExpr(fnode->right),NULL);
		case IR_FORALL:
		case IR_EXISTS:
			return irFnode2intExprQuantified(fnode);
		case IR_ELSE:
			return intexpr_else_create();
		default:
			return NULL;
	}
	
	return intExprBinary(token_type,irFnode2intExpr(fnode->left),
					      irFnode2intExpr(fnode->right));
}

intModule* intmodule_find(wIdent name) {
	
	if (!modules_t)
		return NULL;
	
	intModule* module;
	for (module = modules_t->next; module; module = module->next) {
		//printf("%s, %s\n", wIdentString(module->name), wIdentString(name));
		if (wSetFind(module->labels, name))
			return module;
	}
	
	return NULL;
}

void modules_update(IrStmt stmt, wIdent label) {
	
	current_module = intmodule_find(label);
	
	switch (stmt->type) {
		
		case IR_RAWTRANS: {
			IrRule rule = (IrRule) stmt->content;
			if (!rule->to_stack1) {
				called_module = next_module = NULL;
			}
			else if (!rule->to_stack2) {
				called_module = NULL;
				next_module = intmodule_find(rule->to_stack1);
			}
			else {
				called_module = intmodule_find(rule->to_stack1);
				next_module = intmodule_find(rule->to_stack2);
			}
			return;
		}
	
		case IR_GOTO:
			called_module = NULL;
			next_module = intmodule_find(((IrGoto) stmt->content)->target);
			return;
		
		case IR_CALL:
			called_module = intmodule_find(((IrCall) stmt->content)->module->name);
			next_module = current_module;
			return;
		
		case IR_RET:
			called_module = next_module = NULL;
			return;
			
		case IR_FORK:
			called_module = intmodule_find(((IrFork) stmt->content)->middle);
			next_module = current_module;
			return;
		
		default:
			called_module = NULL;
			next_module = current_module;
			return;
	}
}

/************************************************************************/

int is_ret_var(wIdent varname) {
	
	IrGbl ret;
	for (ret = ir_ret; ret; ret = ret->next) {
		if (ret->var->name == varname)
			return 1;
	}
	
	return 0;
}

/* Returns true if expr contains a node of type EXPR_QVAR */
int containsQVar(intExpr *expr) {

	if (expr->type == EXPR_VAR || expr->type == EXPR_CONST) return 0;
	if (expr->type == EXPR_QVAR) return 1;
	if (expr->type == EXPR_FORALL || expr->type == EXPR_EXISTS)
		return containsQVar(expr->left);
	else
		return (containsQVar(expr->left) || containsQVar(expr->right));
}

/* Creates ampersand-separated expressions of type intExpr, which preserve
   values of variables intVariable. The function creates an expression
   if the variable is not in usedvar. */
intExpr* intvar_preserve(intVariable *var
	, char prime
	, struct VarRefList *usedvar) {
	
	intExpr *expr = NULL, *expr_lhs, *expr_rhs;
	intExpr *expr_assgn, *expr_qnt, *expr_qvar_eq, *expr_qvar_or;
	struct VarRefList *varref;
	
	for (; var; var = var->next) {
		
		if (is_ret_var(var->name))
			continue;
		
		if (!var->dim) {
			
			for (varref = usedvar; varref; varref = varref->next)
				if (varref->vref->var == var)
					break;

			// If the variable is in usedvar, we don't preserve it.
			if (varref)
				continue;

			// Otherwise, create an expression like var'=var.
			expr_lhs = intExprVarRef(var, prime, NULL);
			expr_rhs = intExprVarRef(var, 0, NULL);
			expr_assgn = intExprBinary((var->type == 0) ? EXPR_EQV : EXPR_EQ
				, expr_lhs, expr_rhs);
			expr = (expr) ? 
				intExprBinary(EXPR_AND, expr, expr_assgn) : expr_assgn;
		}
		else {
			struct intQuantifier *q = intCreateQuantifier(default_quant_name
				, var->low_index, var->low_index + var->dim - 1);
			expr_qvar_or = NULL;
			int not_preserve = 0;

			/* Every element in usedvar is checked, in case
			   the array is touched several times
			   (e.g. parallel assignment). */
			for (varref = usedvar; varref; varref = varref->next) {
				if (varref->vref->var == var) {
					intExpr* ind = intexpr_copy(varref->vref->index);
					// If index is NULL, it means that it is of
					// type EXPR_QVAR, which is only possible
					// in case of array assigment. Thus,
					// we do not preserve it
					if (!ind || containsQVar(ind)) {
						not_preserve = 1;
						break;
					}

					expr_qvar_eq = intExprBinary(EXPR_EQ, intExprQvar(q)
						, ind);
					expr_qvar_or = (expr_qvar_or) ? intExprBinary(EXPR_OR
						, expr_qvar_eq, expr_qvar_or) : expr_qvar_eq;
				}
			}
			if (not_preserve)
				continue;
			expr_lhs = intExprVarRef(var, prime, intExprQvar(q));
			expr_rhs = intExprVarRef(var, 0, intExprQvar(q));
			expr_assgn = intExprBinary((var->type == 0) ? EXPR_EQV : EXPR_EQ
				, expr_lhs, expr_rhs);
			expr_qnt = intExprQuantified(EXPR_FORALL, q
				, (expr_qvar_or) ? intExprBinary(EXPR_OR, expr_qvar_or
				, expr_assgn) : expr_assgn);
			expr = (expr) ? intExprBinary(EXPR_AND, expr, expr_qnt) : expr_qnt;
		}
	}
	
	return expr;
}

intExpr* usedvar_preserve(struct VarRefList *usedvar) {
	
	intExpr *expr = NULL, *expr_local = NULL;
	expr = intvar_preserve(globals_t, 1, usedvar);
	
	if (current_module && next_module) {
		expr_local = intvar_preserve(next_module->vars
			, (called_module) ? 2 : 1, usedvar);
		if (expr_local)
			expr = (expr) ? intExprBinary(EXPR_AND, expr, expr_local) 
				      : expr_local;
	}
	
	return expr;
}

intExpr* all_preserve() {
	
	return usedvar_preserve(NULL);
}

intExpr* preserve() {
	
	if (irVerbose > 2) {
		printf("\nPreserved: ");
		struct VarRefList *uv = usedvar;
		for (; uv; uv = uv->next) {
			printf("%s", wIdentString(uv->vref->var->name));
			if (uv->vref->index) {
				printf("[");
				intexpr_print(uv->vref->index);
				printf("]");
			}
			printf(" ");
		}
		printf("\n");
	}
	
	intExpr *expr_preserved = usedvar_preserve(usedvar);
	
	return expr_preserved;
}

intExpr* ret_preserve(intExpr *expr) {
	
	if (!expr)
		return intvar_preserve(globals_t, 1, NULL);
	
	intExpr *expr_preserved = intvar_preserve(globals_t, 1, usedvar);
	
	return expr_preserved;
}

/************************************************************************/

intExpr* irExprList2intExpr(IrExprList exprlist) {
	
	if (!exprlist)
		return NULL;
	
	intExpr *expr = irFnode2intExpr(exprlist->expr);
	for (exprlist = exprlist->next; exprlist; exprlist = exprlist->next) {
		expr = intExprBinary(EXPR_AND, expr, irFnode2intExpr(exprlist->expr));
	}
	
	return expr;
}

intExpr* irStmt2intExpr(IrStmt stmt, intExpr *additional) {
	
	intExpr *expr, *expr_preserved = NULL;
	
	switch (stmt->type) {
		
		case IR_RAWTRANS:
			expr = irFnode2intExpr(((IrRule) stmt->content)->expr);
			break;
			
		case IR_IF:
		case IR_DO:
			expr = all_preserve();
			break;
		
		case IR_ASSGN:
			expr = irExprList2intExpr((IrExprList) stmt->content);
			if (expr) {
				expr_preserved = preserve();
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else
				expr = all_preserve();
			break;
		
		case IR_GOTO:
			expr = irExprList2intExpr(((IrGoto) stmt->content)->expr);
			if (expr) {
				expr_preserved = preserve();
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else
				expr = all_preserve();
			break;
			
		case IR_BREAK:
			expr = all_preserve();
			break;
		
		case IR_SKIP:
			if (stmt->content) {
				expr = irFnode2intExpr((IrFnode) stmt->content);
				expr_preserved = preserve();
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else {
				expr = all_preserve();
			}
			break;
		
		case IR_CALL:
		
			expr = irExprList2intExpr(((IrCall) stmt->content)->params);
			if (expr) {
				expr_preserved = preserve();
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else
				expr = all_preserve();
			break;
			
		case IR_FORK:
		
			expr = irExprList2intExpr(((IrFork) stmt->content)->forked->params);
			if (expr) {
				expr_preserved = preserve();
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else
				expr = all_preserve();
			break;
		
		case IR_RET:
			expr = irExprList2intExpr((IrExprList) stmt->content);
			if (expr) {
				expr_preserved = ret_preserve(expr);
				if (expr_preserved)
					expr = intExprBinary(EXPR_AND, expr, expr_preserved);
			}
			else
				expr = ret_preserve(expr);
			break;
		
		default:
			fprintf(stderr, "unexpected type of statement (%d) "
				"in irStmt2intExpr", stmt->type);
			exit(1);
	}

	varreflist_free(usedvar);
	usedvar = NULL;
	
	if (expr) {
		if (additional)
			return intExprBinary(EXPR_AND, additional, expr);
		else
			return expr;
	}
	else {
		if (additional)
			return additional;
		else
			return intExprConst(1);
	}
		
	
	return (additional) ? intExprBinary(EXPR_AND, additional, expr) : expr;
}

void ircon_transition_print(wIdent p0, wIdent y0,
	wIdent p1, wIdent y1, wIdent y2, intExpr *expr) {
	
	printf("%s <%s> --> %s <", wIdentString(p0)
		, wIdentString(y0), wIdentString(p1));
	if (y1) printf("%s", wIdentString(y1));
	if (y2) printf(" %s", wIdentString(y2));
	printf(">\t(");
	intexpr_print(expr);
	printf(")\n");
	fflush(stdout);
}

/* A clone of intPdsInsert with DPN option */
void int_pds_insert(intPds *pds, IrStmt stmt, wIdent p0, wIdent y0,
		wIdent p1, wIdent y1, wIdent y2, intExpr *expr) {
			
	wRule *rule;
			
	if (DPN) {
		
		if (irVerbose) {
			//ircon_transition_print(q0, y0, q0, y1, y2, expr);
			if (stmt->shared) printf("shared: ");
			ircon_transition_print(q1, y0, q1, y1, y2, expr);
			if (stmt->shared) printf("shared: ");
			ircon_transition_print(q3, y0, q3, y1, y2, expr);
		}
		
		//intPdsInsert(pds, q0, y0, q0, y1, y2, expr);
		rule = intPdsInsert(pds, q1, y0, q1, y1, y2, expr);
		if (stmt->shared) rule->shared = 1;
		rule = intPdsInsert(pds, q3, y0, q3, y1, y2, expr);
		if (stmt->shared) rule->shared = 1;
	} else {
		
		if (irVerbose)
			ircon_transition_print(p0, y0, p1, y1, y2, expr);
		intPdsInsert(pds, p0, y0, p1, y1, y2, expr);
	}
}

void intpds_ifdo_insert(intPds *pds, IrStmt stmt) {
	
	IrIfdo ifdo = (IrIfdo) stmt->content;
	IrStmt ifdostmt;
	for (; ifdo; ifdo = ifdo->next) {
		
		//first statement
		intExpr *expr = irFnode2intExpr(ifdo->cond);
		if ((ifdo->stmt->type != IR_IF) && (ifdo->stmt->type != IR_DO)) {
			intpds_transition_insert(pds, ifdo->stmt, expr);
		}
		else {
			expr = irStmt2intExpr(stmt, expr);
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, ifdo->stmt->label, 0
				, expr);
			if (expr) intExprDelete(expr);
			intpds_transition_insert(pds, ifdo->stmt, NULL);
		}
		
		//other statements
		ifdostmt = ifdo->stmt->next;
		if (ifdostmt) {
			for (; ifdostmt->next; ifdostmt = ifdostmt->next)
				intpds_transition_insert(pds, ifdostmt, NULL);
			intpds_transition_insert(pds, ifdostmt, NULL);
		}
	}
}

void intpds_transition_insert(intPds *pds
	, IrStmt stmt
	, intExpr *additional) {
	
	if (!stmt) return;
	
	modules_update(stmt, (current_ifdo) 
		? current_ifdo->stmt->label : stmt->label);
	
	wIdent nextlabel;
	if (stmt->next) {
		nextlabel = stmt->next->label;
	}
	else if (current_ifdo) {
		if (current_ifdo->loop)
			nextlabel = current_ifdo->stmt->label;
		else
			nextlabel = current_ifdo->end;
	}
	//this shouldn't happen anyway ..
	else if (stmt->type != IR_RET){
		nextlabel = irNameStackFresh();
		if (current_module) {
			wSetInsert(&(current_module->labels), nextlabel);
		}
	}
	
	intExpr *expr = NULL;
	if ((stmt->type != IR_IF) && (stmt->type != IR_DO))
		expr = irStmt2intExpr(stmt, additional);
	if (!expr)
		expr = intExprConst(1);
	
	switch (stmt->type) {
		
		case IR_RAWTRANS: {
			IrRule rule = (IrRule) stmt->content;
			int_pds_insert(pds, stmt, rule->from_state, rule->from_stack
				, rule->to_state, rule->to_stack1, rule->to_stack2, expr);
			break;
		}
		
		case IR_IF:
			current_ifdo_insert(stmt, nextlabel, 0);
			intpds_ifdo_insert(pds, stmt);
			current_ifdo_remove();
			break;
		
		case IR_DO:
			current_ifdo_insert(stmt, nextlabel, 1);
			intpds_ifdo_insert(pds, stmt);
			current_ifdo_remove();
			break;
			
		case IR_ASSGN:
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, nextlabel, 0, expr);
			break;
		
		case IR_GOTO:
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, ((IrGoto) stmt->content)->target, 0
				, expr);
			break;
		
		case IR_BREAK:
			int_pds_insert(pds, stmt, default_state_name
				, stmt->label, default_state_name
				, (current_ifdo) ? current_ifdo->end : nextlabel, 0, expr);
			break;
		
		case IR_SKIP:
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, nextlabel, 0, expr);
			break;
			
		case IR_CALL: {
			IrCall ircall = (IrCall) stmt->content;
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, ircall->module->name, nextlabel, expr);
			break;
		}
			
		case IR_RET:
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, 0, 0, expr);
			break;
		
		case IR_FORK: {
			IrFork irfork = (IrFork) stmt->content;
			int_pds_insert(pds, stmt, default_state_name, stmt->label
				, default_state_name, irfork->middle, nextlabel, expr);
			if (expr) intExprDelete(expr);
				
			IrModule m;
			for (m = ir_module; m; m = m->next) {
				if (m->name == irfork->middle)
					break;
			}
			if (!m) {
				fprintf(stderr, "error in forking command");
				exit(1);
			}
			
			modules_update(m->first_stmt, m->first_stmt->label);
			next_module = intmodule_find(default_state_name);
			expr = irStmt2intExpr(m->first_stmt, NULL);
			IrCall midCall = (IrCall) m->first_stmt->content;
			int_pds_insert(pds, stmt, default_state_name, irfork->middle
				, default_state_name, midCall->module->name, default_state_name, expr);
			break;
		}
	}

	if (expr) intExprDelete(expr);

	return;
}

void intpds_transitions_insert(intPds *pds, IrStmt stmt) {
	
	for (; stmt; stmt = stmt->next) {
		intpds_transition_insert(pds, stmt, NULL);
	}
	/*if (DPN) {
		intExpr *expr = intExprConst(1);
		if (irVerbose)
			ircon_transition_print(q0, pds->pds->start->stack[0], q0, 
				default_state_name, pds->pds->start->stack[0], expr);
		intPdsInsert(pds, q0, pds->pds->start->stack[0], q0, 
			default_state_name, pds->pds->start->stack[0], expr);
		intExprDelete(expr);
	}*/
}

void ircon_intvar_print(char *pre, intVariable *var, char *post) {
	
	for (; var; var = var->next) {
		printf("%s%s %s"
			, pre
			, (var->type) ? "int" : "bool"
			, wIdentString(var->name));
		if (var->dim)
			printf("[%d,%d]", var->low_index,
				var->low_index + var->dim - 1);
		if (var->type)
			printf("(%d)", var->type);
		printf("%s", post);
	}
	
	return;
}

void oldsyntax_header_print() {
	
	intModule *module = modules_t;
	wSet *wset;
	
	if (module->vars) {
		printf("global\n");
		ircon_intvar_print("  ", module->vars, ";\n");
	}
	
	for (module = module->next; module; module = module->next) {
		if (!module->vars) continue;
		
		wset = module->labels;
		printf("local (%s", wIdentString(wset->ident));
		for (wset = wset->next; wset; wset = wset->next)
			printf(",%s", wIdentString(wset->ident));
		printf(")\n");
		ircon_intvar_print("  ", module->vars, ";\n");
	}
	printf("\n");
	
	return;
}

wConfig* irInit2wConfig() {
	
	wConfig *config = Malloc(sizeof(struct wConfig));
	IrName stack;
	int stacklength = 0;
	wIdent *sptr;
	
	for (stack = ir_init->stack; stack; stack = stack->next) {
		stacklength++;
	}
	
	if (DPN) {
		
		config->state = q0;
		config->stack = sptr = Calloc(stacklength + 2, sizeof(wIdent));
		*sptr++ = default_state_name;
	} else {
		
		config->state = ir_init->state;
		config->stack = sptr = Calloc(stacklength + 1, sizeof(wIdent));
	}
	
	for (stack = ir_init->stack; stack; stack = stack->next) {
		*sptr++ = stack->name;
	}
	*sptr = 0;
	
	return config;
}

intPds* ir2int() {
	
	intPds *pds;

	wConfig *start = irInit2wConfig();

	globals_t = irVarlists2intVar(ir_gbl, ir_ret);
	modules_t = Calloc(1,sizeof(struct intModule));
	modules_t->vars = globals_t;
	modules_t->next = irModule2intModule(ir_module);
	
	pds = intPdsCreate(start, modules_t);
	
	if (irVerbose) {
		printf("\n*****Pushdown System*****\n");
		oldsyntax_header_print();
		printf("(%s <%s", wIdentString(start->state)
			, wIdentString(start->stack[0]));

		wIdent *id = start->stack;
		while (*++id) {
			printf(" %s", wIdentString(*id));
			fflush(stdout);
		}
		printf(">)\n\n");
	}
	
	wConfigDelete(start);
	
	/* Now add the transitions. */
	intpds_transitions_insert(pds,ir_stmt);
	
	if (irVerbose)
		printf("*************************\n");

	return pds;
}
