/**CFile***********************************************************************

  FileName    [CheckerCore.c]

  PackageName [compile.type_checking.checkers]

  Synopsis    [Implementaion of class 'CheckerCore']

  Description []

  SeeAlso     [CheckerCore.h]

  Author      [Andrei Tchaltsev, Roberto Cavada]

  Copyright   [
  This file is part of the ``compile.type_checking.checkers'' 
  package of NuSMV version 2. 
  Copyright (C) 2006 by ITC-irst. 

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

  Revision    [$Id: CheckerCore.c,v 1.1.2.16.4.11 2007/05/21 07:31:06 nusmv Exp $]

******************************************************************************/

#include "CheckerCore.h" 
#include "CheckerCore_private.h" 
#include "checkersInt.h"

#include "compile/compile.h"
#include "compile/type_checking/TypeChecker_private.h"
#include "compile/symb_table/symb_table.h"
#include "utils/WordNumber.h" 
#include "parser/symbols.h"
#include "utils/utils.h" 
#include "utils/ustring.h" 

static char rcsid[] UTIL_UNUSED = "$Id: CheckerCore.c,v 1.1.2.16.4.11 2007/05/21 07:31:06 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/* See 'CheckerCore_private.h' for class 'CheckerCore' definition. */ 

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void checker_core_finalize ARGS((Object_ptr object, void* dummy));

static SymbType_ptr
checker_core_check_expr ARGS((CheckerBase_ptr self,
			      node_ptr expression, node_ptr context));

static boolean 
checker_core_viol_handler ARGS((CheckerBase_ptr checker, 
				TypeSystemViolation violation, 
				node_ptr expression));

static void 
print_operator ARGS((FILE* output_stream, node_ptr expr));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The CheckerCore class constructor]

  Description        [The CheckerCore class constructor]

  SideEffects        []

  SeeAlso            [NodeWalker_destroy]   
  
******************************************************************************/
CheckerCore_ptr CheckerCore_create()
{
  CheckerCore_ptr self = ALLOC(CheckerCore, 1);
  CHECKER_CORE_CHECK_INSTANCE(self);

  checker_core_init(self, "Core SMV Type Checker", 
		    NUSMV_EXPR_SYMBOL_FIRST, 
		    NUSMV_EXPR_SYMBOL_LAST - NUSMV_EXPR_SYMBOL_FIRST);
  return self;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The CheckerCore class private initializer]

  Description        [The CheckerCore class private initializer]

  SideEffects        []

  SeeAlso            [CheckerCore_create]   
  
******************************************************************************/
void checker_core_init(CheckerCore_ptr self, 
		       const char* name, int low, size_t num)
{
  /* base class initialization */
  checker_base_init(CHECKER_BASE(self), name, low, num);
  
  /* members initialization */

  /* virtual methods settings */  
  OVERRIDE(Object, finalize) = checker_core_finalize;
  OVERRIDE(CheckerBase, check_expr) = checker_core_check_expr;
  OVERRIDE(CheckerBase, viol_handler) = checker_core_viol_handler;
}


/**Function********************************************************************

  Synopsis           [The CheckerCore class private deinitializer]

  Description        [The CheckerCore class private deinitializer]

  SideEffects        []

  SeeAlso            []   
  
******************************************************************************/
void checker_core_deinit(CheckerCore_ptr self)
{
  /* members deinitialization */

  /* base class initialization */
  checker_base_deinit(CHECKER_BASE(self));
}




