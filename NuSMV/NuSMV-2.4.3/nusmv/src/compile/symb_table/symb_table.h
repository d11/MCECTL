/**CHeaderFile*****************************************************************

  FileName    [symb_table.h]

  PackageName [compile.symb_table]

  Synopsis    [Public interface of compile.symb_table package]

  Description [This package contains just a few public functions.
  Most functionality lays in the classes this package contains.
  So these classes for more info.]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile.symb_table'' package of NuSMV version 2. 
  Copyright (C) 2005 by ITC-irst. 

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

  Revision    [$Id: symb_table.h,v 1.1.2.7 2006/04/04 09:41:58 nusmv Exp $]

******************************************************************************/
#ifndef __SYMB_TABLE_PKG_H__
#define __SYMB_TABLE_PKG_H__

#include "SymbType.h"
#include "node/node.h"

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

/* create simplified types with shared memory */
EXTERN SymbType_ptr SymbTablePkg_no_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_statement_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_boolean_enum_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_pure_symbolic_enum_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_int_symbolic_enum_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_pure_int_enum_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_integer_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_real_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_word_type ARGS((int width));
EXTERN SymbType_ptr SymbTablePkg_wordarray_type ARGS((int awidth, int vwidth));
EXTERN SymbType_ptr SymbTablePkg_boolean_set_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_integer_set_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_symbolic_set_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_integer_symbolic_set_type ARGS((void));
EXTERN SymbType_ptr SymbTablePkg_error_type ARGS((void));


#endif /* __SYMB_TABLE_PKG_H__ */
