/****************************************************************************/
/* BDD-related functions.						    */

#include "bdd.h"
#include "crc32.h"

/***************************************************************************/

static DdManager *bddManager;

void bddInit ()
{
	bddManager = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
	bddDomInit();
	crcInit();
}

void bddFinish ()
{
	bddDomFinish();
	Cudd_Quit(bddManager);
}

extern void bddDebug (int *debug, int *keys, int *nonzero)
{
	int tmp1, tmp2, tmp3;

	/* modification Stefan K. */
	/* printf("Cudd_printInfo():\n");
	   Cudd_PrintInfo(bddManager,stdout);
	*/
	printf("PeakNodeCount: %ld\n",Cudd_ReadPeakNodeCount(bddManager));
	/* end modification */

	tmp1 = Cudd_DebugCheck(bddManager);
	tmp2 = Cudd_CheckKeys(bddManager);
	tmp3 = Cudd_CheckZeroRef(bddManager);

	if (debug) *debug = tmp1;
	if (keys) *keys = tmp2;
	if (nonzero) *nonzero = tmp3;
}

/***************************************************************************/

bddPds* bddPdsCreate (wConfig *start, int num_globals, int num_locals,
			bddShuffleProc shuffle, void *data)
{
	bddPds *pds = Malloc(sizeof(struct bddPds));

	pds->pds = wPDSCreate(&bddSemiring);
	pds->start = wConfigCopy(start);
	pds->mgr = bddManager;
	pds->num_globals = num_globals;
	pds->num_locals = num_locals;
	bddGenerateData(pds,shuffle,data);

	return pds;
}

wRule* bddPdsInsert (bddPds *pds, wIdent p0, wIdent y0,
			wIdent p1, wIdent y1, wIdent y2, DdNode *bdd)
{
	wRule *rule;
	bddDomain *dom;
	int sig = y1? y2? SIG_RULE2 : SIG_RULE1 : SIG_RULE0;

	Cudd_Ref(bdd);		/* must be in right encoding */

	dom = bddDomGenerate(sig,pds,bdd);
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
