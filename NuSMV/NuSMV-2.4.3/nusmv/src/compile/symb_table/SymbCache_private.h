/**CHeaderFile*****************************************************************

  FileName    [SymbCache_private.h]

  PackageName [compile.symb_table]

  Synopsis    [The SymbCache class private interface]

  Description []

  SeeAlso     [SymbCache.h]

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

#ifndef __SYMB_CACHE_PRIVATE_H__
#define __SYMB_CACHE_PRIVATE_H__


#include "SymbCache.h"
#include "SymbTable.h"

#include "utils/utils.h"
#include "node/node.h"

/*---------------------------------------------------------------------------*/
/* Type definitions                                                          */
/*---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------- */
/*     Private methods                                                    */
/* ---------------------------------------------------------------------- */

EXTERN SymbCache_ptr SymbCache_create ARGS((SymbTable_ptr symb_table));

EXTERN void SymbCache_destroy ARGS((SymbCache_ptr self));

EXTERN void 
SymbCache_new_input_var ARGS((SymbCache_ptr self, 
                              node_ptr var, SymbType_ptr type));

EXTERN void 
SymbCache_new_state_var ARGS((SymbCache_ptr self, 
                              node_ptr var, SymbType_ptr type));

EXTERN void 
SymbCache_remove_var ARGS((SymbCache_ptr self, node_ptr var));


EXTERN void 
SymbCache_new_define ARGS((SymbCache_ptr self, 
                           node_ptr name, 
                           node_ptr context, node_ptr definition));

EXTERN void 
SymbCache_remove_define ARGS((SymbCache_ptr self, node_ptr define));

EXTERN void 
SymbCache_new_constant ARGS((SymbCache_ptr self, node_ptr name)); 

EXTERN void 
SymbCache_remove_constant ARGS((SymbCache_ptr self, node_ptr constant));


#endif /* __SYMB_CACHE_PRIVATE_H__ */
