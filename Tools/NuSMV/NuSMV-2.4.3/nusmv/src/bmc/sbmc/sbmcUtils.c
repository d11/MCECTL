/**CFile***********************************************************************

  FileName    [sbmcUtils.c]

  PackageName [bmc.sbmc]

  Synopsis    [Utilities function for SBMC package]

  Description [Utilities function for SBMC package]

  SeeAlso     []

  Author      [Tommi Junttila, Timo Latvala, Marco Roveri]

  Copyright   [
  This file is part of the ``bmc.sbmc'' package of NuSMV version 2. 
  Copyright (C) 2006 by Tommi Junttila, Timo Latvala.

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


#include <stdlib.h>
#include <stdio.h>

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "sbmcUtils.h" 
#include "sbmcStructs.h" 

#include "bmc/bmcInt.h"
#include "bmc/bmcWff.h"
#include "bmc/bmcCheck.h"
#include "bmc/bmcSatTrace.h"
#include "bmc/bmcUtils.h"

#include "utils/list.h"
#include "utils/utils.h"
#include "utils/assoc.h"
#include "node/node.h"
#include "parser/symbols.h" /* for tokens */
#include "enc/enc.h"
#include "compile/compile.h" /* for sym_intern */
#include "opt/opt.h"
#include "prop/prop.h"

#include "trace/Trace.h"
#include "trace/TraceManager.h"

static char rcsid[] UTIL_UNUSED = "$Id: sbmcUtils.c,v 1.1.2.11.4.10 2007/05/14 16:05:42 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define SBMC_LOOPVAR_LAYER_NAME "SBMC LOOP var"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [A wrapper to the sat solver]

  Description []

  SeeAlso     []

******************************************************************************/
struct sbmc_MetaSolver_TAG
{
  BeEnc_ptr        be_enc;
  boolean          using_volatile_group;
  SatIncSolver_ptr solver;
  SatSolverGroup   permanent_group;
  SatSolverGroup   volatile_group;
};

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN node_ptr boolean_range;
static node_ptr _sbmc_loopvar_name = (node_ptr)NULL;


/**Variable********************************************************************

  Synopsis    [Counter to create unique ids.]

  Description [Counter to create unique ids for fresh variables,
  similarly to what happen in LTL2SMV]

  SeeAlso     []

******************************************************************************/
static int __sbmc_unique_id_counter__ = -1;

int sbmc_get_unique_id(void) {
  return __sbmc_unique_id_counter__;
}

void sbmc_reset_unique_id(void) {
  __sbmc_unique_id_counter__ = -1;
}

void sbmc_increment_unique_id(void) {
  __sbmc_unique_id_counter__ += 1;
}

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define METASOLVERCHECK( ms ) \
  nusmv_assert((sbmc_MetaSolver *)NULL != ms); \
  nusmv_assert((BeEnc_ptr)NULL != ms->be_enc); \
  nusmv_assert((SatIncSolver_ptr)NULL != ms->solver); 
 

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/*
 * Auxiliary node printing routine
 */

/**Function********************************************************************

  Synopsis           [Print a node_ptr expression by prefixing and
  suffixing it.]

  Description        [Prints a node_ptr expression in a file stream by
  prefixing and suffixing it with a string. If the prefix and suffix
  strings are NULL they are not printed out.]

  SideEffects        [None]

******************************************************************************/
void sbmc_print_node(FILE * out, const char * prefix, node_ptr node,
                     const char * postfix)
{
  if ((char *)NULL != prefix) fprintf(out, "%s", prefix);
  print_node(out, node);
  if ((char *)NULL != postfix) fprintf(out, "%s", postfix);
}

/**Function********************************************************************

  Synopsis           [Prints a lsList of node_ptr]

  Description        [Prints a lsList of node_ptr in a file stream.]

  SideEffects        [None]

******************************************************************************/
void sbmc_print_node_list(FILE *out, lsList l) 
{
  node_ptr node;
  lsGen iterator;
  char *sep = "";

  lsForEachItem(l, iterator, node) {
    sbmc_print_node(out, sep, node, "");
    sep = ",";
  }
}

/**Function********************************************************************

  Synopsis           [Declare a new boolean state variable in the layer.]

  Description        [Declare a new boolean state variable in the
  layer. The name is specified as a string. If the variable already
  exists, then an error is generated.]

  SideEffects        [None]

******************************************************************************/
node_ptr sbmc_add_new_state_variable(SymbLayer_ptr layer, char *name)
{
  SymbType_ptr symbolicType;
  node_ptr node;
  char * uname;
  size_t uname_size = 0;

  /* The 20 is for keeping track of the possible unique id */
  uname_size = strlen(name) + 10;

  uname = ALLOC(char, uname_size);

  if (snprintf(uname, uname_size, "%d_%s", sbmc_get_unique_id(), name) < 0) {
    FREE(uname);
    internal_error("%s:%d: Unable to create unique string", __FILE__, __LINE__);
  }
  
  /* Create the internal name of the new variable. It is part of the
     main module */
  node = find_node(DOT, Nil, sym_intern(uname));
  nusmv_assert((node_ptr)NULL != node);

  /* uname no longer needed */
  FREE(uname);

  if (!SymbLayer_can_declare_var(layer, node))
    error_redefining(node); 
    
  symbolicType = SymbType_create(SYMB_TYPE_ENUM, boolean_range);
  SymbLayer_declare_state_var(layer, node, symbolicType);
  return node;
}

