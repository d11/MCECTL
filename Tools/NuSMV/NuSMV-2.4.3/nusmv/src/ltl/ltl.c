/**CFile***********************************************************************

  FileName    [ltl.c]

  PackageName [ltl]

  Synopsis    [Routines to perform reduction of LTL model checking to
  CTL model checking.]

  Description [Here we perform the reduction of LTL model checking to
  CTL model checking. The technique adopted has been taken from [1].
  <ol>
    <li>
       O. Grumberg E. Clarke and K. Hamaguchi. "Another Look at LTL
       Model Checking".  <em>Formal Methods in System Design</em>,
       10(1):57--71, February 1997.
    </li>
  </ol>
  ]

  SeeAlso     [mc]

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``ltl'' package of NuSMV version 2. 
  Copyright (C) 1998-2001 by CMU and ITC-irst. 

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

#include "ltl/ltl.h"
#include "ltlInt.h" 
#include "ltl/ltl2smv/ltl2smv.h" 
#include "parser/symbols.h"
#include "parser/parser.h"
#include "prop/prop.h"
#include "fsm/sexp/Expr.h" /* for Expr_ptr */
#include "fsm/bdd/FairnessList.h" 
#include "mc/mc.h"

#include "utils/error.h" /* for CATCH */
#include "utils/utils_io.h"
#include "utils/utils.h"
#include "trace/Trace.h"
#include "trace/TraceManager.h"

#include "enc/enc.h"
#include "opt/opt.h"

static char rcsid[] UTIL_UNUSED = "$Id: ltl.c,v 1.33.4.36.2.1.2.23.4.10 2007/05/21 08:55:58 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void ltlInsertModuleHashReadModule ARGS((node_ptr));

static void 
ltlPropAddTableau ARGS((Prop_ptr, SymbLayer_ptr, FlatHierarchy_ptr));

static int 
ltlBuildTableauAndPropFsm ARGS((SymbTable_ptr, SymbLayer_ptr, Prop_ptr));

