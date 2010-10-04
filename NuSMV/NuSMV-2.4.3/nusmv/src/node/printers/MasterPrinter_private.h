
/**CHeaderFile*****************************************************************

  FileName    [MasterPrinter_private.h]

  PackageName [node.printers]

  Synopsis    [Private interface of class 'MasterPrinter', to be used by printers]

  Description []

  SeeAlso     [MasterPrinter.c]

  Author      [Roberto Cavada]

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

  Revision    [$Id: MasterPrinter_private.h,v 1.1.2.1 2006/03/03 11:02:48 nusmv Exp $]

******************************************************************************/


#ifndef __MASTER_PRINTER_PRIVATE_H__
#define __MASTER_PRINTER_PRIVATE_H__

#include "MasterPrinter.h"
#include "utils/utils.h" 


/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int 
master_printer_print_node ARGS((MasterPrinter_ptr self, node_ptr n, 
				int priority));


/**AutomaticEnd***************************************************************/


#endif /* __MASTER_PRINTER_PRIVATE_H__ */