/**Function********************************************************************

  Synopsis           [Compute the variables that occur in the formula ltlspec.]

  Description        [Compute the variables that occur in the formula ltlspec.
  The formula ltlspec must  be in NNF.]

  SideEffects        [None]

******************************************************************************/
lsList sbmc_find_formula_vars(node_ptr ltlspec)
{
  hash_ptr visit_cache = (hash_ptr)NULL;
  lsList   unprocessed_nodes = (lsList)0;
  lsList   formula_vars = (lsList)0;

  /* Debug output */
  if (opt_verbose_level_ge(options, 5)) {
    sbmc_print_node(nusmv_stderr, "Finding vars occurring in ", ltlspec, "\n");
  }
  
  visit_cache = sbmc_set_create();
  formula_vars = lsCreate();
  unprocessed_nodes = lsCreate();
  lsNewBegin(unprocessed_nodes, (lsGeneric) ltlspec, LS_NH);
  
  while(lsLength(unprocessed_nodes) > 0) {
    node_ptr node, lsf, rsf;
    int has_unprocessed_children;

    /* Get node */
    if ((lsFirstItem(unprocessed_nodes, (lsGeneric*)&node, LS_NH) != LS_OK) ||
        ((node_ptr)NULL == node))
      internal_error(sbmc_SNH_text, __FILE__, __LINE__);

    /* Already visited? */
    if (sbmc_set_is_in(visit_cache, node)) {
      if (lsDelBegin(unprocessed_nodes, (lsGeneric*)&node) != LS_OK)
        internal_error(sbmc_SNH_text,__FILE__,__LINE__);
      continue;
    }

    /* Traverse children and build info */
    lsf = car(node);
    rsf = cdr(node);      
    has_unprocessed_children = 0;
    switch(node_get_type(node)) {
    case ATOM:
    case BIT:
    case DOT:
    case ARRAY:
      lsNewBegin(formula_vars, (lsGeneric)node, LS_NH);
      break;
          
    case TRUEEXP:
    case FALSEEXP:
      break;

    case AND:
    case OR:
    case XOR:
    case XNOR:
    case IMPLIES:
    case IFF:
    case UNTIL:
    case RELEASES:
    case TRIGGERED:
    case SINCE:
      if (!sbmc_set_is_in(visit_cache, lsf)) {
        lsNewBegin(unprocessed_nodes, (lsGeneric)lsf, LS_NH);
        has_unprocessed_children = 1;
      }
      if (!sbmc_set_is_in(visit_cache, rsf)) {
        lsNewBegin(unprocessed_nodes, (lsGeneric)rsf, LS_NH);
        has_unprocessed_children = 1;
      }
      break;
          
    case NOT:
    case OP_GLOBAL:
    case OP_FUTURE:
    case OP_NEXT:
    case OP_HISTORICAL:
    case OP_ONCE:
    case OP_PREC:
    case OP_NOTPRECNOT:
      if (!sbmc_set_is_in(visit_cache, lsf)) {
        lsNewBegin(unprocessed_nodes, (lsGeneric)lsf, LS_NH);
        has_unprocessed_children = 1;
      }
      break;
          
    default:
      print_node(nusmv_stderr, node);
      internal_error("%s:%d: Something not implemented",
                     __FILE__, __LINE__);
      break;
    }
    if (has_unprocessed_children)
      continue;
      
    if (lsDelBegin(unprocessed_nodes, (lsGeneric*)&node) != LS_OK)
      internal_error(sbmc_SNH_text, __FILE__, __LINE__);

    sbmc_set_insert(visit_cache, node);
  }
      
  lsDestroy(unprocessed_nodes, NULL);
  sbmc_set_destroy(visit_cache);

  return formula_vars;
}

/**Function********************************************************************

  Synopsis           [Prints some of the information associated to a
  subformula]

  Description        [Prints some of the information associated to a
  subformula.]

  SideEffects        [None]

******************************************************************************/
void sbmc_print_varmap(FILE *out, node_ptr node, sbmc_node_info *info)
{
  nusmv_assert((node_ptr)NULL != node);
  nusmv_assert((sbmc_node_info *)NULL != info);

  if (opt_verbose_level_ge(options, 2)) {
    unsigned int d;
    array_t * trans_vars = sbmc_node_info_get_trans_vars(info);

    for (d = 0; d <= sbmc_node_info_get_past_depth(info); d++) {
      sbmc_print_node(out, "[[", node, "]]");
      fprintf(out, "^%u = ", d);
      sbmc_print_node(out, "", array_fetch(node_ptr, trans_vars, d), ";\n");
    }
  }
}

