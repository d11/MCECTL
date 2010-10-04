/**CFile***********************************************************************

  FileName    [BmcSatTrace.c]

  PackageName [bmc]

  Synopsis    [Contains the definition of class BmcSatTrace]

  Description []

  SeeAlso     [the sat package]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``bmc'' package of NuSMV version 2.
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

#include "bmcSatTrace.h"
#include "bmcInt.h" /* for 'options' */

#include "mc/mc.h" /* for 'eval_propositional_spec' */
#include "parser/symbols.h" /* for constants */
#include "enc/enc.h"


static char rcsid[] UTIL_UNUSED = "$Id: bmcSatTrace.c,v 1.1.2.3.2.9.4.3 2007/03/20 19:30:07 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/**Struct*********************************************************************

  Synopsis           [Private declaration for class BmcSatTrace]
  Description        []
  SeeAlso            []

******************************************************************************/
typedef struct BmcSatTrace_TAG {
  NodeList_ptr smvModel; /* the symbolic model */
  
  lsList satResult; /* the original solver outcome */
  be_ptr beProb; /* the original problem */
  int smvModel_k; /* the trace length (for lazy evaluation) */

} BmcSatTrace;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
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
static NodeList_ptr
bmc_sat_trace_prop2symb ARGS((const BmcSatTrace_ptr self, 
			      const BeEnc_ptr be_enc)); 

static void 
bmc_sat_trace_calculateSymbModel ARGS((const BmcSatTrace_ptr self,  
				       const BeEnc_ptr be_enc, 
				       const int k));

static void bmc_sat_trace_clear_model ARGS((BmcSatTrace_ptr self));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/



/**Function********************************************************************

  Synopsis           [Class BmcSatTrace constructor]

  Description        [The method gets a satisfiable assignment of literals, 
  expressed in terms of be indices, and the original be problem which this 
  assignments had been calculated from.]

  SideEffects        []
  
  SeeAlso            [BmcSatTrace_destroy]

******************************************************************************/
BmcSatTrace_ptr BmcSatTrace_create(be_ptr beProb, 
				   lsList beModel)
{
  BmcSatTrace_ptr self = ALLOC(BmcSatTrace, 1);
  nusmv_assert(self != (BmcSatTrace_ptr) NULL);
  
  self->beProb = beProb;
  self->satResult = beModel;
  self->smvModel = NodeList_create();
  self->smvModel_k = 0;
  
  return self;
}


/**Function********************************************************************

  Synopsis           [Class BmcSatTrace destructor]

  Description        [Call when you no longer need the given instance]

  SideEffects        [self will be invalidated]
  
  SeeAlso            []

******************************************************************************/
void BmcSatTrace_destroy(BmcSatTrace_ptr* self_ref) 
{
  bmc_sat_trace_clear_model(*self_ref);

  FREE(*self_ref);
  *self_ref = NULL;
}


/**Function********************************************************************

  Synopsis           [Returns the bdd-based symbolic model]

  Description        [Returns the symbolic counterexample with max length k. 
  The BeEnc instance is required by the conversion routine. 
  Conversion is performed with memoizing, so you can call efficiently call 
  this method more than one time. Returned object belongs to self]

  SideEffects        [self's internal state can change]
  
  SeeAlso            []

******************************************************************************/
NodeList_ptr BmcSatTrace_get_symbolic_model(const BmcSatTrace_ptr self,  
					    const BeEnc_ptr be_enc, 
					    const int k)
{
  bmc_sat_trace_calculateSymbModel(self, be_enc, k);
  return self->smvModel;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Private service for class BmcSatTrace methods]

  Description        []

  SideEffects        []
  
  SeeAlso            []

