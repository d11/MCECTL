#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "alloc.h"
#include "irInt.h"

int irVerbose;

IrDef ir_def = NULL;
IrEnum ir_enum = NULL;
IrStruct ir_struct = NULL;
IrGbl ir_gbl = NULL;
IrGbl ir_sgbl = NULL;
IrGbl ir_ret = NULL;
IrQvar ir_qvar = NULL;
IrInit ir_init = NULL;
IrStmt ir_stmt = NULL;
IrRule ir_rule = NULL;
IrModule ir_module = NULL;

char **type_array;
wIdent default_state_name;
wIdent default_return_name;
wIdent default_return_name_array;
wIdent default_return_name_bool;
wIdent default_return_name_bool_array;
wIdent default_quant_name;
wIdent q0;
wIdent q1;
wIdent q2;
wIdent q3;

void init_ir() {

	type_array = Malloc(50);
	type_array[IR_BOOL] = Strdup("bool");
	type_array[IR_INT] = Strdup("int");
	type_array[IR_SGNINT] = Strdup("signed int");
	type_array[IR_ENUM] = Strdup("enum");
	type_array[IR_STRUCT] = Strdup("struct");
	type_array[IR_VOID] = Strdup("void");
	
	default_state_name = wIdentCreate("q");
	default_return_name = wIdentCreate("iret");
	default_return_name_array = wIdentCreate("irets");
	default_return_name_bool = wIdentCreate("bret");
	default_return_name_bool_array = wIdentCreate("brets");
	default_quant_name = wIdentCreate("i");
	
	q0 = wIdentCreate("q0");		// initial state
	q1 = wIdentCreate("q1");		// initial state
	q2 = wIdentCreate("_q2_");
	q3 = wIdentCreate("q3__");		// initial and final state
}

void finish_ir() {

	Free(type_array[IR_BOOL]);
	Free(type_array[IR_INT]);
	Free(type_array[IR_SGNINT]);
	Free(type_array[IR_ENUM]);
	Free(type_array[IR_STRUCT]);
	Free(type_array[IR_VOID]);
	Free(type_array);
}

/* Adds to var->extra a list of variables, which corresponds to fields 
   of the struct. Assume that var has type IR_STRUCT and var->type_ptr
   points to an IrStruct. */
void irAddStructVar(IrVar var) {
	
	IrVarList fields = ((IrStruct) var->type_ptr)->fields;
	for (; fields; fields = fields->next) {
		if (var->dim && fields->var->dim)
			printf("error: array of struct of an array variable not supported");
		
		IrVar new_var = irVarCopy(fields->var);
		new_var->name = irNameStructCreate(var->name, fields->var->name);
		if (var->dim) {
			irDimFree(new_var->dim);
			new_var->dim = var->dim;
		}	
		/*if (fields->var->dim)
			new_var->dim = fields->var->dim;*/
		
		new_var->extra = var->extra;
		var->extra = new_var;
	}
}


IrName irReverseName(IrName name) {

	IrName pr = NULL;
	while (name) {
		IrName tmp = name->next;
		name->next = pr;
		pr = name;
		name = tmp;
	}
	return pr;
}

/* Insert a return (global) variable into ir_ret, the list of return variable */
void irInsertRetVar(wIdent name, int type, IrDim dim, int bits) {

	IrGbl ret;
	IrVar var;

	/* Check whether the variable is already in ir_ret */
	for (ret = ir_ret; ret; ret = ret->next) {
		var = ret->var;
		if (var->name != name) continue;

		/* If name is already in ir_ret */
		if (var->type != type) {
			printf("bug: return vars have same name "
			"but different type");
			exit(1);
		}

		/* Extend bits of found var, if needed */
		if (var->bits < bits) {
			var->bits = bits;
		}

		/* Extend dim of found var, if needed */
		if (!var->dim && dim) {
			var->dim = irDimCopy(dim);
		} else if (var->dim && dim) {
			if (var->dim->len < dim->len) {
				var->dim->len = dim->len;
				var->dim->hi = dim->len - 1;
			}
			/* If the existing variable is 2-dimensional */
			if (dim->next) {
				if (!var->dim->next) {
					var->dim->next = irDimCopy(dim->next);
				} else {
					if (var->dim->next->len < dim->next->len) {
						var->dim->next->len = dim->next->len;
						var->dim->next->hi = dim->next->len - 1;
					}
				}
			}
		}
		break;
	}
	
	/* If it's new, at it to ir_ret */
	if (!ret) {
		IrVar new_var = irVarCreate(name, irDimCopy(dim), bits);
		new_var->type = type;
		IrGbl new_ret = irVarListCreate(new_var, ir_ret);
		ir_ret = new_ret;
	}
}

/* Create a dim variable for return statement of array. 
   i and j denote lower and upper bound. */
void irCreateRetBound(IrDim retDim, IrVar var, int *i, 
		int *j, char **msg) {

	if (!retDim) return;

	if (!var) {
		*msg = "array variable expected";
		return;
	}

	IrDim var_dim = var->dim;
	if (!irDimEquals(retDim, var_dim)) {
		*msg = "array dimension mismatched";
		return;
	}
	
	irFlattenDim(retDim, i, j);
}

/* Check if every module in ir_module has body, i.e. 
   its first_stmt and last_stmt not equal to NULL.
   The program terminates if a module has no body */
void irCheckModule() {

	IrModule m;
	for (m = ir_module; m; m = m->next) {
		if (!m->first_stmt || !m->last_stmt) {
			printf("syntax error in Remopla: "
				"module %s has no body\n", wIdentString(m->name));
			exit(1);
		}
	}
}

