/****************************************************************************/
/* BDD-related functions.						    */

#include <wpds.h>
#include <wpdsInt.h>
#include "alloc.h"
#include "bdd.h"
#include "crc32.h"

/***************************************************************************/
/* The semiring operations:						   */

static wHash *bddDomHash;

static int bddDomCompare (void *va, void *vb)
{
        bddDomain *a = va, *b = vb;

	#define bddHashCompare(field) \
		if (a->field != b->field) return (a < b)? -1 : 1;
	bddHashCompare(signature);
	bddHashCompare(bdd);
	bddHashCompare(pds);
	bddHashCompare(savedValue);
	return 0;
}

static void bddDomRef (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;
	v->refcount++;
}
	
static void bddDomDelete (wSRvalue vv)
{
	bddDomain *v = vv;
	Cudd_RecursiveDeref(v->pds->mgr,v->bdd);
	Free(v);
}

static void bddDomDeref (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;

	if (!v) return;
	if (--(v->refcount)) return;

	wHashRemove(bddDomHash,v);
	bddDomDeref(v->savedValue);
	bddDomDelete(v);
}

/* Compute the hash value, see if the value is already in the 
   table; if so, delete v and return the saved element, otherwise
   return v. In both cases, add a reference to v. */
static bddDomain* bddAddToHash (bddDomain *nv)
{
	bddDomain *v;

	nv->next = NULL;
	nv->hash = crcCompute((char*)&(nv->signature), sizeof(struct bddDomain)
			- sizeof(struct bddDomain*) - sizeof(unsigned long)
			- sizeof(int));
	nv->refcount = 1;

	v = wHashInsert(bddDomHash,nv);
	if (v == nv) return v;

	bddDomDelete(nv);
	bddDomRef(v);
	return v;
}

static wSRvalue bddJoin (wSRvalue vv1, wSRvalue vv2)
{
	bddDomain *v1 = (bddDomain*) vv1;
	bddDomain *v2 = (bddDomain*) vv2;
	bddDomain *v = Malloc(sizeof(struct bddDomain));
	bddPds    *pds = v1->pds;
	DdNode    *tmp0 = NULL;
	DdManager *mgr = pds->mgr;

	v->savedValue = NULL;
	v->pds = v1->pds;

	if (v1->signature == SIG_RULE1 && v2->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(v->bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0_g1l1));
		v->signature = SIG_PRETRANS;
	}

	else if (v1->signature == SIG_RULE2 && v2->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(v->bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g2l2_g1l1));
		v->signature = SIG_RULE1;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE0)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(v->bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1g2l3_g0g1l1));
		v->signature = SIG_EPSILON;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE1)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(v->bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
		v->signature = SIG_POST_INI;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE2)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(v->bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_l2_l0));
		v->signature = SIG_POST_NON;
	}

	else if (v1->signature == SIG_POST_NON && v2->signature == SIG_EPSILON)
	{
		Cudd_Ref(v->bdd = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		v->signature = SIG_POST_INI;
	}

	else
	{
		fprintf(stderr,"Bad BDD operation: wrong signatures in join\n");
		exit(1);
	}

	if (tmp0) Cudd_RecursiveDeref(mgr,tmp0);
	return bddAddToHash(v);
}

static wSRvalue bddUnion (wSRvalue vv1, wSRvalue vv2)
{
	bddDomain *v1 = (bddDomain*) vv1;
	bddDomain *v2 = (bddDomain*) vv2;
	bddDomain *v = Malloc(sizeof(struct bddDomain));

	if (v1->signature != v2->signature)
	{
		fprintf(stderr,"Bad BDD operation: incompatible signatures "
				"in union\n");
		exit(1);
	}

	v->signature = v1->signature;
	v->pds = v1->pds;
	Cudd_Ref(v->bdd = Cudd_bddOr(v1->pds->mgr,v1->bdd,v2->bdd));
	v->savedValue = NULL;
	return bddAddToHash(v);
}

