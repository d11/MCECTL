/****************************************************************************/

#include "bdd.h"

/***************************************************************************/
/* Auxiliary functions for bddCreatePds and bddDeletePds.		   */

extern void bddInitCube (DdManager *mgr, int **cube)
{
	*cube = Malloc(Cudd_ReadSize(mgr) * sizeof(int));
	memset(*cube,0x2,Cudd_ReadSize(mgr) * sizeof(int));
}

extern void bddAddVarsToCube (DdManager *mgr, int *cube,
				DdNode **nodes, int num_vars)
{
	while (num_vars--)
		cube[nodes[num_vars]->index] = 1;
}

extern DdNode* bddCreateCube (DdManager *mgr, int *cube)
{
	DdNode *result;
	Cudd_Ref(result = Cudd_CubeArrayToBdd(mgr,cube));
	Free(cube);
	return result;
}

/***************************************************************************/

extern void bddInitPermutation (DdManager *mgr, int **perm)
{
	int size = Cudd_ReadSize(mgr);
	*perm = Malloc(size * sizeof(int));
	while (size--)
		(*perm)[size] = size;
}

extern void bddAddToPermutation (DdManager *mgr, int *perm,
				DdNode **x, DdNode **y, int num_vars)
{
	while (num_vars--)
	{
		perm[(*x)->index] = (*y)->index;
		x++; y++;
	}
}

/***************************************************************************/

static void bddDefaultShuffle (bddPds *pds, void *data)
{
	DdManager *mgr = pds->mgr;
	int globals = pds->num_globals;
	int locals = pds->num_locals;
	int i;

	int index = Cudd_ReadSize(mgr) + globals * 3 + locals * 4;

	for (i = pds->num_globals-1; i >= 0; i--)
	{
		pds->g2[i] = Cudd_bddIthVar(mgr,--index);
		pds->g1[i] = Cudd_bddIthVar(mgr,--index);
		pds->g0[i] = Cudd_bddIthVar(mgr,--index);
	}

	for (i = pds->num_locals-1; i >= 0; i--)
	{
		pds->l3[i] = Cudd_bddIthVar(mgr,--index);
		pds->l1[i] = Cudd_bddIthVar(mgr,--index);
		pds->l2[i] = Cudd_bddIthVar(mgr,--index);
		pds->l0[i] = Cudd_bddIthVar(mgr,--index);
	}
}

/****************************************************************************/

