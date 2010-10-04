/**CFile***********************************************************************

  FileName    [bmcConv.c]

  PackageName [bmc]

  Synopsis    [Convertion function of BE to corresponding BDD boolean 
  expression, and viceversa]

  Description [This implementation is still depedent on the rbc package]

  SeeAlso     []

  Author      [Alessandro Cimatti and Lorenzo Delana]

  Copyright   [
  This file is part of the ``bmc'' package of NuSMV version 2. 
  Copyright (C) 2000-2001 by ITC-irst and University of Trento. 

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

#include "bmcConv.h"
#include "bmcWff.h"

#include "parser/symbols.h" /* for NUMBER et similia */
#include "compile/compile.h"
#include "rbc/rbcInt.h"     /* for RBCVAR et similia */ 

#include "utils/array.h"
#include "utils/assoc.h"
#include "utils/error.h"

static char rcsid[] UTIL_UNUSED = "$Id: bmcConv.c,v 1.5.2.6.2.4.2.8.4.7 2007/03/27 09:50:48 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [Be2bexpDfsData]

  Description [The data structure used for DFS traversal of RBC]

  SeeAlso     []

******************************************************************************/
typedef struct Be2bexpDfsData_TAG {
  BeEnc_ptr be_enc; 
  array_t* stack;
  int head;
} Be2bexpDfsData;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static hash_ptr bexpr2be_hash = (hash_ptr) NULL;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void bmc_conv_set_cache ARGS((node_ptr bexp, be_ptr be));
static be_ptr bmc_conv_query_cache ARGS((node_ptr bexp));

static be_ptr 
bmc_conv_bexp2be_recur ARGS((BeEnc_ptr be_enc, node_ptr bexp));

/* Primitives for stack used in BE traversal */
static void Be2bexpDfsData_push ARGS((Be2bexpDfsData*, node_ptr));
static node_ptr Be2bexpDfsData_head ARGS((Be2bexpDfsData*));
static node_ptr Be2bexpDfsData_pop  ARGS((Be2bexpDfsData*));