static void ltl_remove_layer ARGS((BddEnc_ptr, SymbLayer_ptr));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [The main routine to perform LTL model checking.]

  Description [The main routine to perform LTL model checking. It
  first takes the LTL formula, prints it in a file. It calls the LTL2SMV
  translator on it an reads in the generated tableau. The tableau is
  instantiated, compiled and then conjoined with the original model
  (both the set of fairness conditions and the transition relation are
  affected by this operation, for this reason we save the current
  model, and after the verification of the property we restore the
  original one).]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void Ltl_CheckLtlSpec(Prop_ptr prop) 
{
  BddFsm_ptr fsm;
  BddEnc_ptr bdd_enc = Enc_get_bdd_encoding();
  DdManager* dd = BddEnc_get_dd_manager(bdd_enc);
  SymbTable_ptr symb_table;
  SymbLayer_ptr tableau_layer;
  bdd_ptr s0;
  int res = 0; /* initialised to suppress warning*/
  boolean full_fairness;
  node_ptr spec_formula = Nil;
  
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "evaluating ");
    print_ltlspec(nusmv_stderr, prop);
    fprintf(nusmv_stderr, "\n");
  }

  symb_table = Compile_get_global_symb_table();
  tableau_layer = SymbTable_create_layer(symb_table, NULL /* temp name */, 
                                         SYMB_LAYER_POS_BOTTOM);
  CATCH {
    res = ltlBuildTableauAndPropFsm(symb_table, tableau_layer, prop);
  } 
  FAIL {    
    ltl_remove_layer(bdd_enc, tableau_layer);
    fprintf(nusmv_stderr, "An error occured during tableau construction.\n");
    nusmv_exit(1);
  }
  
  if (res == 1) {
    ltl_remove_layer(bdd_enc, tableau_layer);
    fprintf(nusmv_stderr, 
            "Ltl_CheckLtlSpec: Problems in Tableau generation.\n");
    nusmv_exit(1);
  }

  fsm = Prop_get_bdd_fsm(prop);
  BDD_FSM_CHECK_INSTANCE(fsm);

  /* If the compassion list is not empty, then activate the full
     fairness algorithm. */
  full_fairness = ! FairnessList_is_empty(
                      FAIRNESS_LIST(BddFsm_get_compassion(fsm)));

  if (full_fairness) {
    s0 = feasible(fsm, bdd_enc);   
  }
  else { /* Old Clarke/Grumberg/Hamaguchi algorithm */
    bdd_ptr tmp;
    spec_formula = 
      new_node(NOT,
               new_node(EG,
                        new_node(TRUEEXP,Nil,Nil), Nil), Nil);

    if (opt_verbose_level_gt(options, 2)) {
      Prop_ptr phi = Prop_create_partial(spec_formula, Prop_Ctl);
      fprintf(nusmv_stderr, "Checking CTL ");
      print_spec(nusmv_stderr, phi);
      fprintf(nusmv_stderr, " generated from the tableau.\n");
      Prop_destroy(phi);
    }

    /* Verification of the property: */
    CATCH {
      s0 = eval_ctl_spec(fsm, bdd_enc, spec_formula, Nil);
    } 
    FAIL {
      ltl_remove_layer(bdd_enc, tableau_layer);
      fprintf(nusmv_stderr, 
              "Ltl_CheckLtlSpec: Problems in Tableau verification.\n");
      nusmv_exit(1);
      return;
    }

    /* Negate the result */
    tmp = bdd_not(dd, s0);
    bdd_free(dd, s0);
    s0 = tmp;

    /* Intersect with init, invar and fair states */
    {
      bdd_ptr init  = BddFsm_get_init(fsm);
      bdd_ptr invar = BddFsm_get_state_constraints(fsm);
      bdd_ptr fair =  BddFsm_get_fair_states(fsm);

      if (bdd_is_zero(dd, fair)) {
        warning_fsm_fairness_empty();
      }
      
      bdd_and_accumulate(dd, &s0, init);
      bdd_and_accumulate(dd, &s0, invar);
      bdd_and_accumulate(dd, &s0, fair);
      bdd_free(dd, fair);
      bdd_free(dd, invar);
      bdd_free(dd, init);
    }
  }
  /* Prints out the result, if not true explain. */
  fprintf(nusmv_stdout, "-- ");
  print_spec(nusmv_stdout, prop);
  
  if (bdd_is_zero(dd, s0)) { 
    fprintf(nusmv_stdout, "is true\n");
    Prop_set_status(prop, Prop_True);
    ltl_remove_layer(bdd_enc, tableau_layer);
  }
  else {
    node_ptr exp;
    Trace_ptr trace;
    int trace_index = 0;
    fprintf(nusmv_stdout, "is false\n");
    Prop_set_status(prop, Prop_False);

    if (opt_counter_examples(options)) {
      char* trace_title = NULL;
      char* trace_title_postfix = " Counterexample";
      bdd_ptr tmp = BddEnc_pick_one_state(bdd_enc, s0);

      bdd_free(dd, s0);
      s0 = tmp;

      if (full_fairness) {
        exp = witness(fsm, bdd_enc, s0); 
      }
      else {
        bdd_ref(s0); /* to put s0 in the list */
        exp = reverse(explain(fsm, bdd_enc, cons((node_ptr)s0, Nil), 
                              spec_formula, Nil));
      }
      if (exp == Nil) {
        /* The counterexample consists of one initial state */
        exp = cons((node_ptr)s0, Nil);
      }

      /* removes all the tableau variables from the result before
         building the resulting trace. This will make simulation
         working, but may show unexistent loops in the shown trace */
      {
        node_ptr iter = exp;
        BddVarSet_ptr tableau_cube = 
          BddEnc_get_layer_vars_cube(bdd_enc, tableau_layer, VFT_CURR_INPUT);

        while (iter != Nil) {
          bdd_ptr sit;
          bdd_ptr si;

          nusmv_assert(node_get_type(iter) == CONS);
          sit = (bdd_ptr) car(iter);
          si = bdd_forsome(dd, sit, tableau_cube);
	  
          bdd_free(dd, sit);
          node_bdd_setcar(iter, si);
	  
          iter = cdr(iter);
        }
      }      
      
      /* The trace title depends on the property type. For example it
         is in the form "LTL Counterexample" */
      trace_title = ALLOC(char, Prop_get_type_as_string(prop) + 
                          strlen(trace_title_postfix) + 1);
      nusmv_assert(trace_title != (char*) NULL);
      strcpy(trace_title, Prop_get_type_as_string(prop));
      strcat(trace_title, trace_title_postfix);
      
      trace = Trace_create_from_state_input_list(bdd_enc, trace_title,
                                                 TRACE_TYPE_CNTEXAMPLE, exp); 

      FREE(trace_title);

      trace_index = TraceManager_register_trace(global_trace_manager, trace);
      Prop_set_trace(prop, Trace_get_id(trace));

      fprintf(nusmv_stdout,
              "-- as demonstrated by the following execution sequence\n");

      /* This is an hack: removal must be executed *before* running the
         dump of counter-example: */
      ltl_remove_layer(bdd_enc, tableau_layer);

      TraceManager_execute_plugin(global_trace_manager, 
               TraceManager_get_default_plugin(global_trace_manager),
                                  trace_index);

      walk_dd(dd, bdd_free, exp);
      free_list(exp);
    }
    else {
      ltl_remove_layer(bdd_enc, tableau_layer);
      /* counter examples are not generated */
    }
  }
  
  bdd_free(dd, s0);
}



