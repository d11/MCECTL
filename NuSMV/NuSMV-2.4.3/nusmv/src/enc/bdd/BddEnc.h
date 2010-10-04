/**CHeaderFile*****************************************************************

  FileName    [BddEnc.h]

  PackageName [enc.bdd]

  Synopsis    [Public interface of class 'BddEnc']

  Description [Encoder for bdds, derived from class BoolEncClient]

  SeeAlso     [BddEnc.c]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``enc.bdd'' package of NuSMV version 2. 
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

  Revision    [$Id: BddEnc.h,v 1.1.2.17.4.9.4.5 2007/05/03 15:49:58 nusmv Exp $]

******************************************************************************/


#ifndef __BDD_ENC_H__
#define __BDD_ENC_H__


#include "enc/bdd/bdd.h"
#include "enc/base/BoolEncClient.h"
#include "enc/bool/BoolEnc.h"
#include "enc/utils/OrdGroups.h"

#include "compile/symb_table/SymbTable.h"
#include "fsm/sexp/Expr.h"
#include "fsm/bdd/bdd.h"
#include "dd/dd.h"

#include "utils/utils.h" 
#include "utils/object.h"
#include "enc/utils/AddArray.h" 
#include "compile/type_checking/TypeChecker.h" 


/**Type***********************************************************************

  Synopsis    [Definition of the public accessor for class BddEnc]

  Description []

******************************************************************************/
typedef struct BddEnc_TAG*  BddEnc_ptr;


/**Macros**********************************************************************

  Synopsis    [To cast and check instances of class BddEnc]

  Description [These macros must be used respectively to cast and to check
  instances of class BddEnc]

******************************************************************************/
#define BDD_ENC(self) \
         ((BddEnc_ptr) self)

#define BDD_ENC_CHECK_INSTANCE(self) \
         (nusmv_assert(BDD_ENC(self) != BDD_ENC(NULL)))


/**Type***********************************************************************

  Synopsis     [Used when dumping ordering file]

  Description  [Used when dumping ordering file]  

  Notes        [see method write_order]

******************************************************************************/
typedef enum {
  DUMP_DEFAULT, 
  DUMP_BITS, 
  DUMP_SCALARS_ONLY
} VarOrderingType;


/**Enum************************************************************************

  Synopsis    [Controls the filter type in some search dependencies routines]

  Description [Controls the filter type in some search dependencies routines.
  The available filters are:
  <dl>
    <dt> <tt>VFT_CURRENT</tt>
       <dd> filters out the current variables</dd>
    </dt>
    <dt> <tt>VFT_NEXT</tt>
       <dd> filters out the next variables</dd>
    </dt>
    <dt> <tt>VFT_INPUT</tt>
       <dd> filters out the input variables</dd>
    </dt>
    <dt> <tt>VFT_STATE</tt>
       <dd> filters out the state variables</dd>
    </dt>
    <dt> <tt>VFT_ALL</tt>
       <dd>filters out all the variables</dd>
    </dt>
  </dl>
  Combined modes can be obtained by bit-or: for example 
  VFT_NEXT | VFT_INPUT is going to search for the variables which
  are next or input variables]

******************************************************************************/
typedef enum VarFilterType_TAG {VFT_CURRENT=1, VFT_NEXT=2, VFT_STATE=3, 
              VFT_INPUT=4, VFT_CURR_INPUT=5, VFT_ALL=7} VarFilterType;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN BddEnc_ptr 
BddEnc_create ARGS((SymbTable_ptr symb_table, TypeChecker_ptr type_checker,
                    BoolEnc_ptr bool_enc, DdManager* dd, 
                    OrdGroups_ptr ord_groups));

EXTERN VIRTUAL 
void BddEnc_destroy ARGS((BddEnc_ptr self));

EXTERN TypeChecker_ptr 
BddEnc_get_type_checker ARGS((const BddEnc_ptr self));

EXTERN DdManager* 
BddEnc_get_dd_manager ARGS((const BddEnc_ptr self));

EXTERN add_ptr 
BddEnc_expr_to_add ARGS((BddEnc_ptr self, const Expr_ptr expr, 
                         const node_ptr context));

EXTERN bdd_ptr 
BddEnc_expr_to_bdd ARGS((BddEnc_ptr self, const Expr_ptr expr, 
                         const node_ptr context));