IrStmt irCreateCallStmt(IrModule current_module, IrModule called_module, 
		IrExprList args, char **msg) {

	IrStmt stmt = irStmtCreate(IR_CALL, 0, NULL, NULL);
	IrCall ircall = irCallCreate(called_module, NULL);
	stmt->content = ircall;

	if (!called_module->param) return stmt;

	IrVarList param = called_module->param;
	IrFnode param_lhs, param_rhs, param_assgn;
	IrVarList fields;
	IrExprList expr_list;

	while (args) {
		
		if (!param || (param && !param->var)) {
			*msg = "parameter mismatched";
			return NULL;
		}

		switch (param->var->type) {
		
		case IR_STRUCT:
			fields = ((IrStruct) param->var->type_ptr)->fields;
			
			// for each field of the struct
			for (; fields; fields = fields->next) {	
				wIdent n1 = irNameStructCreate(param->var->name, fields->var->name);
				wIdent n2 = irNameStructCreate(args->expr->name, fields->var->name);
				
				// if param is an array
				IrVar paramVar = irVarGet(n1, NULL, called_module);
				if (paramVar->dim) {
					IrVar argVar = irVarGet(n2, current_module, NULL);
					int i, j;
					irCreateRetBound(paramVar->dim, argVar, &i, &j, msg);
					if (paramVar->type == IR_BOOL)
						param_assgn = irFnodeAllAssgn(IR_BVAR, IR_CMPEQ, n1, n2, i, j);
					else //field is not of type boolean
						param_assgn = irFnodeAllAssgn(IR_IDENT, IR_EQ, n1, n2, i, j);
				}
				// if param is NOT an array
				else {
					if (fields->var->type == IR_BOOL) {
						param_lhs = irNamedFnodeCreate(
							IR_BVAR, n1, 1 , NULL, NULL
						);
						param_rhs = irNamedFnodeCreate(
							IR_BVAR, n2, 0, NULL
							, irFnodeCopy(args->expr->right)
						);
						param_assgn = irFnodeCreate(
							IR_CMPEQ, 0, param_lhs, param_rhs
						);
					}
					else {
						param_lhs = irNamedFnodeCreate(
							IR_IDENT, n1, 1 , NULL, NULL
						);
						param_rhs = irNamedFnodeCreate(
							IR_IDENT, n2, 0, NULL
							, irFnodeCopy(args->expr->right)
						);
						param_assgn = irFnodeCreate(
							IR_EQ, 0, param_lhs, param_rhs
						);
					}
				}
				
				expr_list = irExprListCreate(param_assgn, ircall->params);
				ircall->params = expr_list;
			} // end for each field
			param = param->next;
				
			break;
		
		case IR_BOOL:
			if (param->var->dim) {
				if (args->expr->token != IR_BVAR) {
					*msg = "invalid argument: array expected";
					return NULL;
				}
				IrVar argVar = irVarGetUnformatted(args->expr->name, current_module, NULL);
				int i, j;
				irCreateRetBound(param->var->dim, argVar, &i, &j, msg);
				if (*msg) return NULL;
				param_assgn = irFnodeAllAssgn(IR_BVAR, IR_CMPEQ, 
					param->var->name, args->expr->name, i, j);
			}
			else {
				param_lhs = irNamedFnodeCreate(
					IR_BVAR, param->var->name, 1 , NULL, NULL);
				param_assgn = irFnodeCreate(
					IR_CMPEQ, 0, param_lhs, irFnodeCopy(args->expr));
			}
			
			expr_list = irExprListCreate(param_assgn, ircall->params);
			ircall->params = expr_list;
			param = param->next;
			
			break;
		
		default:
			if (param->var->dim) {
				if (args->expr->token != IR_IDENT) {
					*msg = "invalid argument: array expected";
					return NULL;
				}
				IrVar argVar = irVarGetUnformatted(args->expr->name, current_module, NULL);
				int i, j;
				irCreateRetBound(param->var->dim, argVar, &i, &j, msg);
				if (*msg) return NULL;
				param_assgn = irFnodeAllAssgn(IR_IDENT, IR_EQ, 
					param->var->name, args->expr->name, i, j);
			}
			else {
				param_lhs = irNamedFnodeCreate(
					IR_IDENT, param->var->name, 1 , NULL, NULL);
				param_assgn = irFnodeCreate(
					IR_EQ, 0, param_lhs, irFnodeCopy(args->expr));
			}
			
			expr_list = irExprListCreate(param_assgn, ircall->params);
			ircall->params = expr_list;
			param = param->next;

			break;
		} // end of switch(param->var->type)
		args = args->next;
	} // end of while(args)

	return stmt;
}

/* Create list of expression for return of type struct. */
IrExprList irCreateStructRetExprList(int direction, 
		wIdent varName, IrFnode varRight, IrStruct s, IrDim retDim, 
		int i, int j) {

	IrExprList expr_list = NULL;
	IrVarList fields = s->fields;
	IrFnode r;
	IrDim dim;
	wIdent n1, n2;
	for (; fields; fields = fields->next) {
		
		if (direction == 1) {
			n1 = irNameRetStructCreate(s->name, fields->var->name);
			n2 = irNameStructCreate(varName, fields->var->name);
		} else { // direction == 2
			n1 = irNameStructCreate(varName, fields->var->name);
			n2 = irNameRetStructCreate(s->name, fields->var->name);
		}
		
		int varType, cmpType;
		if (fields->var->type == IR_BOOL) {
			varType = IR_BVAR;
			cmpType = IR_CMPEQ;
		}
		else {
			varType = IR_IDENT;
			cmpType = IR_EQ;
		}
		
		dim = NULL;
		if (retDim) {
			dim = retDim;
		}
		else if (fields->var->dim) {
			dim = fields->var->dim;
			irFlattenDim(dim, &i, &j);
		}

		if (direction == 1)
			irInsertRetVar(n1, fields->var->type, dim, 
				fields->var->bits);
		/* if the return var or this field is an array */
		if (dim) {
			r = irFnodeAllAssgn(varType, cmpType, n1, n2, i, j);
		}
		/* if the return var or this field is NOT an array */
		else {
			IrFnode f1, f2;
			if (direction == 1) {
				f1 = irNamedFnodeCreate(varType, n1, 1, NULL, NULL);
				f2 = irNamedFnodeCreate(varType, n2, 0, NULL,
					irFnodeCopy(varRight));
			} else { // direction == 2
				f1 = irNamedFnodeCreate(varType, n1, 1, NULL, 
					irFnodeCopy(varRight));
				f2 = irNamedFnodeCreate(varType, n2, 0, NULL, NULL);
			}
			r = irFnodeCreate(cmpType, 0, f1, f2);
		}
		
		expr_list = irExprListCreate(r, expr_list);
	}

	return expr_list;
}

