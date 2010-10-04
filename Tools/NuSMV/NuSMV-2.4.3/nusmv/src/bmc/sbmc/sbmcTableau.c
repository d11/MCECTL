/**CFile***********************************************************************

  FileName    [sbmcTableau.c]

  PackageName [bmc.sbmc]

  Synopsis    [Bmc.Tableau module]

  Description [This module contains all the tableau-related operations]

  SeeAlso     [bmcModel.c, bmcConv.c, bmcVarMgr.c bmcTableau.c,
               bmcTableauLTLformula.c, bmcTableauPLTLformula.c,
               sbmcTableauLTLformula.c, bmcGen.c sbmcGen.c]

  Author      [Timo Latvala, Marco Roveri]

  Copyright   [
  This file is part of the ``bmc.sbmc'' package of NuSMV version 2.
  Copyright (C) 2004 Timo Latvala <timo.latvala@tkk.fi>

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

  For more information of NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/

#include "sbmcTableau.h"
#include "sbmcTableauLTLformula.h"

#include "bmc/bmcInt.h"
#include "bmc/bmcUtils.h"
#include "bmc/bmcModel.h"
#include "bmc/bmcCheck.h"
#include "bmc/bmcWff.h"

#include "parser/symbols.h"
#include "utils/error.h"


static char rcsid[] UTIL_UNUSED = "$Id: sbmcTableau.c,v 1.1.2.8.4.2 2007/05/14 16:05:42 nusmv Exp $";

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

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static node_ptr bmc_get_fairness ARGS((const int l));


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Builds tableau without loop]

  Description        [Fairness is ignored]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
be_ptr Bmc_SBMCTableau_GetNoLoop(const BeFsm_ptr be_fsm,
                                 const node_ptr ltl_wff, const int k)
{
  return BmcInt_SBMCTableau_GetAtTime(BeFsm_get_be_encoding(be_fsm),
                                      ltl_wff, 0, k,
                                      Bmc_Utils_GetNoLoopback());
}


/**Function********************************************************************

  Synopsis           [Builds tableau for a single loop. This function takes
                      into account of fairness]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
be_ptr Bmc_SBMCTableau_GetSingleLoop(const BeFsm_ptr be_fsm,
                                     const node_ptr ltl_wff, const int k,
                                     const int l)
{
  BeEnc_ptr be_enc = BeFsm_get_be_encoding(be_fsm);
  be_ptr loopback = Bmc_SBMCTableau_GetLoopCondition(be_enc, k, l);
  node_ptr fair = bmc_get_fairness(l);
  
  be_ptr tableau_k = BmcInt_SBMCTableau_GetAtTime(be_enc,
                                                  Bmc_Wff_MkAnd(ltl_wff, fair), 
                                                  0, k, l);
  
  return Be_And(BeEnc_get_be_manager(be_enc), loopback, tableau_k);
}


/**Function********************************************************************

  Synopsis           [Builds tableau for all possible loops in \[l, k\[,
  taking into account of fairness using Kepa/Timo method]

  Description        [Fairness is taken care of by adding it to the formula.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
be_ptr Bmc_SBMCTableau_GetAllLoops(const BeFsm_ptr be_fsm,
                                   const node_ptr ltl_wff, const int k,
                                   const int l)
{
  /* asserts on l, k compatibility */
  nusmv_assert(!Bmc_Utils_IsNoLoopback(l));
  nusmv_assert(l < k);
  
  node_ptr fairness = bmc_get_fairness(l);
  BeEnc_ptr be_enc = BeFsm_get_be_encoding(be_fsm);
  be_ptr result = BmcInt_SBMCTableau_GetAtTime(be_enc,
                                               Bmc_Wff_MkAnd(ltl_wff, fairness),
                                               0, k, l);

  return result;
}

/**Function********************************************************************

  Synopsis           [Builds a tableau that constraints state k to be equal to
                      state l. This is the condition for a path of length (k+1)
                      to represent a (k-l)loop (new semantics).]

  Description        [State l and state k are forced to represent the same
                      state by conjuncting the if-and-only-if conditions
                      {Vil<->Vik} between Vil (variable i at time l) and Vik
                      (variable i at time k) for each variable Vi.]

  SideEffects        []

  SeeAlso            [Bmc_Tableau_GetAllLoopsDisjunction]

******************************************************************************/
be_ptr
Bmc_SBMCTableau_GetLoopCondition(const BeEnc_ptr be_enc, const int k,
                                 const int l)
{
  Be_Manager_ptr be_mgr; 
  be_ptr tableau_iff_constraints; 
  int iter; 

  nusmv_assert(l < k);

  be_mgr = BeEnc_get_be_manager(be_enc);
  tableau_iff_constraints = Be_Truth(BeEnc_get_be_manager(be_enc));

  iter = BeEnc_get_first_untimed_var_index(be_enc, BE_VAR_TYPE_CURR);
  while (BeEnc_is_var_index_valid(be_enc, iter)) {
    /* Here we can consider removing the loop variable */
    tableau_iff_constraints =
      Be_And(be_mgr, tableau_iff_constraints,
             Be_Iff(be_mgr, 
                    BeEnc_index_to_timed(be_enc, iter, l), 
                    BeEnc_index_to_timed(be_enc, iter, k)));

    iter = BeEnc_get_next_var_index(be_enc, iter, BE_VAR_TYPE_CURR);
  }

 return tableau_iff_constraints;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Builds AND_i G F fc_i]

  Description        [Builds AND_i G F fc_i]

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static node_ptr
bmc_get_fairness(const int l)
{
  if(Bmc_Utils_IsNoLoopback(l)) {
    return (node_ptr) Bmc_Wff_MkTruth();
  }

  node_ptr list_of_fairness = SexpFsm_get_justice(Prop_master_get_bool_sexp_fsm());
  node_ptr list = Bmc_CheckFairnessListForPropositionalFormulae(list_of_fairness);
  node_ptr result = (node_ptr) Bmc_Wff_MkTruth();
  node_ptr slist = list;

  while(list != Nil) {    
    result =
      Bmc_Wff_MkAnd(result,
                    Bmc_Wff_MkNnf(Bmc_Wff_MkGlobally(Bmc_Wff_MkEventually(car(list)))));
    list = cdr(list);
  }

  free_list(slist);

  node_ptr compassion_list = SexpFsm_get_compassion(Prop_master_get_bool_sexp_fsm());
  /* Here we should add /\_i ((GF p_i) -> (GF q_i)) /\ fltlspec */
  if (!is_list_empty(compassion_list))
    internal_error("%s:%d: Compassion not handled", __FILE__, __LINE__);
  
  return result;
}
