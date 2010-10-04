/**CFile***********************************************************************

  FileName    [PredicateNormaliser.c]

  PackageName [compile]

  Synopsis    [A Predicate-Normaliser class]

  Description [See PredicateNormaliser.h for more info]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2. 
  Copyright (C) 2005 by ITC-irst. 

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information on NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/


#include "PredicateNormaliser.h"
#include "compile/type_checking/type_checking.h"
#include "compile/compileInt.h"
#include "utils/WordNumber.h" 
#include "utils/utils.h"
#include "utils/assoc.h"
#include "parser/symbols.h"

/*---------------------------------------------------------------------------*/
static char rcsid[] UTIL_UNUSED = "$Id: PredicateNormaliser.c,v 1.1.2.4.4.6 2007/05/09 15:43:28 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Type************************************************************************

  Synopsis    [Preicate Normaliser class]

  Description []

  SeeAlso     []

******************************************************************************/
typedef struct PredicateNormaliser_TAG 
{
  hash_ptr expr2normalisedPredicate; /* hash of 
					expr -> predicate-normalised expr */
  TypeChecker_ptr checker; /* type-checker is used to get type info
			      of processed expressions and type check
			      generated expressions */
} PredicateNormaliser;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
/**Macro***********************************************************************

  Synopsis    [A switcher is used force the generated 
  expressions to have linearised ITE only , or allow to have nested ITE.]

  Description [If this macro can be :
   0 -- indicates that all created ITE should be linear, i.e. there
   should not be nested (not-boolean) ITEs.
   1 -- indicates that (not-boolean) ITE can be nested.

   See pred_norm_push_ite_up for more examples.]

  SeeAlso     [pred_norm_push_ite_up]

******************************************************************************/
#define ALLOW_NESTED_CASES 1


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void pred_norm_init ARGS((PredicateNormaliser_ptr self,
				 TypeChecker_ptr checker));
static void pred_norm_deinit ARGS((PredicateNormaliser_ptr self));

static node_ptr pred_norm_normalise(PredicateNormaliser_ptr normaliser,
				    node_ptr expr,
				    node_ptr context);
static node_ptr pred_norm_find_node(int kind, node_ptr oper1, node_ptr oper2);
static boolean pred_norm_is_true_bool_exp(PredicateNormaliser_ptr self,
					  node_ptr expr, node_ptr context);

static node_ptr pred_norm_bool2int(node_ptr expr);
static node_ptr pred_norm_bool2word1(node_ptr expr);

static node_ptr pred_norm_push_ite_up(int op, node_ptr oper1, node_ptr oper2);
static node_ptr pred_norm_push_ite_up_conditioned(node_ptr condition, int op,
						  node_ptr exp1, node_ptr exp2,
						  node_ptr tail);
static node_ptr pred_norm_normalise_ite(node_ptr cond,
					node_ptr then,
					node_ptr tail);

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [The constructor creates a predicate-normaliser]

  Description [See PredicateNormaliser.h for more info on
  predicate normalisation.  The paramer 'checker' is a type checker
  used during predication-normalisation, and subsequent type checking of 
  generated expressions.
  NOTE that the type checker remember the type of checked expressions.]

  SideEffects        []

******************************************************************************/
PredicateNormaliser_ptr PredicateNormaliser_create(TypeChecker_ptr checker)
{
  PredicateNormaliser_ptr self = ALLOC(PredicateNormaliser, 1);

  PREDICATE_NORMALISER_CHECK_INSTANCE(self);

  pred_norm_init(self, checker);
  return self;
}


/**Function********************************************************************

  Synopsis           [Class PredicateNormaliser destructor]

  Description        []

  SideEffects        []

******************************************************************************/
void PredicateNormaliser_destroy(PredicateNormaliser_ptr self)
{
  PREDICATE_NORMALISER_CHECK_INSTANCE(self);
  
  pred_norm_deinit(self);

  FREE(self);
}