/**Function********************************************************************

  Synopsis           [Prints some of the information associated to a G
  formula]

  Description        [Prints some of the information associated to a G
  formula.]

  SideEffects        [None]

******************************************************************************/
void sbmc_print_Gvarmap(FILE *out, node_ptr var, node_ptr formula)
{
  nusmv_assert((node_ptr)NULL != var);
  nusmv_assert((node_ptr)NULL != formula);

  if (opt_verbose_level_ge(options, 2)) {
    sbmc_print_node(nusmv_stderr, "<<G ", formula, ">> = ");
    sbmc_print_node(nusmv_stderr, "", var, ";\n");
  }
}

/**Function********************************************************************

  Synopsis           [Prints some of the information associated to a F
  formula]

  Description        [Prints some of the information associated to a F
  formula.]

  SideEffects        [None]

******************************************************************************/
void sbmc_print_Fvarmap(FILE *out, node_ptr var, node_ptr formula)
{
  nusmv_assert((node_ptr)NULL != var);
  nusmv_assert((node_ptr)NULL != formula);

  if (opt_verbose_level_ge(options, 2)) {
    sbmc_print_node(nusmv_stderr, "<<F ", formula, ">> = ");
    sbmc_print_node(nusmv_stderr, "", var, ";\n");
  }
}


/**Function********************************************************************

  Synopsis           [Creates a new fresh state variable.]

  Description        [Creates a new fresh state variable. The name is
  according to the pattern #LTL_t%u, being %u an unsigned integer. The
  index is incremented by one.]

  SideEffects        [index is incremented by one.]

******************************************************************************/
node_ptr sbmc_1_fresh_state_var(SymbLayer_ptr layer, unsigned int *index)
{
  char new_var_name[16];
  node_ptr new_var = 0;

  nusmv_assert(index != (unsigned int*) NULL);
  
  sprintf(new_var_name, "#LTL_t%u", *index);
  *index = *index + 1;
  new_var = sbmc_add_new_state_variable(layer, new_var_name);
  return new_var;
}

/**Function********************************************************************

  Synopsis           [Creates N new fresh state variables.]

  Description        [Creates N new fresh state variables. The name is
  according to the pattern #LTL_t%u, being %u an unsigned integer. The
  index is incremented by N. The new variables are stroed into an
  array of node_ptr]

  SideEffects        [index is incremented by N.]

******************************************************************************/
array_t * sbmc_n_fresh_state_vars(SymbLayer_ptr layer,
                                  const unsigned int n,
                                  unsigned int *index)
{
  array_t* array;
  unsigned int i;

  nusmv_assert(n > 0);

  nusmv_assert(index != (unsigned int*) NULL);
  nusmv_assert(*index >= 0);

  array = array_alloc(node_ptr, n);
  nusmv_assert((array_t *)NULL != array);

  for (i = 0; i < n; i++) {
    array_insert(node_ptr, array, i, sbmc_1_fresh_state_var(layer, index));
  }
  return array;
}

/**Function********************************************************************

  Synopsis           [Creates info->pastdepth+1 new state variables
  for the main translation in info->trans_vars.]

  Description        [Creates info->pastdepth+1 new state variables
  for the main translation in info->trans_vars. state_vars_formula_pd0,
  state_vars_formula_pdx and new_var_index are updated accordingly.]

  SideEffects        [new_var_index is incremented accordingly to the
  number of variables created. state_vars_formula_pd0,
  state_vars_formula_pdx and new_var_index are updated accordingly.]

******************************************************************************/
void sbmc_allocate_trans_vars(sbmc_node_info *info,
                              SymbLayer_ptr layer,
                              lsList state_vars_formula_pd0,
                              lsList state_vars_formula_pdx,
                              unsigned int* new_var_index)
{
  unsigned int d;
  unsigned int pd;
  array_t * array;

  nusmv_assert(info);

  array = sbmc_node_info_get_trans_vars(info);
  nusmv_assert((array_t *)NULL == array);

  pd = sbmc_node_info_get_past_depth(info);
  array = sbmc_n_fresh_state_vars(layer, pd + 1, new_var_index);
  sbmc_node_info_set_past_trans_vars(info, array);

  /* Update state_vars_formula_pd0 and state_vars_formula_pdx */
  lsNewBegin(state_vars_formula_pd0,
             (lsGeneric)array_fetch(node_ptr, array, 0),
             LS_NH);
  for (d = 1; d <= pd; d++)
    lsNewBegin(state_vars_formula_pdx,
               (lsGeneric)array_fetch(node_ptr, array, d),
               LS_NH);
}


