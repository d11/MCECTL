/**CFile***********************************************************************

  FileName    [CheckerMbp.c]

  PackageName [compile.type_checking.checkers]

  Synopsis    [Implementaion of class 'CheckerMbp'.
  All thse functions are required to type check expressions and 
  statements related to MBP package. This class is very small
  because MBP mostly uses usual NuSMV expressions.
  ]

  Description []

  SeeAlso     [CheckerMbp.h]

  Author      [Andrei Tchaltsv]

  Copyright   [
  This file is part of the ``compile.type_checking.checkers'' package of
  NuSMV version 2. Copyright (C) 2006 by ITC-irst. 

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

  Revision    [$Id: CheckerMbp.c,v 1.1.2.1.4.1 2007/04/04 12:00:33 nusmv Exp $]

******************************************************************************/

#include "CheckerMbp.h" 
#include "CheckerMbp_private.h" 

#include "compile/compile.h"
#include "compile/symb_table/symb_table.h"

#include "parser/symbols.h"
#include "parser/mbp_symbols.h"
#include "utils/utils.h" 
#include "utils/error.h" 

static char rcsid[] UTIL_UNUSED = "$Id: CheckerMbp.c,v 1.1.2.1.4.1 2007/04/04 12:00:33 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

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

static void checker_mbp_finalize ARGS((Object_ptr object, void* dummy));

static SymbType_ptr checker_mbp_check_expr ARGS((CheckerBase_ptr self, 
						 node_ptr e, node_ptr ctx));

static boolean 
checker_mbp_viol_handler ARGS((CheckerBase_ptr self, 
			       TypeSystemViolation violation, 
			       node_ptr expression));

static void print_operator ARGS((FILE* output_stream, node_ptr expr));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The CheckerMbp class constructor]

  Description        [The CheckerMbp class constructor]

  SideEffects        []

  SeeAlso            [NodeWalker_destroy]   
  
******************************************************************************/
CheckerMbp_ptr CheckerMbp_create()
{
  CheckerMbp_ptr self = ALLOC(CheckerMbp, 1);
  CHECKER_MBP_CHECK_INSTANCE(self);

  checker_mbp_init(self);
  return self;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The CheckerMbp class private initializer]

  Description        [The CheckerMbp class private initializer]

  SideEffects        []

  SeeAlso            [CheckerMbp_create]   
  
******************************************************************************/
void checker_mbp_init(CheckerMbp_ptr self)
{
  /* base class initialization */
  checker_base_init(CHECKER_BASE(self), "MBP Type Checker", 
		    NUSMV_MBP_SYMBOL_FIRST, 
		    NUSMV_MBP_SYMBOL_LAST - NUSMV_MBP_SYMBOL_FIRST);
  
  /* members initialization */

  /* virtual methods settings */  
  OVERRIDE(Object, finalize) = checker_mbp_finalize;
  OVERRIDE(CheckerBase, check_expr) = checker_mbp_check_expr;
  OVERRIDE(CheckerBase, viol_handler) = checker_mbp_viol_handler;
}


