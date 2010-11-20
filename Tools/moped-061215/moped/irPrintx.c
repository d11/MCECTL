#include <stdio.h>
#include <string.h>
#include "irInt.h"

void printx_stmt(IrStmt);

void printx_def() {
	
	if (!ir_def)
		return;

	IrDef def;
	for (def = ir_def; def; def = def->next) {
		printf("define %s %d\n"
			, wIdentString(def->name)
			, def->value
		);
	}
}

void printx_var(char *pre, IrVar var, char *post) {
	
	if (!var)
		return;
	
	printf("%s%s %s", pre, type_array[var->type]
		, wIdentString(var->name)
	);
	if (var->dim) {
		if (var->dim->next)
			printf("[0,%d]"
				, var->dim->len * var->dim->next->len - 1);
		else
			printf("[%d,%d]", var->dim->lo, var->dim->hi);
	}
	if (var->bits)
		printf("(%d)", var->bits);
	printf("%s", post);
}

void printx_struct_var(char *pre, IrVar var, char *post) {
	
	if (!var)
		return;
	
	IrVar struct_var = var->extra;
	for (; struct_var; struct_var = struct_var->extra) {
		printf("%s%s %s"
			, pre
			, type_array[struct_var->type]
			, wIdentString(struct_var->name)
		);
		if (struct_var->dim) {
			if (struct_var->dim->next)
				printf("[0,%d]"
					, struct_var->dim->len * struct_var->dim->next->len - 1);
			else
				printf("[%d,%d]", struct_var->dim->lo, struct_var->dim->hi);
		}
		if (struct_var->bits)
			printf("(%d)", struct_var->bits);
		printf("%s", post);
	}
}

void printx_global_var() {
	
	if (!ir_gbl && !ir_ret)
		return;
		
	IrGbl gbl;
	printf("global\n");
	for (gbl = ir_gbl; gbl; gbl = gbl->next) {
		if (gbl->var->type == IR_STRUCT)
			printx_struct_var("  ", gbl->var, ";\n");
		else
			printx_var("  ", gbl->var, ";\n");
	}
	for (gbl = ir_ret; gbl; gbl = gbl->next) {
		printx_var("  ", gbl->var, ";\n");
	}
}

void printx_symbols(IrName symbols) {
	if (symbols)
		printf(" (%s", wIdentString(symbols->name));
	for (symbols = symbols->next; symbols; symbols = symbols->next)
		printf(", %s", wIdentString(symbols->name));
	printf(") ");
}

void printx_local_var() {
	
	if (!ir_module)
		return;
	
	IrModule module;
	for (module = ir_module; module; module = module->next) {
		
		if (module->param || module->localvar) {
			IrVarList varlist;
			printf("local (%s)", wIdentString(module->name));
			//printx_symbols(module->symbols);
			printf("\n");
			for (varlist = module->param; varlist; varlist = varlist->next) {
				if (varlist->var->type == IR_STRUCT)
					printx_struct_var("  ", varlist->var, ";\n");
				else
					printx_var("  ", varlist->var, ";\n");
			}
			for (varlist = module->localvar; varlist; varlist = varlist->next) {
				if (varlist->var->type == IR_STRUCT)
					printx_struct_var("  ", varlist->var, ";\n");
				else
					printx_var("  ", varlist->var, ";\n");
			}
		}
	}
}

void printx_init() {
	
	IrName stack;
	
	printf("\n(%s <%s"
		, wIdentString(ir_init->state)
		, wIdentString(ir_init->stack->name)
	);
	for (stack = ir_init->stack->next; stack; stack = stack->next) {
		printf(" %s", wIdentString(stack->name));
	}
	printf(">)\n");
}

