/****************************************************************************/

#include "bdd.h"
#include "absRef.h"
#include "dpn.h"

/****************************************************************************/

extern int ABSTRACTION;
extern int NEWTRACES;
/* from main.c */

/****************************************************************************/

static wFA* bddComputePrestar (bddPds *pds, wFA *fa, wTrans **found)
{
        wSat *prcs = wSatCreate(pds->pds->sr,SAT_PRE,
			NEWTRACES? TRACE_REF : TRACE_COPY);
	wIdent state = pds->start->state;
	wIdent stack = *(pds->start->stack);
	wIdent finalstate = bddFinalState();
        wRule *pt;
	wTrans *t;

	/* Add all the rules */
        for (pt = pds->pds->rules; pt; pt = pt->next)
	{
		if (pt->to_stack1)
		{
			wSatAugRule(prcs,pt->from_state,pt->from_stack,
				pt->to_state,pt->to_stack1,pt->to_stack2,
				pt->label,pt);
			continue;
		}

		/* pop rules need a different signature */
		bddDomain *d = bddDomConvert(pt->label);
		wSatAugRule(prcs,pt->from_state,pt->from_stack,
			pt->to_state,pt->to_stack1,pt->to_stack2,d,pt);
		bddDomDeref(d);
	}

	/* Add all initial transitions (to the queue) */
        for (t = fa->transitions; t; t = t->next)
                 wSatAddTrans(prcs,t->from,t->name,t->to,t->label);

	/* Process all the transitions */
        while ((t = wSatPopTrans(prcs)))
	{
		/* found target with non-empty BDD? */
		if (t->from == state && t->name == stack &&
			t->to == finalstate && !bddEmpty(t->label))
			break;

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	*found = t;
        return wSatDelete(prcs);
}

static bddResult* bddPreReachability (bddPds *pds, wIdent state, wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));

        wTrans *t;
	wFA *fa = bddTargetAutomaton(pds,state,stack);
	wFA *fa_pre = bddComputePrestar(pds,fa,&t);

	/* pre* is complete; now compute the result and an error trace. */

	result->yesno = t? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;

	if (t) bddGetPreTrace(pds,fa_pre,t,result);

	bddFaDelete(fa);
	bddFaDelete(fa_pre);
	
	return result;
}

/****************************************************************************/


/* This version may substitute the functionality of bddComputePostar,
   see below. It is useful for the version with abstraction and
   performs a complete Poststar search. */
static wFA* bddComputePoststarCompl (bddPds *pds, wFA *fa,
				     wIdent state, wIdent stack, wTrans **found)
{
	/* Note: I don't evaluate NEWTRACES here because I didn't implement
	   TRACE_REF for wsUpdateSpecial. - Stefan S. */
        wSat *prcs = wSatCreate(pds->pds->sr,SAT_POST,TRACE_YES);
 	prcs->special = 1;
	wRule *pt;
	wTrans *t;

	/* Add all the rules */
        for (pt = pds->pds->rules; pt; pt = pt->next)
                wSatAugRule(prcs,pt->from_state,pt->from_stack,
                        pt->to_state,pt->to_stack1,pt->to_stack2,pt->label,pt);

	/* Add all initial transitions (to the queue) */
        for (t = fa->transitions; t; t = t->next)
                 wSatAddTrans(prcs,t->from,t->name,t->to,t->label);

	*found = NULL; /* this is new */
	/* Process all the transitions */
        while ((t = wSatPopTrans(prcs)))
                /* for a breadth-first search do something like that:
		   do {
		     while (t=pop()) list.add(t);
		     if (list.empty()) break;
		     t  = list.remove();
		     aug(t);
		   } while(1);
		*/
	{
 		/* found target with non-empty BDD? */
 		if (t->from == state && t->name == stack && !bddEmpty(t->label))
			/* old: break; */
			if (*found == NULL) *found = t;
		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                if (!bddEmpty(t->label))
			wSatAugTrans(prcs,t);
	}

	/* old : *found = t; */
	/* bddPrintwNodeGraph(prcs); */
        return wSatDelete(prcs);
}


wFA* bddComputePoststar (bddPds *pds, wFA *fa,
				wIdent state, wIdent stack, wTrans **found)
{
	if (ABSTRACTION) 
		return bddComputePoststarCompl(pds,fa,state,stack,found);

        wSat *prcs = wSatCreate(pds->pds->sr,SAT_POST,
			NEWTRACES? TRACE_REF : TRACE_COPY);
	prcs->special = 0;
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
		if (t->from == state && t->name == stack && !bddEmpty(t->label))
			break;

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	*found = t;
        return wSatDelete(prcs);
}

bddResult* bddPostReachability (bddPds *pds, wIdent state, wIdent stack)
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

/***************************************************************************/