/**Function********************************************************************

  Synopsis           [The function performs predicate-normalisation of
  a generic expression]

  Description [The function returns a new expression -- a 
  predicate-normalised version of the given one.
  A predicate normalised expression is a symbolic expression (Sexp) 
  whose not-boolean subexpressions do not encompass boolean subexpression.
  For example, not predicate-normalised expression
     "case a : 3; 1 : 4; esac + 2 = 7" 
  after normalisatio becomes 
     "case a : 3 + 2 = 7; 1 : 4 + 2 = 7; esac" 
  
  The provided expression should be scalar (since there is no
  meaning to normalise booleanised expressions).  The given
  expression must be already type checked (by the type checker
  given to the constructor), but may be or may not be flattened
  and expanded.

  During normalisation the type checker (given during construction) is 
  used to distinguish boolean from not-boolean expressions.
  Note that the generated expressions are type checked, i.e. the type 
  checker remember their types.

  The normaliser caches processed expressions. So, if the same expression is
  given, the same result will be returned.
  
  The returned expression is always flattened and expanded. Also, it 
  is created by find_node function, therefore it belongs to
  the node package (i.e. do not modify it).
  ]

  SideEffects        []

******************************************************************************/
node_ptr 
PredicateNormaliser_normalise_expr(PredicateNormaliser_ptr self, node_ptr expr)
{
  int lineno_tmp;
  node_ptr result;

  /* Nil returned as it is. Probably, only CONS-exprs may have Nil inside */
  if (Nil == expr) return Nil;

  /* new node will be created with for sure error line number */
  lineno_tmp = yylineno;
  yylineno = -1;
  
  /* expressions on the top level are connected by AND and CONS, they are
     not normalised as a whole but one by one.
  */
  if (AND == node_get_type(expr) || CONS == node_get_type(expr)) {
    node_ptr left = PredicateNormaliser_normalise_expr(self, car(expr));
    node_ptr right = PredicateNormaliser_normalise_expr(self, cdr(expr));
    result = find_node(node_get_type(expr), left, right);
  }
  else {
    /* this is a usual expression */
    result = pred_norm_normalise(self, expr, Nil);
  }
  
  yylineno = lineno_tmp; /* restore line number */
  return result;
 }


/**Function********************************************************************

  Synopsis           [The function performs predicate-normalisation of
  specification (SPEC, LTLSPEC, etc)]

  Description [This function does the same things as
  PredicateNormaliser_normalise_expr, except that the input expression
  must be a specification (such expression are returned by Prop_get_expr_core)
  ]

  See also           [PredicateNormaliser_normalise_expr]

  SideEffects        []

******************************************************************************/
node_ptr 
PredicateNormaliser_normalise_specification(PredicateNormaliser_ptr self, 
					    node_ptr expr)
{
  node_ptr result;
  int lineno_tmp;
  boolean isTypeOK;
  
  /* new node will be created with for sure error line number */
  lineno_tmp = yylineno;
  yylineno = -1;
  
  result = pred_norm_normalise(self, expr, Nil);
  
  /* perform type checking .
     The type of the property is of no importance since
     original expressions is already type checked and the normalisation
     keeps this correctness (therefore, no error messages can be). 
     Actually, the type checking is performed just to remember
     the type of generated expressions.
  */
  isTypeOK = TypeChecker_is_specification_wellformed(self->checker,
					     find_node(Prop_Ctl, result, Nil));
  nusmv_assert(isTypeOK);/* the type should be always OK */

  yylineno = lineno_tmp; /* restore line number */					     
  return result;
}