/**Function********************************************************************

  Synopsis           [Takes a property and return the negation of the
  property conjoined with the big and of fairness conditions.]

  Description        [Takes a property and return the negation of the
  property conjoined with the big and of fairness conditions.]

  SideEffects        [None]

******************************************************************************/
node_ptr sbmc_make_boolean_formula(Prop_ptr ltlprop)
{
  node_ptr fltlspec = (node_ptr)NULL;

  fltlspec= Bmc_Wff_MkNot(Compile_detexpr2bexpr(Enc_get_bdd_encoding(),
                                                Prop_get_expr_core(ltlprop)));

  /*
   * Add fairness constraints to the formula
   */
  {
    SexpFsm_ptr sexp_fsm = Prop_get_bool_sexp_fsm(ltlprop);

    nusmv_assert(sexp_fsm != (SexpFsm_ptr)NULL);
    
    node_ptr j_list = SexpFsm_get_justice(sexp_fsm);
    node_ptr justice_list = Bmc_CheckFairnessListForPropositionalFormulae(j_list);

    j_list = justice_list;

    while(!is_list_empty(justice_list)) {
      fltlspec = Bmc_Wff_MkAnd(fltlspec, Bmc_Wff_MkGlobally(Bmc_Wff_MkEventually(car(justice_list))));
      justice_list = cdr(justice_list);
    }
    
    node_ptr compassion_list = SexpFsm_get_compassion(sexp_fsm);
    /* Here we should add /\_i ((GF p_i) -> (GF q_i)) /\ fltlspec */
    if (!is_list_empty(compassion_list))
      internal_error("%s:%d: Compassion not handled", __FILE__, __LINE__);

    free_list(j_list);
  }
  
  return Bmc_Wff_MkNnf(fltlspec);
}

/**Function********************************************************************

  Synopsis           [Find state and input variables that occurr in the formula.]

  Description        [Find state and input variables that occurr in the formula.
  Build the list of system variables for simple path constraints.
 
  <ul>
  <li> state_vars->formula_state_vars will have the state vars occurring
    in the formula bltlspec</li>
  <li> state_vars->formula_input_vars will have the input vars occurring
    in the formula bltlspec</li>
  <li> state_vars->simple_path_system_vars will be the union of
    state_vars->transition_state_vars,
    state_vars->formula_state_vars, and
    state_vars->formula_input_vars </li>
   </ul>]

  SideEffects        [svs is modified to store retrieved information.]

******************************************************************************/
void sbmc_find_relevant_vars(state_vars_struct *svs,
                             BeFsm_ptr be_fsm,
                             node_ptr bltlspec)
{
  lsGen  iterator;
  node_ptr node = (node_ptr)NULL;
  BeEnc_ptr be_enc = (BeEnc_ptr)NULL;
  SymbTable_ptr st = (SymbTable_ptr)NULL;
  lsList f_vars = (lsList)NULL;

  nusmv_assert((state_vars_struct *)NULL != svs);

  /* Get be encoding */
  be_enc = BeFsm_get_be_encoding(be_fsm);
  nusmv_assert((BeEnc_ptr)NULL != be_enc);

  /* Get symbol table */
  st = BaseEnc_get_symbol_table(BASE_ENC(be_enc));
  nusmv_assert((SymbTable_ptr)NULL != st);

  /* Clean lists if not empty */
  if (lsLength(sbmc_state_vars_get_formula_state_vars(svs)) > 0) {
    lsDestroy(sbmc_state_vars_get_formula_state_vars(svs), 0);
    sbmc_state_vars_set_formula_state_vars(svs, lsCreate());
  }
  if (lsLength(sbmc_state_vars_get_formula_input_vars(svs)) > 0) {
    lsDestroy(sbmc_state_vars_get_formula_input_vars(svs), 0);
    sbmc_state_vars_set_formula_input_vars(svs, lsCreate());
  }
  if (lsLength(sbmc_state_vars_get_simple_path_system_vars(svs)) > 0) {
    lsDestroy(sbmc_state_vars_get_simple_path_system_vars(svs), 0);
    sbmc_state_vars_set_simple_path_system_vars(svs, lsCreate());
  }
  
  /* Get all the variables occurring in the formula */
  f_vars = sbmc_find_formula_vars(bltlspec);
  nusmv_assert((lsList)NULL != f_vars);
  
  /* Classify the variables to state and input ones */
  lsForEachItem(f_vars, iterator, node) {
    if (SymbTable_is_symbol_state_var(st, node))
      lsNewEnd(sbmc_state_vars_get_formula_state_vars(svs),
               (lsGeneric)node, LS_NH);
    else if (SymbTable_is_symbol_input_var(st, node))
      lsNewEnd(sbmc_state_vars_get_formula_input_vars(svs),
               (lsGeneric)node, LS_NH);
    else
      internal_error("%s:%d: Unknown variable type (not state or input)",
                     __FILE__, __LINE__);
  }
  /* Release list */
  lsDestroy(f_vars, NULL);
  f_vars = (lsList)NULL;
  
  /* Build simple_path_system_vars */
  {
    lsList l, spsv;;
    hash_ptr union_set = sbmc_set_create();

    spsv = sbmc_state_vars_get_simple_path_system_vars(svs);

    nusmv_assert((lsList)NULL != spsv);

    l = sbmc_state_vars_get_trans_state_vars(svs);
    lsForEachItem(l, iterator, node) {
      if (!sbmc_set_is_in(union_set, node)) {
        sbmc_set_insert(union_set, node);
        lsNewEnd(spsv, (lsGeneric)node, LS_NH);
      }
    }
    l = sbmc_state_vars_get_formula_state_vars(svs);
    lsForEachItem(l, iterator, node) {
      if (!sbmc_set_is_in(union_set, node)) {
        sbmc_set_insert(union_set, node);
        lsNewEnd(spsv, (lsGeneric)node, LS_NH);
      }
    }
    l = sbmc_state_vars_get_formula_input_vars(svs);
    lsForEachItem(l, iterator, node) {
      if (!sbmc_set_is_in(union_set, node)) {
        sbmc_set_insert(union_set, node);
        lsNewEnd(spsv, (lsGeneric)node, LS_NH);
      }
    }

    sbmc_set_destroy(union_set);
  }
}