/**Function********************************************************************

  Synopsis           [The CheckerMbp class private deinitializer]

  Description        [The CheckerMbp class private deinitializer]

  SideEffects        []

  SeeAlso            [CheckerMbp_destroy]   
  
******************************************************************************/
void checker_mbp_deinit(CheckerMbp_ptr self)
{
  /* members deinitialization */

  /* base class initialization */
  checker_base_deinit(CHECKER_BASE(self));
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [The CheckerMbp class virtual finalizer]

  Description [Called by the class destructor]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void checker_mbp_finalize(Object_ptr object, void* dummy) 
{
  CheckerMbp_ptr self = CHECKER_MBP(object);

  checker_mbp_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
static SymbType_ptr checker_mbp_check_expr(CheckerBase_ptr self, 
					   node_ptr expr, node_ptr context)
{

  /* wrap expr into the context. This is required by 
     the facilities which remembers the type of expressions 
     and by the violation handler. */
  node_ptr ctx_expr;
  if (context != Nil) ctx_expr = find_node(CONTEXT, context, expr);
  else ctx_expr = expr;
  
  { /* checks memoizing */
    SymbType_ptr tmp = _GET_TYPE(ctx_expr);
    if (nullType != tmp) return tmp;
  }
  
  switch (node_get_type(expr)) {
  case PLAYER:
  case GAME:
    /* statements and expressions are type checked, not 
       the whole GAME or its PLAYER structures
    */ 
    nusmv_assert(false);
   
    /* these are the usual specification. Therefore the same code as 
       in CheckerStatements.c for specification is used.
    */
  case REACHTARGET:
  case AVOIDTARGET:
  case REACHDEADLOCK:
  case AVOIDDEADLOCK:
  case BUCHIGAME:
  case GENREACTIVITY:
  {
    /* get the operand's type */
    SymbType_ptr type = _THROW(car(expr), Nil);
    
    if (SymbType_is_error(type)) {
      /* earlier error */
      return _SET_TYPE(ctx_expr, SymbTablePkg_error_type()); 
    }
    
    /* The operand must be boolean or statement-type (statements can
       be produced by predicate-normalisation in symbolic FSM (when
       assignments are pushed into expressions). */
    if (SymbType_is_boolean_enum(type) || SymbType_is_statement(type)) {
      return _SET_TYPE(ctx_expr, type);
    }

    /* there is violation */
    if (_VIOLATION(TC_VIOLATION_TYPE_MANDATORY, expr)) {
      return _SET_TYPE(ctx_expr, SymbTablePkg_error_type());
    }

    /* this is not an error after all -> keeps the current type */
    return _SET_TYPE(ctx_expr, type);
  }

  /* this exp is just wrapper. just return the type of operand */
  case MBP_SPEC_WRAPPER:
    return _SET_TYPE(ctx_expr, _THROW(cdr(expr), context));
  /* this exp is just wrapper. just return the type of operand */
  case MBP_EXP_LIST: /* CONS in CheckerCore.c will check the list */
    return _SET_TYPE(ctx_expr, _THROW(car(expr), context));

    /* CONS in CheckerCore.c will check the lists. 
       Then behave the same way as CONS, i.e. return the least common type.
    */
  case MBP_TWO_EXP_LISTS: /* two exp lists */
  {
    SymbType_ptr returnType;
    
    /* get the operands' type */
    SymbType_ptr type1 = _THROW(car(expr), context);
    SymbType_ptr type2 = _THROW(cdr(expr), context);
    
    if (SymbType_is_error(type1) || SymbType_is_error(type2)) {/*earlier error*/
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

  default:
    internal_error("checker_mbp_check_expr: not supported type");
  }
  
  nusmv_assert(false); /* impossible code */
  return nullType;  
}


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
  
  This violation handler outputs only error messages (no warnings)
  and always returns true (indicateing that this is an error, not a warning).

  NB: if the expression is checked in some context (context is not null) then
  before providing the expression to this function the expression should be
  wrapped into context, i.e. with find_node(CONEXT, context, expr)]

  SideEffects       []

  SeeAlso           [TypeSystemViolation]

******************************************************************************/
static boolean 
checker_mbp_viol_handler(CheckerBase_ptr self, 
			 TypeSystemViolation violation, node_ptr expression)
{
  /* get rid of the context the expression may be wrapped in */
  node_ptr context = Nil;
  node_ptr expr = expression;
  if (node_get_type(expression) == CONTEXT) {
    context = car(expression);
    expr = cdr(expression);
  }

  _PRINT_ERROR_MSG(expr, true);
  
  /* this function is very simplified, because only one error situation is 
     expected : TC_VIOLATION_TYPE_MANDATORY
  */
  nusmv_assert(TC_VIOLATION_TYPE_MANDATORY == violation);

  fprintf(nusmv_stderr, "illegal ");

  switch (node_get_type(expr)) {
  case PLAYER:
  case GOTO: 
    nusmv_assert(false); /* this is impossible */  
       
  case REACHTARGET:
  case AVOIDTARGET:
  case REACHDEADLOCK:
  case AVOIDDEADLOCK:
  case BUCHIGAME:
  case GENREACTIVITY:
    fprintf(nusmv_stderr,"type of ");
    print_operator(nusmv_stderr, expr);
    fprintf(nusmv_stderr," expression : ");
    checker_base_print_type(self, nusmv_stderr, car(expr), context);
    break;

  case MBP_SPEC_WRAPPER:
  case MBP_EXP_LIST:
    nusmv_assert(false);    /* this node cannot generate an error */  
    
  case MBP_TWO_EXP_LISTS: /* two exp lists */
    fprintf(nusmv_stderr, "operand types of \"");
    print_operator(nusmv_stderr, expr);
    fprintf(nusmv_stderr,"\" : ");
    checker_base_print_type(self, nusmv_stderr, car(expr), context);
    fprintf(nusmv_stderr," and ");
    checker_base_print_type(self, nusmv_stderr, cdr(expr), context);
    break;
    
  default: 
    nusmv_assert(false); /* unknown kind of exp */
      
  } /* switch (violation) */
    
  return true;
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
  nusmv_assert(expr != Nil);
  switch (node_get_type(expr)) {
  case PLAYER:    fprintf(output_stream,"\n(PLAYER "); return;
  case GOTO:    fprintf(output_stream,"\n(GOTO "); return;

  case REACHTARGET:        fprintf(output_stream,"REACHTARGET"); return;
  case AVOIDTARGET:        fprintf(output_stream,"AVOIDTARGET"); return;
  case REACHDEADLOCK:      fprintf(output_stream,"REACHDEADLOCK"); return;
  case AVOIDDEADLOCK:      fprintf(output_stream,"AVOIDDEADLOCK"); return;
  case BUCHIGAME:          fprintf(output_stream,"BUCHIGAME"); return;
  case GENREACTIVITY:      fprintf(output_stream,"GENREACTIVITY"); return;
  case MBP_SPEC_WRAPPER:   fprintf(output_stream,"MBP_SPEC_WRAPPER"); return;
  case MBP_EXP_LIST:       fprintf(output_stream,"MBP_EXP_LIST"); return;
  case MBP_TWO_EXP_LISTS:  fprintf(output_stream,"->"); return;
        
  default:
    fprintf(nusmv_stderr, "\n%d\n", node_get_type(expr));
    nusmv_assert(false);
  }

}




/**AutomaticEnd***************************************************************/

