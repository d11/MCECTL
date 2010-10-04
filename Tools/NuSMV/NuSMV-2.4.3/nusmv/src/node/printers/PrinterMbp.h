
/**CHeaderFile*****************************************************************

  FileName    [PrinterMbp.h]

  PackageName [node.printers]

  Synopsis    [Public interface of class 'PrinterMbp']

  Description []

  SeeAlso     [PrinterMbp.c]

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

  Revision    [$Id: PrinterMbp.h,v 1.1.2.1 2006/07/24 16:59:57 nusmv Exp $]

******************************************************************************/


#ifndef __PRINTER_MBP__H__
#define __PRINTER_MBP__H__


#include "PrinterBase.h"
#include "utils/utils.h" 


/**Type***********************************************************************

  Synopsis    [Definition of the public accessor for class PrinterMbp]

  Description []

******************************************************************************/
typedef struct PrinterMbp_TAG*  PrinterMbp_ptr;


/**Macros**********************************************************************

  Synopsis    [To cast and check instances of class PrinterMbp]

  Description [These macros must be used respectively to cast and to check
  instances of class PrinterMbp]

******************************************************************************/
#define PRINTER_MBP(self) \
         ((PrinterMbp_ptr) self)

#define PRINTER_MBP_CHECK_INSTANCE(self) \
         (nusmv_assert(PRINTER_MBP(self) != PRINTER_MBP(NULL)))



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN PrinterMbp_ptr PrinterMbp_create ARGS((const char* name));


/**AutomaticEnd***************************************************************/



#endif /* __PRINTER_MBP_H__ */