/* Create a list of expression for return statement.
   dir determines the direction of assignment of return variable.
      dir == 1: ret_var = expr
      dir == 2: expr = ret_var
   module is current_module and called_module for dir == 1 and 2, resp.
   module2 is always current_module
   msg contains error messages back to the parser. */
IrExprList irCreateRetExprList(int dir, IrModule module, IrModule module2, 
		IrFnode expr, char **msg) {

	IrExprList expr_list = NULL;
	
	/* handle the case where an array is returned */
	int i, j;
	IrDim ret_dim = module->retvar->dim;
	IrVar var;
	if (ret_dim) {
		var = irVarGetUnformatted(expr->name, module2, NULL);
		irCreateRetBound(ret_dim, var, &i, &j, msg);
		if (*msg) return NULL;
	}
		
	
	switch(module->retvar->type) {
			
	case IR_BOOL: {
		
		/* if the return var is an array */
		if (ret_dim) {
			
			if ((expr->token != IR_BVAR) || (var->type != IR_BOOL))  {
				*msg = "array of boolean expected";
				return NULL;
			}
			
			wIdent lhsName, rhsName;
			if (dir == 1) {
				irInsertRetVar(default_return_name_bool_array, 
					IR_BOOL, ret_dim, 0);
				lhsName = default_return_name_bool_array;
				rhsName = var->name;
			} else { // dir == 2
				lhsName = expr->name;
				rhsName = default_return_name_bool_array;
			}
			irFnodeFree(expr);
			expr = irFnodeAllAssgn(IR_BVAR, IR_CMPEQ, lhsName, 
				rhsName, i, j);
		}
		/* if the return var is NOT an array */
		else {

			IrFnode lhsExpr, rhsExpr;
			if (dir == 1) {
				irInsertRetVar(default_return_name_bool, 
					IR_BOOL, NULL, 0);
				lhsExpr = irNamedFnodeCreate(IR_BVAR, 
					default_return_name_bool, 1, NULL, NULL);
				rhsExpr = expr;
			} else { // dir == 2
				lhsExpr = expr;
				lhsExpr->value = 1;
				rhsExpr = irNamedFnodeCreate(IR_BVAR, 
					default_return_name_bool, 0, NULL, NULL);
			}
			expr = irFnodeCreate(IR_CMPEQ, 0, lhsExpr, rhsExpr);
		}
		expr_list = irExprListCreate(expr, NULL);
		break;
	}

	case IR_STRUCT: {

		if (expr->token != IR_IDENT) {
			*msg = "struct variable expected";
			return NULL;
		}
		IrVar var = irVarGetUnformatted(expr->name, module2, NULL);
		if (!var || (var->type != IR_STRUCT)) {
			*msg = "struct variable expected";
			return NULL;
		}
		IrStruct s = (IrStruct) module->retvar->type_ptr;
		if (s != (IrStruct) var->type_ptr) {
			*msg = "return variable has mismatched struct type";
			return NULL;
		}
		expr_list = irCreateStructRetExprList(dir, expr->name, expr->right, 
			s, ret_dim, i, j);
		irFnodeFree(expr);
		break;
	}

	default: {
		
		/* if the return var is an array */
		if (ret_dim) {
			
			if ((expr->token != IR_IDENT) 
					|| ((var->type != IR_INT) 
					&& (var->type != IR_ENUM))) {
				*msg = "array of integer expected";
				return NULL;
			}
			
			wIdent lhsName, rhsName;
			if (dir == 1) {
				irInsertRetVar(default_return_name_array, 
					IR_INT, ret_dim, module->retvar->bits);
				lhsName = default_return_name_array;
				rhsName = var->name;
			} else { // dir == 2
				lhsName = expr->name;
				rhsName = default_return_name_array;
			}
			irFnodeFree(expr);
			expr = irFnodeAllAssgn(IR_IDENT, IR_EQ, lhsName, 
				rhsName, i, j);
		}
		/* if the return var is NOT an array */
		else {

			IrFnode lhsExpr, rhsExpr;
			if (dir == 1) {
				irInsertRetVar(default_return_name, 
					IR_INT, NULL, module->retvar->bits);
				lhsExpr = irNamedFnodeCreate(IR_IDENT, 
					default_return_name, 1, NULL, NULL);
				rhsExpr = expr;
			} else { // dir == 2
				lhsExpr = expr;
				lhsExpr->value = 1;
				rhsExpr = irNamedFnodeCreate(IR_IDENT, 
					default_return_name, 0, NULL, NULL);
			}
			expr = irFnodeCreate(IR_EQ, 0, lhsExpr, rhsExpr);
		}
		expr_list = irExprListCreate(expr, NULL);
		break;
	}
	}

	return expr_list;
}

/* Creates an enum type and put it into the list ir_enum.
   The parameter name is the type name. 
   The parameter enumList is the list of names that will
   be assigned as constants starting from zero. */
