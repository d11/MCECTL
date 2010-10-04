/**CFile***********************************************************************

  FileName    [ltlRewrite.c]

  PackageName [ltl]

  Synopsis    [Rewrite formula to keep track of possible inputs]

  Description [This file provide a mechanism to rewrite an LTL formula
  containing inputs in a way that the inputs are interpreted
  correctly. Let us suppose that a formula Phi speak about an input
  boolean variable i. The i refers to the input received to move from
  the previous state if any to the current state. That is we observe
  it in the next state as the result of the transition executed. Thus
  we build a new formula Phi' such that Phi' = Phi\[i/_pi] \& X _pi.
  We modify the transition relation of the domain by imposing that the
  variable _pi store the value of the read input.

  The rewriting can be generalized to non boolean variables
  generalizing the following case (x state, i inpt):

  G (x < i) becomes

  G (exists _p : range(i). (x < _p) & X (_pi = _p))

  and we add the following to the model

  VAR
    _pi : range(i);
  ASSIGN
    next(_pi) = i;

  exists are handled by expanding them as a disjunction over the
  possible values in range(i). In this case if i has range 0..4 it
  becomes:

  G (((x < 0) & X (_pi = 0)) | ((x < 1) & X (_pi = 1)) |
     ((x < 2) & X (_pi = 2)) | ((x < 3) & X (_pi = 3)))

  ]

  SeeAlso     [optional]

  Author      [Marco Roveri]

  Copyright   [   This file is part of the ``ltl'' package of NuSMV version 2. 
  Copyright (C) 2004 by ITC-irst. 

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

#include "ltlInt.h" 
#include "ltl/ltl.h"
#include "parser/symbols.h"
#include "ltlInt.h" 

#include "utils/utils.h"
#include "utils/error.h"
#include "utils/assoc.h"

static char rcsid[] UTIL_UNUSED = "$Id: ltlRewrite.c,v 1.1.2.4.4.8.6.4 2007/05/15 15:22:18 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static int ltl_ltlspec_counter = -1;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define IPRE_PREFIX "LTL_"
#define IPREFIXNAME "_INPUT_"

#define expr_mk_wff(a,b,c) new_node(a, b, c)
#define expr_mk_and(a,b) new_node(AND, a, b)
#define expr_mk_or(a,b) new_node(OR, a, b)
#define expr_mk_false() find_node(FALSEEXP, Nil, Nil)
#define expr_mk_X(a) new_node(OP_NEXT, a, Nil)
#define expr_mk_next(a) new_node(NEXT, a, Nil)
#define expr_mk_setin(a,b) new_node(SETIN, a, b)
#define expr_mk_equal(a,b) new_node(EQUAL, a, b)
#define expr_get_type(a) node_get_type(a)
#define expr_get_left(a) car(a)
#define expr_get_right(a) cdr(a)
#define expr_get_kind(a) node_get_type(a)
#define expr_is_equal(a,b) a == b

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int get_digits ARGS((int n));
static node_ptr extract_inputs ARGS((SymbTable_ptr symb_table, node_ptr expr));
static hash_ptr build_input_vars_assoc ARGS((SymbTable_ptr symb_table, 
					     node_ptr inputs));
static node_ptr expand_input ARGS((SymbTable_ptr symb_table, node_ptr expr, 
				   hash_ptr assoc));
static node_ptr get_var_name ARGS((hash_ptr assoc, node_ptr name));
static node_ptr get_var_values ARGS((hash_ptr assoc, node_ptr name));
static node_ptr get_var_trans ARGS((hash_ptr assoc, node_ptr name));
static node_ptr expr_subst_all ARGS((SymbTable_ptr symb_table, node_ptr expr, 
				     node_ptr s, node_ptr d));
static boolean expr_is_wff ARGS((node_ptr expr));
static boolean expr_is_term ARGS((node_ptr expr));
static boolean expr_is_constant ARGS((SymbTable_ptr symb_table, 
				      node_ptr expr));
static assoc_retval free_assoc_data_aux ARGS((char *key, char *data, char * arg));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Rewrites an LTL formula if inputs are present]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
WFFR_TYPE ltlRewriteWffIfInputPresent(SymbTable_ptr symb_table, node_ptr expr)
{
  WFFR_TYPE result;
  node_ptr inputs = extract_inputs(symb_table, expr);

  if (inputs == Nil) result = WFFR_NOTREWRITTEN(expr);
  else {
    node_ptr e_expr = Compile_FlattenSexpExpandDefine(symb_table, expr, Nil);
    hash_ptr assoc = build_input_vars_assoc(symb_table, inputs);
    node_ptr r_expr = expand_input(symb_table, e_expr, assoc);

    if (opt_verbose_level_gt(options, 2)) {
      fprintf(nusmv_stderr, "The rewritten formula is ");
      print_node(nusmv_stderr, r_expr);
      fprintf(nusmv_stderr, "\n");
    }

    result = WFFR_REWRITTEN(r_expr, WFF_MKASSOC(inputs, assoc));
  }
  return result;
}

/**Function********************************************************************

  Synopsis           [Add the variables introduced by the rewriting]

  Description        [Declares the new variables introduced by the
  rewriting and builds their transition relation]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void ltlHandleInputsVars(SymbLayer_ptr layer, WFFR_TYPE wr,
                         node_ptr* init, node_ptr* invar, node_ptr* trans)
{
  node_ptr i;
  node_ptr inputs = WFF_ASSOC_GET_INPUTS(WFFR_GET_ASSOC(wr));
  hash_ptr assoc = WFF_ASSOC_GET_ASSOC(WFFR_GET_ASSOC(wr));
  
  for (i = inputs; i != Nil; i = cdr(i)) {
    node_ptr name = car(i);
    ASSOC_DATA_TYPE data = find_assoc(assoc, name);

    if (data == (ASSOC_DATA_TYPE)NULL) {
      rpterr("ltlHandleInputsVars: no data associated to variable\n");
    }
    SymbLayer_declare_state_var(layer, assoc_get_var_name(data),
				SymbType_create(SYMB_TYPE_ENUM,
						assoc_get_var_values(data)));
    *trans = expr_mk_and(*trans, assoc_get_trans(data));
  }
}

/**Function********************************************************************

  Synopsis           [Free memory in the rewrite structure]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void ltlFreeWFFR_TYPE(WFFR_TYPE rw) {
  if (WFFR_IS_REWRITTEN(rw)) {
    hash_ptr assoc = WFF_ASSOC_GET_ASSOC(WFFR_GET_ASSOC(rw));
    node_ptr inputs = WFF_ASSOC_GET_INPUTS(WFFR_GET_ASSOC(rw));

    free_list(inputs);
    clear_assoc_and_free_entries(assoc, free_assoc_data_aux);
    free_assoc(assoc);
  }
  WFFR_FREE(rw);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Extracts the set of inputs from a formula]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr extract_inputs(SymbTable_ptr symb_table, node_ptr expr) {
  node_ptr l;
  node_ptr result = Nil;
  Set_t dependencies = Formula_GetDependencies(symb_table, expr, Nil);
  node_ptr list = Set_Set2List(dependencies);


  for(l = list; l != Nil; l = cdr(l)) {
    if (SymbTable_is_symbol_input_var(symb_table, car(l))) {
      result = cons(car(l), result);
    }
  }
  free_list(list);
  return result;
}

/**Function********************************************************************

  Synopsis           [Rewrites an LTL formula]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr 
expand_input(SymbTable_ptr symb_table, node_ptr expr, hash_ptr assoc) 
{
  if (expr == Nil) return expr;
  if (SymbTable_is_symbol_var(symb_table, expr)) {
    if (SymbTable_is_symbol_input_var(symb_table, expr))
      /* we replace the input with the corresponding X p_var */
      return expr_mk_X(get_var_name(assoc, expr));
    else 
      return expr;
  }
  else if (expr_is_wff(expr)) {
    /* recursively descend the formula */
    return expr_mk_wff(expr_get_type(expr),
                       expand_input(symb_table, car(expr), assoc),
                       expand_input(symb_table, cdr(expr), assoc));
  }
  else if (expr_is_term(expr)) {
    node_ptr inputs = extract_inputs(symb_table, expr);

    /* does the term not refer to any input? */
    if (inputs == Nil) return expr;
    else {
      /* we extract the first input of the list of inputs */
      node_ptr var_i = car(inputs);
      node_ptr name = get_var_name(assoc, var_i);
      node_ptr vals = get_var_values(assoc, var_i);
      node_ptr acc_e = expr_mk_false();

      for( ; vals != Nil; vals = cdr(vals)) {
        node_ptr val = car(vals);

        if (acc_e == expr_mk_false()) {
          acc_e = expr_mk_and(expr_subst_all(symb_table, expr, var_i, val),
                              expr_mk_X(expr_mk_equal(name, val)));
        }
        else {
          acc_e = expr_mk_or(acc_e,
                             expr_mk_and(expr_subst_all(symb_table, expr,
							var_i, val),
                                         expr_mk_X(expr_mk_equal(name, val))));
        }
      }

      if (llength(inputs) == 1)
        /* There is no need to recurse, no more inputs will be there */
        return acc_e;
      else 
        return expand_input(symb_table, acc_e, assoc);
    }
  }
  else if (expr_is_constant(symb_table, expr)) return expr;		
  else {
    rpterr("Unable to recognize operator: %d\n", node_get_type(expr));
    return Nil;
  }
}