/* BE traversal functions */
static int  Be2bexp_Set   ARGS((be_ptr be, char* Be2bexpData, nusmv_ptrint sign));
static void Be2bexp_First ARGS((be_ptr be, char* Be2bexpData, nusmv_ptrint sign));
static void Be2bexp_Back  ARGS((be_ptr be, char* Be2bexpData, nusmv_ptrint sign));
static void Be2bexp_Last  ARGS((be_ptr be, char* Be2bexpData, nusmv_ptrint sign));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Given a be, constructs the corresponding boolean
  expression]

  Description        [Descends the structure of the BE with dag-level 
  primitives. Uses the be encoding to perform all time-related operations. ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr Bmc_Conv_Be2Bexp(BeEnc_ptr be_enc, be_ptr be)
{
  Dag_DfsFunctions_t Be2bexpFunctions;
  Be2bexpDfsData Be2bexpData;
  node_ptr ret;

  /* Cleaning the user fields. */
  Dag_Dfs(be, &dag_DfsClean, NIL(char));

  /* Setting up the DFS functions. */
  Be2bexpFunctions.Set        = Be2bexp_Set;
  Be2bexpFunctions.FirstVisit = Be2bexp_First;
  Be2bexpFunctions.BackVisit  = Be2bexp_Back;
  Be2bexpFunctions.LastVisit  = Be2bexp_Last;
 
  /* Setting up the DFS data. */
  /* :TODO??: optimizations on the quantity of nodes */
  Be2bexpData.be_enc = be_enc;  
  Be2bexpData.stack = array_alloc(node_ptr, 10);
  Be2bexpData.head = -1;

  /* Calling DFS on f. */
  Dag_Dfs(be, &Be2bexpFunctions, (char*)(&Be2bexpData));
  ret = Be2bexpDfsData_head(&Be2bexpData);

  array_free(Be2bexpData.stack);

  return ret;
}


/**Function********************************************************************

  Synopsis           [<b>Converts</b> given <b>boolean expression</b> into
  correspondent <b>reduced boolean circuit</b>]

  Description        [Uses the be encoding to perform all 
  time-related operations.]

  SideEffects        [be hash may change]

  SeeAlso            []

******************************************************************************/
be_ptr Bmc_Conv_Bexp2Be(BeEnc_ptr be_enc, node_ptr bexp) 
{
  return bmc_conv_bexp2be_recur(be_enc, bexp);
}


/**Function********************************************************************

  Synopsis           [Private service for Bmc_Conv_Bexp2Be]

  Description        [Recursive service for Bmc_Conv_Bexp2Be, with caching of
  results]

  SideEffects        []

  SeeAlso            [Bmc_Conv_Bexp2Be]

******************************************************************************/
static be_ptr bmc_conv_bexp2be_recur(BeEnc_ptr be_enc, node_ptr bexp)
{
  be_ptr result = (be_ptr) NULL;

  /* if given expression is Nil, returns truth be */
  /* Nil value can be used in AND sequences, so a true value must
     be returned */
  if (bexp == Nil) return Be_Truth(BeEnc_get_be_manager(be_enc));

  /* queries the cache: */
  result = bmc_conv_query_cache(bexp);
  if (result != (be_ptr) NULL) return result;

  switch (node_get_type(bexp)) {
  case TRUEEXP:
    result = Be_Truth(BeEnc_get_be_manager(be_enc));
    break;

  case FALSEEXP:
    result = Be_Falsity(BeEnc_get_be_manager(be_enc));
    break;

  case NEXT: 
    /* converts NEXT arg (a state variable) into the correspondent
       next variable. arg must be a state variable */
    result = BeEnc_name_to_untimed(be_enc, bexp);
    break;

  case NOT:     
    /* NOT arg is converted into an be, and its negation is result =ed */
    result = Be_Not( BeEnc_get_be_manager(be_enc), 
		     bmc_conv_bexp2be_recur(be_enc, car(bexp)) );
    break;

  case CONS: 
  case AND:  
    result = Be_And( BeEnc_get_be_manager(be_enc),  
		   bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		   bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) );
    break;

  case OR:   
    result = Be_Or( BeEnc_get_be_manager(be_enc), 
		  bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		  bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) );
    break;

  case XOR:   
    result = Be_Xor( BeEnc_get_be_manager(be_enc), 
		  bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		  bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) );
    break;

  case XNOR:   
    result = Be_Not( BeEnc_get_be_manager(be_enc),
                   Be_Xor( BeEnc_get_be_manager(be_enc), 
                           bmc_conv_bexp2be_recur(be_enc, car(bexp)),
                           bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) ));
    break;

  case IFF:  
    /* converts IFF args into two BEs, and result = an IFF BE with converted
       BEs as childs */
    result = Be_Iff( BeEnc_get_be_manager(be_enc), 
		   bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		   bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) );
    break;

  case IMPLIES:
    /* convert IMPLIES args into two BEs, and result = the IMPLIES BE with
       converted BEs as childs */
    result = Be_Implies( BeEnc_get_be_manager(be_enc),
		       bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		       bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) );
    break;

  case EQUAL:  
  case ASSIGN: 
    /* converts EQUAL and ASSIGN args into two BEs, and result = an IFF BE
       with converted BEs as childs */
    result = Be_Iff( BeEnc_get_be_manager(be_enc),
		   bmc_conv_bexp2be_recur(be_enc, car(bexp)),
		   bmc_conv_bexp2be_recur(be_enc, cdr(bexp)) ); 
    break;
    
  case CASE: {
    /* converts "if"-"then"-"else" args of CASE into three BEs, and result =
       a BE If-Then-Else with converted BEs as childs */

    /* lazy evaluation and simplification is used here to get rid of 
       FAILURE node in case-expressions 
    */

    be_ptr cond = bmc_conv_bexp2be_recur(be_enc, caar(bexp));
    if (cond == Be_Truth(BeEnc_get_be_manager(be_enc))) {
      result = bmc_conv_bexp2be_recur(be_enc, cdar(bexp));
    }
    else if (cond == Be_Falsity(BeEnc_get_be_manager(be_enc))) {
      result = bmc_conv_bexp2be_recur(be_enc, cdr(bexp));
    }
    /* no simplification possible since the condition is not a constant */
    else result = Be_Ite( BeEnc_get_be_manager(be_enc), 
			  cond,
			  bmc_conv_bexp2be_recur(be_enc, cdar(bexp)),
			  bmc_conv_bexp2be_recur(be_enc, cdr(bexp)));
    break;
  }

  case BIT:                              /* Variable */
  case DOT:                              /* Variable */
    result = BeEnc_name_to_untimed(be_enc, bexp);
    break;

  case ARRAY: 
    /* Must be a boolean variable: */
    nusmv_assert(SymbTable_is_symbol_bool_var(
		      BaseEnc_get_symbol_table(BASE_ENC(be_enc)), 
		      bexp));

    result = BeEnc_name_to_untimed(be_enc, bexp);
    break;

  case ATOM:                             /* Variable */
    internal_error("Not DOT node as variable has been found!\n");

  case WORD:
    /* word is expected to have boolean encoding */
    result = bmc_conv_bexp2be_recur(be_enc, car(bexp));
    break;

  case FAILURE: 
    {
      if (failure_get_kind(bexp) == FAILURE_CASE_NOT_EXHAUSTIVE) {
	warning_case_not_exhaustive(bexp);
	/* forces a default */
	result = Be_Truth(BeEnc_get_be_manager(be_enc));
	break;
      }
      else if (failure_get_kind(bexp) == FAILURE_DIV_BY_ZERO) {
	warning_possible_div_by_zero(bexp);
	/* forces a default */
	result = Be_Truth(BeEnc_get_be_manager(be_enc));
	break;
      }
      else {
	report_failure_node(bexp); /* some error in the input expr */
      }
    }

  case NUMBER:
  default:
    print_sexp(stderr, bexp);
    internal_error("bmc_conv_bexp2be_recur: Unexpected case value. Node type = %d\n",
		   node_get_type(bexp));
  }

  /* updates the cache and returns result*/
  bmc_conv_set_cache( bexp, result);
  return result;
}