IrEnum irCreateTypeEnum(wIdent name, IrName enumList) {

	int count = -1;
	IrName n;
	IrDef def;

	for (n = enumList; n; n = n->next) {
		def = irDefCreate(n->name, ++count, ir_def);
		ir_def = def;
	}
	IrEnum en = irEnumCreate(name, enumList, 0, count+1, ir_enum);
	while (count > 1) {
		count = count >> 1;
		en->bits++;
	}
	en->bits++;
	ir_enum = en;

	return en;
}

/* Return the defined integer of name, or -1 if name is not defined */
int irDefValue(wIdent name) {
	
	IrDef def;
	for (def = ir_def; def; def = def->next) {
		if (def->name == name)
			return def->value;
	}
	return -1;
}

/* Check whether two IrDim variables are internally the same */
int irDimEquals(IrDim dim1, IrDim dim2) {
	
	if (!dim1 && !dim2) {
		return 1;
	}
	if (dim1 && !dim2) {
		return 0;
	}
	if (!dim1 && dim2) {
		return 0;
	}
	
	return ((dim1->len == dim2->len) 
		&& (dim1->lo == dim2->lo) 
		&& (dim1->hi == dim2->hi) 
		&& irDimEquals(dim1->next, dim2->next));
}

void irFlattenDim(IrDim dim, int *i, int *j) {

	if (!dim->next) {	// if one dimension
		*i = dim->lo;
		*j = dim->hi;
	} else {		// if two dimension
		*i = 0;
		*j = dim->len * dim->next->len - 1;
	}
}

/* A q (i,j) name1[q] = name2[q], where q is default_quant_name */
IrFnode irFnodeAllAssgn(int token, int op, wIdent name1, wIdent name2, int i, int j) {
	
	IrFnode qvar_node, index_node, lhs_node, rhs_node;
	IrFnode i_node, j_node, qnt_node, assgn_node;
	
	qvar_node = irNamedFnodeCreate(IR_QVAR, default_quant_name, 0, NULL, NULL);
	index_node = irFnodeCreate(IR_INDEX, 0, NULL, qvar_node);
	lhs_node = irNamedFnodeCreate(token, name1, 1, NULL, index_node);
	
	qvar_node = irNamedFnodeCreate(IR_QVAR, default_quant_name, 0, NULL, NULL);
	index_node = irFnodeCreate(IR_INDEX, 0, NULL, qvar_node);
	rhs_node = irNamedFnodeCreate(token, name2, 0, NULL, index_node);
	
	i_node = irFnodeCreate(IR_NUMBER, i, NULL, NULL);
	j_node = irFnodeCreate(IR_NUMBER, j, NULL, NULL);
	qnt_node = irNamedFnodeCreate(IR_QUANT, default_quant_name, 0, i_node, j_node);
	
	assgn_node = irFnodeCreate(op, 0, lhs_node, rhs_node);
	
	return irFnodeCreate(IR_FORALL, 0, qnt_node, assgn_node);
}

/* A q (i,j) name[q] < num, where q is default_quant_name */
IrFnode irFnodeAllLt(wIdent name, int i, int j, int num) {

	IrFnode qvar_node, index_node, lhs_node, rhs_node;
	IrFnode i_node, j_node, qnt_node, lt_node;

	qvar_node = irNamedFnodeCreate(IR_QVAR, default_quant_name, 0, NULL, NULL);
	index_node = irFnodeCreate(IR_INDEX, 0, NULL, qvar_node);
	lhs_node = irNamedFnodeCreate(IR_IDENT, name, 1, NULL, index_node);

	rhs_node = irFnodeCreate(IR_NUMBER, num, NULL, NULL);

	i_node = irFnodeCreate(IR_NUMBER, i, NULL, NULL);
	j_node = irFnodeCreate(IR_NUMBER, j, NULL, NULL);
	qnt_node = irNamedFnodeCreate(IR_QUANT, default_quant_name, 0, i_node, j_node);

	lt_node = irFnodeCreate(IR_LT, 0, lhs_node, rhs_node);	

	return irFnodeCreate(IR_FORALL, 0, qnt_node, lt_node);
}

int irIsReturnInside(IrStmt stmt) {

	// if the statment is of type IR_RET, return 1
	if (stmt->type == IR_RET) {
		return 1;
	}

	/* if the stament is an if or do statment,
	   go inside and look if there is `any' statement
	   that is a return statement. */
	if (stmt->type == IR_IF || stmt->type == IR_DO) {
		
		IrIfdo ifdo = (IrIfdo) stmt->content;
		for (; ifdo; ifdo = ifdo->next) {
			
			IrStmt ifdoStmt = ifdo->stmt;
			for (; ifdoStmt; ifdoStmt = ifdoStmt->next) {
				if (irIsReturnInside(ifdoStmt))
					return 1;
			}
		}
	}

	return 0;
}

int hasSameLabels(IrStmt stmt1, IrStmt stmt2) {

	if (!stmt1 || !stmt2) {
		return 0;
	}

	if (stmt1->label == stmt2->label) {
		return 1;
	}

	if (stmt1->type == IR_IF || stmt1->type == IR_DO) {
		IrIfdo ifdo = (IrIfdo) stmt1->content;
		for (; ifdo; ifdo = ifdo->next) {
			if (hasSameLabels(ifdo->stmt, stmt2)) {
				return 1;
			}
		}
	}

	if (stmt2->type == IR_IF || stmt2->type == IR_DO) {
		IrIfdo ifdo = (IrIfdo) stmt2->content;
		for (; ifdo; ifdo = ifdo->next) {
			if (hasSameLabels(stmt1, ifdo->stmt)) {
				return 1;
			}
		}
	}

	return 0;
}

