/****************************************************************************/
/* BDD-related functions.						    */

#include "bdd.h"
#include "crc32.h"

/***************************************************************************/
/* The semiring operations:						   */

static wHash *bddDomHash;

static int bddDomCompare (void *va, void *vb)
{
        bddDomain *a = va, *b = vb;

	#define bddHashCompare(field) \
		if (a->field != b->field) return (a->field < b->field)? -1 : 1;
	bddHashCompare(signature);
	bddHashCompare(bdd);
	bddHashCompare(pds);
	return 0;
}

static void bddDomDelete (wSRvalue vv)
{
	bddDomain *v = vv;
	if (v->bdd) Cudd_RecursiveDeref(v->pds->mgr,v->bdd);
	Free(v);
}

extern void bddDomRef (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;
	v->refcount++;
}

extern void bddDomDeref (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;

	if (!v) return;
	if (--(v->refcount)) return;

	wHashRemove(bddDomHash,v);
	bddDomDelete(v);
}

extern bddDomain* bddDomGenerate(int sig, bddPds *pds, DdNode *bdd)
{
	/* Generate a domain element, computes its hash value, and check
	   if the value is already in the hash table; if so, return the
	   saved element, otherwise return the new element. In both cases,
	   add a reference to the returned element. */
	bddDomain *v, *nv = Malloc(sizeof(struct bddDomain));
	
	nv->signature = sig;
	nv->pds = pds;
	nv->bdd = bdd;	/* assumes that bdd has been properly referenced! */

	nv->hash = crcCompute((char*)&(nv->signature),
			sizeof(int) + sizeof(bddPds*) + sizeof(DdNode*));
	nv->next = NULL;
	nv->refcount = 1;

	v = wHashInsert(bddDomHash,nv);
	if (v == nv) return v;

	bddDomDelete(nv);
	bddDomRef(v);
	return v;
}

/***************************************************************************/

static wSRvalue bddExtend (wSRvalue vv1, wSRvalue vv2)
{
	bddDomain *v1 = (bddDomain*) vv1;
	bddDomain *v2 = (bddDomain*) vv2;

	bddPds    *pds;
	DdNode    *bdd, *tmp0 = NULL;
	DdManager *mgr;
	int       sig;

	if (v1->signature == SIG_DONT_CARE) { bddDomRef(v1); return v1; }
	if (v2->signature == SIG_DONT_CARE) { bddDomRef(v2); return v2; }

	pds = v2->pds;
	mgr = pds->mgr;

	if (v1->signature == SIG_RULE1 && v2->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0_g1l1));
		sig = SIG_PRETRANS;
	}

	else if (v1->signature == SIG_RULE2 && v2->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g2l2_g1l1));
		sig = SIG_RULE1;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE0)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1g2l3_g0g1l1));
		sig = SIG_EPSILON;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE1)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
		sig = SIG_POST_INI;
	}

	else if (v1->signature == SIG_POST_INI && v2->signature == SIG_RULE2)
	{
		Cudd_Ref(tmp0 = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_l2_l0));
		sig = SIG_POST_NON;
	}

	else if (v1->signature == SIG_POST_NON && v2->signature == SIG_EPSILON)
	{
		Cudd_Ref(bdd = Cudd_bddAndAbstract(mgr, v1->bdd, v2->bdd,
					pds->helpers->cube_g1l1));
		sig = SIG_POST_INI;
	}

	else
	{
		fprintf(stderr,"Bad BDD operation: wrong signatures in join\n");
		exit(1);
	}

	if (tmp0) Cudd_RecursiveDeref(mgr,tmp0);
	return bddDomGenerate(sig,pds,bdd);
}

