/****************************************************************************/

#include "bdd.h"
#include "absRef.h"
#include "dpn.h"

/****************************************************************************/

extern int BOUND;

typedef struct FaQueue {
	int i;
	wFA *fa;
	struct FaQueue *next;
} FaQueue;

/****************************************************************************/

void printFA(wFA *fa) {
	
	wTrans *trans = fa->transitions;
	for (; trans; trans = trans->next) {
		printf("%s -%s-> %s\n", wIdentString(trans->from),
			wIdentString(trans->name), wIdentString(trans->to));
	}
}

void printTrans(wTrans *t) {
	
	printf("%s -%s-> %s", wIdentString(t->from),
			wIdentString(t->name), wIdentString(t->to));
}

FaQueue* faQueueCreate(int i, wFA *fa, FaQueue *next) {
	
	FaQueue *q = Malloc(sizeof(struct FaQueue));
	q->i = i;
	q->fa = fa;
	q->next = next;
	
	return q;
}

void faQueueInsert(FaQueue **queue, int i, wFA *fa) {
	
	FaQueue *q = faQueueCreate(i, fa, *queue);
	*queue = q;
}

FaQueue* faQueueRemove(FaQueue **queue) {
	
	if (!(*queue))
		return NULL;
	
	FaQueue *q = *queue;
	if (!q->next) {
		FaQueue *tmp = *queue;
		*queue = NULL;
		return tmp;
	}
	
	for (q = *queue; q->next->next; q = q->next)
		;
	FaQueue *tmp = q->next;
	q->next = NULL;
	return tmp;
}

void printWRule (DdManager *manager, wRule *rule) {
	
	printf("%s <%s> --> %s <", wIdentString(rule->from_state), 
			wIdentString(rule->from_stack), wIdentString(rule->to_state));
	if (rule->to_stack1) printf("%s", wIdentString(rule->to_stack1));
	if (rule->to_stack2) printf(" %s", wIdentString(rule->to_stack2));
	printf(">   ");
	Cudd_PrintMinterm(manager, ((bddDomain *) rule->label)->bdd);
}

static int bddCompare (void *va, void *vb)
{
        bddDomain *a = va, *b = vb;

	#define bddHashCompare(field) \
		if (a->field != b->field) return (a < b)? -1 : 1;
	bddHashCompare(signature);
	bddHashCompare(bdd);
	bddHashCompare(pds);
	return 0;
}

void printNewTrans(DdManager *manager, wFA *old, wFA *new) {
	
	wTrans *t, *t_old;
	for (t = new->transitions; t; t = t->next) {
		t_old = wFAFind(old, t->from, t->name, t->to);
		if (t_old == NULL) {
			printTrans(t); printf("   ");
			Cudd_PrintMinterm(manager, ((bddDomain *) t->label)->bdd);
			printf("\n");
		}
		if ((t_old != NULL) && bddCompare(t->label, t_old->label)) {
			printTrans(t); printf("   ");
			Cudd_PrintMinterm(manager, ((bddDomain *) t->label)->bdd);
			printf("\n");
		}
			
	}
}

int isNewTransAdded(wFA *old, wFA *new) {
	
	wTrans *t, *t_old;
	for (t = new->transitions; t; t = t->next) {
		t_old = wFAFind(old, t->from, t->name, t->to);
		if (t_old == NULL)
			return 1;
		if ((t_old != NULL) && bddCompare(t->label, t_old->label))
			return 1;
	}
	
	return 0;
}

/****************************************************************************/

static wFA* dpnComputePrestar (bddPds *pds, wFA *fa, wTrans **found1, wTrans **found2)
{
	wSat *prcs = wSatCreate(pds->pds->sr,SAT_PRE,TRACE_YES);
	wIdent from1, name1, to1;
	wIdent from2, name2, to2;
	
	from1 = wIdentCreate("q0");
	name1 = wIdentCreate("q");
	to1 = wIdentCreate("q1");
	from2 = to1;
	name2 = wIdentCreate("main");
	to2 = wIdentCreate("q3__");
	
    wRule *pt;
	wTrans *t;
	wTrans *t1 = NULL;
	wTrans *t2 = NULL;

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
		if (t->from == from1 && t->name == name1 && t->to==to1 && !bddEmpty(t->label)) {
			if ((t2 = wFAFind(prcs->fa,from2,name2,to2)) != NULL) {
				break;
			}
		}
		if (t->from == from2 && t->name == name2 && t->to==to2 && !bddEmpty(t->label)) {
			if ((t1 = wFAFind(prcs->fa,from1,name1,to1)) != NULL) {
				break;
			}
		}

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	*found1 = (t1) ? t1 : t;
	*found2 = (t2) ? t2 : t;
        return wSatDelete(prcs);
}

bddResult* dpnPreReachability (bddPds *pds, wIdent state, wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));
	
	wTrans *t1;
	wTrans *t2;
	wFA *fa = bddDpnAutomaton(pds,state,stack);
	printf("*** BEFORE ***\n");
	printFA(fa);
	printf("\n");
	wFA *fa_pre = dpnComputePrestar(pds,fa,&t1,&t2);
	printf("*** AFTER ***\n");
	printFA(fa_pre);
	
	result->yesno = (t1 && t2) ? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;
	
	/*wTrans *trans = fa_pre->transitions;
	wIdent q1 = wIdentCreate("q1");
	wIdent q3 = wIdentCreate("q3__");
	wIdent new_q1 = wIdentCreate("_q1_");
	wIdent new_q3 = wIdentCreate("_q3__");
	for (; trans; trans = trans->next) {
		if (trans->from == q1) 	trans->from = new_q1;
		if (trans->to == q1)	trans->to = new_q1;
		if (trans->from == q3) 	trans->from = new_q3;
		if (trans->to == q3)	trans->to = new_q3;
	}*/
	
	//printf("t = %s -%s-> %s\n", wIdentString(t->from), wIdentString(t->name), 
	//	wIdentString(t->to));
	if (t1 && t2) bddGetDpnTrace(pds,fa_pre,t1,t2,result);

	bddFaDelete(fa);
	bddFaDelete(fa_pre);
	
	return result;
}