/**Function********************************************************************

  Synopsis           [The function prints out the expressions's
  predicates, i.e. subexpressions which
  have have not-boolean operands.]

  Description [Only expressions obtained with
  PredicateNormaliser_normalise_expr and with the same
  predicate-normaliser (i.e. 'self') can be given to this function.

  This function just walks the exressions, tries to find 
  a subexpression with not-boolean operands then print it.
  ]

  SideEffects        []

******************************************************************************/
EXTERN void 
PredicateNormaliser_print_predicates_only(FILE* stream,
					  PredicateNormaliser_ptr self,
					  node_ptr expr)
{
  SymbType_ptr type;

  if (Nil == expr) return;

  type = TypeChecker_get_expression_type(self->checker, expr, Nil);
  if (SYMB_TYPE(NULL) == type) {
    /* since expr can be a set of constrains, their connections (through 
       CONS and AND) may not be type checked
    */
    nusmv_assert(CONS == node_get_type(expr) || AND == node_get_type(expr));
    PredicateNormaliser_print_predicates_only(stream, self, car(expr));
    PredicateNormaliser_print_predicates_only(stream, self, cdr(expr));
    return;
  }
  else {
    /* only boolean (or statement) expressions can be met here */
    nusmv_assert(SymbType_is_boolean_enum(type) ||
		 SYMB_TYPE_STATEMENT == SymbType_get_tag(type));
  }


  /* process every kind of an expression */
  switch (node_get_type(expr)) {
  case TRUEEXP:
  case FALSEEXP:
  case FAILURE:
    return;
    /* only 0 and 1 number can be met */
  case NUMBER: nusmv_assert((node_get_int(expr) >> 1) == 0); return;

  case DOT:
  case ATOM:
  case ARRAY:
    nusmv_assert(SymbType_is_boolean_enum(type)); /* only bool var can be met */
    return;
    
    /* unary expressions whose operands are boolean */
  case NEXT: case SMALLINIT:
  case NOT: 
  case EX: case AX: case EF: case AF: case EG:  case AG:
  case EBF: case ABF: case EBG: case ABG: /* ignore the number..number part */
  case ABU: case EBU: /* ignore the number..number part */ 
  case OP_NEXT: case OP_PREC: case OP_NOTPRECNOT: case OP_GLOBAL:
  case OP_HISTORICAL: case OP_FUTURE: case OP_ONCE:
    PredicateNormaliser_print_predicates_only(stream, self, car(expr));
    return;
    
    /* binary expressions whose operands are boolean */
  case AND: case OR: case XOR: case XNOR: case IFF: case IMPLIES:
  case AU: case EU: case UNTIL: case SINCE: 
      PredicateNormaliser_print_predicates_only(stream, self, car(expr));
      PredicateNormaliser_print_predicates_only(stream, self, cdr(expr));
      return;

      /* operand of bool-cast is always word[1] => print it */
  case CAST_BOOL: 
    print_node(stream, car(expr));
    fprintf(stream, "\n\n");
    return;

    /* three operands expression which do not change the type */
  case CASE:
    PredicateNormaliser_print_predicates_only(stream, self, car(car(expr)));
    PredicateNormaliser_print_predicates_only(stream, self, cdr(car(expr)));
    if (FAILURE != node_get_type(cdr(expr))) {
      PredicateNormaliser_print_predicates_only(stream, self, cdr(expr));
    }
    return;
    

    /* binary expression whose operands may not be boolean */
  case EQDEF: case SETIN: 
  case EQUAL: case NOTEQUAL: case LT: case GT: case LE: case GE: {
    TypeChecker_ptr checker = self->checker;
    SymbType_ptr type1 = TypeChecker_get_expression_type(checker,
							 car(expr), Nil);
    SymbType_ptr type2 = TypeChecker_get_expression_type(checker,
							 cdr(expr), Nil);

    /* after normalisation both operands must be boolean or not boolean */
    nusmv_assert( (SymbType_is_boolean_enum(type1) &&
		   SymbType_is_boolean_enum(type2)) ||
		  (!SymbType_is_boolean_enum(type1) &&
		   !SymbType_is_boolean_enum(type2)));

    if (SymbType_is_boolean_enum(type1)) {
      PredicateNormaliser_print_predicates_only(stream, self, car(expr));
      PredicateNormaliser_print_predicates_only(stream, self, cdr(expr));
    } 
    else {
      print_node(stream, expr);
      fprintf(stream, "\n\n");
    }
    
    return;
  } 

  case CONTEXT:  /* after normalisation the expr should be expanded */
    /* thes exps should have beem met earlier*/
  case NUMBER_WORD:
  case NUMBER_FRAC: 
  case NUMBER_REAL:
  case NUMBER_EXP:
  case TWODOTS: 
    /* not- boolean expression cannot be met here */
  case TIMES: case DIVIDE: case PLUS :case MINUS: case MOD: 
  case LSHIFT: case RSHIFT: /*case LROTATE: case RROTATE: */
  case BIT_SELECTION:
  case WAREAD: case WAWRITE:
  case CONCATENATION: 
  case COLON:
  case CAST_WORD1: /* normalisation have already converted CAST_WORD1 to ITE*/
  case UNION: /* UNION is no impelemented at the moment */
    nusmv_assert(false);
    return;
  
  default:
    print_sexp(nusmv_stderr, expr);
    nusmv_assert(false); /* unknown kind of an expression */
    return;
  } /* switch */
}


/*---------------------------------------------------------------------------*/
/* Static function definitions                                               */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [initialiser of an object of this class]

  Description        []

  SideEffects        []

******************************************************************************/
static void pred_norm_init(PredicateNormaliser_ptr self,
			    TypeChecker_ptr checker)
{
  nusmv_assert(TYPE_CHECKER(NULL) != checker);
  
  self->expr2normalisedPredicate = new_assoc();
  self->checker = checker;
}


/**Function********************************************************************

  Synopsis           [de-initialiser of an object of this class]

  Description        []

  SideEffects        []

******************************************************************************/
static void pred_norm_deinit(PredicateNormaliser_ptr self)
{
  nusmv_assert(TYPE_CHECKER(NULL) != self->checker);
  /* check this is initialised object */
  nusmv_assert((hash_ptr)NULL != self->expr2normalisedPredicate);
  
  clear_assoc(self->expr2normalisedPredicate);

  self->expr2normalisedPredicate = (hash_ptr)NULL;
  self->checker = TYPE_CHECKER(NULL);
}


