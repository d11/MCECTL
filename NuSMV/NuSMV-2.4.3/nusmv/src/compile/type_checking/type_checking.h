/**CHeaderFile*****************************************************************

  FileName    [type_checking.h]

  PackageName [compile.type_checking]

  Synopsis    [The interface of the type-checking package.]

  Description [This package contains the functions required to infer
  the type of expressions and check that the input SMV modules and
  specifications do not violation the type system.
  This package works as a intermediate link between the concrete
  data-structures obtained after the flattening and quite abstract
  TypeChecker class which actually performs the type checking.

  The initialisation of the package is performed at the beginning of
  module checking function. And "reset" command di-initialises the module.] 

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2. 
  Copyright (C) 2000-2005 by  ITC-irst. 

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

  Revision    [$Id: type_checking.h,v 1.1.2.13 2006/04/04 09:41:58 nusmv Exp $]

******************************************************************************/
#ifndef __TYPE_CHECKING_H__
#define __TYPE_CHECKING_H__

#include "TypeChecker.h"
#include "compile/symb_table/SymbType.h" /* for SymbType_ptr */
#include "compile/symb_table/SymbLayer.h" /* for SymbLayer_ptr */
#include "node/node.h"  /* for node_ptr */
#include "prop/prop.h"  /* for Prop_ptr */
#include "utils/utils.h" /* for EXTERN */

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/
EXTERN void TypeCheckingPkg_init ARGS((SymbTable_ptr symbolTable));
EXTERN void TypeCheckingPkg_quit ARGS((void));


EXTERN TypeChecker_ptr TypeCheckingPkg_get_global_type_checker ARGS((void));


/**AutomaticEnd***************************************************************/

#endif /* __TYPE_CHECKING_H__ */