/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [The CheckerCore class virtual finalizer]

  Description [Called by the class destructor]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void checker_core_finalize(Object_ptr object, void* dummy) 
{
  CheckerCore_ptr self = CHECKER_CORE(object);

  checker_core_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
static SymbType_ptr checker_core_check_expr(CheckerBase_ptr self, 
					    node_ptr expr, node_ptr context)
{
  /* wrap expr into the context. This is required by 
     the facilities which remembers the type of expressions 
     and by the violation handler.
  */
  node_ptr ctx_expr;
  if (context != Nil) ctx_expr = find_node(CONTEXT, context, expr);
  else ctx_expr = expr;

  { /* checks memoizing */
    SymbType_ptr tmp = _GET_TYPE(ctx_expr);
    if (nullType != tmp) return tmp;
  }
  
  switch (node_get_type(expr)) {
  case SELF:
    return  _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
  case CONTEXT: 
    {
      SymbType_ptr type = _THROW(cdr(expr), car(expr));
      /* since before remembering the type an expression is 
	 wrapped into CONTEXT, that CONTEXT can be equal to
	 this CONTEXT expression, and therefore its type has 
	 been remembered in the above invocation of type_checker_check_expression
      */
      if (nullType == _GET_TYPE(ctx_expr)) {
	_SET_TYPE(ctx_expr, type);
      }
      return type;
    }
    
  case TRUEEXP:
  case FALSEEXP:
    return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    
  case NUMBER: {
    int num = NODE_TO_INT(car(expr));
    return _SET_TYPE(ctx_expr,
		     (num == 1 || num == 0)
		     ? SymbTablePkg_boolean_enum_type()
		     : SymbTablePkg_integer_type());
  }

  case NUMBER_WORD: 
    {
      /* during parsing Word constants are normilised to WordNumber_ptr
	 (the left chilf).
      */
      int wordWidth = WordNumber_get_width(WORD_NUMBER(car(expr)));
      return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(wordWidth));
    }

  case NUMBER_FRAC:
  case NUMBER_REAL:
  case NUMBER_EXP:
    return _SET_TYPE(ctx_expr, SymbTablePkg_real_type());

  case TWODOTS:
    /* two-dots have two NUMBER and nothing else */
    nusmv_assert(node_get_type(car(expr)) == NUMBER && 
		 node_get_type(cdr(expr)) == NUMBER);
    return _SET_TYPE(ctx_expr, SymbTablePkg_integer_set_type());


  case BIT:
    /* this kind of expressions is not checked,
       since it is artificially created
    */
    return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());

  
    /* these exprs mean a variable, a constant, a define or a parameter */
  case DOT:
  case ATOM: 
  case ARRAY:
    {
      /* First, find out what this identifier may mean */
      SymbTable_ptr symb_table = TypeChecker_get_symbol_table(
							      TYPE_CHECKER(NODE_WALKER(self)->master));
      
      node_ptr simpleName = find_atom(expr);

      /* Obtain the resolved name. CompileFlatten_resolve_name should
	 not be applied to a parameter and parameter can be only ATOM. */
      node_ptr resolvedName = (node_get_type(expr) == ATOM) ?
	find_node(DOT, context, simpleName) : 
	CompileFlatten_resolve_name(symb_table, simpleName, context);
    
      /* only ATOMs can be parameters */
      node_ptr parameter = lookup_param_hash(resolvedName);
      boolean isVar = SymbTable_is_symbol_var(symb_table, resolvedName);
      boolean isDefine = SymbTable_is_symbol_define(symb_table, resolvedName);

      /* check the original ATOM expression and then the resolved name 
	 for being a constant */
      boolean isConstant = false;
      if (ATOM == node_get_type(simpleName)) {
	isConstant = SymbTable_is_symbol_constant(symb_table, simpleName);
      }
      if (!isConstant) {
	isConstant = SymbTable_is_symbol_constant(symb_table, resolvedName);
      }

      /* An identifier has not been declared => error */
      if ( Nil == parameter && !isVar && !isDefine && !isConstant) {
	SymbType_ptr type = _VIOLATION(TC_VIOLATION_UNDEF_IDENTIFIER, resolvedName) ?
	  SymbTablePkg_error_type() /* ERROR */
	  : SymbTablePkg_pure_symbolic_enum_type();
	return _SET_TYPE(ctx_expr, type);
      }

      /* An identifier has more than one meaning => error */
      if ( ((parameter != Nil) + isVar + isDefine + isConstant) > 1 ) {
	SymbType_ptr type = _VIOLATION(TC_VIOLATION_AMBIGUOUS_IDENTIFIER, 
				       ctx_expr) ?
	  SymbTablePkg_error_type() /* ERROR */
	  : SymbTablePkg_pure_symbolic_enum_type();
	return _SET_TYPE(ctx_expr, type);
      }

      /* this is a constant */
      if (isConstant) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_pure_symbolic_enum_type());
      }

      /* this is a parameter */
      if (parameter != Nil) {
	SymbType_ptr type = _THROW(parameter, context);
	return _SET_TYPE(ctx_expr, type);
      }

      /* this is a define */
      if (isDefine) {
	/* the bodies of defines are cached(in BddEnc).
	   So, the type of expressions will also be correctly cached.
	 
	   Only flattened body of define is checked, since not
	   flattened body is not used anywhere.
	*/
	node_ptr flatDef = SymbTable_get_define_flatten_body(symb_table, 
							     resolvedName);
	/* the context is Nil because expr is already flattened */
	SymbType_ptr type = _THROW(flatDef, Nil);      
	return _SET_TYPE(ctx_expr, type);
      }
    
      /* This is a variable */
      {
	SymbType_ptr type;
	nusmv_assert(isVar);
	type = SymbTable_get_var_type(symb_table, resolvedName);
	/* Convert the syntactic type to a symbol type.
	   Here we share the variable's type to allow 
	   comparison of pointers instead of content of the types 
	*/
	type = SymbType_make_memory_shared(type);
	return _SET_TYPE(ctx_expr, type);
      } /* end of isVar */    
    } /* case ATOM, DOT, ARRAY */

    /*  boolean unary operand */
  case EX:
  case AX:
  case EF:
  case AF:
  case EG:
  case AG:

  case NOT: /* "NOT" may also have Word */

  case OP_GLOBAL:
  case OP_PREC:
  case OP_NOTPRECNOT:
  case OP_FUTURE:
  case OP_NEXT:
  case OP_HISTORICAL:
  case OP_ONCE:

    /* For EBF ABF EBG ABG - ignore the number..number part (cdr) */
  case EBF:
  case ABF:
  case EBG:
  case ABG: 
    { 
      /* get the operand's type */
      SymbType_ptr type = _THROW(car(expr), context);

      if (SymbType_is_error(type)) { /* earlier error */
	return _SET_TYPE(ctx_expr, type);
      }

      /* the operand must be boolean (or Word for NOT) */
      if (SymbType_is_boolean_enum(type) || 
	  (node_get_type(expr) == NOT && 
	   SymbType_get_tag(type) == SYMB_TYPE_WORD)) {
	return _SET_TYPE(ctx_expr, type);
      }

      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type) ?
		     TC_VIOLATION_TYPE_BACK_COMP : TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep the type what ever it is */
      return _SET_TYPE(ctx_expr, type);
    }
 
		
    /* casts: boolean -> Word[1] and Word[1] -> boolean. */
  case CAST_BOOL:
  case CAST_WORD1: {
    /* get the operands' type */
    SymbType_ptr type = _THROW(car(expr), context);

    if (SymbType_is_error(type)) { /* earlier error */
      return _SET_TYPE(ctx_expr, type);
    }

    /* if the expression is cast_bool, operand is word[1] */
    if (node_get_type(expr) == CAST_BOOL && SymbType_is_word_1(type)) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }
    
    /* OR the expression must be cast_word1, operand is boolean */
    if (node_get_type(expr) == CAST_WORD1 &&
	SymbType_is_boolean_enum(type)) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(1));
    }
	
    /* is this a type error ? */
    if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
    }
    
    /* this is not an error after all -> create the proper return type */
    return _SET_TYPE(ctx_expr,
		     node_get_type(expr) == CAST_BOOL
		     ? SymbTablePkg_boolean_enum_type()
		     : SymbTablePkg_word_type(1));
  }


    /* 'init' and 'next' do not change the type of the expression */
  case SMALLINIT:
  case NEXT: 
    {
      /* apply find_atom on the 'init' or 'next' expression and check
	 the obtained expression for being already checked. 

	 The obtained expression will be the same as original one but
	 some parts of NuSMV generate 'init' and 'next' expression
	 with find_node, so both original and find_atom'ed expression
	 should be checked
      */
      node_ptr normalisedExpr = find_atom(expr);
      SymbType_ptr type;
      /*wrap expression into context.This is required by tc_get_expression_type*/
      if (Nil != context) {
	normalisedExpr = find_node(CONTEXT, context, normalisedExpr);
      }

      type = _GET_TYPE(normalisedExpr);
      /* find_atom'ed expression has been already checked. Remeber and return */
      if (nullType != type) { 
	return _SET_TYPE(ctx_expr, type);
      }

      /* check the expression now */
      type = _THROW(car(expr), context);
      /* associate both the original expression and the find_atom'ed one.*/
      _SET_TYPE(ctx_expr, type);
      if (normalisedExpr != ctx_expr) {
	_SET_TYPE(normalisedExpr, type);
      }
      return type;
    }


    /* binary: w[N]*w[N] or w[N]*boolean or boolean*W[N] or boolean*boolean */
  case CONCATENATION: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
      /* convert boolean operands to Word[1] (to simplify the code */
      if (SymbType_is_boolean_enum(type1)) type1 = SymbTablePkg_word_type(1);
      if (SymbType_is_boolean_enum(type2)) type2 = SymbTablePkg_word_type(1);

      /* both operands must be Word */
      if (SymbType_get_tag(type1) == SYMB_TYPE_WORD &&
	  SymbType_get_tag(type2) == SYMB_TYPE_WORD) {
	int newWidth = SymbType_get_word_width(type1) + 
	  SymbType_get_word_width(type2);
	return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(newWidth));
      }

      /* is this a type error ? */
      if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* this is not an error after all -> create Word type of some width*/
      return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(1));
    }

    /* first operand is Word[N], then two numbers K and L, K >=L and N>=K. */
  case BIT_SELECTION: 
    {
      /* get the operand' type */
      SymbType_ptr type = _THROW(car(expr), context);
      int width;
      int highBound;
      int lowBound;
    
      if (SymbType_is_error(type)) { /* earlier error */
	return _SET_TYPE(ctx_expr, type);
      }

      /* At the moment, second and third operands must be NUMBERS
	 (And this is checked in flattener 
      */
      /*just consistency check*/
      nusmv_assert(COLON == node_get_type(cdr(expr)));
      nusmv_assert(NUMBER == node_get_type(car(cdr(expr))));
      nusmv_assert(NUMBER == node_get_type(cdr(cdr(expr))));
    
      /* check the first operand type */
      if (SymbType_get_tag(type) != SYMB_TYPE_WORD) { /* ERROR */
	if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
	else { /* return arbitrary Word type */
	  return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(1));
	}
      }
      width = SymbType_get_word_width(type);
      highBound = NODE_TO_INT(car(car(cdr(expr))));
      lowBound  = NODE_TO_INT(car(cdr(cdr(expr))));
			 
      /* checks the bit width */
      if (width <= highBound || highBound < lowBound || lowBound < 0) {
	/* ERROR */
	if (_VIOLATION(TC_VIOLATION_OUT_OF_WORD_WIDTH, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
	else { /* give some realistic bit specifiers */
	  highBound = 0;
	  lowBound = 0;
	}
      }
      /* everything is OK */
      return _SET_TYPE(ctx_expr, 
		       SymbTablePkg_word_type(highBound - lowBound + 1));
    }

  case WAWRITE: 
    {
      /* WRITE : WORDARRAY[a,v] x WORD[a] x WORD[v] -> WORDARRAY[a,v] */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(car(cdr(expr)), context);
      SymbType_ptr type3 = _THROW(cdr(cdr(expr)), context);

      nusmv_assert(node_get_type(cdr(expr)) == WAWRITE);

      if (SymbType_is_error(type1) || SymbType_is_error(type2) ||
	  SymbType_is_error(type3)) {
	/* handle error in arguments */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* ERROR with kinds of subexpressions */
      if (SymbType_get_tag(type1) != SYMB_TYPE_WORDARRAY ||
	  SymbType_get_tag(type2) != SYMB_TYPE_WORD ||
	  SymbType_get_tag(type3) != SYMB_TYPE_WORD ) {
	if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
	else { /* return arbitrary WordArray type */
	  return _SET_TYPE(ctx_expr, SymbTablePkg_wordarray_type(1,1));
	}
      }

      /* ERROR with the widths */
      if ((SymbType_get_wordarray_awidth(type1) != 
	   SymbType_get_word_width(type2)) ||
	  (SymbType_get_wordarray_vwidth(type1) != 
	   SymbType_get_word_width(type3))) {
	if (_VIOLATION(TC_VIOLATION_OUT_OF_WORDARRAY_WIDTH, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
      }

      /* No errors, then return the type of the first argument (whatever it is) */
      return _SET_TYPE(ctx_expr, type1);
    }

  case WAREAD: 
    {
      /* WORDARRAY[a,v] x WORD[a] -> WORD[v] */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);

      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* handle error in arguments */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* ERROR with kinds of subexpressions */
      if (SymbType_get_tag(type1) != SYMB_TYPE_WORDARRAY ||
	  SymbType_get_tag(type2) != SYMB_TYPE_WORD) {
	if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
	else { /* return arbitrary WordArray type */
	  return _SET_TYPE(ctx_expr, SymbTablePkg_wordarray_type(1, 1));
	}
      }

      /* ERROR with the widths */
      if (SymbType_get_wordarray_awidth(type1) !=
	  SymbType_get_word_width(type2)) {
	if (_VIOLATION(TC_VIOLATION_OUT_OF_WORDARRAY_WIDTH, ctx_expr)) {
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	}
      }

      /* No errors, then return WORD[v] */
      return _SET_TYPE(ctx_expr,
		       SymbTablePkg_word_type(SymbType_get_wordarray_vwidth(type1)));
    }

    /* unary minus */
  case UMINUS:
    { 
      /* get the operands' type */
      SymbType_ptr type = _THROW(car(expr), context);
            
      if (SymbType_is_error(type)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
  
      /* correct types are boolean, int ,b*b->i, i*i->i, r*r->r,
	 w[n]*w[n]->w[n] */
      if (nullType != type && 
	  (SymbType_is_boolean_enum(type) ||
	   SymbType_get_tag(type) == SYMB_TYPE_INTEGER ||
	   SymbType_get_tag(type) == SYMB_TYPE_REAL ||
	   SymbType_get_tag(type) == SYMB_TYPE_WORD)) {
	/* boolean type should be converted to int */
	if (SymbType_is_boolean_enum(type)) {
	  type = SymbTablePkg_integer_type();
	}
	return _SET_TYPE(ctx_expr, type);
      }
    
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type) ?
		     TC_VIOLATION_TYPE_BACK_COMP :
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep one of the types whatever it is*/
      return _SET_TYPE(ctx_expr, type);
    }

    /* arithmetic operators: b*b->i, i*i->i, r*r->r, w[n]*w[n]->w[n]
       + implicit conversion
    */
  case PLUS:
  case MINUS:
  case TIMES:
  case DIVIDE: 
    { 
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2;
      SymbType_ptr returnType;
      
      /* PLUS and MINUS may be unary */
      if (cdr(expr) != (node_ptr) NULL) type2 = _THROW(cdr(expr), context);
      else {
	/* only PLUS and MINUS are supposed to be possibly unary */
	nusmv_assert(node_get_type(expr) == PLUS || 
		     node_get_type(expr) == MINUS);
	type2 = type1;
      }
      
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
  
      /* implicit conversion of one of the operands */
      returnType = SymbType_get_greater(type1, type2);
    
      /* correct types are boolean, int ,b*b->i, i*i->i, r*r->r,
	 w[n]*w[n]->w[n] */
      if (nullType != returnType && 
	  (SymbType_is_boolean_enum(returnType) ||
	   SymbType_get_tag(returnType) == SYMB_TYPE_INTEGER ||
	   SymbType_get_tag(returnType) == SYMB_TYPE_REAL ||
	   SymbType_get_tag(returnType) == SYMB_TYPE_WORD)) {
	/* boolean type should be converted to int */
	if (SymbType_is_boolean_enum(returnType)) {
	  returnType = SymbTablePkg_integer_type();
	}
	return _SET_TYPE(ctx_expr, returnType);
      }
    
      /* is this a type error ? */
      if (_VIOLATION((SymbType_is_back_comp(type1) && 
		      SymbType_is_back_comp(type2)) ?
		     TC_VIOLATION_TYPE_BACK_COMP :
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep one of the types whatever it is*/
      return _SET_TYPE(ctx_expr, type1);
    }

    /* mod operator: i*i->i, w[n]*w[n]->w[n] (no implicit conversion)*/
  case MOD: 
    { 
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* w[N] * w[N] -> w[N] */
      if (type1 == type2 && /*types are shared => same types == same pointers*/
	  SymbType_get_tag(type1) == SYMB_TYPE_WORD) {
	return _SET_TYPE(ctx_expr, type1);
      }
    
      /* int * int -> int, int * 2 -> bool */
      if (type1 == type2 && /*types are shared => same types == same pointers*/
	  SymbType_get_tag(type1) == SYMB_TYPE_INTEGER) {
	/* special case "int mod 2" -> bool */
	if (NUMBER == node_get_type(cdr(expr)) &&
	    2 == NODE_TO_INT(car(cdr(expr)))) {
	  type1 =  SymbTablePkg_boolean_enum_type();
	}
      
	return _SET_TYPE(ctx_expr, type1);
      }

      /* special cases:
	 int * bool -> bool
	 bool * int -> bool
	 bool * bool -> bool
	 These are technically useless expressions,
	 they are allowed but warning is output:
      */
      if ( (SymbType_get_tag(type1) == SYMB_TYPE_INTEGER &&
	    SymbType_is_boolean_enum(type2))
	   ||(SymbType_is_boolean_enum(type1) &&
	      SymbType_get_tag(type2) == SYMB_TYPE_INTEGER)
	   ||(SymbType_is_boolean_enum(type1) &&
	      SymbType_is_boolean_enum(type2)) ) {
      
	if (_VIOLATION(TC_VIOLATION_TYPE_WARNING, ctx_expr))
	  return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
	else
	  return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
    
      /* is this a type error ? */
      if (_VIOLATION((SymbType_is_back_comp(type1) && 
		      SymbType_is_back_comp(type2)) ?
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep one of the types whatever it is*/
      return _SET_TYPE(ctx_expr, type1);
    }

    /* left operand is word and the right is Int, Bool or word */
  case LSHIFT: case RSHIFT: case LROTATE: case RROTATE: 
    { 
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) { 
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* correct types are w[n]*boolean, w[n]*int */
      if (SymbType_get_tag(type1) == SYMB_TYPE_WORD &&
	  (SymbType_is_boolean_enum(type2) ||
	   SymbType_get_tag(type2) == SYMB_TYPE_INTEGER || 
	   SymbType_get_tag(type2) == SYMB_TYPE_WORD)) {
	/* In future, maybe, it is worth to check that if right operand
	   is constant its value < width of Word (otherwise - warning)
	*/
	return _SET_TYPE(ctx_expr, type1);
      }
    
      /* is this a type error ? */
      if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep the left operand type */
      return _SET_TYPE(ctx_expr, type1);
    }


    /* the operands should be implicitly converted to one type,
       otherwise they must be either boolean or Word[1] 
    */
  case EQUAL:
  case NOTEQUAL: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
      SymbType_ptr returnType;
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* implicit conversion of one of the operands */
      returnType = SymbType_get_greater(type1, type2);

      /* returned type is enum, int, real, word or word-array
	 or operands are boolean with Word[1].
	 Virtually, only set types cannot be compared.
      */
      if ( (nullType != returnType &&
	    (SymbType_get_tag(returnType) == SYMB_TYPE_ENUM ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_INTEGER ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_WORD ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_WORDARRAY ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_REAL)) ||
	   (SymbType_is_boolean_enum(type1) && SymbType_is_word_1(type2)) ||
	   (SymbType_is_boolean_enum(type2) && SymbType_is_word_1(type1)) ) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ?
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the boolean type */
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }

    
    /* Assignment. The right operand should be implicitly converted to the
       left operand's type (or its set-type counterpart),
       otherwise they must be boolean with Word[1].
       The type of the whole expression is statement-type.
       It is required to process EQDEF with other expressions since 
       in symbolic-FSM after predicate-normalisation EQDEF can become
       a part of case-expression in init, trans, etc, lists.

       NB: the left operand is not checked for being a variable since
       parser guarantees this.
    */
  case EQDEF: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
      SymbType_ptr returnType;
	
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* If the  right operand is of a set-type, convert the set-type to a
	 usual one.
	 Convert the right operand's type then to the type of the left 
	 operands.
      */
      returnType = SymbType_convert_right_to_left(type1,
						  SymbType_make_from_set_type(type2));

      /* returned type is any enum-type, int, real, word or
	 word-array or the operands are boolean with Word[1]
	 Virtually, only set types cannot be compared.
      */
      if ( (nullType != returnType &&
	    (SymbType_get_tag(returnType) == SYMB_TYPE_ENUM ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_INTEGER ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_WORD ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_WORDARRAY ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_REAL
	     )) ||
	   (SymbType_is_boolean_enum(type1) && SymbType_is_word_1(type2)) ||
	   (SymbType_is_boolean_enum(type2) && SymbType_is_word_1(type1)) ) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_statement_type());
      }
  
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ?
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the statement-type */
      return _SET_TYPE(ctx_expr, SymbTablePkg_statement_type());
    }


    /*  allowed types(with implicit conversion):
	b*b, i*i, r*r, w[n]*w[n], b*w[1], w[1]*b
    */
  case LT:
  case GT:
  case LE:
  case GE: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
      SymbType_ptr returnType;
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* implicit conversion on both operands */
      returnType = SymbType_get_greater(type1, type2);
    
      /* return type is not-null and arithmetic 
	 or operands are boolean or Word[1] */
      if ( (nullType != returnType && 
	    (SymbType_is_boolean_enum(returnType) ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_INTEGER ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_REAL ||
	     SymbType_get_tag(returnType) == SYMB_TYPE_WORD)) ||
	   (SymbType_is_boolean_enum(type1) && SymbType_is_word_1(type2)) ||
	   (SymbType_is_boolean_enum(type2) && SymbType_is_word_1(type1)) ) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the boolean type*/
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }

    /*  signed comparison : w[n]*w[n]  */
  case SLT:
  case SGT:
  case SLE:
  case SGE: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
      SymbType_ptr returnType;
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* implicit conversion on both operands */
      returnType = SymbType_get_greater(type1, type2);
    
      /* return type is not-null and arithmetic 
	 or operands are boolean or Word[1] */
      if ((nullType != returnType && 
	   SymbType_get_tag(returnType) == SYMB_TYPE_WORD)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
      /* is this a type error ? */
      if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the boolean type*/
      return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(1));
    }

    /* sign extend: left is word[N], right is NUMBER M, result is word[N+M] */
  case SIGN_EXTEND:
    {
      /* get the operands' type */
      SymbType_ptr type = _THROW(car(expr), context);
      
      if (SymbType_is_error(type)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      nusmv_assert(NUMBER == node_get_type(cdr(expr)));
    
      if (SymbType_get_tag(type) == SYMB_TYPE_WORD) {
	return _SET_TYPE(ctx_expr,
			 SymbTablePkg_word_type(SymbType_get_word_width(type) + 
						NODE_TO_INT(car(cdr(expr)))));
      }

      /* is this a type error ? */
      if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the boolean type*/
      return _SET_TYPE(ctx_expr, SymbTablePkg_word_type(1));
    }

    /* the operator are boolean or Words of the same size */
  case OR:
  case XOR:
  case XNOR:
  case IMPLIES:
  case IFF: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) { 
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* both operands must be boolean or Word of the same size */
      if ((SymbType_is_boolean_enum(type1) && SymbType_is_boolean_enum(type2)) ||
	  (SymbType_get_tag(type1) == SYMB_TYPE_WORD && type1 == type2)) {
	return _SET_TYPE(ctx_expr, type1);
      }

      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY,
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep one of the types whatever it is*/
      return _SET_TYPE(ctx_expr, type1);
    }


    /* AND is an unusual binary operator because sometimes it is used
       as connector in expression lists.
       The distinction from usual binary logical operator:
       1. if one of the operands is Nill => it is skipped
       2. operands can be boolean 
       3. operands can be statement-type. This can happen only for
       artificially created expressions (not parsed ones)
    */
  case AND: 
    {
      SymbType_ptr type1;
      SymbType_ptr type2;
      if (Nil == car(expr)) {
	type2 = _THROW(cdr(expr), context);
	return _SET_TYPE(ctx_expr, type2);
      }

      if (Nil == cdr(expr)) {
	type1 = _THROW(car(expr), context);
	return _SET_TYPE(ctx_expr, type1);
      }
    
      /* get the operands' type */
      type1 = _THROW(car(expr), context);
      type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* both operands must be boolean or Word of the same size */
      if ((SymbType_is_boolean_enum(type1) && SymbType_is_boolean_enum(type2)) ||
	  (SymbType_get_tag(type1) == SYMB_TYPE_WORD && type1 == type2)) {
	return _SET_TYPE(ctx_expr, type1);
      }
    
      /* AND is a list connector; operands must be boolean or statement-type */
      if ( (SymbType_is_boolean_enum(type1) ||
	    SymbType_get_tag(type1) == SYMB_TYPE_STATEMENT) &&
	   (SymbType_is_boolean_enum(type2) ||
	    SymbType_get_tag(type2) == SYMB_TYPE_STATEMENT) ) {
	return _SET_TYPE(ctx_expr, type1);
      }

      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY,
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> keep one of the types whatever it is*/
      return _SET_TYPE(ctx_expr, type1);
    }
    

    /* both operands are converted to (corresponding) set-types and
       then to the minimal common type.
       As result only the following types can be here:
       any of enum types (bool, symb, int-symb), int  or any of set type
       (i.e. integer-set, symbolic-set, integer-symbolic-set)
    */
  case UNION: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
      SymbType_ptr returnType;
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* convert operands to set types (if this is possible) and then  
	 convert them to one type.
      */
      {
	SymbType_ptr tmp1 = SymbType_make_set_type(type1);
	SymbType_ptr tmp2 = SymbType_make_set_type(type2);

	if (nullType != tmp1 && nullType != tmp2) {
	  returnType = SymbType_get_minimal_common(tmp1, tmp2);
	}
	else returnType = nullType;
      }

      /* both types are convertable to a set type */
      if (nullType != returnType) {
	return _SET_TYPE(ctx_expr, returnType);
      }
    
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY,
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return the boolean type */
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }

    /* both operands are converted to (corresponding) set-types and
       then ONE of the operand is converted to the type of the othe one.
       As result only the following types can be here:
       any of enum types (bool, symb, int-symb), int  or any of set type
       (i.e. integer-set, symbolic-set, integer-symbolic-set)
    */
  case SETIN:  { 
    /* get the operands' type */
    SymbType_ptr type1 = _THROW(car(expr), context);
    SymbType_ptr type2 = _THROW(cdr(expr), context);
    SymbType_ptr returnType;
    
    if (SymbType_is_error(type1) || SymbType_is_error(type2)){/*earlier error*/
      return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
    }
        
    /* convert operands to set types (if this is possible) and then  
       convert one of the them to the other one.
    */
    {
      SymbType_ptr tmp1 = SymbType_make_set_type(type1);
      SymbType_ptr tmp2 = SymbType_make_set_type(type2);

      if (nullType != tmp1 && nullType != tmp2) {
	returnType = SymbType_get_greater(tmp1, tmp2);
      }
      else returnType = nullType;
    }
    
    /* Note that the types should be convertable to one of the set types (
       and be therefore, any of enum (i.e. Bool, Symb and IntSymb),
       Int, any of set types (i.e. int-set, symb-set, int-symb-set)).
       Real, Word and WordArray should not be here because
       even though assignment internally behaves the same way as 
       "in"-operatoris, assignment is never syntactically converted to 
       SETIN expression.
    */
    if (nullType != returnType) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }

    /* is this a type error ? */
    if (_VIOLATION(SymbType_is_back_comp(type1) && 
		   SymbType_is_back_comp(type2) ? 
		   TC_VIOLATION_TYPE_BACK_COMP :
		   TC_VIOLATION_TYPE_MANDATORY, 
		   ctx_expr)) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
    }

    /* this is not an error after all -> return the boolean type */
    return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
  }

    /* left expression is bool, and the union of right expression 
       and the next case (if it is not the end of list) is returned.
    */
  case CASE: 
    {
      /* get the operands' type */
      SymbType_ptr condType = _THROW(car(car(expr)), context);
      SymbType_ptr expType = _THROW(cdr(car(expr)), context);
      SymbType_ptr nextCaseType;
      SymbType_ptr returnType;
    
      if (SymbType_is_error(condType) || SymbType_is_error(expType)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* left operand of ':' should be boolean */
      if ( (!SymbType_is_boolean_enum(condType)) && 
	   _VIOLATION(SymbType_is_back_comp(condType) ? 
		      TC_VIOLATION_TYPE_BACK_COMP : 
		      TC_VIOLATION_TYPE_MANDATORY,
		      Nil == context ? 
		      car(expr) : 
		      find_node(CONTEXT, context, car(expr)))) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is the last element of case-list */
      if (FAILURE == node_get_type(cdr(expr))) {
	return _SET_TYPE(ctx_expr, expType);
      }

      /* there are more case-elements in the list */
      nextCaseType = _THROW(cdr(expr), context);
      if (SymbType_is_error(nextCaseType)) {
	/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* both CASE expressions should be convertable to a common type.
	 If one of the expressions is of a set-type then before implicit
	 convertion the other expression is converted to 
	 a corresponding set-type.
      */
      {
	/* if one of the type is a set-type, convert also the other one to
	   a corresponding set type
	*/
	SymbType_ptr tmp1 = SymbType_is_set(nextCaseType) ?
	  SymbType_make_set_type(expType) : expType;

	SymbType_ptr tmp2 = SymbType_is_set(expType) ?
	  SymbType_make_set_type(nextCaseType) : nextCaseType;
      
	if (nullType != tmp1 && nullType != tmp2) {
	  returnType = SymbType_get_minimal_common(tmp1, tmp2);
	}
	else returnType = nullType;
      }
    
      /* we do not care which type exactly is obtained, since only
	 correct type could pass the above code
      */
      if (nullType != returnType) { 
	return _SET_TYPE(ctx_expr, returnType);
      }
    
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(expType) && 
		     SymbType_is_back_comp(nextCaseType) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY, 
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return one of the types  */
      return _SET_TYPE(ctx_expr, expType);
    }


    /* both operands are boolean */
  case AU:
  case EU:
  case UNTIL:
  case SINCE: 
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) { 
	/*earlier error*/
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
     
      /* both operands must be boolean */
      if (SymbType_is_boolean_enum(type1) && SymbType_is_boolean_enum(type2)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
    
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY,
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return boolean type */
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }
    
    /* ignore the number..number part and check A-U or E-U expressions */
  case ABU:
  case EBU: 
    {
      SymbType_ptr type = _THROW(car(expr), context);
      return _SET_TYPE(ctx_expr, type);
    }

  case FAILURE: /* FAILURE is a part of CASE-expression.
		   Should be analysed there.
		   NB: If a more complex use of FAILURE will be adopted,
		   then a new failure-type should be created, and 
		   all expression-checking procedures should take into
		   account this type.*/
    return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());


  case COLON: /* COLON is a part of BIT_SELECTION or CASE. 
		 Should be analysed there 
	      */
    nusmv_assert(false);
    
    /* these are used into COMPUTE statement */
  case MINU:
  case MAXU:
    {
      /* get the operands' type */
      SymbType_ptr type1 = _THROW(car(expr), context);
      SymbType_ptr type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {       
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type()); /*earlier error*/
      }
     
      /* both operands must be boolean */
      if (SymbType_is_boolean_enum(type1) && SymbType_is_boolean_enum(type2)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
      }
    
      /* is this a type error ? */
      if (_VIOLATION(SymbType_is_back_comp(type1) && 
		     SymbType_is_back_comp(type2) ? 
		     TC_VIOLATION_TYPE_BACK_COMP : 
		     TC_VIOLATION_TYPE_MANDATORY,
		     ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return boolean type */
      return _SET_TYPE(ctx_expr, SymbTablePkg_boolean_enum_type());
    }

    /* The type of a list is the least common type of its expressions.
       CONS is NOT a possible input (parsed) expression, but can only be
       generated inside NuSMV.  At the moment at least GENREACTIVITY
       and BUCHIGAME
       specification generates such expressions.  The invoker should
       know what he is doing when CONS expression is type checked.
    */
  case CONS: 
    {
      SymbType_ptr type1;
      SymbType_ptr type2;
      SymbType_ptr returnType;
    
      if (Nil == cdr(expr)) { /* one element list */
	type1 = _THROW(car(expr), context);
	return _SET_TYPE(ctx_expr, type1);
      }
    
      /* get the operands' type */
      type1 = _THROW(car(expr), context);
      type2 = _THROW(cdr(expr), context);
    
      if (SymbType_is_error(type1) || SymbType_is_error(type2)) {/* earlier error */
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }
    
      /* implicit conversion to the least common type of the operands */
      returnType = SymbType_get_minimal_common(type1, type2);

      /* types can be implicitily converted to one type */
      if (nullType != returnType) {
	return _SET_TYPE(ctx_expr, returnType);
      }
      /* is this a type error ? */
      if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, ctx_expr)) {
	return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
      }

      /* this is not an error after all -> return left operand's type */
      return _SET_TYPE(ctx_expr, type1);
    }

  default: /* there are no other kinds of expressions */
    fprintf(nusmv_stderr, "*** type_checker_check_expression,");
    fprintf(nusmv_stderr, "*** UNABLE TO HANDLE EXPRESSION ");
    print_node(nusmv_stderr, expr);
    nusmv_assert(("UNABLE TO HANDLE EXPRESSION" == 0));
  } /* switch */

  return nullType;
}