/**Function********************************************************************

  Synopsis           [Performs the predicate-normalisation of an expression]

  Description        [See PredicateNormaliser_normalise_expr
  for more info on predicate-normalisation.
  This function is the main part of normalisation.

  NB for developers: the main idea behind this fucntion is that
  if a not-boolean operation is applied (to one or two not-boolean operands),
  then ITE from operands must be pushed up to the root of the expression.
  For example,
     "case a : 3; true : 5; esac + 400" 
  is converted to 
      "case a : 3 + 400; true : 5 + 400; esac"
  i.e. after convertion not-boolean operand will not have ITE, and
  therefore will not have boolean subexpression.
  
  Boolean expressions are not changed since boolean expressions
  may have any subexpression.
  ]

  SideEffects        []

  SeeAlso            [PredicateNormaliser_normalise_expr]

******************************************************************************/
static node_ptr pred_norm_normalise(PredicateNormaliser_ptr normaliser,
                                    node_ptr expr,
                                    node_ptr context)
{
  SymbType_ptr type;  
  node_ptr result;

  nusmv_assert(Nil != expr);

  /* already processed */
  result = find_assoc(normaliser->expr2normalisedPredicate, expr);
  if (Nil != result) return result;
  
  type = TypeChecker_get_expression_type(normaliser->checker, expr, context);

  /* process every kind of an expression */
  switch (node_get_type(expr)) {
  case CONTEXT: 
    nusmv_assert(context == Nil); /* nested contexts are not allowed */
    result = pred_norm_normalise(normaliser, cdr(expr), car(expr));
    break;

    /* list of simple operator that do not need normaliasation */
  case FAILURE:
  case FALSEEXP:
  case TRUEEXP:
  case NUMBER:
  case NUMBER_WORD:
  case NUMBER_FRAC: 
  case NUMBER_REAL: 
  case TWODOTS:
    result = find_node(node_get_type(expr), car(expr), cdr(expr));
    break;

  case DOT:
  case ATOM:
  case ARRAY: {
    /* The expression is a symbol.
       It can be a variable, a define, a constant or a parameter.
       The expression may not have been flattened as well as not flattened.

       Note, that NO ERRORS CAN BE HERE, since all the error
       situations have been checked during type-checking of the 
       original expression.
    */
    
    /* First, try to resolve the symbol */
    node_ptr simpleName = find_atom(expr);

    SymbTable_ptr symbolTable = 
      TypeChecker_get_symbol_table(normaliser->checker);

    node_ptr resolvedName = 
      CompileFlatten_resolve_name(symbolTable, simpleName, context);

    
    /* Check whether this is a variable */
    if (SymbTable_is_symbol_var(symbolTable, resolvedName)) {
      result = resolvedName;
    }
    
    /* check whether is a define */
    else if (SymbTable_is_symbol_define(symbolTable,  resolvedName)) {
      node_ptr def = SymbTable_get_define_flatten_body(symbolTable,
                                                       resolvedName);
      /* the context is Nil because expr is already flattened */
      result = pred_norm_normalise(normaliser, def, Nil);
    }

    /* check whether this symbol is a (simple) constant */
    else if (ATOM == node_get_type(simpleName) &&
             SymbTable_is_symbol_constant(symbolTable, simpleName)) {
      result = simpleName;
    }
    /* check whether this symbol is a (generated) constant */
    else if (SymbTable_is_symbol_constant(symbolTable, resolvedName)) {
      result = resolvedName;
    }
    /* check whether this symbol is a parameter */
    else {  
      node_ptr param = lookup_param_hash(resolvedName);
      nusmv_assert(param != Nil); /* it must be a parameter, since the
                                     expr is already type-checked, but
                                     being a parameter is the last
                                     possibility
                                  */
      result = pred_norm_normalise(normaliser, param, context);
    }

    break;
  } /* ATOM */
    
    /* unary expression */
  case EX: case AX: case EF: case AF: case EG: case AG:
  case OP_NEXT: case OP_PREC: case OP_NOTPRECNOT: case OP_GLOBAL:
  case OP_HISTORICAL: case OP_FUTURE: case OP_ONCE:
  case EBF: case ABF: case EBG: case ABG: /* ignore the number..number part */
  case ABU: case EBU: /* ignore the number..number part */  
    nusmv_assert(SymbType_is_boolean_enum(type)); /* only boolean can be here */

  case UMINUS:
  case NEXT: 
  case SMALLINIT:
  case NOT: {
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    /* do not normalise right operand if any */
    node_ptr oper2 = find_atom(cdr(expr));

    if (SymbType_is_boolean_enum(type)) { /* this is boolean => just return */
      result = find_node(node_get_type(expr), oper1, oper2);
    }
    else { /* this is not a boolean => push all ITE up */
      result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);
    }

    break;
  }

    /* binary expression */
  case AND: case OR: case XOR: case XNOR: case IFF: case IMPLIES:
  case AU: case EU: 
  case UNTIL: case SINCE: {
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    node_ptr oper2 = Nil == cdr(expr) ? Nil
      : pred_norm_normalise(normaliser, cdr(expr), context);

    if (SymbType_is_boolean_enum(type)) { /* this is boolean => just return */
      result = pred_norm_find_node(node_get_type(expr), oper1, oper2);
    }
    else { /* this is not a boolean => push all ITE up */
      result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);
    }

    break;
  }

    /* relational operators */
  case EQDEF: case SETIN: 
  case EQUAL: case NOTEQUAL: case LT: case GT: case LE: case GE: {
    TypeChecker_ptr checker = normaliser->checker;
    SymbType_ptr type1 = TypeChecker_get_expression_type(checker,
                                                         car(expr), context);
    SymbType_ptr type2 = TypeChecker_get_expression_type(checker,
                                                         cdr(expr), context);
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    node_ptr oper2 = pred_norm_normalise(normaliser, cdr(expr), context);
    
    /* some type covertion may be required */
    /* both operands are boolean */
    if (SymbType_is_boolean_enum(type1) && SymbType_is_boolean_enum(type2)) {
      /* do nothing; just return */
      result = pred_norm_find_node(node_get_type(expr), oper1, oper2);
    } 
    /* left is boolean and the right is word[1] => convert word1 to bool */
    else if (SymbType_is_boolean_enum(type1) && SymbType_is_word_1(type2)) {
      result = pred_norm_find_node(node_get_type(expr), oper1,
                                   find_node(CAST_BOOL, oper2, Nil));
    } 
    /* left is word[1] and the right is boolean => convert word1 to bool */
    else if (SymbType_is_word_1(type1) && SymbType_is_boolean_enum(type2)) {
      result = pred_norm_find_node(node_get_type(expr),
                                   find_node(CAST_BOOL, oper1, Nil),
                                   oper2);
    } 
    /* both operands are scalar => convert "true boolean" operand (if
       ther is one) to 1, and then push all ITE up from operands.
       For example, (5 mod 2) = 4 should not be converted, but
       (5 = 3) > 4 should be converted to "ITE(5=3, 1, 0) > 4" then
       normalisation shoud push the ITE up.
       NB: Originally "(5 mod 2)", "0", "1" are all boolean.
    */
    else {
      if (pred_norm_is_true_bool_exp(normaliser, car(expr), context)) {
        oper1 = pred_norm_bool2int(oper1);
      }
    
      if (pred_norm_is_true_bool_exp(normaliser, cdr(expr), context)) {
        oper2 = pred_norm_bool2int(oper2);
      }
      result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);
    }

    break;
  }
    
    /* these exprs are always scalar, so only
       "true boolean" operand should be converted to integer.
       The pushing the ITE up from operands is always performed.
       (even though "exp mod 2" is a boolean expression,
       there is no need to deal with it as with boolean).
    */
  case TIMES: case DIVIDE: case PLUS :case MINUS: case MOD: 
  case LSHIFT: case RSHIFT: /*case LROTATE: case RROTATE: */
  case WAREAD: case WAWRITE: { /* these two WordArray operators cannot have
                                  boolean operands (type checked already).
                                  WAWRITE has actually three-operands
                                  but the second and third are dealt recursively
                               */
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    node_ptr oper2 = pred_norm_normalise(normaliser, cdr(expr), context);
    
    if (pred_norm_is_true_bool_exp(normaliser, car(expr), context)) {
      oper1 = pred_norm_bool2int(oper1);
    }
    
    if (pred_norm_is_true_bool_exp(normaliser, cdr(expr), context)) {
      oper2 = pred_norm_bool2int(oper2);
    }
    
    result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);

    break;
  }

    /* COLON can go as a independent operation */ 
  case COLON: nusmv_assert(false);

  case BIT_SELECTION: { 
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    node_ptr oper2 = find_node(COLON, find_atom(car(cdr(expr))),
                               find_atom(cdr(cdr(expr))));
    
    /*just consistency check*/
    nusmv_assert(COLON == node_get_type(cdr(expr)));
    nusmv_assert(NUMBER == node_get_type(car(cdr(expr))));
    nusmv_assert(NUMBER == node_get_type(cdr(cdr(expr))));
    
    result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);
    break;
  }
    
    /* concatenation requires two word arguments (i.e. scalar).
       the only required thing is to convert bool to word1.
    */
  case CONCATENATION: {
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    node_ptr oper2 = pred_norm_normalise(normaliser, cdr(expr), context);
    
    if (pred_norm_is_true_bool_exp(normaliser, car(expr), context)) {
      oper1 = pred_norm_bool2word1(oper1);
    }
    
    if (pred_norm_is_true_bool_exp(normaliser, cdr(expr), context)) {
      oper2 = pred_norm_bool2word1(oper2);
    }
    
    result = pred_norm_push_ite_up(node_get_type(expr), oper1, oper2);

    break;
  }
    /* pushes down bool cast if operand is a case */
  case CAST_BOOL: {
    node_ptr oper1 = pred_norm_normalise(normaliser, car(expr), context);
    nusmv_assert(cdr(expr) == Nil);

    result = pred_norm_push_ite_up(node_get_type(expr), oper1, Nil);

    /* RC: disabled old code 
       result = find_node(node_get_type(expr),
       pred_norm_normalise(normaliser, car(expr), context),
       Nil);    
    */
    break;
  }

  case CAST_WORD1:
    result = pred_norm_bool2word1(pred_norm_normalise(normaliser, car(expr),
                                                      context));
    break;

    /* UNION's operands are sets, i.e. cannot be boolean
       and therefore they are always scalar. 
       Boolean operands should be converted to integer and ITEr moved then up,
       but currently it is impossible to distinguish true boolean operand 
       from a set boolean operands, i.e. "1" from "{1,0}".
       I.e. we do not know then to apply bool2int function.
       Instead, we normalise each element in the set.
    */ 
  case UNION: 
    result = find_node(UNION, 
                       pred_norm_normalise(normaliser, car(expr), context), 
                       pred_norm_normalise(normaliser, cdr(expr), context));
    break;

  case CASE: {
    node_ptr cond, then, tail;
    nusmv_assert(COLON == node_get_type(car(expr)));
    cond = pred_norm_normalise(normaliser, car(car(expr)), context);
    then = pred_norm_normalise(normaliser, cdr(car(expr)), context);
    tail = pred_norm_normalise(normaliser, cdr(expr), context);

    if (SymbType_is_boolean_enum(type)) {
      /* this is boolean ITE  => do nothing */
      result = find_node(CASE, find_node(COLON, cond, then), tail);
    }
    else result = pred_norm_normalise_ite(cond, then, tail);

    break;
  }
    
  default:
    print_sexp(nusmv_stderr, expr);
    nusmv_assert(false); /* unknown kind of an expression */
  } /* switch */

  /* remember the processed expression */
  /* the expression cannot be processed twice */
  nusmv_assert(Nil == find_assoc(normaliser->expr2normalisedPredicate, expr));
  insert_assoc(normaliser->expr2normalisedPredicate, expr, result);
  return result;
}

