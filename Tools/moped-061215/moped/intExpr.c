/*****************************************************************************/

#include "alloc.h"
#include "int.h"

/***************************************************************************/

intExpr* intExprBinary (char type, intExpr *left, intExpr *right)
{
	intExpr *expr = Calloc(1,sizeof(struct intExpr));

	expr->type = type;
	expr->left = left;
	expr->right = right;

	return expr;
}

intExpr* intExprQuantified (char type, intQuantifier *q, intExpr *subexpr)
{
	intExpr *expr = Calloc(1,sizeof(struct intExpr));

	expr->type = type;
	expr->q = q;
	expr->left = subexpr;

	return expr;
}

intExpr* intExprVarRef (intVariable *var, char primes, intExpr *index)
{
	intExpr *expr = Calloc(1,sizeof(struct intExpr));

	expr->type = EXPR_VAR;
	expr->vref = Malloc(sizeof(struct intVarRef));
	expr->vref->var = var;
	expr->vref->primes = primes;
	expr->vref->index = index;

	return expr;
}

intExpr* intExprQvar (intQuantifier *q)
{
	intExpr *expr = Calloc(1,sizeof(struct intExpr));

	expr->type = EXPR_QVAR;
	expr->q = q;

	return expr;
}

intExpr* intExprConst (int value)
{
	intExpr *expr = Calloc(1,sizeof(struct intExpr));

	expr->type = EXPR_CONST;
	expr->value = value;

	return expr;
}

intQuantifier* intCreateQuantifier (wIdent name, int lower, int upper)
{
	intQuantifier *q = Malloc(sizeof(struct intQuantifier));

	q->name = name;
	q->lower = lower;
	q->upper = upper;

	return q;
}

/***************************************************************************/

static void intExprDeleteVrefs (intVarRef *vref)
{
	while (vref)
	{
		intVarRef *tmp = vref->next;
		Free(vref);
		vref = tmp;
	}
}

void intExprDelete (intExpr *expr)
{
	intVarRef *vref;

	if (!expr) return;
	switch (expr->type)
	{
	    case EXPR_OR:   case EXPR_AND:   case EXPR_EQV:  case EXPR_XOR:
	    case EXPR_PLUS: case EXPR_MINUS: case EXPR_MULT:
        case EXPR_DIV:  case EXPR_SHL:   case EXPR_SHR:
        case EXPR_BIT_OR: case EXPR_BIT_AND: case EXPR_BIT_XOR:
		intExprDelete(expr->left);
		intExprDelete(expr->right);
		break;
	    case EXPR_LT:   case EXPR_LE:    case EXPR_EQ:
	    case EXPR_NE:   case EXPR_GE:    case EXPR_GT:
		intExprDelete(expr->left);
		intExprDelete(expr->right);
		intExprDeleteVrefs((intVarRef*)(expr->value));
		break;
	    case EXPR_NOT:
		intExprDelete(expr->left);
		break;
	    case EXPR_FORALL:
	    case EXPR_EXISTS:
		intExprDelete(expr->left);
		Free(expr->q);
		break;
	    case EXPR_VAR:
		vref = expr->vref;
		if (vref->index) intExprDelete(vref->index);
		if (vref->var->type == 0 && vref->index)
			intExprDeleteVrefs((intVarRef*)(expr->value));
		if (vref->var->type == 0) Free(vref);
		break;
	    case EXPR_QVAR:
	    case EXPR_CONST:
		break;
	    default:
		fprintf(stderr,"unknown token in intFreeExpr\n");
		exit(1);
	}
	Free(expr);
}

/***************************************************************************/
