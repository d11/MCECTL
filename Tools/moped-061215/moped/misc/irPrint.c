#include <stdio.h>
#include <string.h>
#include "irInt.h"

void print_info() {
	
	print_def();
	print_global_var();
	print_init();
	print_module();
	print_rules();
}

void print_def() {
	
	if (!ir_def)
		return;

	IrDef def;
	printf("Define:\n");
	for (def = ir_def; def; def = def->next) {
		printf("  %s %d\n", wIdentString(def->name)
			, def->value
		);
	}
}

void print_var(char *pre, IrVar var, char *post) {
	printf("%s%s %s", pre, type_array[var->type]
		, wIdentString(var->name)
	);
	if (var->dim) {
		if (var->dim->next)
			printf("[0,%d]", (var->dim->len * var->dim->next->len) - 1);
		else
			printf("[%d,%d]", var->dim->lo, var->dim->hi);
	}
	if (var->bits)
		printf("(%d)", var->bits);
	printf("%s", post);
}

void print_struct_var(char *pre, IrVar var, char *post) {
	
	IrVar struct_var = var->extra;
	for (; struct_var; struct_var = struct_var->extra) {
		printf("%s%s %s"
			, pre
			, type_array[struct_var->type]
			, wIdentString(struct_var->name)
		);
		if (struct_var->dim) {
			if (struct_var->dim->next)
				printf("[0,%d]", (struct_var->dim->len * struct_var->dim->next->len) - 1);
			else
				printf("[%d,%d]", struct_var->dim->lo, struct_var->dim->hi);
		}
		if (struct_var->bits)
			printf("(%d)", struct_var->bits);
		printf("%s", post);
	}
}

void print_global_var() {
	
	if (!ir_gbl && !ir_ret)
		return;
		
	IrGbl gbl;
	printf("Global:\n");
	for (gbl = ir_gbl; gbl; gbl = gbl->next) {
		if (gbl->var->type == IR_STRUCT)
			print_struct_var("  ", gbl->var, "\n");
		else
			print_var("  ", gbl->var, "\n");
	}
	for (gbl = ir_ret; gbl; gbl = gbl->next) {
		print_var("  ", gbl->var, "\n");
	}
}

void print_init() {
	
	printf("Init:\n");
	printf("  ");
	if (ir_init->state) printf("%s "
		, wIdentString(ir_init->state)
	);
	printf("<%s>"
		, wIdentString(ir_init->stack)
	);
	printf("\n");
}

void print_module() {
	
	if (!ir_module)
		return;
	
	IrModule module;
	printf("Module:\n");
	for (module = ir_module; module; module = module->next) {
		
		//print return type & module name
		printf("  %s %s("
			, type_array[module->retvar->type]
			, wIdentString(module->name)
		);
		
		//print parameters
		IrVarList varlist;
		if (module->param) {
			varlist = module->param;
			print_var("", varlist->var, "");
			for (varlist = varlist->next; varlist; varlist = varlist->next)
				print_var(", ", varlist->var, "");
		}
		printf(")\n");
		
		//print symbols
		IrName name1;
		printf("    Symbols:");
		for (name1 = module->symbols; name1; name1 = name1->next)
			printf(" %s", wIdentString(name1->name));
		printf("\n");
		
		//print local variables
		if (module->localvar)
			printf("    Variables:\n");
		for (varlist = module->localvar; varlist; varlist = varlist->next) {
			if (varlist->var->type == IR_STRUCT)
				print_struct_var("      ", varlist->var, "\n");
			else
				print_var("      ", varlist->var, "\n");
		}
	}
}

void print_rules() {
	
	IrRule rule;
	printf("Rule:\n");
	for (rule = ir_rule; rule; rule = rule->next) {
		print_one_rule(rule);
		
		if(rule->expr || rule->preserved) {
			printf("\t(");
			if (rule->expr) {
				print_formula(rule->expr);
				if (rule->expr->extra && rule->expr->extra->token == IR_ELSE)
					print_else(rule->expr->extra->extra);
				if (rule->expr->right && rule->expr->right->extra && rule->expr->right->extra->token == IR_ELSE)
					print_else(rule->expr->right->extra->extra);
			}
			printf(")");
			if (rule->preserved)
				print_preserved_simplified(rule);
		}
		printf("\n");
	}
	fflush(stdout);
}