/**Function********************************************************************

  Synopsis           [Create a new node with the help of find_node function]

  Description         [This function is the same as find_node, except that if 
  the operator is commutative then operands are ordered before applying
  find_node. This may result in a greater sharing.

  Note that it is necessary to use find_node during construction
  of the expression, because later subexpressions with not-boolean operands
  will be assigned a new boolean var. Since find_node is used,
  the same var will be assigned to the same subexpression (because pointer 
  will be the same).]

  SideEffects        []

******************************************************************************/
static node_ptr pred_norm_find_node(int kind, node_ptr oper1, node_ptr oper2)
{
  node_ptr tmp;
  switch (kind) {
    /* commutative operations */
  case AND: case OR: case XOR: case XNOR: case IFF: 
  case EQUAL: case NOTEQUAL:  case TIMES: case PLUS:
  case UNION:
    /* order the operands (somehow) */
    if (oper1 > oper2) {
      tmp = oper1;
      oper1 = oper2;
      oper2 = tmp;
    }
    break;

    /* noncommutative operations */
  case FALSEEXP: case TRUEEXP: case FAILURE:
  case DOT:  case ATOM:  case SELF:  case ARRAY:  case BIT_SELECTION:
  case NEXT: case SMALLINIT: case NOT: case UMINUS: case IMPLIES: 
  case COLON: case CASE:  case CAST_WORD1:  case CAST_BOOL: 
  case EQDEF:  /* I think, EQDEF is not commutative */
  case LT: case GT: case LE: case GE:
  case DIVIDE: case MINUS: case LSHIFT: case RSHIFT: case MOD:
  case NUMBER: case NUMBER_WORD: case NUMBER_FRAC: case NUMBER_REAL: 
  case NUMBER_EXP:
  case TWODOTS:  case CONCATENATION: 
  case EX: case AX: case EF: case AF: case EG: case AG: 
  case ABU: case EBU: case EBF: case ABF: case EBG: case ABG: 
  case AU: case EU:     
  case OP_NEXT: case OP_PREC: case OP_NOTPRECNOT: case OP_GLOBAL:
  case OP_HISTORICAL: case OP_FUTURE: case OP_ONCE:
  case UNTIL: case SINCE: case CONTEXT:
  case WAREAD: case WAWRITE:
    break;

  default:
    print_sexp(nusmv_stderr, new_node(kind, oper1, oper2));
    nusmv_assert(false); /* unknown kind of an expression */
  } /* switch */
  
  tmp = find_node(kind, oper1, oper2);
  return tmp;
}