int irHasSameLabels(IrStmt stmt1, IrStmt stmt2) {

	IrStmt s1, s2;
	for (s1 = stmt1; s1; s1 = s1->next) {
		for (s2 = stmt2; s2; s2 = s2->next) {
			if (hasSameLabels(s1, s2)) {
				return 1;
			}
		}
	}

	return 0;
}

/* Check if label exists in stmt.
   Return 1 if found, 0 otherwise */
int irLabelExist(IrStmt stmt, wIdent label) {
	
	if (!stmt) return 0;

	for (; stmt; stmt = stmt->next) {
		if (stmt->label == label) return 1;

		if (stmt->type == IR_IF || stmt->type == IR_DO) {
			IrIfdo ifdo = (IrIfdo) stmt->content;
			for (; ifdo; ifdo = ifdo->next) {
				if (ifdo->stmt) {
					if (ifdo->stmt->type == IR_IF || ifdo->stmt->type == IR_DO) {
						if (irLabelExist(ifdo->stmt, label)) return 1;
					} else {
						if (irLabelExist(ifdo->stmt->next, label)) return 1;
					}
				}
			}
		}
	}
	return 0;
}

/* Look inside stmt, and insert new label if necessary */
void irLabelUpdate(IrStmt stmt) {

        if (!stmt) return;

        for (; stmt; stmt = stmt->next) {

                if(!stmt->label) stmt->label = irNameStackFresh();

		if ((stmt->type != IR_IF) && (stmt->type != IR_DO)) continue;

                IrIfdo ifdo = (IrIfdo) stmt->content;
		for (; ifdo; ifdo = ifdo->next) {
			if (!ifdo->stmt) continue;
			if ((ifdo->stmt->type == IR_IF) || (ifdo->stmt->type == IR_DO)) {
				irLabelUpdate(ifdo->stmt);
			}
			else {
				/* the first statement of ifdo statement 
				   always has the same label as the stmt */
				ifdo->stmt->label = stmt->label;
				irLabelUpdate(ifdo->stmt->next);
			}
		}
        }
}

/* Return an IrModule which has name moduleName
   NULL, if not found */
IrModule irModuleFind(wIdent moduleName) {

	IrModule m;
	for (m = ir_module; m; m = m->next)
		if (m->name == moduleName) break;

	return m;
}

int isSharedGlobal(wIdent name) {
	
	if (!ir_sgbl) return 0;
	
	IrGbl sgbl;
	for (sgbl = ir_sgbl; sgbl; sgbl = sgbl->next)
		if (sgbl->var->name == name)
			return 1;
	
	return 0;
}

IrFnode irTwoDimIndex2Fnode(IrVar v, IrFnode dim1, IrFnode dim2) {

	/* a[i][j] --> a[(i-lo1)*n2 + (j-lo2)], where n2 is 2nd-dim length
		   --> a[(i*n2 - lo1*n2) + (j-lo2)] */
	IrFnode i_n2, x1, x2;
	i_n2 = irFnodeCreate(IR_MULT, 0, dim1, 
		irFnodeCreate(IR_NUMBER, v->dim->next->len, NULL, NULL));
	if (v->dim->lo > 0) {
		IrFnode lo1_n2 = irFnodeCreate(IR_MULT, 0
			, irFnodeCreate(IR_NUMBER, v->dim->lo, NULL, NULL)
			, irFnodeCreate(IR_NUMBER, v->dim->next->len, NULL, NULL));
		x1 = irFnodeCreate(IR_MINUS, 0, i_n2, lo1_n2);
	}
	else
		x1 = i_n2;
	
	x2 = (v->dim->next->lo > 0) ? irFnodeCreate(IR_MINUS, 0, dim2,
			irFnodeCreate(IR_NUMBER, v->dim->next->lo, NULL, NULL))
			: dim2;

	return irFnodeCreate(IR_INDEX, 0, NULL
		, irFnodeCreate(IR_PLUS, 0, x1, x2));

}

void irUpdateCallExprList(IrStmt stmt, IrExprList exprList) {

	if (!exprList) return;

	IrExprList el;
	IrFnode e;

	/* Every lhs variables except global variables get double primes */
	for (el = exprList; el; el = el->next) {
		e = el->expr->left;
		if (!irVarFind(e->name, ir_gbl))
			e->value = 2;
	}

	/* Put exprList at the end of parameter assignment lists */
	IrCall ircall = (IrCall) stmt->content;
	if (!ircall->params) {
		ircall->params = exprList;
	} else {
		for (el = ircall->params; el->next; el = el->next) {}
		el->next = exprList;
	}
}

/* Find an IrVar with its formatted name inside list.
   When not found, NULL is return. */
IrVar irVarFind(wIdent name, IrVarList list) {
	
	IrVarList vl;
	IrVar v;
	for (vl = list; vl; vl = vl->next) {
		if (vl->var->type == IR_STRUCT) {
			for (v = vl->var->extra; v; v = v->extra)
				if (v->name == name)
					return v;
		}
		else {
			if (vl->var->name == name)
				return vl->var;
		}
	}
	
	return NULL;
}

/* Find an IrVar from its (formatted) name inside ir_gbl, thisModule, 
   and nextModule. Note: formated names mean names that their structs
   and fields are combined with underscore.

   When not found, NULL is return. */
IrVar irVarGet(wIdent name, IrModule thisModule, IrModule nextModule) {
	
	IrVar v = NULL;
	v = irVarFind(name, ir_gbl);
	if (v) return v;
	
	if (thisModule) {
		v = irVarFind(name, thisModule->param);
		if (v) return v;
		
		v = irVarFind(name, thisModule->localvar);
		if (v) return v;
	}
	
	if (nextModule) {
		v = irVarFind(name, nextModule->param);
		if (v) return v;
	}
	
	return NULL;
}

