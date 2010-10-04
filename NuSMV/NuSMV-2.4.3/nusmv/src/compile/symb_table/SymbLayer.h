/**CHeaderFile*****************************************************************

  FileName    [SymbLayer.h]

  PackageName [compile.symb_table]

  Synopsis    [The wide system symbols layer interface]

  Description [This is the public interface of the class SymbLayer]
                                               
  SeeAlso     [SymbolLayer.c]

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

#ifndef __SYMB_LAYER_H__
#define __SYMB_LAYER_H__

#include "SymbType.h"
#include "utils/utils.h"
#include "utils/NodeList.h"
#include "node/node.h"


/**Type***********************************************************************

  Synopsis    [Public type accessor for class SymbLayer]

  Description [See the description of class SymbLayer for further
  information.]

******************************************************************************/
typedef struct SymbLayer_TAG* SymbLayer_ptr;

#define SYMB_LAYER(x)  \
        ((SymbLayer_ptr) x)

#define SYMB_LAYER_CHECK_INSTANCE(x)  \
        (nusmv_assert(SYMB_LAYER(x) != SYMB_LAYER(NULL)))


/**Type***********************************************************************

  Synopsis [To be used as a policy when a layer is pushed in the
  layers stack of a SymbTable]

  Description [WHen a layer is pushed within a symbol table, it will
  be inserted according to an insertion order that will change the order the 
  symbols occuring within the layer are encoded in the encodings. 
  The default behaviour is to push the layer on the top of the stack.

  Forced positions (SYMB_LAYER_POS_FORCE_*) make the layer to stay always 
  at that position. Only one layer can be added to a symbol table with 
  the same forced postion at a given time, i.e. two or more layers are not 
  allowed to have the same forced position into the same symbol table. 
  ]

******************************************************************************/
typedef enum LayerInsertPolicy_TAG {
  SYMB_LAYER_POS_DEFAULT,   /* default is equal to SYMB_LAYER_POS_TOP */
  SYMB_LAYER_POS_FORCE_TOP, /* layer is forced to be always at the top */
  SYMB_LAYER_POS_TOP,       /* Inserted before other top */
  SYMB_LAYER_POS_BOTTOM,    /* inserted after other bottom */
  SYMB_LAYER_POS_FORCE_BOTTOM /* layer is forced to be always at the bottom */
} LayerInsertPolicy;


/* ---------------------------------------------------------------------- */
/*     Public methods                                                     */
/* ---------------------------------------------------------------------- */

EXTERN const char* 
SymbLayer_get_name ARGS((const SymbLayer_ptr self));

EXTERN boolean 
SymbLayer_can_declare_constant ARGS((const SymbLayer_ptr self, 
                                     const node_ptr name));

EXTERN void 
SymbLayer_declare_constant ARGS((SymbLayer_ptr self, node_ptr name));

EXTERN boolean 
SymbLayer_can_declare_var ARGS((const SymbLayer_ptr self, 
                                const node_ptr name));

EXTERN void 
SymbLayer_declare_input_var ARGS((SymbLayer_ptr self, node_ptr var, 
                                  SymbType_ptr type));

EXTERN void 
SymbLayer_declare_state_var ARGS((SymbLayer_ptr self, node_ptr var, 
                                  SymbType_ptr type));

EXTERN boolean 
SymbLayer_can_declare_define ARGS((const SymbLayer_ptr self,
                                   const node_ptr name));

EXTERN void 
SymbLayer_declare_define ARGS((SymbLayer_ptr self, node_ptr name, 
                               node_ptr ctx, node_ptr definition));

EXTERN void SymbLayer_remove_var ARGS((SymbLayer_ptr self, node_ptr name));


EXTERN int SymbLayer_get_constants_num ARGS((const SymbLayer_ptr self));

EXTERN int SymbLayer_get_state_vars_num ARGS((const SymbLayer_ptr self));
EXTERN int SymbLayer_get_bool_state_vars_num ARGS((const SymbLayer_ptr self));
EXTERN int SymbLayer_get_input_vars_num ARGS((const SymbLayer_ptr self));
EXTERN int SymbLayer_get_bool_input_vars_num ARGS((const SymbLayer_ptr self));
EXTERN int SymbLayer_get_defines_num ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_constants ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_defines ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_all_vars ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_all_symbols ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_state_vars ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_input_vars ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_bool_input_vars ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_bool_state_vars ARGS((const SymbLayer_ptr self));

EXTERN NodeList_ptr 
SymbLayer_get_bool_vars ARGS((const SymbLayer_ptr self));

EXTERN boolean 
SymbLayer_must_insert_before ARGS((const SymbLayer_ptr self, 
                                   const SymbLayer_ptr other));


#endif /* __SYMB_LAYER_H__ */