/**Function********************************************************************

  Synopsis           [Returns true is the given expression is a boolean
  expression]

  Description        [This function returns true if the expression
  is "really boolean", i.e. has a boolean types and 
  NOT constants "0" and "1" and NOT mod-operation.
  (Currently, "0", "1" and "expr mod 2" is officially a boolean expression).

  This function is used to detect cases when a boolean expression
  must be converted to an integer one. And this function decreases the number
  of such conversions.]

  SideEffects        []

******************************************************************************/
static boolean pred_norm_is_true_bool_exp(PredicateNormaliser_ptr normaliser, 
					  node_ptr expr,
					  node_ptr context)
{
  return (SymbType_is_boolean_enum(
	   TypeChecker_get_expression_type(normaliser->checker, expr, context))
  && (NUMBER != node_get_type(expr) || 0 != (NODE_TO_INT(car(expr)) >> 1))
  && MOD != node_get_type(expr));
}


/**Function********************************************************************

  Synopsis           [Casts a boolean expression to an integer expression]

  Description        [the function takes an boolean expression and
   converts its to integer by the following way:
   if "expr" is original expressions, then the result is
   "case expr: 1; 0; esac"]

  SideEffects        []

******************************************************************************/
static node_ptr pred_norm_bool2int(node_ptr expr)
{
  return find_node(CASE, find_node(COLON, expr, one_number),
		   zero_number);
}