IrVar irVarGetUnformatted(wIdent name, IrModule thisModule, IrModule nextModule) {

	IrVarList varList;
	for (varList = ir_gbl; varList; varList = varList->next)
		if (varList->var->name == name) return varList->var;

	if (thisModule) {
		for (varList = thisModule->param; varList; varList = varList->next)
			if (varList->var->name == name) return varList->var;
		for (varList = thisModule->localvar; varList; varList = varList->next)
			if (varList->var->name == name) return varList->var;
	}
	
	if (nextModule) {
		for (varList = nextModule->param; varList; varList = varList->next)
			if (varList->var->name == name) return varList->var;
	}

	return NULL;
}

/* Find the type of variable name inside list. 
   For variable of type struct, it must has structName */
int irVarType(IrVarList list, wIdent structName, wIdent name) {

	IrVarList vl;
	for (vl = list; vl; vl = vl->next) {
		if (vl->var->name == structName && vl->var->type == IR_STRUCT) {
			IrVarList fields = ((IrStruct) vl->var->type_ptr)->fields;
			for (; fields; fields = fields->next) {
				if (fields->var->name == name) 
					return fields->var->type; 
			}
		}
		if (vl->var->name == name) return vl->var->type;
	}

	return -1;
}


/******************************* Reallocator ********************************/

IrDim irDimCopy(IrDim dim) {
	
	if (!dim) return NULL;
	
	IrDim d = Malloc(sizeof(struct IrDimStruct));
	d->len = dim->len;
	d->lo = dim->lo;
	d->hi = dim->hi;
	d->next = irDimCopy(dim->next);
	
	return d;
}

IrExprList irExprListCopy(IrExprList exprlist) {
	
	if (!exprlist) return NULL;
	
	IrExprList e = Malloc(sizeof(struct IrExprListStruct));
	e->expr = irFnodeCopy(exprlist->expr);
	e->next = irExprListCopy(exprlist->next);
	
	return e;
}

IrFnode irFnodeCopy(IrFnode fnode) {
	
	if (!fnode) return NULL;
	
	IrFnode f = Malloc(sizeof(struct IrFnodeStruct));
	f->token = fnode->token;
	f->name = fnode->name;
	f->value = fnode->value;
	f->left = irFnodeCopy(fnode->left);
	f->right = irFnodeCopy(fnode->right);
	
	return f;
}

IrModule irModuleCopy(IrModule module) {
	
	if (!module) return NULL;
	
	IrModule m = Malloc(sizeof(struct IrModuleStruct));
	m->name = module->name;
	m->retvar = irVarCopy(module->retvar);
	m->param = irVarListCopy(module->param);
	m->localvar = irVarListCopy(module->localvar);
	m->first_stmt = module->first_stmt;
	m->last_stmt = module->last_stmt;
	m->next = module->next;
	
	return m;
}

IrVar irVarCopy(IrVar var) {
	
	if (!var) return NULL;
	
	IrVar v = Malloc(sizeof(struct IrVarStruct));
	v->name = var->name;
	v->type = var->type;
	v->type_ptr = var->type_ptr;
	v->dim = irDimCopy(var->dim);
	v->bits = var->bits;
	v->extra = irVarCopy(var->extra);
	
	return v;
}

IrVarList irVarListCopy(IrVarList varlist) {
	
	if (!varlist) return NULL;
	
	IrVarList v = Malloc(sizeof(struct IrVarListStruct));
	v->var = irVarCopy(varlist->var);
	v->next = irVarListCopy(varlist->next);
	
	return v;
}

/******************************* Constructor ********************************/

IrCall irCallCreate(IrModule module, IrExprList params) {
	
	IrCall ca = Malloc(sizeof(struct IrCallStruct));
	ca->module = module;
	ca->params = params;
	
	return ca;
}

IrDef irDefCreate(wIdent name, int value, IrDef next) {
	
	IrDef def = Malloc(sizeof(struct IrDefStruct));
	def->name = name;
	def->value = value;
	def->next = next;
	
	return def;
}

IrDim irDimCreate(int len, int lo, int hi, IrDim next) {

	IrDim d = Malloc(sizeof(struct IrDimStruct));
	d->len = len;
	d->lo = lo;
	d->hi = hi;
	d->next = next;
	
	return d;
}

IrEnum irEnumCreate(wIdent name, IrName items, int bits, int size, IrEnum next) {
	
	IrEnum en = Malloc(sizeof(struct IrEnumStruct));
	en->name = name;
	en->items = items;
	en->bits = bits;
	en->size = size;
	en->next = next;
	
	return en;
}

IrExprList irExprListCreate(IrFnode expr, IrExprList next) {
	
	IrExprList ex = Malloc(sizeof(struct IrExprListStruct));
	ex->expr = expr;
	ex->next = next;
	
	return ex;
}

IrFnode irFnodeCreate(int token, int value, 
		IrFnode left, IrFnode right) {
			
	return irNamedFnodeCreate(token, 0, value, left, right);
}

IrFork irForkCreate(wIdent middle, IrCall forked) {
	
	IrFork f = Malloc(sizeof(struct IrForkStruct));
	f->middle = middle;
	f->forked = forked;
	
	return f;
}

IrGoto irGotoCreate(wIdent target, IrExprList expr) {
	
	IrGoto gt = Malloc(sizeof(struct IrGotoStruct));
	gt->target = target;
	gt->expr = expr;
	
	return gt;
}

IrIfdo irIfdoCreate(IrFnode cond, IrStmt stmt, IrIfdo next) {
	
	IrIfdo ifdo = Malloc(sizeof(struct IrIfdoStruct));
	ifdo->cond = cond;
	ifdo->stmt = stmt;
	ifdo->next = next;
	
	return ifdo;
}

