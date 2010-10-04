/**CFile***********************************************************************

  FileName    [rbcUtils.c]

  PackageName [rbc]

  Synopsis    [Some general functions working on RBCs]

  Description []

  SeeAlso     [rbc.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``rbc'' package of NuSMV version 2. 
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

  Revision    [$Id: rbcUtils.c,v 1.1.2.2 2007/03/15 13:47:31 nusmv Exp $]

******************************************************************************/

#include "rbc/rbcInt.h"

#include "dag/dag.h"
#include "utils/list.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/**Struct**********************************************************************
  Synopsis      [Data passing in inlining-DFS ]
  Description   [Data passing in inlining-DFS ]
  SeeAlso       []
******************************************************************************/
typedef struct DepDfsData_TAG {
  Rbc_Manager_t* mgr;
  lsList list;
} DepDfsData;


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

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
static int rbc_dep_set(Rbc_t* f, char* _data, nusmv_ptrint sign);
static void rbc_dep_first(Rbc_t* f, char* _data, nusmv_ptrint sign);
static void rbc_dep_back(Rbc_t* f, char* _data, nusmv_ptrint sign);
static void rbc_dep_last(Rbc_t* f, char* _data, nusmv_ptrint sign);


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Package initialization]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
void Rbc_pkg_init()
{
  rbc_inlining_cache_init();
}


/**Function********************************************************************

  Synopsis    [Package deinitialization]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
void Rbc_pkg_quit()
{
  rbc_inlining_cache_quit(); 
}



/**Function********************************************************************

  Synopsis    []

  Description []

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
lsList RbcUtils_get_dependencies(Rbc_Manager_t* rbcManager, Rbc_t* f, 
				 boolean reset_dag)
{
  Dag_DfsFunctions_t funcs;
  DepDfsData data;

  /* lazy evaluation */
  if ((f == rbcManager->one) || (f == rbcManager->zero)) return lsCreate();

  /* clears the user fields. */
  if (reset_dag) { Dag_Dfs(f, &dag_DfsClean, (char*) NULL); }
  
  /* sets up the DFS functions */
  funcs.Set        = rbc_dep_set;
  funcs.FirstVisit = rbc_dep_first;
  funcs.BackVisit  = rbc_dep_back;
  funcs.LastVisit  = rbc_dep_last;
 
  /* sets up the DFS data */
  data.mgr = rbcManager;
  data.list = lsCreate();

  /* Calling DFS on f. */
  Dag_Dfs(f, &funcs, (char*)(&data));

  /* processes result */
  return data.list;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    []

  Description []

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int rbc_dep_set(Rbc_t* f, char* _data, nusmv_ptrint sign)
{ return 0; }
static void rbc_dep_first(Rbc_t* f, char* _data, nusmv_ptrint sign)
{}
static void rbc_dep_back(Rbc_t* f, char* _data, nusmv_ptrint sign)
{}


/**Function********************************************************************

  Synopsis    []

  Description []

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static void rbc_dep_last(Rbc_t* f, char* _data, nusmv_ptrint sign)
{
  DepDfsData* data = (DepDfsData*) _data;

  if (f->symbol == RBCVAR) {
    lsNewBegin(data->list, (lsGeneric) f, LS_NH);
  }
}