/**Function********************************************************************

  Synopsis           [Subst occurrence of s with d]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr 
expr_subst_all(SymbTable_ptr symb_table, node_ptr expr, node_ptr s, node_ptr d)
{
  if (expr == Nil) return expr;
  if (expr_is_constant(symb_table, expr)) return expr;
  if (SymbTable_is_symbol_var(symb_table, expr)) {
    if (expr_is_equal(expr, s)) 
      return d;
    else
      return expr;
  }
  else if (expr_is_wff(expr) || expr_is_term(expr)) {
    return expr_mk_wff(expr_get_kind(expr),
                       expr_subst_all(symb_table, expr_get_left(expr), s, d),
                       expr_subst_all(symb_table, expr_get_right(expr), s, d));
  }
  else {
    rpterr("Unable to recognize operator: %d\n", node_get_type(expr));
    return Nil;
  }
}

/**Function********************************************************************

  Synopsis           [checks wheter the formula is a wff]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean expr_is_wff(node_ptr expr) {
  switch(expr_get_type(expr)) {
  case NOT:
  case AND: case OR: case IMPLIES: case IFF: case XOR: case XNOR:
  case UNTIL: case SINCE: case RELEASES: case TRIGGERED:
  case OP_NEXT: case OP_PREC: case OP_NOTPRECNOT: case OP_GLOBAL:
  case OP_HISTORICAL: case OP_FUTURE: case OP_ONCE: case COLON:
  return true;
  default:
    return false;
  }
}

/**Function********************************************************************

  Synopsis           [checks wheter the formula is a term]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean expr_is_term(node_ptr expr) {
  switch(expr_get_type(expr)) {
  case EQUAL: case NOTEQUAL:
  case LT: case GT:
  case LE: case GE:
  case PLUS: case MINUS:
  case TIMES: case MOD: case DIVIDE:
  case UNION: case SETIN: case CASE:
    return true;
  default:
    return false;
  }
}

/**Function********************************************************************

  Synopsis           [checks wheter the formula is a constant]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean expr_is_constant(SymbTable_ptr symb_table, node_ptr expr) 
{
  switch(expr_get_type(expr)) {
  case TRUEEXP: case FALSEEXP: case NUMBER: 
    return true;
	case FAILURE: return true;
  default:
    return SymbTable_is_symbol_constant(symb_table, expr);
  }
}


/**Function********************************************************************

  Synopsis           [Builds assoc variables]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static hash_ptr 
build_input_vars_assoc(SymbTable_ptr symb_table, node_ptr inputs) 
{
  hash_ptr result = new_assoc();

  for( ;inputs != Nil; inputs = cdr(inputs)) {
    size_t len;
    char * cname;
    node_ptr new_name;
    node_ptr name = car(inputs);

    len = strlen(IPRE_PREFIX) + strlen(IPREFIXNAME) + 
      get_digits(++ltl_ltlspec_counter);
    cname = ALLOC(char, len+1);

    if (cname == NIL(char)) {
      rpterr("build_input_vars_assoc: Unable to allocate memory.");
    }
    snprintf(cname, len, "%s%d%s", IPRE_PREFIX, ltl_ltlspec_counter, IPREFIXNAME);

    new_name = find_node(DOT, Nil, sym_intern(cname));

    /* mem no longer needed */
    FREE(cname);

    {
      SymbType_ptr type;
      node_ptr values, trans, data;

      type = SymbTable_get_var_type(symb_table, name);
      /* Only ENUM type is implemented, so I added the assertion */
			if (SYMB_TYPE_ENUM != SymbType_get_tag(type)) {
				error_not_supported_feature("Input variable has a type that is not "\
																		"currently supported by BDD-based LTL");
			}

      values = SymbType_get_enum_type_values(type);
      trans = expr_mk_setin(expr_mk_next(new_name), name);
      data = assoc_mk_data(new_name, values, trans);
      

      if (opt_verbose_level_gt(options, 2)) {
        fprintf(nusmv_stderr, "Associating ");
        print_node(nusmv_stderr, name);
        fprintf(nusmv_stderr, " to ");
        print_node(nusmv_stderr, new_name);
        fprintf(nusmv_stderr, "\n");
      }
      /* Inserting in the asscoaitive table the data */
      insert_assoc(result, name, data);
    }
  }
  return result;
}