/**Function********************************************************************

  Synopsis           [Extracts a trace from a sat assignment.]

  Description        [Extracts a trace from a sat assignment.]

  SideEffects        [None]

  SeeAlso            [Bmc_Utils_generate_and_print_cntexample]

******************************************************************************/
Trace_ptr Sbmc_Utils_generate_and_print_cntexample(BeEnc_ptr be_enc,
                                                   sbmc_MetaSolver * solver,
                                                   BddEnc_ptr bdd_enc,
                                                   node_ptr l_var,
                                                   const int k,
                                                   const char * trace_name)
{
  nusmv_ptrint cnfLiteral;
  int bmc_tr, loopback, lvindex;;
  BmcSatTrace_ptr sat_trace;
  Be_Manager_ptr be_mgr;
  Trace_ptr trace;
  node_ptr path;
  lsList be_model, cmodel, model;
  lsGen gen;

  nusmv_assert((be_ptr) NULL != l_var);

  be_mgr = BeEnc_get_be_manager(be_enc);

  /* The satisfying assignment produced by the SAT solver. */
  /* it is a list of cnf variable index, positive or negative */
  cmodel = sbmc_MS_get_model(solver);

  /* The list of the corresponding BE variables */
  model = Be_CnfModelToBeModel(BeEnc_get_be_manager(be_enc), cmodel);

  /* 
     we consider only the cnf variables corresponding to BE variables
     in the range [sbmc_real_k(0),sbmc_real_k(k)]. 

     Thus we ignore the cnf variables that are not corresponding to
     the encoding of the:
     - model variables;
     - encoding variables (sub formulas, loop variables, ...)
  */
  loopback = -1;
  lvindex = BeEnc_name_to_index(be_enc, l_var);

  be_model = lsCreate();
  lsForEachItem(model, gen, cnfLiteral) {
    int varindex = Be_BeLiteral2BeIndex(be_mgr, (int) cnfLiteral);
    int vtime;

    if (!BeEnc_is_index_untimed(be_enc, varindex)) {
      vtime = BeEnc_index_to_time(be_enc, varindex);

      if ((vtime >= sbmc_real_k(0)) && (vtime <= sbmc_real_k(k))) {
        /* here we must convert it to the right time */
        int idx, lit, uvindex;
        be_ptr var, var_model_i;

        uvindex = BeEnc_index_to_untimed_index(be_enc, varindex);
        var = BeEnc_index_to_var(be_enc, uvindex);
        var_model_i = BeEnc_var_to_timed(be_enc, var, sbmc_model_k(vtime));

        idx = BeEnc_var_to_index(be_enc, var_model_i);
        lit = (cnfLiteral > 0) ? 
          Be_BeIndex2BeLiteral(be_mgr, idx) :
          -Be_BeIndex2BeLiteral(be_mgr, idx);
        
        lsNewEnd(be_model, PTR_FROM_INT(lsGeneric, lit), LS_NH);

        if ((lvindex == uvindex) && (0 < cnfLiteral)) {
          nusmv_assert(-1 == loopback);
          loopback = sbmc_model_k(vtime);
        }
      }
    }
  }

  lsDestroy(model, NULL);

  /* As no real be_prob was ever generated, it is replaced by a dummy,
     non-constant be, i.e. l_var */
  sat_trace = BmcSatTrace_create(l_var, be_model);

  /* be_model must not be freed since it is the responisbility of
     sat_trace to free it */
  /* lsDestroy(be_model, NULL); */

  path = NodeList_to_node_ptr(
                      BmcSatTrace_get_symbolic_model(sat_trace, be_enc, k));

  trace = Trace_create_from_state_input_list(bdd_enc,
                                             trace_name,
                                             TRACE_TYPE_CNTEXAMPLE,
                                             path);

  bmc_tr = TraceManager_register_trace(global_trace_manager, trace);

  /* Print the trace using default plugin */
  fprintf(nusmv_stdout,
          "-- as demonstrated by the following execution sequence\n");

  if (loopback != -1) {
    /* NuSMV prints traces so that first state is numbered 1 */
    fprintf(nusmv_stdout,
            "   the loop starts at state %d (that is redundantly printed also as state %d)\n", 
            loopback+1-1, k+1);
  }
  else {
    fprintf(nusmv_stdout, "   the execution has no loop\n");
  }

  TraceManager_execute_plugin(global_trace_manager, 
                              TraceManager_get_default_plugin(global_trace_manager),
                              bmc_tr);

  BmcSatTrace_destroy(&sat_trace);

  return trace;
}

