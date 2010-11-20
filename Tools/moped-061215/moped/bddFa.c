/***************************************************************************/

#include "bdd.h"

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
	bddDomain *dom;

	int sig = (fa->dir == SAT_PRE)? SIG_PRETRANS :
				p? wIsInitial(p)? SIG_POST_INI : SIG_POST_NON
				 : SIG_EPSILON ;
	Cudd_Ref(bdd);

	dom = bddDomGenerate(sig,pds,bdd);
	trans = wFAInsert(fa,p,y,q,dom,NULL);
	bddDomDeref(dom);

	return trans;
}

extern void bddFaDelete (wFA *fa)
{
	wFADelete(fa);
}

extern wFA* bddInitialAutomaton (bddPds *pds)
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

extern wIdent bddFinalState ()
{
	return wIdentCreate("__s__");
}

extern wFA* bddTargetAutomaton (bddPds *pds, wIdent state, wIdent stack)
{
	wFA *result = bddFaCreate(pds,SAT_PRE);
	wIdent final_state = bddFinalState();
	wRule *pt;

	bddFaInsert(result,state,stack,final_state,pds->one);

	/* Add a loop around the final state; for simplicity, we only
	   add loops for "return addresses" of push rules. */
	for (pt = pds->pds->rules; pt; pt = pt->next)
		if (pt->to_stack2)
			bddFaInsert(result,final_state,pt->to_stack2,
				final_state,pds->one);

	return result;
}

/* Added by Remy */
extern wFA* bddDpnAutomaton (bddPds *pds, wIdent state, wIdent stack)
{
	wFA *result = bddFaCreate(pds,SAT_PRE);
	wIdent q0 = wIdentCreate("q0");		// initial state
	wIdent q1 = wIdentCreate("q1");		// initial state
	wIdent q2 = wIdentCreate("_q2_");
	wIdent q3 = wIdentCreate("q3__");	// initial and final state
	wRule *pt;
	
	bddFaInsert(result,q0,state,q1,pds->one);	// q0 -state-> q1
	bddFaInsert(result,q1,stack,q3,pds->one);	// q1 -stack-> q3
	
	bddFaInsert(result,q3,state,q3,pds->one);	// q3 -state-> q3
	for (pt = pds->pds->rules; pt; pt = pt->next) {
		
		if (!pt->to_stack1)
			continue;
		
		// q3 -Gamma-> q3
		if (pt->to_stack1 != state)
			bddFaInsert(result,q3,pt->to_stack1,q3,pds->one);
		if (pt->to_stack2 && (pt->to_stack2 != state))
			bddFaInsert(result,q3,pt->to_stack2,q3,pds->one);
		
		// q1 -Gamma\{stack}-> q2
		if ((pt->to_stack1 != stack) && (pt->to_stack1 != state))
			bddFaInsert(result,q1,pt->to_stack1,q2,pds->one);
		if (pt->to_stack2 && (pt->to_stack2 != stack) && (pt->to_stack2 != state))
			bddFaInsert(result,q1,pt->to_stack2,q2,pds->one);
		
		// q2 -Gamma-> q2
		if (pt->to_stack1 != state)
			bddFaInsert(result,q2,pt->to_stack1,q2,pds->one);
		if (pt->to_stack2 && (pt->to_stack2 != state))
			bddFaInsert(result,q2,pt->to_stack2,q2,pds->one);
	}
	bddFaInsert(result,q2,state,q1,pds->one);	// q2 -state-> q1
	
	return result;
}

/***************************************************************************/
