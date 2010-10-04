/**CHeaderFile*****************************************************************

  FileName    [rbcInt.h]

  PackageName [rbc]

  Synopsis    [Formula handling with Reduced Boolean Circuits (RBCs).]

  Description [Internal functions and data structures of the rbc package.]

  SeeAlso     []

  Author      [Armando Tacchella and Tommi Junttila]

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

  Revision    [$Id: rbcInt.h,v 1.3.6.2.2.2.2.2.6.5 2007/04/06 11:06:35 nusmv Exp $]

******************************************************************************/

#ifndef _RBCINT
#define _RBCINT

#include "rbc/rbc.h"
#include "rbc/InlineResult.h"

#include "opt/opt.h"
#include "utils/utils.h"
#include "utils/assoc.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/* Rbc operators (symbols) */
#define RBCTOP   (int) 0
#define RBCVAR   (int) 1
#define RBCAND   (int) 2 
#define RBCIFF   (int) 3
#define RBCITE   (int) 4

/* special value for a rbc node */
#define RBCDUMMY ((Dag_Vertex_t*) 5)

/* Rbc statistics. */
#define RBCVAR_NO   (int)  0  /* How many variables. */ 
#define RBCMAX_STAT (int)  1


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************
  Synopsis      [RBC manager.]
  Description   [Handles rbcs:
                 <ul> 
                 <li> dagManager, to handle the associated pool of vertices;
                 <li> varTable, to index variable vertices;
		 <li> varCapacity, the maximum number of variables;
		 <li> one and zero, the logical constants true and false;
		 <li> rbcNode2cnfVar: RBC node -> CNF var
                      (used only in CNF convertion);
		 <li> cnfVar2rbcNode: CNF var -> RBC node;
                      (used only to obtain original variables from
                       CNF formula solution);
		 <li> maxUnchangedRbcVariable is the maximal RBC var
                      that will have the same index in CNF
                      (used for ease the readability of CNF formulas)
                      It is set during the first invocation of Rbc_Convert2Cnf;
		 <li> maxCnfVariable is maximal variable used in CNF formula,
                      used to generate new unique CNF variables.
		 <li> stats, for bookkeeping.
                 </ul>]
  SeeAlso       []
******************************************************************************/
struct RbcManager {
  Dag_Manager_t* dagManager;
  Rbc_t** varTable; 
  int varCapacity;
  Rbc_t* one;
  Rbc_t* zero;

  hash_ptr rbcNode2cnfVar;
  hash_ptr cnfVar2rbcNode;
  int maxUnchangedRbcVariable;
  int maxCnfVariable;

  int stats[RBCMAX_STAT];
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN FILE* nusmv_stderr;
EXTERN FILE* nusmv_stdout;
EXTERN options_ptr options;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**Macro**********************************************************************
  Synopsis    [Controls the sign of an rbc.]
  Description [The pointer is filtered by a bitwise-xor with either RBC_FALSE
              or RBC_TRUE. The pointer is not altered, but the leftmost bit
	      is complemented when s==RBC_FALSE and goes unchanged when
	      s == RBC_TRUE.]
  SideEffects [none]
  SeeAlso     []
******************************************************************************/
#define RbcId(r,s) \
  (Rbc_t*)((nusmv_ptrint)s ^ (nusmv_ptrint)r)


/**Macro**********************************************************************
  Synopsis    [Control the kind of CNF conversion has to be used]
  Description []
  SideEffects [none]
  SeeAlso     []
******************************************************************************/
//#define CNF_CONV_SIMP 1
#define CNF_CONV_COMP 1

/**Macro**********************************************************************
  Synopsis    [Control the way compact CNF conversion is performed]
  Description []
  SideEffects [none]
  SeeAlso     []
******************************************************************************/
//#define CNF_CONV_SP 0


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

int Rbc_Convert2CnfSimple ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
				lsList clauses, lsList vars, 
				int* literalAssignedToWholeFormula));

int Rbc_Convert2CnfCompact ARGS((Rbc_Manager_t* rbcManager, Rbc_t* f, 
				 int polarity, 
				 lsList clauses, lsList vars,
				 int* literalAssignedToWholeFormula));

int Rbc_get_node_cnf ARGS((Rbc_Manager_t* rbcm, Rbc_t* f, int* maxvar));

/* inlining cache control */
void rbc_inlining_cache_init ARGS((void));
void rbc_inlining_cache_quit ARGS((void));
void rbc_inlining_cache_add_result ARGS((Rbc_t* f, InlineResult_ptr res));
InlineResult_ptr rbc_inlining_cache_lookup_result ARGS((Rbc_t* f));

/**AutomaticEnd***************************************************************/

#endif /* _RBCINT */