bddResult* bddAnalyse (bddPds *pds, bddProblem *problem)
{
	if (problem->type == PROBLEM_REACHABILITY) {
		if (ABSTRACTION)
			return absRef(pds,problem->state,problem->stack);
		else
			return bddPostReachability(pds,problem->state,problem->stack);
	}
	else if (problem->type == PROBLEM_REACHABILITY_PRE)
		return bddPreReachability(pds,problem->state,problem->stack);
	else if (problem->type == PROBLEM_REACHABILITY_DPN)
		return dpnPreReachability(pds,problem->state,problem->stack);
	else if (problem->type == PROBLEM_REACHABILITY_ADPN)
		return adpnPreReachability(pds,problem->state,problem->stack);
	else if (problem->type == PROBLEM_LTL)
		fprintf(stderr,"not implemented yet\n");
	else
		fprintf(stderr,"unknown problem\n");
	return NULL;
}

/***************************************************************************/

static void bddPrintCube (int *cube, DdNode **vars, int num_vars, char *mask)
{
	int i;

	for (i = 0; i < num_vars; i++)
	{
		if (cube[vars[i]->index] == 2) continue;
		printf("\t"); printf(mask,i);
		printf("=%d",cube[vars[i]->index]);
		if (i%9 == 8) printf("\n");
	}
}

static void bddPrintConfig (bddPds *pds, bddConfig *config)
{
	bddPath *path;
	int *cube;
	int i = 0;

	printf("<%s,",wIdentString(config->state));
	for (path = config->stack; path; path = path->rest_path)
		printf(" %s",wIdentString(path->stack));
	printf(">\n");

	printf("Global variables:\n");
	bddInitCube(pds->mgr,&cube);
	Cudd_BddToCubeArray(pds->mgr,config->bdd,cube);
	bddPrintCube(cube,pds->g0,pds->num_globals,"g%d");
	Free(cube);
	printf("\n");

	for (path = config->stack; path; path = path->rest_path)
	{
		printf("Local variables at stack position %d:\n",++i);
		bddInitCube(pds->mgr,&cube);
		Cudd_BddToCubeArray(pds->mgr,path->bdd,cube);
		bddPrintCube(cube,pds->l0,pds->num_locals,"l%d");
		Free(cube);
		printf("\n");
	}
}

static void bddPrintVars (bddPds *pds, DdNode *bdd, int *i)
{
	int *cube;

	printf("Local variables at stack position %d:\n",++(*i));
	bddInitCube(pds->mgr,&cube);
	Cudd_BddToCubeArray(pds->mgr,bdd,cube);
	bddPrintCube(cube,pds->l0,pds->num_locals,"l%d");
	Free(cube);
	printf("\n");
}

static void bddPrintReverse (bddPds *pds, bddPath *path, int *i)
{
	if (!path) return;
	bddPrintReverse(pds,path->rest_path,i);
	if (i)
		bddPrintVars(pds,path->bdd,i);
	else
		printf("%s ",wIdentString(path->stack));
}

static void bddPrintForward (bddPds *pds, bddPath *path, int *i)
{
	while (path)
	{
		if (i)
			bddPrintVars(pds,path->bdd,i);
		else
			printf("%s ",wIdentString(path->stack));
		path = path->rest_path;
	}
}


void bddPrintConfigDPN (bddPds *pds, bddConfig *config)
{
	int i = 0;

	bddPrintReverse(pds,config->prefix,NULL);
	bddPrintForward(pds,config->stack,NULL);
	printf("\n");

	bddPrintReverse(pds,config->prefix,&i);
	bddPrintForward(pds,config->prefix,&i);
}

extern void bddPrintResult (bddPds *pds, bddResult *result)
{
	bddConfig *config;

	if (result->yesno == RESULT_NO)
	{
		printf("Not reachable.\n");
		return;
	}

	printf("Reachable. Trace:\n");
	printf("\n-------------------\n\n");
	/*return;*/

	for (config = result->prefix; config; config = config->next_config)
	{
		bddPrintConfig(pds,config);
		printf("\n-------------------\n\n");
		if (config->rule)
		{
			wRule *r = config->rule;
			printf("rule used: <%s,%s> -> <%s",
				wIdentString(r->from_state),
				wIdentString(r->from_stack),
				wIdentString(r->to_state));
			if (r->to_stack1)
				printf(",%s",wIdentString(r->to_stack1));
			if (r->to_stack2)
				printf(" %s",wIdentString(r->to_stack2));
			printf(">\n\n-------------------\n\n");
		}
	}
}

/***************************************************************************/

static void bddDeletePath (bddPds *pds, bddPath *path)
{
	while (path)
	{
		bddPath *next = path->rest_path;
		if (--path->refcount) break;
		Cudd_RecursiveDeref(pds->mgr,path->bdd);
		Free(path);
		path = next;
	}
}

static void bddDeleteConfig (bddPds *pds, bddConfig *config)
{
	while (config)
	{
		bddConfig *next = config->next_config;
		Cudd_RecursiveDeref(pds->mgr,config->bdd);
		bddDeletePath(pds,config->stack);
		Free(config);
		config = next;
	}
}

extern void bddDeleteResult (bddPds *pds, bddResult *result)
{
	bddDeleteConfig(pds,result->prefix);
	bddDeleteConfig(pds,result->loop);
	Free(result);
}
