/**CHeaderFile*****************************************************************

  FileName    [type_checking.c]

  PackageName [compile.type_checking]

  Synopsis    [the implementation of the type checking package]

  Description [See .h file for more info]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile.type_checking'' package of NuSMV version 2.
  Copyright (C) 2005 by  ITC-irst. 

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

#include "type_checking.h"
#include "type_checkingInt.h"
#include "TypeChecker.h"

#include "compile/type_checking/checkers/CheckerCore.h"
#include "compile/type_checking/checkers/CheckerStatement.h"
#include "compile/type_checking/checkers/CheckerPsl.h"
#include "compile/type_checking/checkers/CheckerMbp.h"

#include "parser/symbols.h"
#include "utils/ustring.h" 
#include "utils/assoc.h" 

static char rcsid[] UTIL_UNUSED = "$Id: type_checking.c,v 1.1.2.28 2006/07/24 17:02:26 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
  
/**Variable********************************************************************

  Synopsis           [The global type checker]

  Description [An instance of a type checker is created and destroyed
  in the init and end functions of the package. During its lifetime a
  type checker can deal with only one symbol table. Moreover, type
  checker remember the expressions being already checked. As result an
  instance of a type checker can deal with only symbol table and one
  input SMV model only.]

******************************************************************************/
static TypeChecker_ptr typeChecker = TYPE_CHECKER(NULL); 


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initialises the type checking package.]

  Description        [Can be invoked only after the symbol table has been
  created.]

  SideEffects        []

******************************************************************************/
void TypeCheckingPkg_init(SymbTable_ptr symbolTable)
{
  NodeWalker_ptr checker;

  nusmv_assert(TYPE_CHECKER(NULL) == typeChecker); /* re-initialisation */

  typeChecker = TypeChecker_create(symbolTable);

  /* checkers creation and registration */
  checker = NODE_WALKER(CheckerCore_create()); /* core */
  MasterNodeWalker_register_walker(MASTER_NODE_WALKER(typeChecker), checker);

  checker = NODE_WALKER(CheckerStatement_create()); /* statements */
  MasterNodeWalker_register_walker(MASTER_NODE_WALKER(typeChecker), checker);

  checker = NODE_WALKER(CheckerPsl_create()); /* psl */
  MasterNodeWalker_register_walker(MASTER_NODE_WALKER(typeChecker), checker);

  checker = NODE_WALKER(CheckerMbp_create()); /* mbp */
  MasterNodeWalker_register_walker(MASTER_NODE_WALKER(typeChecker), checker);

}


/**Function********************************************************************

  Synopsis           [Shuts down the type checking package.]

  Description        [WARNING: All symbolic types SymbTypes
  generated during type checking will be destroyed after this package
  de-initialisation.]

  SideEffects        []

******************************************************************************/
void TypeCheckingPkg_quit()
{
  if (TYPE_CHECKER(NULL) != typeChecker) { /* package is initialised */
    TypeChecker_destroy(typeChecker);
    typeChecker = TYPE_CHECKER(NULL);
  }
}


/**Function********************************************************************

  Synopsis           [Returns the global type checker, used by 
  the this package.]

  Description        [WARNING: This function can be invoked 
  only after initialisation and before de-initialisation.]

  SideEffects        []

******************************************************************************/
TypeChecker_ptr TypeCheckingPkg_get_global_type_checker()
{
  return typeChecker;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