/*
 * Routines for the state indexing scheme
 * State 0 is the L state and 1 is the E state
 * The first real state is state 2.
 */
/**Function********************************************************************

  Synopsis           [Routines for the state indexing scheme]

  Description        [State 0 is the L state]

  SideEffects        [None]

  SeeAlso            [sbmc_E_state sbmc_real_k sbmc_model_k sbmc_real_k_string]

******************************************************************************/
int sbmc_L_state(void) 
{
  return 0; 
}

/**Function********************************************************************

  Synopsis           [Routines for the state indexing scheme]

  Description        [State 1 is the E state]

  SideEffects        [None]

  SeeAlso            [sbmc_L_state sbmc_real_k sbmc_model_k sbmc_real_k_string]

******************************************************************************/
int sbmc_E_state(void) 
{
  return 1; 
}

/**Function********************************************************************

  Synopsis           [Routines for the state indexing scheme]

  Description        [The first real state is 2]

  SideEffects        [None]

  SeeAlso            [sbmc_L_state sbmc_E_state sbmc_model_k sbmc_real_k_string]

******************************************************************************/
int sbmc_real_k(int k) 
{
  return k+2; 
}

/**Function********************************************************************

  Synopsis           [Routines for the state indexing scheme]

  Description        [Given a real k return the corresponding model k (real - 2)]

  SideEffects        [None]

  SeeAlso            [sbmc_L_state sbmc_E_state sbmc_real_k sbmc_real_k_string]

******************************************************************************/
unsigned int sbmc_model_k(int k) 
{
  nusmv_assert(k >= 2); 
  return k-2; 
}

/**Function********************************************************************

  Synopsis           [Routines for the state indexing scheme]

  Description        [Returns a string correspondingg to the state considered. E, L, Real]

  SideEffects        [The returned value must be freed]

  SeeAlso            [sbmc_L_state sbmc_E_state sbmc_real_k sbmc_model_k]

******************************************************************************/
char* sbmc_real_k_string(const unsigned int k_real)
{
  char *str = ALLOC(char, 32);

  if (k_real == sbmc_L_state()) snprintf(str, 32, "L");
  else if (k_real == sbmc_E_state()) snprintf(str, 32, "E");
  else snprintf(str, 32, "%u", sbmc_model_k(k_real));
  return str;
}


/**Function********************************************************************

  Synopsis           [Creates a meta solver wrapper]

  Description        [Creates a meta solver wrapper]

  SideEffects        [None]

******************************************************************************/
sbmc_MetaSolver * sbmc_MS_create(BeEnc_ptr be_enc)
{

  nusmv_assert((BeEnc_ptr)NULL != be_enc);

  sbmc_MetaSolver * ms = ALLOC(sbmc_MetaSolver, 1);

  nusmv_assert((sbmc_MetaSolver *)NULL != ms);

  ms->be_enc = be_enc;
  ms->using_volatile_group = false;
  ms->solver = SAT_INC_SOLVER(NULL);
  ms->solver = Sat_CreateIncSolver(get_sat_solver(options));

  if (ms->solver == SAT_INC_SOLVER(NULL)) {
    fprintf(nusmv_stderr, "Incremental sat solver '%s' is not available.\n", 
            get_sat_solver(options));
    FREE(ms);
    return (sbmc_MetaSolver *)NULL;
  }
  ms->permanent_group = SatSolver_get_permanent_group(SAT_SOLVER(ms->solver));

  return ms;
}