void printx_formula(IrFnode fnode) {
	
	if (!fnode)
		return;
	
	// var = undef
	if ((fnode->token == IR_EQ || fnode->token == IR_CMPEQ) && !fnode->right) {
		printx_formula(fnode->left);
		printf(" = UNDEF");
		return;
	}
	
	if (fnode->token == IR_FORALL || fnode->token == IR_EXISTS) {
		if (fnode->token == IR_FORALL)
			printf("(A ");
		if (fnode->token == IR_EXISTS)
			printf("(E ");
		printf("%s (%d,%d) "
			, wIdentString(fnode->left->name)
			, fnode->left->left->value, fnode->left->right->value);
		printx_formula(fnode->right);
		printf(")");
		return;
	}
	
	// keyword true and false
	if (fnode->right) {
		if (fnode->right->token == IR_FALSE) {
			printf("!");
			printx_formula(fnode->left);
			return;
		}
		if (fnode->right->token == IR_TRUE) {
			printx_formula(fnode->left);
			return;
		}
	}

	int outer_parenthesis = 0;
	if (fnode->token == IR_EQ || is_relational_op(fnode->token)) {
		printf("(");
		outer_parenthesis = 1;
	}
	
	int inner_parenthesis = 0;
	int inner_brace = 0;
	if (fnode->left) {
		if (!is_var(fnode->left->token)
			&& !is_num(fnode->left->token)) {
				if (is_logical_op(fnode->token)) {
					printf("(");
					inner_parenthesis = 1;
				}
				if (is_intterm_op(fnode->token)) {
					printf("{");
					inner_brace = 1;
				}
		}
		printx_formula(fnode->left);
		if (inner_brace) {
			printf("}");
			inner_brace = 0;
		}
		if (inner_parenthesis) {
			printf(")");
			inner_parenthesis = 0;
		}
	}
		
	switch (fnode->token) {
		case IR_NUMBER:
			printf("%d", fnode->value);
			break;
		case IR_IDENT:
			printf("%s", wIdentString(fnode->name));
			if (fnode->value == 1) printf("'");
			if (fnode->value == 2) printf("''");
			break;
		case IR_BVAR:
			printf("%s", wIdentString(fnode->name));
			if (fnode->value == 1) printf("'");
			if (fnode->value == 2) printf("''");
			break;
		case IR_QVAR:
			printf("%s", wIdentString(fnode->name));
			break;
		case IR_ELSE:
			printf("ELSE");
			break;
		case IR_EQ:
			printf("=");
			break;
		case IR_PLUS:
			printf("+");
			break;
		case IR_MINUS:
			printf("-");
			break;
		case IR_MULT:
			printf("*");
			break;
		case IR_DIV:
			printf("/");
			break;
		case IR_SHL:
			printf("<<");
			break;
		case IR_SHR:
			printf(">>");
			break;
		case IR_AND:
			printf("&");
			break;
		case IR_OR:
			printf("|");
			break;
		case IR_XOR:
			printf("^");
			break;
		case IR_LT:
			printf("<");
			break;
		case IR_LE:
			printf("<=");
			break;
		case IR_CMPEQ:
			printf("==");
			break;
		case IR_NE:
			printf("!=");
			break;
		case IR_GE:
			printf(">=");
			break;
		case IR_GT:
			printf(">");
			break;
		case IR_LGC_AND:
			printf(" & ");
			break;
		case IR_LGC_OR:
			printf(" | ");
			break;
		case IR_LGC_NOT:
			printf("!");
			break;
		case IR_LGC_XOR:
			printf("^^");
			break;
		case IR_INDEX:
			printf("[");
			printx_formula(fnode->right);
			printf("]");
			return;
	}
	
	if (fnode->right) {
		if ( !is_var(fnode->right->token)
			&& !is_num(fnode->right->token)) {
				if (is_logical_op(fnode->token)) {
					printf("(");
					inner_parenthesis = 1;
				}
				if (is_intterm_op(fnode->token)) {
					printf("{");
					inner_brace = 1;
				}
		}
		printx_formula(fnode->right);
		if (inner_brace) {
			printf("}");
			inner_brace = 0;
		}
		if (inner_parenthesis) {
			printf(")");
			inner_parenthesis = 0;
		}
	}
	
	if (outer_parenthesis) {
		printf(")");
		outer_parenthesis = 0;
	}
	
	fflush(stdout);
}

void printx_one_rule(IrRule rule) {
	
	printf("%s <%s> --> %s <"
		, wIdentString(rule->from_state)
		, wIdentString(rule->from_stack)
		, wIdentString(rule->to_state)
	);
	fflush(stdout);
	
	if (rule->to_stack1)
		printf("%s"
			, wIdentString(rule->to_stack1));
	if (rule->to_stack2)
		printf(" %s"
			, wIdentString(rule->to_stack2));
	printf(">");
	fflush(stdout);
}

void printx_rule(IrRule rule) {
	
	printx_one_rule(rule);
	
	if(rule->expr) {
		printf("\t(");
		printx_formula(rule->expr);
		printf(")");
	}
	printf("\n");
	fflush(stdout);
}

void printx_ifdo(IrIfdo ifdo) {
	
	for (; ifdo; ifdo = ifdo->next) {
		printx_formula(ifdo->cond);
		printf(" --> ");
		fflush(stdout);
		printx_stmt(ifdo->stmt);
		fflush(stdout);
	}
	fflush(stdout);
}

void printx_exprlist(IrExprList exprlist) {
	
	for (; exprlist; exprlist = exprlist->next) {
		printx_formula(exprlist->expr);
		printf(" ");
		fflush(stdout);
	}
}

void printx_call(IrCall ircall) {
	
	printf("CALL %s(", wIdentString(ircall->module->name));
	printx_exprlist(ircall->params);
	printf(")\n");
	fflush(stdout);
}

void printx_stmt(IrStmt stmt) {
	
	for (; stmt; stmt = stmt->next) {
		if (stmt->label) {
			printf("%s: ", wIdentString(stmt->label));
			fflush(stdout);
		}
		
		switch(stmt->type) {
			
			case IR_RAWTRANS:
				printx_rule((IrRule) stmt->content);
				break;
			
			case IR_IF:
				printf("IF\n");
				printx_ifdo((IrIfdo) stmt->content);
				printf("FI\n");
				break;
			
			case IR_DO:
				printf("DO\n");
				printx_ifdo((IrIfdo) stmt->content);
				printf("OD\n");
				break;
			
			case IR_ASSGN:
				printx_exprlist((IrExprList) stmt->content);
				printf("\n");
				break;
			
			case IR_GOTO:
				printf("GOTO %s\n", wIdentString(((IrGoto) stmt->content)->target));
				break;
			
			case IR_BREAK:
				printf("BREAK\n");
				break;
			
			case IR_SKIP:
				printf("SKIP\n");
				if (stmt->content) {
					printf("\t");
					printx_formula((IrFnode) stmt->content);
				}
				break;
			
			case IR_CALL:
				printx_call((IrCall) stmt->content);
				break;
				
			case IR_RET:
				printf("RETURN\n");
				break;
				
			case IR_FORK:
				printf("FORK ");
				printx_call(((IrFork) stmt->content)->forked);
				break;
		}
	}
}

void irPrintInfo() {
	
	printf("\n*****irPrintInfo()*****\n");
	printx_def();
	printx_global_var();
	printx_local_var();
	printx_init();
	printx_stmt(ir_stmt);
	printf("***********************\n");
}
