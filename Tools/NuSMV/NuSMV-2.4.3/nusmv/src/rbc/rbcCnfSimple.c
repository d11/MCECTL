/**CFile***********************************************************************

  FileName    [rbcCnf.c]

  PackageName [rbc]

  Synopsis    [Conjunctive Normal Form (CNF) conversions.]

  Description [External functions included in this module:
		<ul>
		<li> <b>Rbc_Convert2CnfSimple()</b>  
		</ul>]
		
  SeeAlso     []

  Author      [Armando Tacchella, Tommi Junttila and Marco Roveri ]

  Copyright   [
  This file is part of the ``rbc'' package of NuSMV version 2. 
  Copyright (C) 2000-2001 by University of Genova.
  Copyright (C) 2007 by ITC-irst.

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

#include <limits.h>

#include "rbc/rbcInt.h"
#include "utils/error.h" /* for internal_error */


static char rcsid[] UTIL_UNUSED = "$Id: rbcCnfSimple.c,v 1.1.4.3 2007/03/20 19:30:14 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************
  Synopsis      [Data passing in cnf-DFS.]
  Description   [Data passing in cnf-DFS.]
  SeeAlso       []
******************************************************************************/
struct CnfDfsData {
  Rbc_Manager_t* rbcManager;
  lsList clauses;
  lsList vars;
  int result;
};


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct CnfDfsData CnfDfsData_t;


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int CnfSet(Rbc_t * f, char * cnfData, nusmv_ptrint sign);
static void CnfFirst(Rbc_t * f, char * cnfData, nusmv_ptrint sign);
static void CnfBack(Rbc_t * f, char * cnfData, nusmv_ptrint sign);
static void CnfLast(Rbc_t * f, char * cnfData, nusmv_ptrint sign);
static lsGeneric SwapSign(lsGeneric data);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of external functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Translates the rbc into the corresponding (equisatisfiable)
               set of clauses.]

  Description [Given `rbcManager' and `f', `clauses' is filled with the
               disjunctions corresponding to the rbc nodes according to 
	       the rules:
	       
	       f = A & B => -f A              f = A <-> B =>  f  A  B
	                    -f B                              f -A -B
			     f -A -B                         -f -A  B
                                                             -f  A -B
	       
	       f = if A then B else C =>  f  A -C
	                                  f -A -B
					 -f  A  C
					 -f -A  B

	       `vars' is filled with the variables that occurred in `f' 
	       (original or model variables converted into corresponding CNF 
               variables). It is user's responsibility 
	       to create `clauses' and `vars' *before* calling the function.
               New variables are added by the conversion: the maximum
	       index is returned by the function.
               The literal associated to 'f' is assigned to parameter 
	       *literalAssignedToWholeFormula (it may be negative). 
	       Special case - A CONSTANT (this is consistent with description
                 of Be_Cnf_ptr): if the formula is a constant
                 then *literalAssignedToWholeFormula will be INT_MAX
		 and the return value will 0.
		 if formula is true, `clauses' is the empty list,
                 if formula is false, `clauses' contains a single empty clause.]
							     
  SideEffects [`clauses', `vars' and '*literalAssignedToWholeFormula'
              are filled up. Fields inside rbcManager might change ]

  SeeAlso     []

******************************************************************************/
int Rbc_Convert2CnfSimple(Rbc_Manager_t* rbcManager, Rbc_t* f, 
			  lsList clauses, lsList vars, 
			  int* literalAssignedToWholeFormula)
{
  Dag_DfsFunctions_t cnfFunctions;
  CnfDfsData_t       cnfData;

  /* The caller will ensure this */
  nusmv_assert(*literalAssignedToWholeFormula == INT_MAX);

   /* Setting up the DFS functions. */
  cnfFunctions.Set        = CnfSet;
  cnfFunctions.FirstVisit = CnfFirst;
  cnfFunctions.BackVisit  = CnfBack;
  cnfFunctions.LastVisit  = CnfLast;
 
  /* Setting up the DFS data. */
  cnfData.rbcManager = rbcManager; 
  cnfData.clauses    = clauses;
  cnfData.vars       = vars;
  cnfData.result     = 0;

  /* Calling DFS on f. */
  Dag_Dfs(f, &cnfFunctions, (char*)(&cnfData));

  /* Remember the unit clause (literal) that stands for the whole formula f */
  *literalAssignedToWholeFormula = cnfData.result;

  return rbcManager->maxCnfVariable;
  
} /* End of Rbc_Convert2CnfSimple. */


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Dfs Set for CNF conversion.]

  Description [Dfs Set for CNF conversion.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int CnfSet(Rbc_t* f, char* cnfData, nusmv_ptrint sign)
{
  CnfDfsData_t* cd = (CnfDfsData_t*)cnfData;

  /* Set the current integer reference as result. */
  cd->result = (sign != 0 ? -1 * (f->iRef) : f->iRef);

  /* All nodes should be visited once and only once. */
  return 0;
} /* End of CnfSet. */


/**Function********************************************************************

  Synopsis    [Dfs FirstVisit for CNF conversion.]

  Description [Dfs FirstVisit for CNF conversion.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void CnfFirst(Rbc_t* f, char* cnfData, nusmv_ptrint sign)
{
  if (f->symbol != RBCVAR) { 
    /* Create a temporary list (use vertex's own general reference). */
    f->gRef = (char*) lsCreate();
  }
} /* End of CnfFirst. */


