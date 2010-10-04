/**CHeaderFile*****************************************************************

  FileName    [bmcBmc.h]

  PackageName [bmc]

  Synopsis    [High-level functionalities interface file]

  Description [High level functionalities allow to perform Bounded Model 
  Checking for LTL properties and invariants, as well as simulations.]

  SeeAlso     []

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``bmc'' package of NuSMV version 2. 
  Copyright (C) 2000-2001 by ITC-irst and University of Trento. 

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

  Revision    [$Id: bmcBmc.h,v 1.2.4.3.2.4.2.3 2005/06/22 15:03:45 nusmv Exp $]

******************************************************************************/

#ifndef _BMC_BMC_H
#define _BMC_BMC_H

#include "bmcDump.h"

#include "fsm/be/BeFsm.h"

#include "utils/utils.h"
#include "prop/prop.h"


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/
EXTERN int Bmc_GenSolveLtl ARGS((Prop_ptr ltlprop,
				 const int k, const int relative_loop,
				 const boolean must_inc_length,
				 const boolean must_solve, 
				 const Bmc_DumpType dump_type,
				 const char* dump_fname_template));


EXTERN int Bmc_GenSolveInvar ARGS((Prop_ptr invarprop,
				   const boolean must_solve, 
				   const Bmc_DumpType dump_type,
				   const char* dump_fname_template));

EXTERN int 
Bmc_GenSolveInvar_EenSorensson ARGS((Prop_ptr invarprop,  
				     const int max_k, 
				     const Bmc_DumpType dump_type,
				     const char* dump_fname_template));

EXTERN int 
Bmc_Simulate ARGS((const BeFsm_ptr be_fsm, const int k, 
		   const boolean print_trace, const boolean changes_only)); 

/* incremental algorithms */

int Bmc_GenSolveLtlInc ARGS((Prop_ptr ltlprop,
			     const int k, const int relative_loop,
			     const boolean must_inc_length));

int Bmc_GenSolveInvarZigzag ARGS((Prop_ptr invarprop, const int max_k));

int Bmc_GenSolveInvarDual ARGS((Prop_ptr invarprop, const int max_k));


EXTERN int Bmc_check_psl_property ARGS((Prop_ptr prop, 
					boolean dump_prob, 
					boolean inc_sat, 
					boolean single_prob, 
					int k, int rel_loop));

#endif /* _BMC_BMC_H */