/**Function********************************************************************

  Synopsis           [<b>Converts</b> given <b>boolean expressions list </b> 
  into correspondent <b>reduced boolean circuits list</b>]

  Description        []

  SideEffects        [be hash may change]

  SeeAlso            []

******************************************************************************/
node_ptr Bmc_Conv_BexpList2BeList(BeEnc_ptr be_enc, node_ptr bexp_list)
{
  if (bexp_list == Nil) 
  {
    return(Nil);
  }
  else 
  {
    return cons( (node_ptr)Bmc_Conv_Bexp2Be(be_enc, car(bexp_list)),
		 Bmc_Conv_BexpList2BeList(be_enc, cdr(bexp_list)) );
  }
}


/**Function********************************************************************

  Synopsis           [Removes from the cache those entries that depend on 
  the given symbol]

  Description [Called by the BeEnc when removing a layer, to make safe
  later declaration of symbols with the same name but different
  semantics.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Bmc_Conv_cleanup_cached_entries_about(BeEnc_ptr be_enc, 
																					 NodeList_ptr symbs)
{
  SymbTable_ptr st;
  node_ptr all_exprs = assoc_get_keys(bexpr2be_hash, true);
  node_ptr iter;
  
  st = BaseEnc_get_symbol_table(BASE_ENC(be_enc));

  for (iter=all_exprs; iter != Nil; iter = cdr(iter)) {
    node_ptr expr = car(iter);
    NodeList_ptr list = Compile_get_expression_dependencies(st, expr);
    ListIter_ptr sym_iter;
    
    for (sym_iter=NodeList_get_first_iter(symbs); !ListIter_is_end(sym_iter); 
	 sym_iter=ListIter_get_next(sym_iter)) {
      node_ptr name = NodeList_get_elem_at(symbs, sym_iter);

      if (NodeList_belongs_to(list, name)) {
	/* removes the corresponding entry */
	bmc_conv_set_cache(expr, (be_ptr) NULL);
      }
    }

    NodeList_destroy(list);
  }
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Initializes module Conv]

  Description        [This package function is called by bmcPkg module]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Bmc_Conv_init_cache()
{
  nusmv_assert(bexpr2be_hash == (hash_ptr) NULL);

  bexpr2be_hash = new_assoc();
  nusmv_assert(bexpr2be_hash != (hash_ptr) NULL);
}


/**Function********************************************************************

  Synopsis           [De-initializes module Conv]

  Description        [This package function is called by bmcPkg module]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Bmc_Conv_quit_cache()
{
  if (bexpr2be_hash != (hash_ptr) NULL) {
    free_assoc(bexpr2be_hash);
    bexpr2be_hash = (hash_ptr) NULL; 
  }
}



/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Sets a node into the stack]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static void Be2bexpDfsData_push(Be2bexpDfsData* data, 
				  node_ptr value)
{
  (data -> head) ++;

  array_insert(node_ptr, data -> stack, data -> head, value);
}

/**Function********************************************************************

  Synopsis           [Be2bexpDfsData_head]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static node_ptr Be2bexpDfsData_head(Be2bexpDfsData* data)
{
  /* there is space available into the stack: */
  nusmv_assert((data -> head) != (-1));

  return (node_ptr) array_fetch(node_ptr, data->stack, data->head);
}

