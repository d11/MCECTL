/**CHeaderFile*****************************************************************

  FileName    [SymbCache.h]

  PackageName [compile.symb_table]

  Synopsis    [The public interface of class SymbCache]

  Description []
                                               
  SeeAlso     [SymbCache.c]

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

#ifndef __SYMB_CACHE_H__
#define __SYMB_CACHE_H__

#include "SymbType.h"
#include "node/node.h"
#include "utils/utils.h"
#include "utils/NodeList.h"



/**Type***********************************************************************

  Synopsis     [The SymbCache type ]

  Description [An instance of class SymbCache is hold by each instance
  of SymbTable. This means that the life cycle of a SymbCache is never
  managed by the user. Furthermore, only tests on symbols are allowed
  to be performed by the user. All other features (e.g. creation of
  new symbols) are performed by SymbLayers and by SymbTable, by using a 
  private interface]

  Notes        []

******************************************************************************/
typedef struct SymbCache_TAG*  SymbCache_ptr;

#define SYMB_CACHE(x) \
          ((SymbCache_ptr) x)

#define SYMB_CACHE_CHECK_INSTANCE(x) \
          ( nusmv_assert(SYMB_CACHE(x) != SYMB_CACHE(NULL)) )


/* ---------------------------------------------------------------------- */
/* Class SymbCache's public methods                                       */
/* ---------------------------------------------------------------------- */

EXTERN NodeList_ptr 
SymbCache_get_vars ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_state_vars ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_input_vars ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_defines ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_constants ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_state_symbols ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_input_symbols ARGS((const SymbCache_ptr self));

EXTERN NodeList_ptr 
SymbCache_get_state_input_symbols ARGS((const SymbCache_ptr self));

EXTERN SymbType_ptr
SymbCache_get_var_type ARGS((const SymbCache_ptr self, const node_ptr name));

EXTERN node_ptr 
SymbCache_get_define_body ARGS((const SymbCache_ptr self, 
                                const node_ptr name));

EXTERN node_ptr 
SymbCache_get_define_flatten_body ARGS((const SymbCache_ptr self,
                                        const node_ptr name));

EXTERN node_ptr 
SymbCache_get_define_context ARGS((const SymbCache_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_state_var ARGS((const SymbCache_ptr self, 
                                    const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_input_var ARGS((const SymbCache_ptr self, 
                                    const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_var ARGS((const SymbCache_ptr self, const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_declared ARGS((const SymbCache_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_define ARGS((const SymbCache_ptr self, 
                                 const node_ptr name));

EXTERN boolean 
SymbCache_is_symbol_constant ARGS((const SymbCache_ptr self, 
                                   const node_ptr name));

EXTERN boolean 
SymbCache_list_contains_input_var ARGS((const SymbCache_ptr self, 
                                        const NodeList_ptr var_list));

EXTERN boolean 
SymbCache_list_contains_state_var ARGS((const SymbCache_ptr self, 
                                        const NodeList_ptr var_list));

EXTERN boolean 
SymbCache_list_contains_undef_var ARGS((const SymbCache_ptr self,
                                        const NodeList_ptr var_list));


#endif /* __SYMB_CACHE_H__ */