/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



/**Function********************************************************************

  Synopsis           [The type core violation handler.]

  Description        [The violation handler is implemented as 
  a virtual method, which is invoked by the checker when an expression
  being checked violates the type system.
  See the violation handler TypeCheckingViolationHandler_ptr
  for more explanations.

  The below function is the default violation handler, and a
  user can potentially define its own violation handler, by deriving 
  a new class from this class and by overriding this virtual method. 
  
  This violation handler outputs an error and warning message to
  nusmv_stderr. A warning is output if the detected violation is
  TC_VIOLATION_TYPE_BACK_COMP and the system variable
  "type_checking_backward_compatibility" is true. Also the
  TC_VIOLATION_TYPE_WARNING violation outputs a warning. Only in
  this case the false value is returned, indicating that this is NOT
  an error. Otherwise the true value is returned, indicating that this
  is an error.

  Also, if the system variable "type_check_warning_on" is false,
  warning messages are not output.

  NB: if the expression is checked in some context (context is not null) then
  before providing the expression to this function the expression should be
  wrapped into context, i.e. with find_node(CONEXT, context, expr)]

  SideEffects       []

  SeeAlso           [TypeSystemViolation]

******************************************************************************/
static boolean 
checker_core_viol_handler(CheckerBase_ptr self, 
			  TypeSystemViolation violation, node_ptr expression)
{
  /* In the output message, the information about the expression
     location are output. So, make sure that the input file name and
     line number are correctly set!
  */

  boolean isError = true; /* is this error or warning */

  /* get rid of the context the expression may be wrapped in */
  node_ptr context = Nil;
  node_ptr expr = expression;

  if (node_get_type(expression) == CONTEXT) {
    context = car(expression);
    expr = cdr(expression);
  }

  /* checks the given violation */
  nusmv_assert(TypeSystemViolation_is_valid(violation));

  /* only violation TC_VIOLATION_TYPE_BACK_COMP and the variable
     type_checking_backward_compatibility being true, may make a
     warning from an error.
     TC_VIOLATION_TYPE_WARNING always forces a warning
  */
  if (  TC_VIOLATION_TYPE_WARNING == violation 
     || ( TC_VIOLATION_TYPE_BACK_COMP == violation 
         && opt_backward_comp(options))) {
    isError = false;
  }

  if (!isError && !opt_type_checking_warning_on(options)) {
    /* this is a warning and warning messages are not allowed.
     So, do nothing, just return false (this is not an error)
    */
    return false;
  }

  _PRINT_ERROR_MSG(expr, isError);
  
  switch (violation) {
  case TC_VIOLATION_UNDEF_IDENTIFIER:
    fprintf(nusmv_stderr,  "undefined identifier '");
    print_node(nusmv_stderr, expr);
    fprintf(nusmv_stderr, "'\n");
    break;

  case TC_VIOLATION_AMBIGUOUS_IDENTIFIER:
    fprintf(nusmv_stderr,  "identifier '");
    print_node(nusmv_stderr, expr);
    fprintf(nusmv_stderr, "' is ambiguous\n");
    break;

  case TC_VIOLATION_OUT_OF_WORD_WIDTH:
    /* only bit-selection may have this kind of error */
    nusmv_assert(BIT_SELECTION == node_get_type(expr));
    fprintf(nusmv_stderr,  "bit number out of the Word's width ('");
    print_operator(nusmv_stderr, expr);
    fprintf(nusmv_stderr,  "' operator)\n");
    break;

  case TC_VIOLATION_OUT_OF_WORDARRAY_WIDTH:
    nusmv_assert(WAWRITE == node_get_type(expr) ||
		 WAREAD == node_get_type(expr));
    if (WAWRITE == node_get_type(expr)) {
      fprintf(nusmv_stderr,
	      "in WRITE expression the width of address or value operands"
	      " is not consistent with word-array operand\n");
    }
    else if (WAREAD == node_get_type(expr)) {
      fprintf(nusmv_stderr,
	      "in READ operator the width of address operand"
	      " is not consistent with word-array operand\n");
    }
    else { 
      /* only WRITE or READ (of word-array) may have this kind of error */
      nusmv_assert(false);
    }
    break;

  case TC_VIOLATION_TYPE_MANDATORY:
  case TC_VIOLATION_TYPE_BACK_COMP:
  case TC_VIOLATION_TYPE_WARNING:
    if (isError) fprintf(nusmv_stderr, "illegal ");
    else         fprintf(nusmv_stderr, "potentially incorrect ");

    switch (node_get_type(expr)) {
    case FAILURE: /* FAILURE does not participate in the type-checking */
    case ATOM: /* undeclared identifies checked earlier */
    case NUMBER:
    case NUMBER_WORD:
    case NUMBER_REAL:
    case NUMBER_FRAC:
    case NUMBER_EXP:
    case TRUEEXP:
    case FALSEEXP:
    case BIT:   /* undeclared identifies checked earlier */
    case BOOLEAN:
    case SELF:
    case INTEGER:
    case REAL:
    case WORD:
    case SCALAR:
    case ARRAY:
    case DOT:
    case TWODOTS: 
    case NEXT:
    case SMALLINIT:

    case CONTEXT:
    case BDD:
    case SEMI:
    case IFTHENELSE:
      print_sexp(nusmv_stderr, expr);
      nusmv_assert(false); /* this is impossible */
      
      /* possibly unary or binary operators */
    case PLUS:
    case MINUS:
    case UMINUS:
      fprintf(nusmv_stderr, "operand types of \"");
      print_operator(nusmv_stderr, expr);
      fprintf(nusmv_stderr,"\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      if (cdr(expr) != (node_ptr) NULL) {
	fprintf(nusmv_stderr," and ");
	checker_base_print_type(self, nusmv_stderr, cdr(expr), context);
      }
      break;
     
      /* Binary operators */
    case CONCATENATION:
    case CONS:
    case OR:
    case XOR:
    case XNOR:
    case IMPLIES:
    case IFF:
    case EQDEF:
    case AND:
    case RELEASES:
    case TRIGGERED:
    case UNTIL:
    case SINCE:
    case AU:
    case EU:
    case LSHIFT:
    case RSHIFT:
    case LROTATE:
    case RROTATE:
    case TIMES:
    case DIVIDE:
    case MOD:
    case EQUAL:
    case NOTEQUAL:
    case LT:
    case GT:
    case LE:
    case GE:
    case SLT:
    case SGT:
    case SLE:
    case SGE:
    case SIGN_EXTEND:
    case UNION:
    case SETIN:
    case MINU:
    case MAXU:
    case WAREAD:
      fprintf(nusmv_stderr, "operand types of \"");
      print_operator(nusmv_stderr, expr);
      fprintf(nusmv_stderr,"\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      fprintf(nusmv_stderr," and ");
      checker_base_print_type(self, nusmv_stderr, cdr(expr), context);
      break;

      /* Unary operators */
    case CAST_BOOL:
    case CAST_WORD1:
    case NOT:
    case EX:
    case AX:
    case EF:
    case AF:
    case EG:
    case AG:
    case OP_GLOBAL:
    case OP_HISTORICAL:
    case OP_NEXT:
    case OP_PREC:
    case OP_NOTPRECNOT:
    case OP_FUTURE:
    case OP_ONCE:
      fprintf(nusmv_stderr,"operand type of \"");
      print_operator(nusmv_stderr, expr);
      fprintf(nusmv_stderr,"\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      break;

    case COLON: /* for CASE */
     fprintf(nusmv_stderr,"left operand type of \":\". "
	     "It should be boolean, but is ");
     checker_base_print_type(self, nusmv_stderr, car(expr), context);
     break;
 
    case BIT_SELECTION:
      fprintf(nusmv_stderr,"operand types of \"[ : ]\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      fprintf(nusmv_stderr,", ");
      checker_base_print_type(self, nusmv_stderr, car(cdr(expr)), context);
      fprintf(nusmv_stderr," and ");
      checker_base_print_type(self, nusmv_stderr, cdr(cdr(expr)), context);
      break;

    case WAWRITE:
      fprintf(nusmv_stderr,"operand types of \"WRITE\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      fprintf(nusmv_stderr,", ");
      checker_base_print_type(self, nusmv_stderr, car(cdr(expr)), context);
      fprintf(nusmv_stderr," and ");
      checker_base_print_type(self, nusmv_stderr, cdr(cdr(expr)), context);
      break;

    case CASE:
      fprintf(nusmv_stderr,"types of \"case\" list elements : ");
      checker_base_print_type(self, nusmv_stderr, cdr(car(expr)), context);
      fprintf(nusmv_stderr," and ");
      checker_base_print_type(self, nusmv_stderr, cdr(expr), context);
      break;

    case EBF:
    case ABF:
    case EBG:
    case ABG:
      fprintf(nusmv_stderr,"operand type of \"");
      print_operator(nusmv_stderr, expr);
      fprintf(nusmv_stderr,"\" : ");
      checker_base_print_type(self, nusmv_stderr, car(expr), context);
      break;
 
    case ABU:
    case EBU:
      fprintf(nusmv_stderr,"operand types of \"");
      print_operator(nusmv_stderr, expr);
      fprintf(nusmv_stderr,"\" : ");
      checker_base_print_type(self, nusmv_stderr, car(car(expr)), context);
      fprintf(nusmv_stderr," and ");
      checker_base_print_type(self, nusmv_stderr, cdr(car(expr)), context);
      break;


    default: /* unknown kind of an expression */
      nusmv_assert(false);
    } /* switch (node_get_type(expr)) */
    fprintf(nusmv_stderr,"\n");
    break;
    
  default: 
    nusmv_assert(false); /* unknown kind of an error */
  } /* switch (errorKind) */
  
  return isError;
}



/**Static function*************************************************************

  Synopsis           [Just prints an expression's operator to output_stream]

  Description        [This function is the almost the same as 
  print_sexp, except this function does not print the children of the node.
  The expr must be a correct expression. 
  The function is used in printing of an error messages only.]

  SideEffects        []

  SeeAlso            [print_sexp]
******************************************************************************/
static void print_operator(FILE* output_stream, node_ptr expr)
{
  nusmv_assert((node_ptr) Nil != expr);
  switch(node_get_type(expr)){

  case CONCATENATION: fprintf(output_stream,"::"); return;
  case BIT_SELECTION: fprintf(output_stream,"[ : ]"); return;
  case CAST_BOOL:     fprintf(output_stream,"bool"); return;
  case CAST_WORD1:    fprintf(output_stream,"word1"); return;
  case AND:           fprintf(output_stream,"&"); return;
  case OR:            fprintf(output_stream,"|"); return;
  case XOR:           fprintf(output_stream,"xor"); return;
  case XNOR:          fprintf(output_stream,"xnor"); return;
  case IMPLIES:       fprintf(output_stream,"->"); return;
  case IFF:   fprintf(output_stream,"<->"); return;
  case NOT:   fprintf(output_stream,"!"); return;
  case EX:    fprintf(output_stream,"EX"); return;
  case AX:    fprintf(output_stream,"AX"); return;
  case EF:    fprintf(output_stream,"EF"); return;
  case AF:    fprintf(output_stream,"AF"); return;
  case EG:    fprintf(output_stream,"EG"); return;
  case AG:    fprintf(output_stream,"AG"); return;
  case OP_GLOBAL:     fprintf(output_stream,"G"); return;
  case OP_HISTORICAL: fprintf(output_stream,"H"); return;
  case RELEASES:      fprintf(output_stream,"V"); return;
  case TRIGGERED:     fprintf(output_stream,"T"); return;
  case UNTIL:    fprintf(output_stream,"U"); return;
  case SINCE:    fprintf(output_stream,"S"); return;
  case OP_NEXT:  fprintf(output_stream,"X"); return;
  case OP_PREC:  fprintf(output_stream,"Y"); return;
  case OP_NOTPRECNOT:    fprintf(output_stream,"Z"); return;
  case OP_FUTURE:        fprintf(output_stream,"F"); return;
  case OP_ONCE:          fprintf(output_stream,"O"); return;
  case AU:   fprintf(output_stream,"A-U"); return;
  case EU:   fprintf(output_stream,"E-U"); return;
  case EBF:  fprintf(output_stream,"EBF"); return;
  case ABF:  fprintf(output_stream,"ABF"); return;
  case EBG:  fprintf(output_stream,"EBG"); return;
  case ABG:  fprintf(output_stream,"ABG"); return;
  case ABU:  fprintf(output_stream,"ABU"); return;
  case EBU:  fprintf(output_stream,"EBU"); return;
  case LSHIFT:    fprintf(output_stream,"<<"); return;
  case RSHIFT:    fprintf(output_stream,">>"); return;
  case LROTATE:   fprintf(output_stream,"<<<"); return;
  case RROTATE:   fprintf(output_stream,">>>"); return;
  case PLUS:      fprintf(output_stream,"+"); return;
  case MINUS:     fprintf(output_stream,"-"); return;
  case UMINUS:    fprintf(output_stream,"unary -"); return;
  case TIMES:     fprintf(output_stream,"*"); return;
  case DIVIDE:    fprintf(output_stream,"/"); return;
  case MOD:       fprintf(output_stream,"mod"); return;
  case EQUAL:     fprintf(output_stream,"="); return;
  case NOTEQUAL:  fprintf(output_stream,"!="); return;
  case LT:    fprintf(output_stream,"<"); return;
  case GT:    fprintf(output_stream,">"); return;
  case LE:    fprintf(output_stream,"<="); return;
  case GE:    fprintf(output_stream,">="); return;
  case SLT:    fprintf(output_stream,"signedLT"); return;
  case SGT:    fprintf(output_stream,"signedGT"); return;
  case SLE:    fprintf(output_stream,"signedLE"); return;
  case SGE:    fprintf(output_stream,"signedGE"); return;
  case SIGN_EXTEND:    fprintf(output_stream,"sign-extend"); return;
  case UNION: fprintf(output_stream,"union"); return;
  case SETIN: fprintf(output_stream,"in"); return;
  case COLON: fprintf(output_stream,":"); return;
  case WAREAD: fprintf(output_stream,"READ"); return;
  case WAWRITE: fprintf(output_stream,"WRITE"); return;

  case EQDEF:     fprintf(output_stream,":="); return;
  case MINU:    fprintf(output_stream,"MIN"); return;
  case MAXU:    fprintf(output_stream,"MAX"); return;
  case CASE:      fprintf(output_stream,"\n(CASE "); return;
  case CONS:      fprintf(output_stream,","); return;
  case VAR:       fprintf(output_stream,"\n(VAR "); return;
  case FAILURE:      fprintf(output_stream,"\n(FAILURE)"); return;
  case ATOM:      fprintf(output_stream,"\n(ATOM "); return;
  case NUMBER:    
    fprintf(output_stream,"\n(NUMBER %d)",NODE_TO_INT(car(expr))); 
    return;
  case NUMBER_WORD: fprintf(output_stream,"\n(NUMBER_WORD ");
                    WordNumber_print(output_stream, WORD_NUMBER(car(expr)));
                    fprintf(output_stream,")"); return;
  case NUMBER_REAL: fprintf(output_stream,"\n(NUMBER_REAL %s)",
			    get_text((string_ptr)car(expr))); return;
  case NUMBER_FRAC: fprintf(output_stream,"\n(NUMBER_FRAC %s)",
			    get_text((string_ptr)car(expr))); return;
  case NUMBER_EXP: fprintf(output_stream,"\n(NUMBER_EXP %s)",
			    get_text((string_ptr)car(expr))); return;
  case TRUEEXP:     fprintf(output_stream,"(TRUE)"); return;
  case FALSEEXP:    fprintf(output_stream,"(FALSE)"); return;
  case BIT:         fprintf(output_stream,"\n(BIT "); return;
  case BOOLEAN:     fprintf(output_stream,"(BOOLEAN)"); return;
  case INTEGER:     fprintf(output_stream,"(INT)"); return;
  case REAL:    fprintf(output_stream,"(REAL)"); return;
  case WORD:    fprintf(output_stream,"(WORD "); return;
  case SELF:    fprintf(output_stream,"(SELF)"); return;
  case SCALAR:  fprintf(output_stream,"(SCALAR "); return;
  case ARRAY:   fprintf(output_stream,"\n(ARRAY "); return;
  case DOT:     fprintf(output_stream,"(DOT "); return;
  case TWODOTS:    fprintf(output_stream, ".."); return;
  case NEXT:    fprintf(output_stream,"NEXT"); return;
  case SMALLINIT:    fprintf(output_stream,"\n(SMALLINIT "); return;

  case CONTEXT:    fprintf(output_stream,"\n(CONTEXT "); return;
  case BDD:   fprintf(output_stream,"(BDD TO BE PRINTED)"); return;
  case SEMI:    fprintf(output_stream, "\n(SEMI "); return;
  case IFTHENELSE:    fprintf(output_stream, "\n (IFTHENELSE "); return;
        
  default:
    fprintf(nusmv_stderr, "\n%d\n", node_get_type(expr));
    nusmv_assert(false);
  }

}



/**AutomaticEnd***************************************************************/