/**Function********************************************************************

  Synopsis           [Be2bexpDfsData_pop]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static node_ptr Be2bexpDfsData_pop(Be2bexpDfsData* data)
{
  node_ptr value = (node_ptr)Be2bexpDfsData_head(data);

  (data->head) --;

  return(value);
}

/**Function********************************************************************

  Synopsis           [Be2bexpSet]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static int Be2bexp_Set(be_ptr be, char* Be2bexpData, nusmv_ptrint sign)
{
  return -1;
}

/**Function********************************************************************

  Synopsis           [Be2bexpFirst]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static void Be2bexp_First(be_ptr be, char* Be2bexpData, nusmv_ptrint sign)
{
  return;
}

/**Function********************************************************************

  Synopsis           [Be2bexp_Back]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static void Be2bexp_Back(be_ptr be, char* Be2bexpData, nusmv_ptrint sign)
{
  return; 
}

/**Function********************************************************************

  Synopsis           [Be2bexp_Last]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static void Be2bexp_Last(be_ptr be, char* Be2bexpData, nusmv_ptrint sign)
{
  int identifier = 0;
  int time, var_id;
  node_ptr left, right;
  Be_Manager_ptr be_mgr; 
  Rbc_t* rbc;

  BeEnc_ptr be_enc = ((Be2bexpDfsData*)Be2bexpData)->be_enc;
  nusmv_assert(be_enc != NULL);

  /* :WARNING: This code strongly depends on the RBC structure */
  be_mgr = BeEnc_get_be_manager(be_enc); 
  rbc = (Rbc_t*) Be_Manager_Be2Spec(be_mgr, be);

  switch (rbc->symbol) {
  case RBCTOP:
    if (sign == RBC_FALSE) {
      Be2bexpDfsData_push((Be2bexpDfsData*)Be2bexpData, Bmc_Wff_MkFalsity());
    }
    else {
      Be2bexpDfsData_push((Be2bexpDfsData*)Be2bexpData, Bmc_Wff_MkTruth());
    }
    break;    

  case RBCVAR:
    /* substitute the variable index, in the stack, with its correspondent
       state variable */
    
    time = BeEnc_index_to_time(be_enc, BeEnc_var_to_index(be_enc, rbc));
    var_id = BeEnc_index_to_untimed_index(be_enc, 
					  BeEnc_var_to_index(be_enc, rbc));

    if (sign == RBC_FALSE) {
      Be2bexpDfsData_push( (Be2bexpDfsData*) Be2bexpData,
        Bmc_Wff_MkNot(Bmc_Wff_MkXopNext(BeEnc_index_to_name(be_enc, var_id),
					 time)) );
    }
    else {
      Be2bexpDfsData_push( (Be2bexpDfsData*) Be2bexpData,
	   Bmc_Wff_MkXopNext(BeEnc_index_to_name(be_enc, var_id), time) );
    }
    break;

  case RBCAND:
  case RBCIFF:
    /* get the left bexp from the stack */
    right = Be2bexpDfsData_pop((Be2bexpDfsData*) Be2bexpData);

    /* get the right bexp from the stack */
    left = Be2bexpDfsData_pop((Be2bexpDfsData*) Be2bexpData);

    switch (rbc->symbol) {
    case RBCAND:
      identifier = AND;
      break;
    case RBCIFF:
      identifier = IFF;
      break;
    }

    if (sign == RBC_FALSE) {
      Be2bexpDfsData_push( (Be2bexpDfsData*) Be2bexpData,
			   Bmc_Wff_MkNot(find_node(identifier, left, right)) );
    }
    else {
      Be2bexpDfsData_push( (Be2bexpDfsData*) Be2bexpData,
			    find_node(identifier, left, right) );
    }
    break;
    
  default:
    /* execution should never be here: */
    internal_error("rbc->symbol had an invalid value: %d\n", rbc->symbol);
  }
}


/**Function********************************************************************

  Synopsis           [Update the bexpr -> be cache]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void bmc_conv_set_cache(node_ptr bexp, be_ptr be)
{
  nusmv_assert(bexpr2be_hash != (hash_ptr) NULL);
  insert_assoc(bexpr2be_hash, bexp, (node_ptr) be); 
}


/**Function********************************************************************

  Synopsis           [Queries the bexpr->be cache]

  Description        [Return NULL if association not found]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static be_ptr bmc_conv_query_cache(node_ptr bexp)
{
  nusmv_assert(bexpr2be_hash != (hash_ptr) NULL);
  return (be_ptr) find_assoc(bexpr2be_hash, bexp);
}