/**Function********************************************************************

  Synopsis           [Destroy a meta solver wrapper]

  Description        [Destroy a meta solver wrapper]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_destroy(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  SatIncSolver_destroy(ms->solver); 
  ms->solver = SAT_INC_SOLVER(NULL);
}

/**Function********************************************************************

  Synopsis           [Create the volatile group in the meta solver wrapper]

  Description        [Create the volatile group in the meta solver wrapper. Use
  of the volatile group is not forced]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_create_volatile_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  nusmv_assert(!ms->using_volatile_group);
  ms->volatile_group = SatIncSolver_create_group(ms->solver);
}

/**Function********************************************************************

  Synopsis           [Destroy the volatile group of the meta solver wrapper and
  force use of the permanent one]

  Description        [Destroy the volatile group of the meta solver wrapper and
  force use of the permanent one]
  
  SideEffects        [None]

******************************************************************************/
void sbmc_MS_destroy_volatile_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  SatIncSolver_destroy_group(ms->solver, ms->volatile_group);
  ms->using_volatile_group = false;
}

/**Function********************************************************************

  Synopsis           [Force use of the permanent group of
  the meta solver wrapper]

  Description        [Force use of the permanent group of
  the meta solver wrapper. Volatile group is left in place, if existing]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_switch_to_permanent_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  ms->using_volatile_group = false;
}

/**Function********************************************************************

  Synopsis           [Force use of the volatile group of
  the meta solver wrapper]

  Description        [Force use of the volatile group of
  the meta solver wrapper. The volatile group must have been previously
  created]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_switch_to_volatile_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  ms->using_volatile_group = true;
}

/**Function********************************************************************

  Synopsis           [Destroy the volatile group of the meta solver wrapper]

  Description        [Destroy the volatile group of the meta solver
  wrapper, thus only considering the permanent group.]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_goto_permanent_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  nusmv_assert(ms->using_volatile_group);
  SatIncSolver_destroy_group(ms->solver, ms->volatile_group);
  ms->using_volatile_group = false;
}

/**Function********************************************************************

  Synopsis           [Create and force use of the volatile group of
  the meta solver wrapper]

  Description        [Create and force use of the volatile group of
  the meta solver wrapper.]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_goto_volatile_group(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  nusmv_assert(!ms->using_volatile_group);
  ms->volatile_group = SatIncSolver_create_group(ms->solver);
  ms->using_volatile_group = true;
}

/**Function********************************************************************

  Synopsis           [Forces a BE to be true in the solver.]

  Description        [Forces a BE to be true in the solver. The BE
  converted to CNF, the CNF is then forced in the group in use,
  i.e. in the permanent or in the volatile group.]

  SideEffects        [None]

******************************************************************************/
void sbmc_MS_force_true(sbmc_MetaSolver *ms, be_ptr be_constraint)
{
  Be_Manager_ptr be_mgr;
  Be_Cnf_ptr cnf;
  be_ptr inconstr;

  METASOLVERCHECK(ms);

  be_mgr = BeEnc_get_be_manager(ms->be_enc);
  SatSolver_ptr solver = SAT_SOLVER(ms->solver);

  inconstr = Bmc_Utils_apply_inlining(be_mgr, be_constraint);
  cnf = Be_ConvertToCnf(be_mgr, inconstr, 1);

  if (ms->using_volatile_group) {
    SatSolver_add(solver, cnf, ms->volatile_group);
    SatSolver_set_polarity(solver, cnf, 1, ms->volatile_group);
  }
  else {
    SatSolver_add(solver, cnf, ms->permanent_group);
    SatSolver_set_polarity(solver, cnf, 1, ms->permanent_group);
  }
  Be_Cnf_Delete(cnf);
}

/**Function********************************************************************

  Synopsis           [Forces a list of BEs to be true in the solver.]

  Description        [Forces a list of BEs to be true in the
  solver. Each is converted to CNF, the CNF is then forced in the
  group in use, i.e. in the permanent or in the volatile group.]

  SideEffects        [None]

  SeeAlso            [sbmc_MS_force_true]

******************************************************************************/
void sbmc_MS_force_constraint_list(sbmc_MetaSolver *ms, lsList constraints)
{
  lsGen  iterator;
  be_ptr be_constraint;
  
  lsForEachItem(constraints, iterator, be_constraint) {
    sbmc_MS_force_true(ms, be_constraint);
  }
}

/**Function********************************************************************

  Synopsis           [Solves all groups belonging to the solver and
  returns the flag.]

  Description        [Solves all groups belonging to the solver and
  returns the flag.]

  SideEffects        [None]

  SeeAlso            [SatSolver_solve_all_groups]

******************************************************************************/
SatSolverResult sbmc_MS_solve(sbmc_MetaSolver *ms)
{
  METASOLVERCHECK(ms);
  return SatSolver_solve_all_groups(SAT_SOLVER(ms->solver));
}