void print_formula(IrFnode fnode) {
	
	if (!fnode)
		return;
	
	if (fnode->token == IR_ARGS || fnode->token == IR_PAR) {
		print_next(fnode->next);
		return;
	}
	
	if (fnode->token == IR_FORALL || fnode->token == IR_EXISTS) {
		if (fnode->token == IR_FORALL)
			printf("A ");
		if (fnode->token == IR_EXISTS)
			printf("E ");
		IrQnt qnt;
		for (qnt = ir_qnt; qnt; qnt = qnt->next)
			if (qnt->name == fnode->left->name)
				break;
		if (!qnt)
			printf("error in ir_qnt\n");
		printf("%s (%d,%d) "
			, wIdentString(qnt->name)
			, qnt->lower, qnt->upper);
		print_formula(fnode->right);
		return;
	}

	print_formula(fnode->left);
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
		case IR_TRUE:
			printf("true");
			break;
		case IR_FALSE:
			printf("false");
			break;
		case IR_EQ:
			printf("==");
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
			printf(" && ");
			break;
		case IR_LGC_OR:
			printf(" || ");
			break;
		case IR_LGC_NOT:
			printf("!");
			break;
		case IR_XOR:
			printf("^");
			break;
		case IR_PAR:
			printf(" && ");
			break;
		case IR_INDEX:
			printf("[");
			print_formula(fnode->right);
			printf("]");
			return;
		case IR_ELSE:
			print_else(fnode->next);
			break;
	}
	print_formula(fnode->right);
}

void print_else(IrFnode fnode) {
	if (!fnode)
		return;
	for (; fnode; fnode = fnode->extra) {
		printf(" !(");
		print_formula(fnode);
		printf(")");
	}
}

void print_next(IrFnode fnode) {
	if (!fnode)
		return;
	for (; fnode; fnode = fnode->next) {
		printf(" ");
		print_formula(fnode);
	}
}

void print_one_rule(IrRule rule) {
	
	printf("  %s <%s> --> %s <"
		, rule->from_state != -1 ? wIdentString(rule->from_state) : "XXX"
		, rule->from_stack != -1 ? wIdentString(rule->from_stack) : "XXX"
		, rule->to_state != -1 ? wIdentString(rule->to_state) : "XXX"
	);
	fflush(stdout);
	
	if (rule->to_stack1)
		printf("%s"
			, rule->to_stack1 != -1 ? wIdentString(rule->to_stack1) : "XXX");
	if (rule->to_stack2)
		printf(" %s"
			, rule->to_stack2 != -1 ? wIdentString(rule->to_stack2) : "XXX");
	printf(">");
	fflush(stdout);
}

void print_preserved_simplified(IrRule rule) {
	
	IrIndVar pvar;
	printf(" <Preserved:");
	for (pvar = rule->preserved; pvar; pvar = pvar->next) {
		if (pvar->ind) {
			printf(" !%s[", wIdentString(pvar->var->name));
			print_formula(pvar->ind);
			printf("]");
		}
		else
			printf(" %s", wIdentString(pvar->var->name));
	}
	printf(">");
}

void print_preserved(IrRule rule) {
	
	IrIndVar pvar = rule->preserved;
	for (pvar = rule->preserved; pvar; pvar = pvar->next) {
		if (pvar->var->dim) {
			if (pvar->var->dim->next)
				printf(" && A i (0,%d) "
					, pvar->var->dim->len * pvar->var->dim->next->len - 1);
			else
				printf(" && A i (%d,%d) "
					, pvar->var->dim->lo, pvar->var->dim->hi);
		}
		else
			printf(" && ");
		
		printf("%s%s==%s"
			, wIdentString(pvar->var->name)
			, (rule->to_stack2 != 0 && pvar->scope == 2) ? "''" : "'"
			, wIdentString(pvar->var->name));
	}
}