IrInit irInitCreate(wIdent state, IrFnode state_fnode,
		IrName stack, IrFnode stack_fnode) {
	
	IrInit in = Malloc(sizeof(struct IrInitStruct));
	in->state = state;
	in->state_fnode = state_fnode;
	in->stack = stack;
	in->stack_fnode = stack_fnode;
	
	return in;
}

IrModule irModuleCreate(wIdent name, IrVar retvar, IrVarList param) {

	IrModule module = Malloc(sizeof(struct IrModuleStruct));
	module->name = name;
	module->retvar = retvar;
	module->param = param;
	module->localvar = NULL;
	module->first_stmt = NULL;
	module->last_stmt = NULL;
	module->next = NULL;

	return module;
}

IrName irNameCreate(wIdent name, IrName next) {
	
	IrName nm = Malloc(sizeof(struct IrNameStruct));
	nm->name = name;
	nm->next = next;
	
	return nm;
}

IrFnode irNamedFnodeCreate(int token, wIdent name, int value, 
		IrFnode left, IrFnode right) {

	IrFnode node = (IrFnode) Malloc(sizeof(struct IrFnodeStruct));
	node->token = token;
	node->name = name;
	node->value = value;
	node->left = left;
	node->right = right;
	
	return node;
}

IrQvar irQvarCreate(wIdent name, int lower, int upper) {
	
	IrQvar qvar = (IrQvar) Malloc(sizeof(struct IrQvarStruct));
	qvar->name = name;
	qvar->lower = lower;
	qvar->upper = upper;
	qvar->next = NULL;
	
	return qvar;
}

IrRule irRuleCreate(wIdent from_state, wIdent from_stack, 
		wIdent to_state, wIdent to_stack1, wIdent to_stack2) {
			
	IrRule rule = Malloc(sizeof(struct IrRuleStruct));
	rule->from_state = from_state;
	rule->from_stack = from_stack;
	rule->to_state = to_state;
	rule->to_stack1 = to_stack1;
	rule->to_stack2 = to_stack2;
	rule->expr = NULL;
	rule->label = NULL;
	
	return rule;
}

IrStmt irStmtCreate(int type, wIdent label, void *content, IrStmt next) {
	
	IrStmt stmt = Malloc(sizeof(struct IrStmtStruct));
	stmt->type = type;
	stmt->label = label;
	stmt->content = content;
	stmt->shared = 0;
	stmt->next = next;
	
	return stmt;
}

IrStruct irStructCreate(wIdent name, IrVarList fields, IrStruct next) {
	
	IrStruct st = Malloc(sizeof(struct IrVarStruct));
	st->name = name;
	st->fields = fields;
	st->next = next;
	
	return st;
}

IrVar irVarCreate(wIdent name, IrDim dim, int bits) {
	
	IrVar var = Malloc(sizeof(struct IrVarStruct));
	var->name = name;
	var->type = 0;
	var->type_ptr = NULL;
	var->dim = dim;
	var->bits =  bits;
	var->extra = NULL;
	
	return var;
}

IrVar irVarTypeCreate(int type) {
	
	IrVar var = Malloc(sizeof(struct IrVarStruct));
	var->name = 0;
	var->type = type;
	var->type_ptr = NULL;
	var->dim = NULL;
	var->bits =  0;
	var->extra = NULL;
	
	return var;
}

IrVarList irVarListCreate(IrVar var, IrVarList next) {
	
	IrVarList varlist = Malloc(sizeof(struct IrVarListStruct));
	varlist->var = var;
	varlist->next = next;
	
	return varlist;
}

/******************************* Destructor ********************************/

void irCallFree(IrCall ircall) {

	irExprlistFree(ircall->params);
	Free(ircall);

	return;
}

void irDefFree(IrDef def) {
	
	if (!def) return;
	
	if (def->next) irDefFree(def->next);
	Free(def);
	
	return;
}

void irDimFree(IrDim dim) {

	if (!dim) return;

	if (dim->next) irDimFree(dim->next);
	Free(dim);

	return;
}

void irEnumFree(IrEnum enm) {
	
	if (!enm) return;
	
	if(enm->next) irEnumFree(enm->next);
	irNameFree(enm->items);
	Free(enm);
	
	return;
}

void irExprlistFree(IrExprList exprlist) {

	if (!exprlist) return;
	
	if (exprlist->next) irExprlistFree(exprlist->next);

	irFnodeFree(exprlist->expr);
	Free(exprlist);

	return;
}

void irFnodeFree(IrFnode fnode) {

	if (!fnode) return;
	
	if (fnode->left) irFnodeFree(fnode->left);
	if (fnode->right) irFnodeFree(fnode->right);
	Free(fnode);
	
	return;
}

void irGotoFree(IrGoto irgoto) {
	
	irExprlistFree(irgoto->expr);
	Free(irgoto);
	
	return;
}

void irIfdoFree(IrIfdo ifdo) {
	
	if (ifdo->next) irIfdoFree(ifdo->next);

	irFnodeFree(ifdo->cond);
	irStmtFree(ifdo->stmt);
	Free(ifdo);

	return;
}

void irIndvarFree(IrIndVar indvar) {
	
	if (!indvar) return;
	
	if (indvar->next) irIndvarFree(indvar->next);
	Free(indvar);
	
	return;
}

void irInitFree(IrInit init) {

	if (!init) return;
	
	irFnodeFree(init->state_fnode);
	irNameFree(init->stack);
	irFnodeFree(init->stack_fnode);
	Free(init);
	
	return;
}

void irModuleFree(IrModule module) {
	
	if (!module) return;
	
	if (module->next) irModuleFree(module->next);
	irVarFree(module->retvar);
	irVarlistFree(module->param);
	irVarlistFree(module->localvar);
	Free(module);
	
	return;
}

void irNameFree(IrName name) {
	
	if (!name) return;
	
	if (name->next)
		irNameFree(name->next);
	Free(name);
	
	return;
}