static wSRvalue bddQone (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;
	bddDomain *nv = Malloc(sizeof(struct bddDomain));
	bddPds    *pds = nv->pds = v->pds;
	DdNode    *tmp0, *tmp1;

	if (v->signature != SIG_POST_NON)
	{
		fprintf(stderr,"Bad BDD operation: wrong signature "
				"in quasione\n");
		exit(1);
	}

	nv->signature = SIG_POST_INI;
	Cudd_Ref(tmp0 = Cudd_bddExistAbstract(pds->mgr,
				v->bdd, pds->helpers->cube_l0g2l3));
	Cudd_Ref(tmp1 = Cudd_bddPermute(pds->mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
	Cudd_RecursiveDeref(pds->mgr,tmp0);
	Cudd_Ref(nv->bdd = Cudd_bddAnd(pds->mgr, tmp1,
					pds->helpers->eqv_g0g2_l0l3));
	Cudd_RecursiveDeref(pds->mgr,tmp1);
	nv->savedValue = NULL;

	return bddAddToHash(nv);
}

static int bddEq (wSRvalue vv1, wSRvalue vv2)
{
	return vv1 == vv2;
}

static wSemiring bddSemiring
	= { bddJoin,	/* extend  */
	    bddUnion,	/* combine */
	    NULL,	/* diff    */
	    NULL,	/* one; undefined (probably not needed) */
	    NULL,	/* zero; undefined (probably not needed) */
	    bddQone,
	    bddEq,
	    bddDomRef,
	    bddDomDeref
	  };


/***************************************************************************/
/* Auxiliary functions for bddCreatePds and bddDeletePds.		   */

static void bddInitCube (DdManager *mgr, int **cube)
{
	*cube = Malloc(Cudd_ReadSize(mgr) * sizeof(int));
	memset(*cube,0x2,Cudd_ReadSize(mgr) * sizeof(int));
}

static void bddAddVarsToCube (DdManager *mgr, int *cube,
				DdNode **nodes, int num_vars)
{
	while (num_vars--)
		cube[nodes[num_vars]->index] = 1;
}

static DdNode* bddCreateCube (DdManager *mgr, int *cube)
{
	DdNode *result;
	Cudd_Ref(result = Cudd_CubeArrayToBdd(mgr,cube));
	Free(cube);
	return result;
}

static void bddInitPermutation (DdManager *mgr, int **perm)
{
	int size = Cudd_ReadSize(mgr);
	*perm = Malloc(size * sizeof(int));
	while (size--)
		(*perm)[size] = size;
}

static void bddAddToPermutation (DdManager *mgr, int *perm,
				DdNode **x, DdNode **y, int num_vars)
{
	while (num_vars--)
	{
		perm[(*x)->index] = (*y)->index;
		x++; y++;
	}
}

static void bddDefaultShuffle (int index, bddPds *pds, void *data)
{
	DdManager *mgr = pds->mgr;
	int globals = pds->num_globals;
	int locals = pds->num_locals;
	int i;

	index += Cudd_ReadSize(mgr) + globals * 3 + locals * 4;

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

static void bddGenerateData (bddPds *pds, void *data, bddShuffleProc shuffle)
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
	(shuffle? shuffle : bddDefaultShuffle)(Cudd_ReadSize(mgr),pds,data);

	Cudd_Ref(pds->one = Cudd_ReadOne(mgr));
	Cudd_Ref(pds->zero = Cudd_Not(pds->one));

	pds->helpers = Malloc(sizeof(struct bddHelpers));

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
}

static void bddDeleteData (bddPds *pds)
{
	DdManager *mgr = pds->mgr;
	bddHelpers *hlp = pds->helpers;

	Cudd_RecursiveDeref(mgr,hlp->cube_g1l1);
	Cudd_RecursiveDeref(mgr,hlp->cube_g0l0);
	Cudd_RecursiveDeref(mgr,hlp->cube_l0g2l3);
	Cudd_RecursiveDeref(mgr,hlp->eqv_g0g2_l0l3);

	Free(hlp->perm_g0l0_g1l1);	Free(hlp->perm_g2l2_g1l1);
	Free(hlp->perm_g0l0g1_g1l1g2);	Free(hlp->perm_g1g2l3_g0g1l1);
	Free(hlp->perm_g1l1_g0l0);	Free(hlp->perm_l2_l0);

	Free(hlp);

	Free(pds->g0); Free(pds->g1); Free(pds->g2);
	Free(pds->l0); Free(pds->l1); Free(pds->l2); Free(pds->l3);

	Cudd_RecursiveDeref(pds->mgr,pds->one);
	Cudd_RecursiveDeref(pds->mgr,pds->zero);
}

/***************************************************************************/
/* Functions visible to the outside:					   */

static DdManager *bddManager;

void bddInit ()
{
	bddManager = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
	bddDomHash = wHashCreate(bddDomCompare);
	crcInit();
}

void bddFinish ()
{
	wHashDelete(bddDomHash,bddDomDelete);
	Cudd_Quit(bddManager);
}

/***************************************************************************/

bddPds* bddPdsCreate (int num_globals, int num_locals,
			bddShuffleProc shuffle, void *data)
{
	bddPds *pds = Malloc(sizeof(struct bddPds));

	pds->pds = wPDSCreate(&bddSemiring);
	pds->start = NULL;
	pds->mgr = bddManager;
	pds->num_globals = num_globals;
	pds->num_locals = num_locals;
	bddGenerateData(pds,data,shuffle);

	return pds;
}

wRule* bddPdsInsert (bddPds *pds, wIdent p0, wIdent y0,
			wIdent p1, wIdent y1, wIdent y2, DdNode *bdd)
{
	wRule *rule;
	bddDomain *dom = Malloc(sizeof(struct bddDomain));

	dom->signature = y1? y2? SIG_RULE2 : SIG_RULE1 : SIG_RULE0;
	dom->pds = pds;
	Cudd_Ref(dom->bdd = bdd);	/* must be in right encoding */
	dom->savedValue = NULL;

	dom = bddAddToHash(dom);
	rule = wPDSInsert(pds->pds,p0,y0,p1,y1,y2,dom);
	bddDomDeref(dom);

	return rule;
}

/* Will not delete the BDD variables for this PDS from the manager! */
void bddPdsDelete (bddPds *pds)
{
	if (pds->start) wConfigDelete(pds->start);
	wPDSDelete(pds->pds);
	bddDeleteData(pds);
	Free(pds);
}

/***************************************************************************/

static wFA* bddFaCreate (bddPds *pds, char pre_post)
{
	wFA *fa = wFACreate(&bddSemiring);
	fa->dir = pre_post;
	fa->user = pds;
	return fa;
}

static wTrans* bddFaInsert (wFA *fa, wIdent p, wIdent y, wIdent q, DdNode *bdd)
{
	wTrans *trans;
	bddPds *pds = (bddPds*) fa->user;
	bddDomain *dom = Malloc(sizeof(struct bddDomain));

	dom->signature = (fa->dir == SAT_PRE)? SIG_PRETRANS :
				p? wIsInitial(p)? SIG_POST_INI : SIG_POST_NON
				 : SIG_EPSILON ;
	dom->pds = pds;
	Cudd_Ref(dom->bdd = bdd);	/* must be in right encoding */
	dom->savedValue = NULL;

	dom = bddAddToHash(dom);
	trans = wFAInsert(fa,p,y,q,dom,NULL);
	bddDomDeref(dom);

	return trans;
}

static void bddFaDelete (wFA *fa)
{
	wFADelete(fa);
}

/***************************************************************************/

wFA* bddPrestar (bddPds *pds, wFA *fa)
{
	wRule *pt;
	wFA *result;

	/* This is a hack that renames the variables in BDDs for
	   epsilon rules. We need to do this because the WPDS lib
	   will create transition labels from these values. */
	for (pt = pds->pds->rules; pt; pt = pt->next)
	{
		bddDomain *dom, *saved = (bddDomain*)(pt->label);

		if (pt->to_stack1) continue;
		dom = Malloc(sizeof(struct bddDomain));
		dom->pds = pds;
		dom->signature = SIG_PRETRANS;
		Cudd_Ref(dom->bdd = Cudd_bddPermute(pds->mgr, saved->bdd,
				pds->helpers->perm_g0l0g1_g1l1g2));
		dom->refcount = 1;
		bddDomRef(dom->savedValue = saved);
		pt->label = bddAddToHash(dom);
	}

	result = wPrestar(pds->pds,fa,TRACE_NO);

	/* Having computed the result, we rename the variables back. */
	for (pt = pds->pds->rules; pt; pt = pt->next)
	{
		bddDomain *dom = (bddDomain*)(pt->label),
			  *saved = dom->savedValue;

		if (pt->to_stack1) continue;

		bddDomDeref(dom);
		pt->label = saved;
	}

	return result;
}

/***************************************************************************/

static wFA* bddInitialAutomaton (bddPds *pds)
{
	wFA *result = bddFaCreate(pds,SAT_POST);
	wIdent *stack = pds->start->stack;
	wIdent start, target = pds->start->state;
	char name[16];
	int count = 0;

	while (*stack)
	{
		sprintf(name,"%ss%d%s",stack[1]? "_" : "__",
				++count,stack[1]? "_" : "__");
		start = target;
		target = wIdentCreate(name);
		bddFaInsert(result,start,*stack,target,pds->one);
		stack++;
	}

	return result;
}

static wFA* bddComputePoststar (bddPds *pds, wFA *fa,
				wIdent state, wIdent stack, wTrans **found)
{
        wSat *prcs = wSatCreate(pds->pds->sr,SAT_POST,TRACE_YES);
        wRule *pt;
	wTrans *t;

	/* Add all the rules */
        for (pt = pds->pds->rules; pt; pt = pt->next)
                wSatAugRule(prcs,pt->from_state,pt->from_stack,
                        pt->to_state,pt->to_stack1,pt->to_stack2,pt->label,pt);

	/* Add all initial transitions (to the queue) */
        for (t = fa->transitions; t; t = t->next)
                 wSatAddTrans(prcs,t->from,t->name,t->to,t->label);

	/* Process all the transitions */
        while ((t = wSatPopTrans(prcs)))
	{
		/* found target with non-empty BDD? */
		if (t->from == state && t->name == stack
				&& ((bddDomain*) t->label)->bdd != pds->zero)
			break;

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	*found = t;
        return wSatDelete(prcs);
}

static bddPath* bddCreateTransition (wTrans *t, bddDomain *values)
{
	bddPath *path = Calloc(1,sizeof(struct bddPath));

	path->trans = t;
	path->values = values;
	path->refcount = 1;

	return path;
}

static wTrans* bddFindNextTransition (wFA *fa, bddTrans *path, wIdent state)
{
	/* Find a transition starting with state such that the target
	   does not yet appear in the path. Note: This is fairly
	   inefficient. We should exploit the head table of the sat
	   process for this (but we have deleted the table at this
	   point... sigh). Fix this at a later stage. */
	wTrans *tr;
	bddResult *p;

	for (tr = fa->transitions; tr; tr = tr->next)
	{
		if (t->from != state) continue;
		if (path->trans->from == t->to) continue;
		for (p = path; p; p = p->rest_path)
			if (p->trans->to == t->to) break;
		if (!p) return tr;
	}
}

static void bddGetPostTrace (bddPds *pds, wFA *fa,
				wTrans *start, bddResult *result)
{
	bddPath *current;
	wTrans *t = start;

	/* pick a loop-free path to an accepting state */
	--> bddSelect(start->label),
	current = bddCreateTransition(start,...);

	while (!wIsFinal(t->to))
	{
		t = bddFindNextTransition(fa,t->to);
		
	}
				
	
}

bddResult* bddPostReachability (bddPds* pds, wIdent state, wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));

        wTrans *t;
	wFA *fa = bddInitialAutomaton(pds);
	wFA *fa_post = bddComputePoststar(pds,fa,state,stack,&t);

	/* post* is complete; now compute the result and an error trace. */

	result->yesno = t? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;

	if (t) bddGetPostTrace(pds,fa_post,t,result);

	bddFaDelete(fa);
	bddFaDelete(fa_post);
	
	return result;
}