/****************************************************************************/

static wFA* adpnComputePrestar (bddPds *pds, wRule *rule, wFA *fa, 
		wTrans *goal1, wTrans *goal2, wTrans **found1, wTrans **found2)
{
	wSat *prcs = wSatCreate(pds->pds->sr,SAT_PRE,TRACE_YES);
	wRule *pt;
	wTrans *t;
	wTrans *t1 = NULL;
	wTrans *t2 = NULL;
	
	/* Add all the rules */
	for (pt = pds->pds->rules; pt; pt = pt->next) {
		if (pt->shared)
			continue;
		
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
	if (rule) {
		wSatAugRule(prcs,rule->from_state,rule->from_stack,
			rule->to_state,rule->to_stack1,rule->to_stack2,
			rule->label,rule);
	}
	
	/* Add all initial transitions (to the queue) */
    for (t = fa->transitions; t; t = t->next)
		wSatAddTrans(prcs,t->from,t->name,t->to,t->label);
	
	/* Process all the transitions */
	printf("*** Processing pre* ***\n");
	while ((t = wSatPopTrans(prcs)))
	{
		printTrans(t); printf("\t");
		Cudd_PrintMinterm(pds->mgr, ((bddDomain*) t->label)->bdd);
		/* found target with non-empty BDD? */
		if (t->from == goal1->from && t->name == goal1->name 
				&& t->to == goal1->to && !bddEmpty(t->label)) {
			if (((t2 = wFAFind(prcs->fa,goal2->from,goal2->name,goal2->to)) != NULL)
					&& !bddEmpty(t2->label)) {
				printf("t2 founded: ");
				printTrans(t2);
				Cudd_PrintMinterm(pds->mgr, ((bddDomain*) t2->label)->bdd);
				break;
			}
		}
		if ((t->from == goal2->from) && (t->name == goal2->name) 
				&& (t->to == goal2->to) && !bddEmpty(t->label)) {
			if (((t1 = wFAFind(prcs->fa,goal1->from,goal1->name,goal1->to)) != NULL)
					&& !bddEmpty(t1->label)) {
				printf("t1 founded: ");
				printTrans(t1);
				Cudd_PrintMinterm(pds->mgr, ((bddDomain*) t1->label)->bdd);
				break;
			}
		}

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
		wSatAugTrans(prcs,t);
	}
	printf("*** End of pre* ***\n");

	*found1 = (t1) ? t1 : t;
	*found2 = (t2) ? t2 : t;
	return wSatDelete(prcs);
}

// To be fixed.
bddResult* adpnPreReachability (bddPds *pds, wIdent state, 
										wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));
	
	int *ruleIndex = wPDSSharedRule(pds->pds);
	int i = 0;
	printf("*** SHARED RULES ***\n");
	while (ruleIndex[i]) {
		printWRule(pds->mgr, (wRule*) ruleIndex[i]);
		i++;
	}
	printf("********************\n");
	
	wTrans *goal1 = Malloc(sizeof(struct wTrans));
	wTrans *goal2 = Malloc(sizeof(struct wTrans));
	goal1->from = wIdentCreate("q0");
	goal1->name = wIdentCreate("q");
	goal1->to = wIdentCreate("q1");
	goal2->from = goal1->to;
	goal2->name = wIdentCreate("main");
	goal2->to = wIdentCreate("q3__");
	
	wTrans *t1;
	wTrans *t2;
	wFA *fa = bddDpnAutomaton(pds,state,stack);
	printf("*** BEFORE ***\n"); printFA(fa); printf("\n\n");
	wFA *fa_pre = adpnComputePrestar(pds,NULL,fa,goal1,goal2,&t1,&t2);
	printf("*** FIRST ADDED ***\n"); printNewTrans(pds->mgr, fa, fa_pre); printf("\n\n");
	
	FaQueue *fa_queue = faQueueCreate(0, fa_pre, NULL);
	FaQueue *fa_work;
	while ((fa_work = faQueueRemove(&fa_queue))) {
		if (fa_work->i == BOUND)
			continue;
		
		i = 0;
		while (ruleIndex[i]) {
			printf("********** i=%d **********\n", fa_work->i); 
			printf("Rule: "); printWRule(pds->mgr, (wRule*) ruleIndex[i]);
			fa_pre = adpnComputePrestar(pds, (wRule*) ruleIndex[i],
				fa_work->fa, goal1, goal2, &t1, &t2);
			//printf("*** New transitions ***\n");
			//printNewTrans(pds->mgr, fa_work->fa, fa_pre); printf("\n");
			//printf("*** End of new transitions ***\n");
			//if (isNewTransAdded(fa_work->fa, fa_pre)) {
				faQueueInsert(&fa_queue, fa_work->i + 1, fa_pre);
				printf("new FA added to queue\n\n");
			//}
			if (t1 && t2) {
				break;
			}
			i++;
		}
		if (t1 && t2) break;
	}
	
	result->yesno = (t1 && t2) ? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;
	
	if (t1 && t2) bddGetDpnTrace(pds,fa_pre,t1,t2,result);

	Free(goal1); Free(goal2);
	bddFaDelete(fa);
	bddFaDelete(fa_pre);
	
	return result;
}