/**Function********************************************************************

  Synopsis           [Cast a boolean expression to a Word[1] expression]

  Description        [ the function takes an boolean expression and
   converts its to Word[1] by the following way:
   if "expr" is an original expressions then the result is
   "case expr: 0b1_1; 0b1_0; esac"
  ]

  SideEffects        []

******************************************************************************/
static node_ptr pred_norm_bool2word1(node_ptr expr)
{
  return find_node(CASE, find_node(COLON, expr,
				   find_node(NUMBER_WORD,
					     (node_ptr)WordNumber_from_integer(1,1),
					     Nil)),
		   find_node(NUMBER_WORD,
			     (node_ptr)WordNumber_from_integer(0,1),
			     Nil));
}



/**Function********************************************************************

  Synopsis [Applies an operators to two (or one) not-boolean operands
  The returned value is predicate-normalised]

  Description        [The function takes an operator and its two operands and 
   return an expression of applying the operator to operands.
   The operands are assumed to be not-boolean.
   The distinguishing feature of the function is that 
   if operands are ITE expresssions, then ITE will be pushed up from 
   operands to the root of the result expression. For example,
   if input operator is "+",
   the left operator is "case a1 : e1; a2 : e2; 1 : e3; esac"
   the right opeator is "case b1 : s1; b2 : s2; 1 : s3; esac"
   then the result will be (if nested ITE are not allowed, see
   ALLOW_NESTED_CASES)
       "case a1 & b1 : e1 + s1;
             a1 & b2 : e1 + s2;
	     a1      : e1 + s3;
	     a2 & b1 : e2 + s1;
	     a2 & b2 : e2 + s2;
	     a2      : e2 + s3;
	     b1      : e3 + s1;
	     b2      : e3 + s2;
	     1       : e3 + s3;
	esac"
   another possible result can be (if nested ITE are allowed):
       "case a1 : case b1 : e1 + s1;
                       b2 : e1 + s2;
		       1  : e1 + s3; esac;
	     a2 : case b1 : e2 + s1;
	               b2 : e2 + s2;
		       1  : e2 + s3; esac;
	     b1 : e3 + s1;
	     b2 : e3 + s2;
	     1  : e3 + s3;
	esac"
   Macro ALLOW_NESTED_CASES switches these two behaviours.

   If the given operator is unary then the oper2 is Nil.

   NB: If the operator is unary, the second operand will be Nil.

   NB: if the operator is NEXT or SMALLINIT, then it, of course, applied to
   the conditions of ITE also (not only to the second and third operators).
]

  SideEffects        []

******************************************************************************/
static node_ptr pred_norm_push_ite_up(int op, node_ptr oper1, node_ptr oper2) {
  /* Note : 
     NEXT and SMALLINIT are special operators since they 
     have to be applied not only to the second and third operands of
     ITE but also to the first.
  */
  
  if (CASE == node_get_type(oper1)) {
    /* push_ite_up(ITE(c, t, e) + oper2) =
       ITE(c, puch_ite_up(t + oper2), push_ite_up(e + oper2))
       and normalise, since  oper2 - may be a list of ITE.
    */ 
    node_ptr cond = car(car(oper1));
    /* apply the operation on oper2 and oper1's tail (without first CASE).
     Of oper1's tail is FAILURE simply return this tail
    */
    node_ptr tail = (FAILURE == node_get_type(cdr(oper1))) ? cdr(oper1)
      : pred_norm_push_ite_up(op, cdr(oper1), oper2);

    if (NEXT == op || SMALLINIT == op) cond = find_node(op, cond, Nil);
    
    /* apply the operation on oper2 and the first CASE of oper1.
       Here the problem is that oper2 can be a CASE and since 
       CASEs may not be nestable, it may be necessary to push all CASE form
       oper2 to the CASE list of oper1 
    */
    return pred_norm_push_ite_up_conditioned(cond,
					     op, cdr(car(oper1)), oper2,
					     tail);
  }
  else if (Nil != oper2 && CASE == node_get_type(oper2)) {
    /* push_ite_up(oper1 + ITE(c, t, e)) =
       ITE(c, oper1 + t, push_ite_up(oper1 + e))

       oper1 - cannot be an ITE.
       Even if nested ITE are not allowed
       nor futher normalisation required since t then cannot be a list of ITE.
       NB: If oper2's tail is FAILURE then keep it as it is.
    */ 
    node_ptr cond = car(car(oper2));
    node_ptr first_expr = pred_norm_push_ite_up(op, oper1, cdr(car(oper2)));
    node_ptr second_expr
      = FAILURE == node_get_type(cdr(oper2)) ? cdr(oper2)
      : pred_norm_push_ite_up(op, oper1, cdr(oper2));

    return find_node(CASE, find_node(COLON, cond, first_expr),  second_expr);
  }
  /* both operands are not ITE */
  else {
    return pred_norm_find_node(op, oper1, oper2);
  }
}