/**Function********************************************************************

  Synopsis    [Returns the model (of previous solving)]

  Description [ The previous solving call should have returned SATISFIABLE.
  The returned list is a list of values in dimac form (positive literal
  is included as the variable index, negative literal as the negative 
  variable index, if a literal has not been set its value is not included). 

  Returned list must be NOT destroyed.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
lsList sbmc_MS_get_model(sbmc_MetaSolver *ms) 
{
  METASOLVERCHECK(ms);
  return SatSolver_get_model(SAT_SOLVER(ms->solver));
}


/* To keep track of previoulsy failed removal */
static boolean loop_var_currently_added = false;

/**Function********************************************************************

  Synopsis    [Declares a new layer to contain the loop variable.]

  Description [Declares a new layer to contain the loop variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void sbmc_add_loop_variable(BeFsm_ptr be_fsm) 
{
  BeEnc_ptr be_enc;
  BoolEnc_ptr bool_enc;
  SymbTable_ptr symb_table;
  SymbLayer_ptr ltl_layer;
  node_ptr loop_var_name;

  /* Get be encoding */
  be_enc = BeFsm_get_be_encoding(be_fsm);
  nusmv_assert((BeEnc_ptr)NULL != be_enc);

  /* Get Boolean encoding */
  bool_enc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(be_enc));
  nusmv_assert((BoolEnc_ptr)NULL != bool_enc);

  /* Get the symbol table */
  symb_table = Compile_get_global_symb_table();
  nusmv_assert((SymbTable_ptr)NULL != symb_table);

  /* checks if previous removal failed due to an interruption by the user */
  if (loop_var_currently_added) sbmc_remove_loop_variable(be_fsm);

  /* Add a new layer for translation variables */
  ltl_layer = SymbTable_create_layer(symb_table, SBMC_LOOPVAR_LAYER_NAME,
                                     SYMB_LAYER_POS_BOTTOM);
  nusmv_assert((SymbLayer_ptr)NULL != ltl_layer);

  /* We increment to counter for the creation of unique ids among
     different calls */
  sbmc_increment_unique_id();

  nusmv_assert((node_ptr)NULL == sbmc_loop_var_name_get());
  loop_var_name = sbmc_add_new_state_variable(ltl_layer, "#SBMC_LTL_l");
  sbmc_loop_var_name_set(loop_var_name);

  /*
   * After introducing all new variables, commit ltl_layer
   */
  BaseEnc_commit_layer(BASE_ENC(bool_enc), SymbLayer_get_name(ltl_layer));
  BaseEnc_commit_layer(BASE_ENC(be_enc), SymbLayer_get_name(ltl_layer));
  BaseEnc_commit_layer(BASE_ENC(Enc_get_bdd_encoding()),
                       SymbLayer_get_name(ltl_layer));

  loop_var_currently_added = true;
}

/**Function********************************************************************

  Synopsis    [Remove the new layer to contain the loop variable.]

  Description [Remove the new layer to contain the loop variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void sbmc_remove_loop_variable(BeFsm_ptr be_fsm) 
{
  BeEnc_ptr be_enc;
  BoolEnc_ptr bool_enc;
  SymbTable_ptr symb_table;
  SymbLayer_ptr ltl_layer;

  /* Get be encoding */
  be_enc = BeFsm_get_be_encoding(be_fsm);
  nusmv_assert((BeEnc_ptr)NULL != be_enc);

  /* Get Boolean encoding */
  bool_enc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(be_enc));
  nusmv_assert((BoolEnc_ptr)NULL != bool_enc);

  /* Get the symbol table */
  symb_table = BaseEnc_get_symbol_table(BASE_ENC(be_enc));
  nusmv_assert((SymbTable_ptr)NULL != symb_table);

  /* Retrieve the new layer for translation variables */
  ltl_layer = SymbTable_get_layer(symb_table, SBMC_LOOPVAR_LAYER_NAME);
  nusmv_assert((SymbLayer_ptr)NULL != ltl_layer);
  /*
   * Remove extra encoding layers
   */
  BaseEnc_remove_layer(BASE_ENC(Enc_get_bdd_encoding()), SBMC_LOOPVAR_LAYER_NAME);
  BaseEnc_remove_layer(BASE_ENC(be_enc), SBMC_LOOPVAR_LAYER_NAME);
  BaseEnc_remove_layer(BASE_ENC(bool_enc), SBMC_LOOPVAR_LAYER_NAME);
  SymbTable_remove_layer(symb_table, ltl_layer);

  sbmc_loop_var_name_set((node_ptr)NULL);

  loop_var_currently_added = false;
}


/**Function********************************************************************

  Synopsis    [Sets the name of the loop variable.]

  Description [Sets the name of the loop variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void sbmc_loop_var_name_set(node_ptr n) {
  _sbmc_loopvar_name = n;
}

/**Function********************************************************************

  Synopsis    [Gets the name of the loop variable.]

  Description [Gets the name of the loop variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
node_ptr sbmc_loop_var_name_get(void) {
  return _sbmc_loopvar_name;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


