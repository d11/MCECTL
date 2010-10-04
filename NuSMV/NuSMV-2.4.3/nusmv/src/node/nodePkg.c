/**CFile***********************************************************************

  FileName    [nodePkg.c]

  PackageName [node]

  Synopsis    [Initialization and deinitialization for package node and 
  subpackages]

  Description [Initialization and deinitialization for package node and 
  subpackages]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``node'' package of NuSMV version 2. 
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

******************************************************************************/

#include "node.h"
#include "nodeInt.h" 

#include "node/printers/MasterPrinter.h"
#include "node/printers/PrinterWffCore.h"
#include "node/printers/PrinterPsl.h"
#include "node/printers/PrinterMbp.h"

#include "utils/error.h" /* for CATCH */


static char rcsid[] UTIL_UNUSED = "$Id: nodePkg.c,v 1.1.2.4 2006/07/24 17:00:47 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

  Synopsis    [The nodes master printer]

  Description []

******************************************************************************/
MasterPrinter_ptr master_wff_printer = MASTER_PRINTER(NULL);


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the node package]

  Description        [Creates master and printers, and initializes the node 
  structures]

  SideEffects        []

  SeeAlso            [node_pkg_quit]

******************************************************************************/
void node_pkg_init()
{
  node_init();

  if (master_wff_printer == MASTER_PRINTER(NULL)) {
    /* Core printer */
    master_wff_printer = MasterPrinter_create();

    CATCH {
      PrinterBase_ptr printer;

      printer = PRINTER_BASE(PrinterWffCore_create("Core Wff Printer"));
      MasterNodeWalker_register_walker(MASTER_NODE_WALKER(master_wff_printer), 
				       NODE_WALKER(printer));

      /* printer for PSL: */
      printer = PRINTER_BASE(PrinterPsl_create("PSL Printer"));
      MasterNodeWalker_register_walker(MASTER_NODE_WALKER(master_wff_printer), 
				       NODE_WALKER(printer));
      /* printer for MBP: */
      printer = PRINTER_BASE(PrinterMbp_create("MBP Printer"));
      MasterNodeWalker_register_walker(MASTER_NODE_WALKER(master_wff_printer), 
				       NODE_WALKER(printer));
    }
    FAIL {
      node_pkg_quit();
      nusmv_exit(1);
    }   
  }

}


/**Function********************************************************************

  Synopsis [Deinitializes the packages, finalizing all internal
  structures]

  Description        []

  SideEffects        []

  SeeAlso            [node_pkg_init]

******************************************************************************/
void node_pkg_quit()
{
  if (master_wff_printer != MASTER_PRINTER(NULL)) {
    MasterNodeWalker_destroy(MASTER_NODE_WALKER(master_wff_printer));
    master_wff_printer = MASTER_PRINTER(NULL);
  }  
  
  node_quit();
}


/**Function********************************************************************

  Synopsis           [Returns the global master wff printer]

  Description        [Returns the global master wff printer.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
MasterPrinter_ptr node_pkg_get_global_master_wff_printer()
{
  nusmv_assert(master_wff_printer != MASTER_PRINTER(NULL)); /* initialized */
  return master_wff_printer;
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