/**Function********************************************************************

  Synopsis           [This function takes condition (cond),
  operator (op), three expressions (exp1, exp2 and tail) and creates 
  an expression ITE(cond, op(exp1, exp2), tail) and then pushes all 
  ITE from exp1 and exp2 up (if required).]
  

  Description        [The function creates a new expression with the semantics
   ITE(condition, exp1 op exp2, tail)
   'condition' is the condition of ITE,
   op - binary operation,
   exp1, exp2 - expression,
   tail - is the else-expression of created ITE.

   This function is used to normalise not-boolean expressions (exp1 and exp2).
   
   If nested ITE are not allowed, then exp1 cannot be an ITE expression, but
   exp2 can be. In this case the return value is
   a linearised (one-dimensional) ITE expression (i.e. only
   else-expression of ITE can contain another ITE). To obtain such
   form, given 'condition' will be conjuncted with all conditions in
   exp2, and exp1 will be applied to all expressions in exp2 and the
   final expression in the list of ITE will be 'tail'. 

   Otherwise, nested ITE are allowed and no normalisation is required.

   If the given operator is unary then the oper2 is Nil.]

  SideEffects        []

******************************************************************************/
static node_ptr pred_norm_push_ite_up_conditioned(node_ptr condition,
						 int op,
						 node_ptr exp1,
						 node_ptr exp2,
						 node_ptr tail)
{
#if 0 == ALLOW_NESTED_CASES  /* nested ITE are NOT allowed! */

  nusmv_assert(CASE != node_get_type(exp1));
  nusmv_assert(FAILURE != node_get_type(exp1));

  if (Nil != exp2 && CASE == node_get_type(exp2)) {
    /* exp2 is a list of ITE */
    node_ptr tmp = pred_norm_find_node(COLON,
				       pred_norm_find_node(AND,
							   condition,
							   car(car(exp2))),
				       pred_norm_find_node(op, exp1,
							   cdr(car(exp2))));
    node_ptr new_tail = FAILURE == node_get_type(cdr(exp2)) ? cdr(exp2)
      : pred_norm_push_ite_up_conditioned(condition, op, exp1, cdr(exp2), tail);
      
    return find_node(CASE, tmp, new_tail);
  }
  /* exp2 is not ITE expr, simple apply the operator */
  else {
    node_ptr tmp = find_node(COLON, condition, 
			     pred_norm_find_node(op, exp1, exp2));
    return pred_norm_find_node(CASE, tmp, tail);
  }

#elif 1 == ALLOW_NESTED_CASES   /* nested ITE are allowed */

  node_ptr tmp = pred_norm_push_ite_up(op, exp1, exp2);
  return find_node(CASE, find_node(COLON, condition, tmp), tail);

#else
#error ALLOW_NESTED_CASES is incorrectly defined!
#endif
}


/**Function********************************************************************

  Synopsis           [The function take three expressions (cond, e1, e2) and
  creates ITE(cond, e1, e2). Normalisation is performed if required.]

  Description        [normalises ITE expression, i.e.
  the function creates "ITE(cond, then, tail)"  and normalised it.

  The 'then' and 'tail' expressions are assumed to be not-boolean.
  
  If nested ITE are not allowed and 'then' is an ITE-expression
  then normalisation is performed, i.e. all ITE are moved up from 'then'.
  For example,
     ITE(c1, ITE(c2, t2, e2), e1)
  will become
     ITE(c1&c2, t2, ITE(c1, e2, e1)),
  and the result will be ITE list (without nested ITE). Note that in
  this case t2 cannot be ITE expression(has been already
  normalised)).]

  SideEffects        []

******************************************************************************/
static node_ptr 
pred_norm_normalise_ite(node_ptr cond, node_ptr then, node_ptr tail)
{
#if 0 == ALLOW_NESTED_CASES  /* nested ITE are NOT allowed */
  
  if (CASE == node_get_type(then)) {
    /* 'then' is ITE expression => modify it.*/
    node_ptr new_tail = (FAILURE == node_get_type(cdr(then))) ? cdr(then)
      : pred_norm_normalise_ite(cond, cdr(then), tail);

    return find_node(CASE, 
		     find_node(COLON,
			       pred_norm_find_node(AND, cond, car(car(then))),
			       cdr(car(then))),
		     new_tail);
  }
  /* 'then' is not ITE expression => just create the expression*/
  else return find_node(CASE, find_node(COLON, cond, then), tail);
  
  
#elif 1 == ALLOW_NESTED_CASES /* nested ITE are allowed */
  /* just create the expression */
  return find_node(CASE, find_node(COLON, cond, then), tail);
  
#else
#error ALLOW_NESTED_CASES is incorrectly defined!
#endif
}