static wSRvalue bddCombine (wSRvalue vv1, wSRvalue vv2)
{
	bddDomain *v1 = (bddDomain*) vv1;
	bddDomain *v2 = (bddDomain*) vv2;
	DdNode *bdd;

	if (v1->signature != v2->signature)
	{
		fprintf(stderr,"Bad BDD operation: incompatible signatures "
				"in union\n");
		exit(1);
	}

	Cudd_Ref(bdd = Cudd_bddOr(v1->pds->mgr,v1->bdd,v2->bdd));
	return bddDomGenerate(v1->signature,v1->pds,bdd);
}

static wSRvalue bddQone (wSRvalue vv)
{
	bddDomain *v = (bddDomain*) vv;
	bddPds    *pds = v->pds;
	DdNode    *tmp0, *tmp1, *bdd;

	if (v->signature != SIG_POST_NON)
	{
		fprintf(stderr,"Bad BDD operation: wrong signature "
				"in quasione\n");
		exit(1);
	}

	Cudd_Ref(tmp0 = Cudd_bddExistAbstract(pds->mgr,
				v->bdd, pds->helpers->cube_l0g2l3));
	Cudd_Ref(tmp1 = Cudd_bddPermute(pds->mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
	Cudd_RecursiveDeref(pds->mgr,tmp0);
	Cudd_Ref(bdd = Cudd_bddAnd(pds->mgr, tmp1,
					pds->helpers->eqv_g0g2_l0l3));
	Cudd_RecursiveDeref(pds->mgr,tmp1);

	return bddDomGenerate(SIG_POST_INI,pds,bdd);
}

static wSRvalue bddDontCare ()
{
	return bddDomGenerate(SIG_DONT_CARE,NULL,NULL);
}

static int bddEq (wSRvalue vv1, wSRvalue vv2)
{
	return vv1 == vv2;
}

/***************************************************************************/

/* Some additional functions of general interest: */

extern int bddEmpty (bddDomain *v)
{
	return (v->bdd == v->pds->zero);
}

extern bddDomain* bddUnion (bddDomain *v1, bddDomain *v2)
{
	DdNode *bdd;

	if (v1->signature != v2->signature)
	{
		fprintf(stderr,"Bad BDD operation: incompatible signatures "
				"in union\n");
		exit(1);
	}

	Cudd_Ref(bdd = Cudd_bddOr(v1->pds->mgr,v1->bdd,v2->bdd));
	return bddDomGenerate(v1->signature,v1->pds,bdd);
}

extern bddDomain* bddIntersect (bddDomain *v1, bddDomain *v2)
{
	DdNode *bdd;

	if (v1->signature != v2->signature)
	{
		fprintf(stderr,"Bad BDD operation: incompatible signatures "
				"in intersection\n");
		exit(1);
	}

	Cudd_Ref(bdd = Cudd_bddAnd(v1->pds->mgr,v1->bdd,v2->bdd));
	return bddDomGenerate(v1->signature,v1->pds,bdd);
}

/***************************************************************************/

/* A function to convert the format of a pop rule for a pre* computation.  */

extern bddDomain* bddDomConvert (bddDomain *v)
{
	DdNode *bdd;

	if (v->signature != SIG_RULE0)
	{
		fprintf(stderr,"Bad BDD operation: wrong signature "
				"in bddDomConvert\n");
		exit(1);
	}

	Cudd_Ref(bdd = Cudd_bddPermute(v->pds->mgr,v->bdd,
			v->pds->helpers->perm_g0l0g1_g1l1g2));
	
	return bddDomGenerate(SIG_PRETRANS,v->pds,bdd);
}

/***************************************************************************/

wSemiring bddSemiring
	= { bddExtend,
	    bddCombine,
	    NULL,	/* diff    */
	    bddDontCare,/* one; undefined (probably not needed) */
	    NULL,	/* zero; undefined (probably not needed) */
	    bddQone,
	    bddEq,
	    bddDomRef,
	    bddDomDeref
	  };

extern void bddDomInit ()
{
	bddDomHash = wHashCreate(bddDomCompare);
}

extern void bddDomFinish ()
{
	wHashDelete(bddDomHash,bddDomDelete);
}