extern void bddGenerateData (bddPds *pds, bddShuffleProc shuffle, void *data)
{
	DdManager *mgr = pds->mgr;
	DdNode *tmp0, *tmp1, *bdd;
	int globals = pds->num_globals;
	int locals = pds->num_locals;
	int i, *cube, *perm;

	/* Generate global and local BDD variables */
	pds->g0 = Malloc(globals * sizeof(DdNode*));
	pds->g1 = Malloc(globals * sizeof(DdNode*));
	pds->g2 = Malloc(globals * sizeof(DdNode*));
	pds->l0 = Malloc(locals * sizeof(DdNode*));
	pds->l1 = Malloc(locals * sizeof(DdNode*));
	pds->l2 = Malloc(locals * sizeof(DdNode*));
	pds->l3 = Malloc(locals * sizeof(DdNode*));
	(shuffle? shuffle : bddDefaultShuffle)(pds,data);

	Cudd_Ref(pds->one = Cudd_ReadOne(mgr));
	Cudd_Ref(pds->zero = Cudd_Not(pds->one));

	pds->helpers = Malloc(sizeof(struct bddHelpers));
	pds->helpers->array_size = Cudd_ReadSize(mgr);

	/* Generate cubes */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g1,globals);
	bddAddVarsToCube(mgr,cube,pds->l1,locals);
	pds->helpers->cube_g1l1 = bddCreateCube(mgr,cube);

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g0,globals);
	bddAddVarsToCube(mgr,cube,pds->l0,locals);
	pds->helpers->cube_g0l0 = bddCreateCube(mgr,cube);

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->l0,locals);
	bddAddVarsToCube(mgr,cube,pds->g2,globals);
	bddAddVarsToCube(mgr,cube,pds->l3,locals);
	pds->helpers->cube_l0g2l3 = bddCreateCube(mgr,cube);

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->l2,locals);
	pds->helpers->cube_l2 = bddCreateCube(mgr,cube);

	/* Generate the permutations */
	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g0,pds->g1,globals);
	bddAddToPermutation(mgr,perm,pds->l0,pds->l1,locals);
	pds->helpers->perm_g0l0_g1l1 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g2,pds->g1,globals);
	bddAddToPermutation(mgr,perm,pds->l2,pds->l1,locals);
	pds->helpers->perm_g2l2_g1l1 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g0,pds->g1,globals);
	bddAddToPermutation(mgr,perm,pds->l0,pds->l1,locals);
	bddAddToPermutation(mgr,perm,pds->g1,pds->g2,globals);
	pds->helpers->perm_g0l0g1_g1l1g2 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g1,pds->g0,globals);
	bddAddToPermutation(mgr,perm,pds->l1,pds->l0,locals);
	pds->helpers->perm_g1l1_g0l0 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g1,pds->g0,globals);
	bddAddToPermutation(mgr,perm,pds->g2,pds->g1,globals);
	bddAddToPermutation(mgr,perm,pds->l3,pds->l1,locals);
	pds->helpers->perm_g1g2l3_g0g1l1 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->l2,pds->l0,locals);
	pds->helpers->perm_l2_l0 = perm;

	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->l0,pds->l2,locals);
	pds->helpers->perm_l0_l2 = perm;

	/* Generate the BDD for eqv_g0g2_l0l3 */
	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,pds->g0,pds->g2,globals);
	bddAddToPermutation(mgr,perm,pds->l0,pds->l3,locals);

	i = Cudd_ReadSize(mgr); Cudd_Ref(bdd = pds->one);
	while (i--)
	{
		if (perm[i] == i) continue;
		Cudd_Ref(tmp0 = Cudd_bddXnor(mgr,Cudd_bddIthVar(mgr,i),
						 Cudd_bddIthVar(mgr,perm[i])));
		Cudd_Ref(tmp1 = Cudd_bddAnd(mgr,bdd,tmp0));
		Cudd_RecursiveDeref(mgr,bdd);
		Cudd_RecursiveDeref(mgr,tmp0);
		bdd = tmp1;
	}
	pds->helpers->eqv_g0g2_l0l3 = bdd;
	Free(perm);

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g1,globals);
	bddAddVarsToCube(mgr,cube,pds->l1,locals);
	bddAddVarsToCube(mgr,cube,pds->g2,globals);
	pds->helpers->select_pretrans = cube;

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g0,globals);
	bddAddVarsToCube(mgr,cube,pds->l0,locals);
	bddAddVarsToCube(mgr,cube,pds->g2,globals);
	bddAddVarsToCube(mgr,cube,pds->l3,locals);
	pds->helpers->select_post_ini = cube;

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g1,globals);
	bddAddVarsToCube(mgr,cube,pds->l1,locals);
	bddAddVarsToCube(mgr,cube,pds->l0,locals);
	bddAddVarsToCube(mgr,cube,pds->g2,globals);
	bddAddVarsToCube(mgr,cube,pds->l3,locals);
	pds->helpers->select_post_non = cube;

	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,pds->g2,globals);
	bddAddVarsToCube(mgr,cube,pds->l3,locals);
	pds->helpers->select_state = cube;
}

/****************************************************************************/

extern void bddDeleteData (bddPds *pds)
{
	DdManager *mgr = pds->mgr;
	bddHelpers *hlp = pds->helpers;

	Cudd_RecursiveDeref(mgr,hlp->cube_g1l1);
	Cudd_RecursiveDeref(mgr,hlp->cube_g0l0);
	Cudd_RecursiveDeref(mgr,hlp->cube_l0g2l3);
	Cudd_RecursiveDeref(mgr,hlp->cube_l2);
	Cudd_RecursiveDeref(mgr,hlp->eqv_g0g2_l0l3);

	Free(hlp->perm_g0l0_g1l1);	Free(hlp->perm_g2l2_g1l1);
	Free(hlp->perm_g0l0g1_g1l1g2);	Free(hlp->perm_g1g2l3_g0g1l1);
	Free(hlp->perm_g1l1_g0l0);	Free(hlp->perm_l2_l0);
	Free(hlp->perm_l0_l2);

	Free(hlp->select_pretrans);	Free(hlp->select_post_ini);
	Free(hlp->select_post_non);	Free(hlp->select_state);

	Free(hlp);

	Free(pds->g0); Free(pds->g1); Free(pds->g2);
	Free(pds->l0); Free(pds->l1); Free(pds->l2); Free(pds->l3);

	Cudd_RecursiveDeref(pds->mgr,pds->one);
	Cudd_RecursiveDeref(pds->mgr,pds->zero);
}

/****************************************************************************/
