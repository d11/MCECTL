/**CHeaderFile*****************************************************************

  FileName    [rbc.h]

  PackageName [rbc]

  Synopsis    [Formula handling with Reduced Boolean Circuits (RBCs).]

  Description [External functions and data structures of the rbc package.]

  SeeAlso     []

  Author      [Armando Tacchella]

  Copyright   [
  This file is part of the ``rbc'' package of NuSMV version 2. 
  Copyright (C) 2000-2001 by University of Genova. 

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

  Revision    [$Id: rbc.h,v 1.4.6.4.2.3.2.4.6.6 2007/04/06 11:06:35 nusmv Exp $]

******************************************************************************/

#ifndef _RBC
#define _RBC


#if HAVE_CONFIG_H
# include "config.h"
#endif

/* Standard includes. */
#if HAVE_MALLOC_H
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif  
# include <malloc.h>
#elif HAVE_SYS_MALLOC_H
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif  
# include <sys/malloc.h>
#elif HAVE_STDLIB_H
# include <stdlib.h>
#endif

#include <stdio.h>

/* Submodule includes. */
#include "dag/dag.h"
#include "utils/list.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/**Enum**********************************************************************
  Synopsis      [RBC boolean values.]
  Description   [RBC boolean values.]
  SeeAlso       []
******************************************************************************/
typedef enum Rbc_Bool {
  RBC_FALSE = DAG_BIT_SET, 
  RBC_TRUE = 0
} Rbc_Bool_c;                            

typedef struct RbcManager Rbc_Manager_t; /* The rbc manager. */
typedef Dag_Vertex_t      Rbc_t;         /* An rbc is also a dag vertex. */  

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void Rbc_pkg_init ARGS((void));
EXTERN void Rbc_pkg_quit ARGS((void));

EXTERN int Rbc_Convert2Cnf 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, int polarity,
      lsList clauses, lsList vars, int* literalAssignedToWholeFormula));

EXTERN int Rbc_CnfVar2RbcIndex ARGS((Rbc_Manager_t* rbcManager, int cnfVar));
EXTERN int Rbc_RbcIndex2CnfVar ARGS((Rbc_Manager_t* rbcManager, int rbcIndex));

EXTERN Rbc_t* Rbc_GetOne ARGS((Rbc_Manager_t* rbcManager));
EXTERN Rbc_t* Rbc_GetZero ARGS((Rbc_Manager_t* rbcManager));
EXTERN boolean Rbc_IsConstant ARGS((Rbc_Manager_t* manager, Rbc_t* f));

EXTERN Rbc_t* Rbc_GetIthVar ARGS((Rbc_Manager_t* rbcManager, int varIndex));
EXTERN Rbc_t* Rbc_MakeNot ARGS((Rbc_Manager_t* rbcManager, Rbc_t* left));

EXTERN Rbc_t* Rbc_MakeAnd 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* left, Rbc_t* right, Rbc_Bool_c sign));

EXTERN Rbc_t* Rbc_MakeOr 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* left, Rbc_t* right, Rbc_Bool_c sign));

EXTERN Rbc_t* Rbc_MakeIff 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* left, Rbc_t* right, Rbc_Bool_c sign));

EXTERN Rbc_t* Rbc_MakeXor 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* left, Rbc_t* right, Rbc_Bool_c sign));

EXTERN Rbc_t* Rbc_MakeIte 
ARGS((Rbc_Manager_t* rbcManager, Rbc_t* c, Rbc_t* t, Rbc_t* e, 
      Rbc_Bool_c sign));

EXTERN Rbc_t* Rbc_GetLeftOpnd ARGS((Rbc_t* f));
EXTERN Rbc_t* Rbc_GetRightOpnd ARGS((Rbc_t* f));
EXTERN int Rbc_GetVarIndex ARGS((Rbc_t* f));
EXTERN void Rbc_Mark ARGS((Rbc_Manager_t* rbc, Rbc_t* f));
EXTERN void Rbc_Unmark ARGS((Rbc_Manager_t* rbc, Rbc_t* f));
EXTERN Rbc_Manager_t* Rbc_ManagerAlloc ARGS((int varCapacity));

EXTERN void Rbc_ManagerReserve 
ARGS((Rbc_Manager_t* rbcManager, int newVarCapacity));

EXTERN int Rbc_ManagerCapacity ARGS((Rbc_Manager_t* rbcManager));
EXTERN void Rbc_ManagerFree ARGS((Rbc_Manager_t* rbcManager));
EXTERN void Rbc_ManagerGC ARGS((Rbc_Manager_t* rbcManager));

EXTERN void 
Rbc_OutputDaVinci ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, FILE * outFile));

EXTERN void 
Rbc_OutputSexpr ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, FILE * outFile));

EXTERN void 
Rbc_OutputGdl ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, FILE * outFile));

EXTERN Rbc_t* 
Rbc_Subst ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, int * subst));

EXTERN Rbc_t* Rbc_LogicalSubst ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
                                     int * subst, const int* log2phy, 
                                     const int* phy2log));

EXTERN Rbc_t* Rbc_Shift ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, int shift));

EXTERN Rbc_t* 
Rbc_LogicalShift ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
                       int shift, const int* log2phy, const int* phy2log));
EXTERN Rbc_t* 
Rbc_SubstRbc ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, Rbc_t** substRbc));

EXTERN Rbc_t* 
Rbc_LogicalSubstRbc ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
                          Rbc_t** substRbc, int* phy2log));
EXTERN void 
Rbc_PrintStats ARGS((Rbc_Manager_t* rbcManager, int clustSz, FILE * outFile));

EXTERN lsList 
RbcUtils_get_dependencies ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
                                boolean reset_dag));

EXTERN struct InlineResult_TAG*
RbcInline_apply_inlining ARGS((Rbc_Manager_t* rbcm, Rbc_t* f));


/**AutomaticEnd***************************************************************/

#endif /* _RBC */

