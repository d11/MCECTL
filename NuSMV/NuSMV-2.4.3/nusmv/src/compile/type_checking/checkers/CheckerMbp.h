/**CHeaderFile*****************************************************************

  FileName    [CheckerMbp.h]

  PackageName [compile.type_checking.checkers]

  Synopsis    [Public interface of class 'CheckerMbp']

  Description []

  SeeAlso     [CheckerMbp.c]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile.type_checking.checkers'' package of 
  NuSMV version 2. Copyright (C) 2006 by ITC-irst. 

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

  Revision    [$Id: CheckerMbp.h,v 1.1.2.1.4.1 2007/04/04 12:00:33 nusmv Exp $]

******************************************************************************/


#ifndef __CHECKER_MBP_H__
#define __CHECKER_MBP_H__


#include "CheckerBase.h" 
#include "utils/utils.h" 


/**Type***********************************************************************

  Synopsis    [Definition of the public accessor for class CheckerMbp]

  Description []

******************************************************************************/
typedef struct CheckerMbp_TAG*  CheckerMbp_ptr;


/**Macros**********************************************************************

  Synopsis    [To cast and check instances of class CheckerMbp]

  Description [These macros must be used respectively to cast and to check
  instances of class CheckerMbp]

******************************************************************************/
#define CHECKER_MBP(self) \
         ((CheckerMbp_ptr) self)

#define CHECKER_MBP_CHECK_INSTANCE(self) \
         (nusmv_assert(CHECKER_MBP(self) != CHECKER_MBP(NULL)))



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN CheckerMbp_ptr CheckerMbp_create ARGS((void));


/**AutomaticEnd***************************************************************/



#endif /* __CHECKER_MBP_H__ */