/**Function********************************************************************

  Synopsis    [Dfs BackVisit for CNF conversion.]

  Description [Dfs BackVisit for CNF conversion.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void
CnfBack(Rbc_t* f, char* cnfData, nusmv_ptrint sign)
{
  CnfDfsData_t * cd   = (CnfDfsData_t*)cnfData;

  /* Get the current result and add it (negated) to the temporary list. */
  (void) lsNewEnd((lsList)(f->gRef), 
		  PTR_FROM_INT(lsGeneric, -1 * (cd->result)), LS_NH);

  return;

} /* End of CnfBack. */


/**Function********************************************************************

  Synopsis    [Dfs LastVisit for CNF conversion.]

  Description [Dfs LastVisit for CNF conversion.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void CnfLast(Rbc_t* f, char* cnfData, nusmv_ptrint sign)
{
  lsList    tmpCl;
  lsGen     gen;
  nusmv_ptrint s;
  int pol;

  CnfDfsData_t* cd = (CnfDfsData_t*) cnfData;
  lsList sons = (lsList) (f->gRef);

  int cnfVar = Rbc_get_node_cnf(cd->rbcManager, f, 
				&cd->rbcManager->maxCnfVariable);
  f->iRef = cnfVar;

  if (f->symbol == RBCVAR) {

    /* Fill in the list of 'original' model vars converted into CNF vars */
    (void) lsNewEnd(cd->vars, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);

  } 
  else {
    
    /* Generate and append clauses. */
    if (f->symbol == RBCAND) {

      /* Add the binary clauses {-f s_i} */
      gen = lsStart(sons);
      while (lsNext(gen, (lsGeneric*) &s, LS_NH) == LS_OK) {
	tmpCl = lsCreate();
	(void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * cnfVar), LS_NH);
	(void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * (int) s), LS_NH);
	(void) lsNewBegin(cd->clauses, (lsGeneric)tmpCl, LS_NH);
      }
      lsFinish(gen);
      
      /* Add the clause {f -s_1 -s_2} */
      (void) lsNewBegin(sons, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric)sons, LS_NH);

    } else if (f->symbol == RBCIFF) {

      /* Add the clause {-f s_1 -s_2} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * cnfVar), LS_NH);
      gen = lsStart(sons);
      pol = -1;
      while (lsNext(gen, (lsGeneric*) &s, LS_NH) == LS_OK) {
	(void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) s * pol), LS_NH);
	pol *= -1;
      }
      lsFinish(gen);
      (void) lsNewBegin(cd->clauses, (lsGeneric)tmpCl, LS_NH);

      /* Add the clause {-f -s_1 s_2} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * cnfVar), LS_NH);
      gen = lsStart(sons);
      pol = 1;
      while (lsNext(gen, (lsGeneric*) &s, LS_NH) == LS_OK) {
	(void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) s * pol), LS_NH);
	pol *= -1;
      }
      lsFinish(gen);
      (void) lsNewBegin(cd->clauses, (lsGeneric)tmpCl, LS_NH);

      /* Add the clause {f s_1 s_2} */
      tmpCl = lsCopy(sons, SwapSign);
      (void) lsNewBegin(tmpCl, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric)tmpCl, LS_NH);

      /* Add the clause {f -s_1 -s_2} */
      (void) lsNewBegin(sons, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric)sons, LS_NH);

    } else if (f->symbol == RBCITE) {
      nusmv_ptrint i, t, e;

      gen = lsStart(sons);

      /* Should have three children */
      if(lsNext(gen, (lsGeneric*) &i, LS_NH) != LS_OK) nusmv_assert(false);
      if(lsNext(gen, (lsGeneric*) &t, LS_NH) != LS_OK) nusmv_assert(false);
      if(lsNext(gen, (lsGeneric*) &e, LS_NH) != LS_OK) nusmv_assert(false);
      lsFinish(gen);
      
      /* Add the clause {-f -i t} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * cnfVar), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) i), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * (int) t), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric) tmpCl, LS_NH);

      /* Add the clause {-f i e} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * cnfVar), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * (int) i), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * (int) e), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric) tmpCl, LS_NH);

      /* Add the clause {f -i -t} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) i), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) t), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric) tmpCl, LS_NH);

      /* Add the clause {f i -e} */
      tmpCl = lsCreate();
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, cnfVar), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, -1 * (int) i), LS_NH);
      (void) lsNewEnd(tmpCl, PTR_FROM_INT(lsGeneric, (int) e), LS_NH);
      (void) lsNewBegin(cd->clauses, (lsGeneric) tmpCl, LS_NH);

      lsDestroy(sons, 0);
    }
    else
      internal_error("CnfLast: unkown RBC symbol");

  }
  
  /* Adjust the sign of the result. */
  cd->result = (sign != 0 ? -1 * (f->iRef) : f->iRef);

  return;

} /* End of CnfLast. */


/**Function********************************************************************

  Synopsis    [Swaps the sign of the argument.]

  Description [Swaps the sign of the argument.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static lsGeneric SwapSign(lsGeneric data)	    
{
  return PTR_FROM_INT(lsGeneric, -1 * PTR_TO_INT(data));
} /* End of CnfSwapSign. */
    
	       