/**Function********************************************************************

  Synopsis           [Print the LTL specification.]

  Description        [Print the LTL specification.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void print_ltlspec(FILE* file, Prop_ptr prop)
{
  indent(file);
  fprintf(file, "LTL specification ");
  Prop_print(prop, file); 
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Add the tableau module to the list of know modules]

  Description        [Add the tableau module to the list of know modules]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void ltlInsertModuleHashReadModule(node_ptr ltl_parsed_module)
{ 
  /* We insert the definition of the current module in the module_hash
     in order to make it available for the Compile_FlattenHierarchy
     routines. */
  node_ptr name = find_atom(car(car(ltl_parsed_module)));
  node_ptr params = cdr(car(ltl_parsed_module));
  node_ptr def = cdr(ltl_parsed_module);

  if (lookup_module_hash(name)) error_redefining(name);
  insert_module_hash(name, new_node(LAMBDA, params, def));
}


/**Function********************************************************************

  Synopsis           [Main routine to add the tableau to the FSM]

  Description        [The bdd fsm into the property will change]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void ltlPropAddTableau(Prop_ptr prop, 
			      SymbLayer_ptr tableau_layer, 
			      FlatHierarchy_ptr hierarchy)
{
  SexpFsm_ptr tableau_sexp_fsm;
  BddFsm_ptr prop_bdd_fsm, tableau_bdd_fsm;
  TransType  trans_type;
  BoolEnc_ptr bool_enc;
  BddEnc_ptr bdd_enc;
  
  bdd_enc = Enc_get_bdd_encoding();
  bool_enc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(bdd_enc));

  BaseEnc_commit_layer(BASE_ENC(bool_enc), SymbLayer_get_name(tableau_layer)); 
  BaseEnc_commit_layer(BASE_ENC(bdd_enc), SymbLayer_get_name(tableau_layer));
  
  /* Creation of the corresponding FSMs: */
  tableau_sexp_fsm = SexpFsm_create(bdd_enc, 
				    SYMB_LAYER(NULL), /*scalar, not boolean*/
				    SYMB_LAYER(NULL), /*no inlining */
				    VAR_SET(NULL), /* empty vars list, not used */
				    hierarchy);
  
  prop_bdd_fsm = Prop_get_bdd_fsm(prop); 
  if (prop_bdd_fsm == BDD_FSM(NULL)) {
    if (opt_cone_of_influence(options)) {
      Prop_apply_coi_for_bdd(prop, global_fsm_builder);
      prop_bdd_fsm = Prop_get_bdd_fsm(prop);
    }
    else {
      Prop_set_fsm_to_master(prop);
      prop_bdd_fsm = Prop_get_bdd_fsm(prop);
    }
  }
  
  BDD_FSM_CHECK_INSTANCE(prop_bdd_fsm);
  
  /* Inherits reachable states from the model fsm, if COI is not
     enabled and reachable states are required. This will disable
     calculation of reachable states in the tableau fsm. If option
     ltl_tableau_forward_search is enabled, reachables will be not
     inherited (and re-calculated when needed) */
  if ((!opt_cone_of_influence(options)) && 
      opt_use_reachable_states(options) && 
      !opt_ltl_tableau_forward_search(options)) {
    BddFsm_copy_reachable_states(prop_bdd_fsm, Prop_master_get_bdd_fsm(), 
				 true, /* keeps the family, with wanted
					  side effect on the master fsm */
				 true  /* forces calculation */);
  }
  
  trans_type = 
    GenericTrans_get_type( GENERIC_TRANS(BddFsm_get_trans(prop_bdd_fsm)) );

  tableau_bdd_fsm = FsmBuilder_create_bdd_fsm(global_fsm_builder, 
					      bdd_enc, 
					      tableau_sexp_fsm, trans_type);
  
  /* Carries out the synchronous product. this has side-effect on the
     bdd fsm inside the property, so any previously existing reference
     to it still will work */
  BddFsm_apply_synchronous_product(prop_bdd_fsm, tableau_bdd_fsm);
  
  BddFsm_destroy(tableau_bdd_fsm);
}