/**Function********************************************************************

  Synopsis           [Retrieves values of new var associated to name]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr get_var_values(hash_ptr assoc, node_ptr name) {
  node_ptr d = find_assoc(assoc, name);

  if (d != NULL) {
    return assoc_get_var_values(d);
  }
  return Nil;
}
    
/**Function********************************************************************

  Synopsis           [Retrieves name of new var associated to name]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr get_var_name(hash_ptr assoc, node_ptr name) {
  node_ptr d = find_assoc(assoc, name);

  if (d != NULL) {
    return assoc_get_var_name(d);
  }
  return Nil;
}
    
/**Function********************************************************************

  Synopsis           [Retrieves transition relation of new var associated to name]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static node_ptr get_var_trans(hash_ptr assoc, node_ptr name) {
  node_ptr d = find_assoc(assoc, name);

  if (d != NULL) {
    return assoc_get_trans(d);
  }
  return Nil;
}

/**Function********************************************************************

  Synopsis           [Retrieves numbers of digit of a number]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int get_digits(int n)
{
  int d;
  for (d = 1; n != 0; ++d)  n = n/10;
  return d;
}

    

/**Function********************************************************************

  Synopsis           [Retrieves numbers of digit of a number]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static assoc_retval free_assoc_data_aux(char *key, char *data, char * arg) {
  node_ptr d = (node_ptr)data;

  if (d != (node_ptr)NULL) {
    free_assoc_data(d);
  }

  return(ASSOC_DELETE);
}