******************************************************************************/
static void 
bmc_sat_trace_calculateSymbModel(const BmcSatTrace_ptr self,  
				 const BeEnc_ptr be_enc, 
				 const int k)
{
  Be_Manager_ptr be_mgr; 
  be_mgr = BeEnc_get_be_manager(be_enc);

  /* lazy evaluation on the original problem: */
  if (Be_IsTrue(be_mgr, self->beProb) || Be_IsFalse(be_mgr, self->beProb)) {
    /* The counterexample explainer could be smarter (it might say "Is
       it a trivial problem?") */
    bmc_sat_trace_clear_model(self);
    self->smvModel = NodeList_create();
  }
  else {
    /* lazy evaluation on already performed conversions: */
    if ((NodeList_get_length(self->smvModel) == 0) ||
	(self->smvModel_k != k)) {
      self->smvModel_k = k;
      bmc_sat_trace_clear_model(self);
      self->smvModel = bmc_sat_trace_prop2symb(self, be_enc);
    }
  }
}


/**Function********************************************************************

  Synopsis           [Private service for BmcSatTrace methods]

  Description        [Example of mapping of STATE VARIABLES and BE VARIABLES: 
  <PRE>
    ----------------------------------------------------------
    VAR        x    y    x'   y'   x0   y0   x1   y1   x2   y2
    BE index  0    1    2    3    4    5    6    7    8    9  
    CNF index  10    1    2    3    4    5    6    7    8    9 
    Time         -2    |   -1   |     0   |     1   |     2   |
    Varindex   0    1  | 0    1 |  0    1 |  0    1 |  0    1 |
    </PRE>
    ]

  SideEffects        []
  
  SeeAlso            []

******************************************************************************/
static NodeList_ptr
bmc_sat_trace_prop2symb(const BmcSatTrace_ptr self, 
			const BeEnc_ptr be_enc) 
{
  Be_Manager_ptr be_mgr; 
  be_mgr = BeEnc_get_be_manager(be_enc);

  int vars_num;
  lsGen genLit;
  nusmv_ptrint lit;
  node_ptr* model; 
  int time;
  
  SymbTable_ptr symb_table;
  NodeList_ptr trace; 

  symb_table = BaseEnc_get_symbol_table(BASE_ENC(be_enc));

  vars_num = BeEnc_get_vars_num(be_enc);
  trace = NodeList_create();

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stderr, 
	    "The propositional to symbolic model converter says that "
	    "\n the input BE model is the following:\n (");
    
    genLit = lsStart(self->satResult);
    while (lsNext(genLit, (lsGeneric*) &lit, LS_NH) == LS_OK) {
      fprintf(nusmv_stderr, "%d ", lit);
    }
    lsFinish(genLit);
    fprintf(nusmv_stderr, " )\n");
  }
  
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, 
	    "Converting propositional model into symbolic model\n");
  }

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stderr, 
	    "Number of state and input variables: %d, ", vars_num);
    fprintf(nusmv_stderr, "Time steps from 0 to %d\n", self->smvModel_k);
    fprintf(nusmv_stderr, "\nmodel[time][var_index] = literal and symbol:\n");
  }

  /* allocates memory for model, and initialize all locations (default
     value for any unassigned literal is 'false') */
  model = ALLOC(node_ptr, (self->smvModel_k + 1));
  nusmv_assert(model != NULL);

  for (time = 0; time <= self->smvModel_k; ++time) model[time] = Nil;

  /* scan list of literals */
  genLit = lsStart(self->satResult);
  while (lsNext(genLit, (lsGeneric*) &lit, LS_NH) == LS_OK) {
    int var_idx = Be_BeLiteral2BeIndex(be_mgr, lit);
    int ut_index; 

    /* here only timed state and input variables indexes are allowed */
    nusmv_assert(!BeEnc_is_index_untimed(be_enc, var_idx));

    time = BeEnc_index_to_time(be_enc, var_idx); 
    ut_index = BeEnc_index_to_untimed_index(be_enc, var_idx);
    
    model[time] = cons((node_ptr) lit, model[time]);

    if (opt_verbose_level_gt(options, 1)) {
      fprintf( nusmv_stderr, "model[%d][%d] = % d      ", time, var_idx, lit);
      print_node(nusmv_stderr, BeEnc_index_to_name(be_enc, ut_index));
      fprintf(nusmv_stderr, "\n");
    }
  } /* end of literal scan */
  lsFinish(genLit);

  if (opt_verbose_level_gt(options, 1)) fprintf(nusmv_stderr, "\n");


  /*************************************************************
   * BUILDS A COUNTEREXAMPLE AS SEQUENCE OF BDDs
   *************************************************************/
  {
    BddEnc_ptr enc = Enc_get_bdd_encoding();
    NodeList_ptr state_symbs = SymbTable_get_model_state_symbols(symb_table);
    NodeList_ptr input_symbs = SymbTable_get_model_input_symbols(symb_table);

    BddEnc_print_bdd_begin(enc, 
			   NodeList_to_node_ptr(state_symbs),
			   false);

    for (time = 0; time <= self->smvModel_k; ++time) {
      node_ptr iter = model[time];
      node_ptr state_exp = new_node(TRUEEXP, Nil, Nil);
      node_ptr input_exp = new_node(TRUEEXP, Nil, Nil);
      
      if (opt_verbose_level_gt(options, 10)) {
	fprintf(nusmv_stderr, " --------------- Time %d --------------- \n", 
		time);
      }

      while (iter != Nil) {
	node_ptr var_expr;
	int val = NODE_TO_INT(car(iter));
	int idx = BeEnc_index_to_untimed_index(be_enc, 
			       Be_BeLiteral2BeIndex(be_mgr, val));
	
	var_expr = BeEnc_index_to_name(be_enc, idx);
	
	/* negate if needed: */
	if (val < 0) var_expr = new_node(NOT, var_expr, Nil);

	if (BeEnc_is_index_untimed_curr(be_enc, idx)) {
	  state_exp = new_node(AND, var_expr, state_exp);
	}
	else {
	  nusmv_assert(BeEnc_is_index_untimed_input(be_enc, idx));
	  input_exp = new_node(AND, var_expr, input_exp);
	}
	
	iter = cdr(iter);
      } /* cycle on variables for a given time */
    
      /* Evaluates and append state and input to list: */
      {
	bdd_ptr state_bdd;
	bdd_ptr input_bdd;
	
	/* calculates, append and if needed print states */
	state_bdd = BddEnc_expr_to_bdd(enc, state_exp, Nil);
	NodeList_append(trace, (node_ptr) state_bdd);
	if (opt_verbose_level_gt(options, 10)) {
	  printf("At time %d, STATES:\n", time);
	  BddEnc_print_bdd(enc, state_bdd, nusmv_stdout);
	}
	
	/* at last step inputs are not appended */
	if (time != self->smvModel_k) {
	  input_bdd = BddEnc_expr_to_bdd(enc, input_exp, Nil);
	  NodeList_append(trace, (node_ptr) input_bdd); 
	  if (opt_verbose_level_gt(options, 10)) {	    
	    BddEnc_print_bdd_begin(enc, NodeList_to_node_ptr(input_symbs), 
				   false);
	    
	    printf("At time %d, INPUTS:\n", time);
	    BddEnc_print_bdd(enc, input_bdd, nusmv_stdout);
	    BddEnc_print_bdd_end(enc);
	  }
	}
      }

    } /* end of time cycle */

    BddEnc_print_bdd_end(enc);
    
    NodeList_destroy(input_symbs);
    NodeList_destroy(state_symbs);
  }

  /* destroys previously allocated model array: */
  for (time = 0; time <= self->smvModel_k; ++time) {
    free_list(model[time]);
  }
  FREE(model);

  return trace;
}



/**Function********************************************************************

  Synopsis           [Service function]

  Description        [Clears the list of bdds that represents the model]

  SideEffects        []
  
  SeeAlso            []

******************************************************************************/
static void bmc_sat_trace_clear_model(BmcSatTrace_ptr self) 
{
  ListIter_ptr iter;
  DdManager* dd = BddEnc_get_dd_manager(Enc_get_bdd_encoding());

  /* frees the list of bdds */
  iter = NodeList_get_first_iter(self->smvModel);
  while ( !ListIter_is_end(iter) ) {
    bdd_free(dd, (bdd_ptr) NodeList_get_elem_at(self->smvModel, iter));

    iter = ListIter_get_next(iter);
  }
  NodeList_destroy(self->smvModel);  

  self->smvModel = NODE_LIST(NULL);
}
