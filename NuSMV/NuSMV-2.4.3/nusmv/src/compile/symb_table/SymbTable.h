/**CHeaderFile*****************************************************************

  FileName    [SymbTable.h]

  PackageName [compile.symb_table]

  Synopsis    [The system wide symbol table interface]

  Description []
                                               
  SeeAlso     [SymbTable.c]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``compile.symb_table'' package of NuSMV
  version 2.  Copyright (C) 2004 by ITC-irst.

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

#ifndef __SYMB_TABLE_H__
#define __SYMB_TABLE_H__

#include "SymbLayer.h"
#include "SymbCache.h"

#include "utils/utils.h"
#include "utils/array.h"


/**Type***********************************************************************

  Synopsis    [SymbTable class accessors]

  Description []

******************************************************************************/
typedef struct SymbTable_TAG* SymbTable_ptr;

#define SYMB_TABLE(x)  \
        ((SymbTable_ptr) x)

#define SYMB_TABLE_CHECK_INSTANCE(x)  \
        (nusmv_assert(SYMB_TABLE(x) != SYMB_TABLE(NULL)))



/* ---------------------------------------------------------------------- */
/*     Public methods                                                     */
/* ---------------------------------------------------------------------- */

EXTERN SymbTable_ptr SymbTable_create ARGS((void));

EXTERN void SymbTable_destroy ARGS((SymbTable_ptr self));

/* -------------------------------------- */
/*            Layers handling             */
/* -------------------------------------- */
EXTERN SymbLayer_ptr 
SymbTable_create_layer ARGS((SymbTable_ptr self, const char* layer_name, 
                             const LayerInsertPolicy ins_policy));

EXTERN void 
SymbTable_remove_layer ARGS((SymbTable_ptr self, SymbLayer_ptr layer));

EXTERN SymbLayer_ptr 
SymbTable_get_layer ARGS((const SymbTable_ptr self, 
                          const char* layer_name)); 

EXTERN void
SymbTable_rename_layer ARGS((const SymbTable_ptr self, 
                             const char* layer_name, const char* new_name));


/* -------------------------------------- */
/*                Symbols                 */
/* -------------------------------------- */

/* Lists of symbols: */
EXTERN NodeList_ptr 
SymbTable_get_vars ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_state_vars ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_input_vars ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_defines ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_constants ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_state_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_input_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_state_input_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_model_state_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_model_input_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_model_state_input_symbols ARGS((const SymbTable_ptr self));

EXTERN NodeList_ptr 
SymbTable_get_layers_state_symbols ARGS((SymbTable_ptr self, 
                                         const array_t* layer_names));
EXTERN NodeList_ptr 
SymbTable_get_layers_input_symbols ARGS((SymbTable_ptr self, 
                                         const array_t* layer_names));
EXTERN NodeList_ptr 
SymbTable_get_layers_state_input_symbols ARGS((SymbTable_ptr self, 
                                               const array_t* layer_names));

/* Length of lists of symbols: */
EXTERN int SymbTable_get_vars_num ARGS((const SymbTable_ptr self));

EXTERN int SymbTable_get_state_vars_num ARGS((const SymbTable_ptr self));

EXTERN int SymbTable_get_input_vars_num ARGS((const SymbTable_ptr self));

EXTERN int SymbTable_get_defines_num ARGS((const SymbTable_ptr self));

EXTERN int SymbTable_get_constants_num ARGS((const SymbTable_ptr self));


/* Symbols related info: */
EXTERN SymbType_ptr 
SymbTable_get_var_type ARGS((const SymbTable_ptr self, const node_ptr name));

EXTERN node_ptr 
SymbTable_get_define_body ARGS((const SymbTable_ptr self, 
                                const node_ptr name));

EXTERN node_ptr 
SymbTable_get_define_flatten_body ARGS((const SymbTable_ptr self, 
                                        const node_ptr name));

EXTERN node_ptr 
SymbTable_get_define_context ARGS((const SymbTable_ptr self, 
                                   const node_ptr name));

/* Queries: */
EXTERN boolean 
SymbTable_is_symbol_state_var ARGS((const SymbTable_ptr self, 
                                    const node_ptr name));

EXTERN boolean 
SymbTable_is_symbol_input_var ARGS((const SymbTable_ptr self, 
                                    const node_ptr name));

EXTERN boolean 
SymbTable_is_symbol_var ARGS((const SymbTable_ptr self, const node_ptr name));

EXTERN boolean
SymbTable_is_symbol_bool_var ARGS((const SymbTable_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbTable_is_symbol_declared ARGS((const SymbTable_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbTable_is_symbol_define ARGS((const SymbTable_ptr self, 
                                 const node_ptr name));

EXTERN boolean 
SymbTable_is_symbol_constant ARGS((const SymbTable_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbTable_list_contains_input_var ARGS((const SymbTable_ptr self, 
                                        const NodeList_ptr var_list));

EXTERN boolean 
SymbTable_list_contains_state_var ARGS((const SymbTable_ptr self, 
                                        const NodeList_ptr var_list));

EXTERN boolean 
SymbTable_list_contains_undef_var ARGS((const SymbTable_ptr self, 
                                        const NodeList_ptr var_list));

#endif /* __SYMB_TABLE_H__ */