void irRuleFree(IrRule rule) {
	
	if (rule->expr) irFnodeFree(rule->expr);
	if (rule->label) Free(rule->label);
	Free(rule);

	return;
}

void irStmtFree(IrStmt stmt) {
	
	if (!stmt) return;
	
	if (stmt->next) irStmtFree(stmt->next);
	switch (stmt->type) {
		
		case IR_RAWTRANS:
			irRuleFree((IrRule) stmt->content);
			break;

		case IR_IF:
		case IR_DO:
			irIfdoFree((IrIfdo) stmt->content);
			break;

		case IR_ASSGN:
			irExprlistFree((IrExprList) stmt->content);
			break;

		case IR_GOTO:
			irGotoFree((IrGoto) stmt->content);
			break;
			
		case IR_BREAK:
			break;

		case IR_SKIP:
			if (stmt->content)
				irFnodeFree((IrFnode) stmt->content);
			break;

		case IR_CALL:
			irCallFree((IrCall) stmt->content);
			break;

		case IR_RET:
			if (stmt->content)
				irExprlistFree((IrExprList) stmt->content);
			break;
	}
	Free(stmt);
	
	return;
}

void irStructFree(IrStruct strct) {
	
	if (!strct) return;
	
	if (strct->next) irStructFree(strct->next);
	irVarlistFree(strct->fields);
	Free(strct);
	
	return;
}

void irVarExtraFree(IrVar var) {
	
	if (!var) return;
	
	if (var->extra) irVarExtraFree(var->extra);
	Free(var);
	
	return;
}

void irVarFree(IrVar var) {
	
	if (!var) return;
	
	if (var->extra) irVarExtraFree(var->extra);
	if (var->dim) irDimFree(var->dim);
	Free(var);
	
	return;
}

void irVarlistFree(IrVarList varlist) {
	
	if (!varlist) return;
	
	if (varlist->next) irVarlistFree(varlist->next);
	irVarFree(varlist->var);
	Free(varlist);
	
	return;
}

void irFree() {

	irDefFree(ir_def);
	irEnumFree(ir_enum);
	irStructFree(ir_struct);
	irVarlistFree(ir_gbl);
	irVarlistFree(ir_ret);
	irInitFree(ir_init);
	irStmtFree(ir_stmt);
	irModuleFree(ir_module);
}

/****************************************************************************/

int is_arithmetic_op(int x) {
	if (x == IR_PLUS || x == IR_MINUS
		|| x == IR_MULT || x == IR_DIV)
			return 1;
	else
		return 0;
}

int is_bitwise_op(int x) {
	if (x == IR_AND || x == IR_OR
		|| x == IR_XOR || x == IR_SHL
		|| x == IR_SHR)
			return 1;
	else
		return 0;
}

int is_relational_op(int x) {
	if (x == IR_LT || x == IR_LE
		|| x == IR_GE || x == IR_GT
		|| x == IR_CMPEQ || x == IR_NE)
			return 1;
	else
		return 0;
}

int is_logical_op(int x) {
	if (x == IR_LGC_AND || x == IR_LGC_OR
		|| x == IR_LGC_NOT || x == IR_LGC_EQV)
			return 1;
	else
		return 0;
}

int is_operator(int x) {
	if (is_arithmetic_op(x)
		|| is_bitwise_op(x)
		|| is_relational_op(x)
		|| is_logical_op(x))
			return 1;
	else
		return 0;
}

int is_intterm_op(int x) {
	if (is_arithmetic_op(x) || x==IR_SHL)
		return 1;
	else
		return 0;
}

int is_var(int x) {
	if (x == IR_IDENT || x == IR_BVAR)
		return 1;
	else
		return 0;
}

int is_num(int x) {
	if (x == IR_NUMBER)
		return 1;
	else
		return 0;
}
/****************************************************************************/

intPds *dummy_pds() {
	
	wIdent q = wIdentCreate("q");
	wIdent m = wIdentCreate("m");
	wIdent a = wIdentCreate("a");
	wIdent b = wIdentCreate("b");
	wIdent c = wIdentCreate("c");
	
	wConfig *start = wConfigCreate(q, m, 0);
	
	intModule *modules_t = Malloc(sizeof(struct intModule));
	wSet *labels = wSetCreate();
	wSetInsert(&labels, m);
	wSetInsert(&labels, a);
	wSetInsert(&labels, b);
	wSetInsert(&labels, c);
	modules_t->labels = labels;
	modules_t->vars = NULL;
	modules_t->next = NULL;
	
	intModule *modules_a = Malloc(sizeof(struct intModule));
	modules_a->labels = wSetCreate();
	wSetInsert(&(modules_a->labels), a);
	wSetInsert(&(modules_a->labels), b);
	modules_a->vars = NULL;
	modules_a->next = NULL;
	
	modules_t->next = modules_a;
	
	intPds *pds = intPdsCreate(start, modules_t);
	intPdsInsert(pds, q, m, q, a, c, intExprConst(1));
	intPdsInsert(pds, q, a, q, b, 0, intExprConst(1));
	intPdsInsert(pds, q, b, q, 0, 0, intExprConst(1));
	
	return pds;
}

intPds* irReadPds (char *src, wIdent label)
{
#ifdef YYDEBUG
	{
	  extern int irdebug;
	  irdebug = 1;
	}
#endif
	intPds *pds;

	init_ir();
	irParse(src);
	irCheckModule();
	if (irVerbose > 1)
		irPrintInfo();
	if (label && !irLabelExist(ir_stmt, label))
		printf("Warning: Label %s does not exist.\n", 
			wIdentString(label));
	pds = ir2int();
	//pds = dummy_pds();
	irFree();
	finish_ir();
	if (irVerbose)
		printf("ir done\n");
	return pds;
}
