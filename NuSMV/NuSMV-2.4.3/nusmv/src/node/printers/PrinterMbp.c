
/**CFile***********************************************************************

  FileName    [PrinterMbp.c]

  PackageName [node.printers]

  Synopsis    [Implementaion of class 'PrinterMbp']

  Description [This class is used to print expressions and 
  constructs related to MBP (Game) package]

  SeeAlso     [PrinterMbp.h]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``node.printers'' package of NuSMV version 2. 
  Copyright (C) 2006 by ITC-irst. 

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

  Revision    [$Id: PrinterMbp.c,v 1.1.2.1 2006/07/24 16:59:57 nusmv Exp $]

******************************************************************************/

#include "PrinterMbp.h" 
#include "PrinterMbp_private.h" 

#include "parser/symbols.h"
#include "parser/mbp_symbols.h"

#include "utils/error.h"

static char rcsid[] UTIL_UNUSED = "$Id: PrinterMbp.c,v 1.1.2.1 2006/07/24 16:59:57 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/* See 'PrinterMbp_private.h' for class 'PrinterMbp' definition. */ 

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**Macro***********************************************************************

  Synopsis           [Short way of calling printer_base_throw_print_node]

  Description        [Use this macro to recursively recall print_node]

  SeeAlso            []
  
******************************************************************************/
#define _THROW(n, p)  printer_base_throw_print_node(PRINTER_BASE(self), n, p) 


/**Macro***********************************************************************

  Synopsis           [Short way of calling printer_base_print_string]

  Description [Use to print a string (that will be redirected to the
  currently used stream)]

  SeeAlso            []
  
******************************************************************************/
#define _PRINT(str)  printer_base_print_string(PRINTER_BASE(self), str)



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void printer_mbp_finalize ARGS((Object_ptr object, void* dummy));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The PrinterMbp class constructor]

  Description        [The PrinterMbp class constructor]

  SideEffects        []

  SeeAlso            [PrinterMbp_destroy]   
  
******************************************************************************/
PrinterMbp_ptr PrinterMbp_create(const char* name)
{
  PrinterMbp_ptr self = ALLOC(PrinterMbp, 1);
  PRINTER_MBP_CHECK_INSTANCE(self);

  printer_mbp_init(self, name, 
			NUSMV_MBP_SYMBOL_FIRST, 
			NUSMV_MBP_SYMBOL_LAST - NUSMV_MBP_SYMBOL_FIRST);
  return self;
}




/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The PrinterMbp class private initializer]

  Description        [The PrinterMbp class private initializer]

  SideEffects        []

  SeeAlso            [PrinterMbp_create]   
  
******************************************************************************/
void printer_mbp_init(PrinterMbp_ptr self, 
			   const char* name, int low, size_t num)
{
  /* base class initialization */
  printer_base_init(PRINTER_BASE(self), name, low, num);
  
  /* members initialization */

  /* virtual methods settings */  
  OVERRIDE(Object, finalize) = printer_mbp_finalize;
  OVERRIDE(PrinterBase, print_node) = printer_mbp_print_node;

}


/**Function********************************************************************

  Synopsis           [The PrinterMbp class private deinitializer]

  Description        [The PrinterMbp class private deinitializer]

  SideEffects        []

  SeeAlso            [PrinterMbp_destroy]   
  
******************************************************************************/
void printer_mbp_deinit(PrinterMbp_ptr self)
{
  /* members deinitialization */


  /* base class initialization */
  printer_base_deinit(PRINTER_BASE(self));
}


/**Function********************************************************************

  Synopsis    [Virtual menthod that prints the given node 
  (MBP specific nodes are handled here)]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
int printer_mbp_print_node(PrinterBase_ptr self, node_ptr n, int priority)
{
  switch (node_get_type(n)) {
  case GAME:
  case PLAYER:
    /* these node are not expected to be printted as usual expression */
    rpterr("Unexpected printing of high-level MBP constructs");

  case REACHTARGET:
  case AVOIDTARGET:
  case REACHDEADLOCK:
  case AVOIDDEADLOCK:
  case BUCHIGAME:
  case GENREACTIVITY:
    /* these specifications are not expected to be printted as usual exp */
    rpterr("Unexpected printing of high-level MBP specifications");


    /* this is MBP specifications wrapper. Player name is on the left
       and the expression on the right */
  case MBP_SPEC_WRAPPER:
    return _THROW(car(n), 0) /* print the player name */ && 
      _PRINT(" ") && 
      _THROW(cdr(n), 0) /* print the expression */;
    break;

  case MBP_EXP_LIST: /* print parentheses around the expressions in the list */
    nusmv_assert(Nil == cdr(n) &&
		 Nil != car(n) && CONS == node_get_type(car(n)));

    return _PRINT("(") && _THROW(car(n), 0) &&  _PRINT(")");
    break;
    
    /* print parentheses around the the lists and put "->" between them */
  case MBP_TWO_EXP_LISTS:
    nusmv_assert(Nil != car(n) && Nil != cdr(n) &&
		 CONS == node_get_type(car(n)) && 
		 CONS == node_get_type(cdr(n)));

    return _PRINT("(") &&
      _THROW(car(n), 0) &&
      _PRINT(") -> (") &&
      _THROW(cdr(n), 0) &&
      _PRINT(")");
    
  default:
    internal_error("printer_mbp_print_node: unsupported type = %d", 
		   node_get_type(n));
  }
  
  rpterr("PrinterMbp.c : Impossible code");
  return 1;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [The PrinterMbp class virtual finalizer]

  Description [Called by the class destructor]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void printer_mbp_finalize(Object_ptr object, void* dummy) 
{
  PrinterMbp_ptr self = PRINTER_MBP(object);

  printer_mbp_deinit(self);
  FREE(self);
}




/**AutomaticEnd***************************************************************/