node_ptr BddEnc_add_to_expr ARGS((BddEnc_ptr self, const add_ptr add, 
                                  SymbLayer_ptr det_layer));

EXTERN node_ptr 
BddEnc_bdd_to_expr ARGS((BddEnc_ptr self, const bdd_ptr bdd));

EXTERN BddVarSet_ptr 
BddEnc_get_state_vars_cube ARGS((const BddEnc_ptr self));

EXTERN BddVarSet_ptr 
BddEnc_get_next_state_vars_cube ARGS((const BddEnc_ptr self));

EXTERN BddVarSet_ptr 
BddEnc_get_input_vars_cube ARGS((const BddEnc_ptr self));

EXTERN BddVarSet_ptr 
BddEnc_get_layer_vars_cube ARGS((const BddEnc_ptr self,
                                 SymbLayer_ptr layer, 
                                 VarFilterType vt));

EXTERN boolean 
BddEnc_is_var_in_cube ARGS((const BddEnc_ptr self, 
                            node_ptr name, add_ptr cube));

EXTERN add_ptr 
BddEnc_state_var_to_next_state_var_add ARGS((const BddEnc_ptr self, 
                                             add_ptr add));

EXTERN add_ptr 
BddEnc_next_state_var_to_state_var_add ARGS((const BddEnc_ptr self, 
                                             add_ptr add));

EXTERN bdd_ptr 
BddEnc_state_var_to_next_state_var ARGS((const BddEnc_ptr self, bdd_ptr bdd));

EXTERN bdd_ptr 
BddEnc_next_state_var_to_state_var ARGS((const BddEnc_ptr self, bdd_ptr bdd));

EXTERN void 
BddEnc_print_bdd_begin ARGS((BddEnc_ptr self, node_ptr symbols, 
                             boolean changes_only));

EXTERN void 
BddEnc_print_bdd_end ARGS((BddEnc_ptr self));

EXTERN int 
BddEnc_print_bdd ARGS((BddEnc_ptr self, bdd_ptr bdd, FILE* file));

EXTERN void 
BddEnc_print_set_of_states ARGS((BddEnc_ptr self, bdd_ptr states, 
                                 boolean changes_only, FILE* file));

EXTERN void 
BddEnc_print_set_of_inputs ARGS((BddEnc_ptr self, bdd_ptr inputs,
                                 boolean changes_only, FILE* file));

EXTERN void 
BddEnc_print_set_of_state_input_pairs  ARGS((BddEnc_ptr self, 
                                             bdd_ptr state_input_pairs,
                                             boolean changes_only, 
                                             FILE* file));
EXTERN node_ptr
BddEnc_assign_symbols ARGS((BddEnc_ptr self, bdd_ptr bdd, 
                            node_ptr symbols, 
                            boolean onlyRequiredSymbs,
                            bdd_ptr* resultBdd));

EXTERN void 
BddEnc_print_vars_in_cube ARGS((BddEnc_ptr self, bdd_ptr cube, 
                                node_ptr list_of_sym, 
                                FILE* file));

NodeList_ptr BddEnc_get_var_ordering ARGS((const BddEnc_ptr self, 
                                           const VarOrderingType ord_type));

EXTERN void 
BddEnc_write_var_ordering ARGS((const BddEnc_ptr self, 
                                const char* output_order_file_name, 
                                const VarOrderingType dump_type));

EXTERN int BddEnc_get_reordering_count ARGS((const BddEnc_ptr self));

EXTERN void BddEnc_reset_reordering_count ARGS((BddEnc_ptr self));

EXTERN double 
BddEnc_count_states_of_add ARGS((const BddEnc_ptr self, add_ptr add)); 

EXTERN double 
BddEnc_count_states_of_bdd ARGS((const BddEnc_ptr self, bdd_ptr bdd)); 

EXTERN double 
BddEnc_count_inputs_of_bdd ARGS((const BddEnc_ptr self, bdd_ptr bdd)); 

EXTERN double 
BddEnc_count_states_inputs_of_bdd ARGS((const BddEnc_ptr self, bdd_ptr bdd)); 

EXTERN double 
BddEnc_get_minterms_of_add ARGS((const BddEnc_ptr self, add_ptr add)); 

EXTERN double 
BddEnc_get_minterms_of_bdd ARGS((const BddEnc_ptr self, bdd_ptr bdd)); 