/**Function********************************************************************

  Synopsis           [Creates the tableau]

  Description [Creates the tableau for a LTL property.  The FSM of the
  property contains the tableau. Returns 1 if an error is encountered
  during the tableau generation, 0 otherwise]

  SideEffects        [The bdd fsm into the prop will change]

  SeeAlso            []

******************************************************************************/
static int ltlBuildTableauAndPropFsm(SymbTable_ptr symb_table, 
				     SymbLayer_ptr tableau_layer, 
				     Prop_ptr prop)
{
  static unsigned int ltl_spec_counter = -1;

  Expr_ptr spec;
  Expr_ptr ltl_formula; 
  node_ptr context;
  char* Ltl_Module_Name; 
  WFFR_TYPE rw;
  node_ptr generated_ltl_module;

  ++ltl_spec_counter;   /* to declare unique identifiers in the tableau */

  spec = Prop_get_expr_core(prop);
  rw = ltlRewriteWffIfInputPresent(symb_table, spec);

  if (WFFR_ISNOT_REWRITTEN(rw)) {
    if (node_get_type(spec) == CONTEXT) {
      context     = car(spec);
      ltl_formula = cdr(spec);
    }
    else {
      /* the formula has been flattened before */
      context     = Nil;
      ltl_formula = spec;
    }
  }
  else {
    /* if the formula contains inputs and thus it has been rewritten,
       then it has also been flattened and define expanded, thus the
       context must be Nil (i.e. the main) and the ltl_formula is the
       result of the rewriting. */
    context = Nil; 
    ltl_formula = WFFR_GET_WFF(rw);
  }

  /* The formula has to be negated */
  ltl_formula = Expr_not(ltl_formula);

  /* invoking ltl2smv invernaly */
  generated_ltl_module = ltl2smv(ltl_spec_counter, ltl_formula);
  
  if (opt_verbose_level_gt(options, 0)) fprintf(nusmv_stderr, "....done\n");
  
  /* 6 for number, plus terminator */
  Ltl_Module_Name = ALLOC(char, strlen(LTL_MODULE_BASE_NAME)+7);
  if (Ltl_Module_Name == NIL(char)) {
    fprintf(nusmv_stderr, "Unable to allocate \"Ltl_Module_Name\".\n");
    nusmv_exit(1);
  }
  sprintf(Ltl_Module_Name, "%s%u", LTL_MODULE_BASE_NAME, ltl_spec_counter);

  /* We insert the definition of the current module in the module_hash
     in order to make it available for the Compile_FlattenHierarchy
     routines. */
  ltlInsertModuleHashReadModule(generated_ltl_module);

  {
    FlatHierarchy_ptr hierarchy;
    node_ptr ltl_i_init          = Nil;
    node_ptr ltl_i_invar         = Nil;
    node_ptr ltl_i_trans         = Nil;

    if (WFFR_IS_REWRITTEN(rw)) {
      if (opt_verbose_level_gt(options, 0)) {
        fprintf(nusmv_stderr,
                "Adding state variables to model to handle with inputs within the formula.\n");
      }

      /* In this way the new variables introduced to handle with the
	 inputs will be added before the tableau variables. */
      ltlHandleInputsVars(tableau_layer, rw,
                          &ltl_i_init, &ltl_i_invar, &ltl_i_trans);
    }
    
    /* Rewriting structure no longer needed */
    ltlFreeWFFR_TYPE(rw);

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Flattening the generated tableau....");
    }  

    /* We call Compile_FlattenTableau with the name of the generated
       tableau, and as root name the actual property context. In this
       way local variables of the tableau and local variables of the
       formula will be contextualized to the right module. */    
    hierarchy = Compile_FlattenHierarchy(symb_table, tableau_layer, 
					 sym_intern(Ltl_Module_Name), context,
					 Nil, 
					 false /*do not create process vars*/);
    
    FREE(Ltl_Module_Name); /* this is no longer needed */

    /* If rewriting had effect update the FSM accordingly */
    if (ltl_i_init != Nil) {
      ltl_i_init = find_node(AND, ltl_i_init, FlatHierarchy_get_init(hierarchy));
      FlatHierarchy_set_init(hierarchy, ltl_i_init);
    }
    if (ltl_i_invar != Nil) {
      ltl_i_invar = find_node(AND,ltl_i_invar,FlatHierarchy_get_invar(hierarchy));
      FlatHierarchy_set_invar(hierarchy, ltl_i_invar);
    }
    if (ltl_i_trans != Nil) {
      ltl_i_trans = find_node(AND,ltl_i_trans,FlatHierarchy_get_trans(hierarchy));
      FlatHierarchy_set_trans(hierarchy, ltl_i_trans);
    }

    /* Check if we are using an old version of ltl2smv */
    if (FlatHierarchy_get_spec(hierarchy) != Nil) {
      internal_error("Error: CTL specification in tableau construction"
		     " (check ltl2smv).\n");
    }

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, ".... done\n");
      fprintf(nusmv_stderr, "Creating LTL tableau variables...\n");
    }

    /* The error trapping mechanism is enough in this block. All the
       other errors even external to this block are trapped and the
       else of the CATCH is executed. */
    CATCH { 
      ltlPropAddTableau(prop, tableau_layer, hierarchy);
    } FAIL {
      FlatHierarchy_destroy(hierarchy);
      return 1;
    }

    FlatHierarchy_destroy(hierarchy);
  }

  return 0;
}


/**Function********************************************************************

  Synopsis           [Private service that removes the given layer from 
  the symbol table, and from both the boolean and bdd encodings.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void ltl_remove_layer(BddEnc_ptr bdd_enc, SymbLayer_ptr layer)
{
  SymbTable_ptr symb_table;
  BoolEnc_ptr bool_enc;

  symb_table = BaseEnc_get_symbol_table(BASE_ENC(bdd_enc));
  bool_enc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(bdd_enc));
  
  if (BaseEnc_layer_occurs(BASE_ENC(bdd_enc), SymbLayer_get_name(layer))) {
    BaseEnc_remove_layer(BASE_ENC(bdd_enc), SymbLayer_get_name(layer));
  }

  if (BaseEnc_layer_occurs(BASE_ENC(bool_enc), SymbLayer_get_name(layer))) {
    BaseEnc_remove_layer(BASE_ENC(bool_enc), SymbLayer_get_name(layer)); 
  }
  
  SymbTable_remove_layer(symb_table, layer);
}