EXTERN bdd_ptr 
BddEnc_pick_one_state ARGS((const BddEnc_ptr self, bdd_ptr states));

EXTERN bdd_ptr 
BddEnc_pick_one_input ARGS((const BddEnc_ptr self, bdd_ptr inputs));

EXTERN boolean 
BddEnc_pick_all_terms_states_inputs ARGS((const BddEnc_ptr self, 
                                          bdd_ptr bdd, 
                                          bdd_ptr* result_array, 
                                          const int array_len));

EXTERN boolean 
BddEnc_pick_all_terms_states ARGS((const BddEnc_ptr self, bdd_ptr bdd, 
                                   bdd_ptr* result_array, 
                                   const int array_len));

EXTERN boolean 
BddEnc_pick_all_terms_inputs ARGS((const BddEnc_ptr self, bdd_ptr bdd, 
                                   bdd_ptr* result_array, 
                                   const int array_len));

EXTERN bdd_ptr 
BddEnc_pick_one_state_rand ARGS((const BddEnc_ptr self, bdd_ptr states));

EXTERN bdd_ptr 
BddEnc_pick_one_input_rand ARGS((const BddEnc_ptr self, bdd_ptr inputs));

EXTERN node_ptr 
BddEnc_get_var_name_from_index ARGS((const BddEnc_ptr self, int index));

EXTERN int 
BddEnc_get_var_index_from_name ARGS((const BddEnc_ptr self, node_ptr name));

EXTERN add_ptr 
BddEnc_constant_to_add ARGS((const BddEnc_ptr self, node_ptr constant));

EXTERN add_ptr 
BddEnc_eval_sign_add ARGS((BddEnc_ptr self, add_ptr a, int flag));

EXTERN bdd_ptr 
BddEnc_eval_sign_bdd ARGS((BddEnc_ptr self, bdd_ptr a, int flag));

EXTERN int 
BddEnc_eval_num ARGS((BddEnc_ptr self, node_ptr e, node_ptr context));

EXTERN add_ptr 
BddEnc_eval_constant ARGS((BddEnc_ptr self, Expr_ptr expr, node_ptr context)); 

EXTERN AddArray_ptr 
BddEnc_get_symbol_add ARGS((BddEnc_ptr self, node_ptr name));

EXTERN add_ptr 
BddEnc_get_state_vars_mask_add ARGS((BddEnc_ptr self));

EXTERN add_ptr 
BddEnc_get_input_vars_mask_add ARGS((BddEnc_ptr self));

EXTERN add_ptr 
BddEnc_get_state_input_vars_mask_add ARGS((BddEnc_ptr self));

EXTERN bdd_ptr 
BddEnc_get_state_vars_mask_bdd ARGS((BddEnc_ptr self));

EXTERN bdd_ptr 
BddEnc_get_input_vars_mask_bdd ARGS((BddEnc_ptr self));

EXTERN bdd_ptr 
BddEnc_get_state_input_vars_mask_bdd ARGS((BddEnc_ptr self));

EXTERN add_ptr 
BddEnc_apply_state_vars_mask_add ARGS((BddEnc_ptr self, add_ptr states));

EXTERN add_ptr 
BddEnc_apply_input_vars_mask_add ARGS((BddEnc_ptr self, add_ptr inputs));

EXTERN add_ptr 
BddEnc_apply_state_input_vars_mask_add ARGS((BddEnc_ptr self, 
                                             add_ptr states_inputs));

EXTERN BddStates 
BddEnc_apply_state_vars_mask_bdd ARGS((BddEnc_ptr self, BddStates states));

EXTERN BddInputs 
BddEnc_apply_input_vars_mask_bdd ARGS((BddEnc_ptr self, BddInputs inputs));

EXTERN BddStatesInputs 
BddEnc_apply_state_input_vars_mask_bdd ARGS((BddEnc_ptr self,
                                             BddStatesInputs states_inputs));

EXTERN add_ptr 
BddEnc_get_var_mask ARGS((BddEnc_ptr self, node_ptr var_name));


EXTERN array_t* 
BddEnc_ComputePrimeImplicants ARGS((BddEnc_ptr self, 
                                    const array_t* layer_names,
                                    bdd_ptr formula));


/**AutomaticEnd***************************************************************/



#endif /* __BDD_ENC_H__ */
