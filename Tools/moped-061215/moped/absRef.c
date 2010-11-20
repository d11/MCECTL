#include <time.h>

#include "alloc.h"

#include "absRef.h"
#include "absCache.h"
#include "absRefHelpers.c"
#include "abstract.c"

void printTraceShort (bddResult *res)
{
	bddConfig *conf;
	for (conf = res->prefix; conf; conf = conf->next_config)
	{
		printf("Trace: %s\n",wIdentString(conf->stack->stack));
	}
}

int lengthSymList (wIdentList *symbols)
{
	int i = 0;
	wIdentList *li;
	for (li = symbols; li; li = li->next) i++;
	return i;
}

/* check if wId is in the list symbols */
int in (wIdent wId, wIdentList *symbols)
{
	for ( ; symbols; symbols = symbols->next)
	{
		if (symbols->wId == wId) return 1;
	}
	return 0;
}

/* insert newWId into *symbols and return 1 iff it's not there yet */
int insert (wIdent newWId, wIdentList **symbols)
{
	if (in(newWId,*symbols)) return 0;
	wIdentList *wIdList;
	wIdList = *symbols;
	*symbols = Malloc(sizeof(wIdentList));
	(*symbols)->wId  = newWId;
	(*symbols)->next = wIdList;
	return 1;
}

void freeWIdentList (wIdentList *l)
{
	wIdentList *next;
	while(l)
	{
		next = l->next;
		Free(l);
		l = next;
	}
}

void initConstPointers ()
{
	pRULE_NORMAL  = Malloc(sizeof(ruleType)); *pRULE_NORMAL  = RULE_NORMAL;
	pRULE_PUSH    = Malloc(sizeof(ruleType)); *pRULE_PUSH    = RULE_PUSH;
	pRULE_POP     = Malloc(sizeof(ruleType)); *pRULE_POP     = RULE_POP;
	pRULE_COMBINE = Malloc(sizeof(ruleType)); *pRULE_COMBINE = RULE_COMBINE;
	pRULE_EVAL    = Malloc(sizeof(ruleType)); *pRULE_EVAL    = RULE_EVAL;
	pRULE_ENTRY   = Malloc(sizeof(ruleType)); *pRULE_ENTRY   = RULE_ENTRY;
}

void freeConstPointers ()
{
	Free(pRULE_NORMAL);
	Free(pRULE_PUSH); 
	Free(pRULE_POP);
	Free(pRULE_COMBINE);
	Free(pRULE_EVAL);
	Free(pRULE_ENTRY);
}

void initLocInf (int numSym)
{
	/* first initialize globInf */
	globInf = Malloc(sizeof(locationInfo));
	globInf->id = -1;
	globInf->type = LOC_GLOBAL;
	globInf->nPreds = 0;
	globInf->preds = NULL;
	Cudd_Ref(globInf->coll0 = one); 
	Cudd_Ref(globInf->coll1 = one);
	globInf->collFlag = 0;
	
	/* now locInf */
	maxLPreds = 0;
	NLocs = 0;
	locInf = Malloc(sizeof(locationInfo*) * numSym);
	int i;
	for (i=0; i<numSym; i++) locInf[i] = NULL;
	/* loop over the rules to figure out the different stack symbols */
	wRule *rule;
	wIdent loc;
	for (rule = concPds->pds->rules; rule; rule = rule->next)
		for (i=1; i<=3; i++)
		{
			if (i==1) loc = rule->from_stack;
			if (i==2) loc = rule->to_stack1;
			if (i==3) loc = rule->to_stack2;
			if (loc == 0) continue;
			assert (loc < numSym);
			if (locInf[loc] != NULL) continue;
			/* else */
			locInf[loc] = Malloc(sizeof(locationInfo));
			locInf[loc]->id       = NLocs;
			locInf[loc]->nPreds   = 0;
			locInf[loc]->preds    = NULL;
	                locInf[loc]->collFlag = 0;
	       Cudd_Ref(locInf[loc]->coll0    = one); 
	       Cudd_Ref(locInf[loc]->coll1    = one); 
	                if (in(loc,evalSymbols))
				locInf[loc]->type = LOC_CALL;
			else if (in(loc,entrySymbols))
				locInf[loc]->type = LOC_ENTRY;
			else
				locInf[loc]->type = LOC_NORMAL;
			NLocs++;
		}
	printf("NLocs == %d\n", NLocs);

	/* fill the array locs with all locations */
 	locs = Malloc(sizeof(wIdent) * NLocs);
 	for (rule = concPds->pds->rules; rule; rule = rule->next)
 		for (i=1; i<=3; i++)
 		{
 			if (i==1) loc = rule->from_stack;
 			if (i==2) loc = rule->to_stack1;
 			if (i==3) loc = rule->to_stack2;
 			if (loc == 0) continue;
 			/* else */
			assert (locInf[loc]->id < NLocs);
 			locs[locInf[loc]->id] = loc;
 		}
}

void freePredList (predicateList *pl)
{
	predicateList *next;
	while (pl)
	{
		next = pl->next;
		Cudd_RecursiveDeref(mgr,pl->J);
		Free(pl);
		pl = next;
	}
}	

void freeLocationInfo (locationInfo *li)
{
	if (li == NULL) return;
	freePredList(li->preds);
	if (li->coll0) {Cudd_RecursiveDeref(mgr,li->coll0); li->coll0 = NULL;}
	if (li->coll1) {Cudd_RecursiveDeref(mgr,li->coll1); li->coll1 = NULL;}
	Free(li);
}

void addPred (DdNode *pred, locationInfo *li, int local)
{
	assert(li != NULL);
	/* First, check if the predicate is already here */
	if (pred == NULL || pred == zero || pred == one) return;
	DdNode *not_pred;
	Cudd_Ref(not_pred = Cudd_Not(pred));
	predicateList *preds;
	if (VERBOSE) printf("addPred(): li->type == %d, ", li->type);
	for (preds = li->preds; preds; preds = preds->next)
	{
		if ((pred == preds->J) || (not_pred == preds->J))
		{
			Cudd_RecursiveDeref(mgr,not_pred);
			if (VERBOSE) printf("already here.\n");
			return;
		}
	}
	Cudd_RecursiveDeref(mgr,not_pred);
	/* Add it ... */
	(li->nPreds)++;
	if (VERBOSE) printf("li->nPreds == %d\n",li->nPreds);
	/* Cudd_PrintMinterm(mgr,pred); */
	if (local && (li->nPreds > maxLPreds)) maxLPreds = li->nPreds;
	predicateList *pl = Malloc(sizeof(predicateList));
	Cudd_Ref(pl->J = pred);
	pl->next = li->preds;
	pl->id = li->nPreds - 1;
	/* ... at the front */
	li->preds = pl;
}
		
int lengthPds ()
{
	int i = 0;
	wRule *rule;
	for (rule = concPds->pds->rules; rule; rule = rule->next) i++;
	return i;
}

void addFrameRules ()
/* add procedure entry symbols and stack symbols for an evaluation step after
   returning from a procedure call */
{
	entrySymbols = evalSymbols = NULL;
	wRule *rule;
	for (rule = concPds->pds->rules; rule; rule = rule->next)
	{
		if (rule->to_stack2 == 0)
		{
			if (rule->to_stack1 == 0)
			{
				rule->user = pRULE_POP;
			}
			else
			{
				rule->user = pRULE_NORMAL;
			}
			continue;
		}
		assert(rule->to_stack1 != 0);
		rule->user = pRULE_PUSH;

		char *newEntryString = strdup(">");
		append(&newEntryString, wIdentString(rule->to_stack1));
		wIdent newEntryWId = wIdentCreate(newEntryString);
		free(newEntryString);
		wIdent old_to_stack1 = rule->to_stack1;
		rule->to_stack1 = newEntryWId;

		char *newEvalString = strdup(wIdentString(rule->to_state));
		append(&newEvalString, "|");
		append(&newEvalString, wIdentString(newEntryWId));
		append(&newEvalString, "|");
		append(&newEvalString, wIdentString(rule->to_stack2));
		wIdent newEvalWId = wIdentCreate(newEvalString);
		free(newEvalString);
		wIdent old_to_stack2 = rule->to_stack2;
		rule->to_stack2 = newEvalWId;

		int isEntryNew = insert(newEntryWId, &entrySymbols);
		if (isEntryNew)
		{
			if (VERBOSE) printf("entry symbol created: %s\n",
					    wIdentString(newEntryWId));
			wRule *nr = bddPdsInsert(concPds,
						 rule->to_state,newEntryWId,
						 rule->to_state,old_to_stack1,0,
			       helpers->eqv_G0G1_L0L1);/* the bdd doesn't matter */
			nr->user = pRULE_ENTRY;
			if (VERBOSE) printf("lengthPds: %d\n",lengthPds());
		}

		int isEvalNew = insert(newEvalWId, &evalSymbols);
		if (isEvalNew)
		{ 
			if (VERBOSE) printf("eval symbol created: %s\n",
					    wIdentString(newEvalWId));
			wRule *nr = bddPdsInsert(concPds,
						 rule->to_state,newEvalWId,
						 rule->to_state,old_to_stack2,0,
			       helpers->eqv_G0G1_L0L1);
			nr->user = pRULE_EVAL;
			if (VERBOSE) printf("lengthPds: %d\n",lengthPds());
		}
	}
}

void computeConcHeadTable ()
{
	concHeadTable = wHTCreate(NULL);
	wHead *head;
	wRule *rule;
	wHeadEdge *edge;
	for (rule = concPds->pds->rules; rule; rule = rule->next)
	{
		assert (rule != NULL);
		head = wHTInsert(concHeadTable,rule->from_state,rule->from_stack);
		edge = Calloc(1,sizeof(struct wHeadEdge));
		edge->origin = rule; /* origin is just a field with a rule */
		edge->postnext = head->post;
		head->post = edge;
	}
}

void deleteHead (void *vh)
/* cf. wHeadDelete in heads.c */
{
	wHead *head = vh;
	while (head->post)
	{
		wHeadEdge *e = head->post->postnext;
		Free(head->post);
		head->post = e;
	}
	Free(head);
}

void freeHeadTable (wHeadTable *headTable)
/* cf. wHTDelete in heads.c */
{
	wHashDelete(headTable->hash,deleteHead);
	Free(headTable);
}

void freeGlobalMemory (int numSym)
{
	freeLocationInfo(globInf);          globInf = NULL;
	int i;
	for (i=0; i<numSym; i++)
		if (locInf[i]) freeLocationInfo(locInf[i]);
	Free(locInf);
	freeHelpers(helpers);               helpers = NULL;
	freeWIdentList(evalSymbols);    evalSymbols = NULL;
	freeWIdentList(entrySymbols);  entrySymbols = NULL;
	Free(locs);
	freeConstPointers();
	freeHeadTable(concHeadTable); concHeadTable = NULL;
}

#include "absAnalyse.c"

/* state,stack is the configuration to be reachability-checked
 */
bddResult* absRef (bddPds *concretePds, wIdent state, wIdent stack)
{
	/* return bddPostReachability(concretePds,state,stack); */
	clock_t startTime, endTime;
	double elapsedTime;
	startTime = clock();

	printf("absRef Start\n");

	concPds = concretePds;
	mgr     = concPds->mgr;
        one     = concPds->one;
        zero    = concPds->zero;
        helpers = Malloc(sizeof(absRefHelpers));
        computeConcHelpers(helpers);
	initAbsHelpers(helpers);
	initConstPointers();

	printf("wident_globals_used: %i\n", (int) wident_globals_used);
	printf("lengthPds: %d\n",lengthPds());
	addFrameRules();
	printf("lengthSymList(evalSymbols) = %d\n", lengthSymList(evalSymbols));
	printf("lengthSymList(entrySymbols) = %d\n", lengthSymList(entrySymbols));
	int numSym = (int) wident_globals_used;
	printf("numSym: %i\n", numSym);

	initLocInf(numSym);
	computeConcHeadTable();
	andAbsHashCreate();

	bddPds *absPds = NULL;
	bddPds *old_absPds;
	bddResult *absRes;
	analyseResult anRes;

	int i = 0;
	do
	{ /* the CEGAR loop */
		printf("-----------------------------------------------");
		printf(" absRef(): Cycle %d ----\n",i++);
		old_absPds = absPds;
		absPds = abstract(old_absPds);
		assert (absPds != NULL);

		if (old_absPds)
		{
 			bddPdsDelete(old_absPds);
		}
		
		anRes = absAnalyse(absPds,state,stack);

	} while (anRes == ABS_RES_SPURIOUS_CE);
	
	if (anRes == ABS_RES_REAL_CE)
	{
		absRes = bddPostReachability(absPds,state,stack);
		assert (absRes->yesno == RESULT_YES);
		printf("absRef(): Printing abstract trace:\n");
		printf("          Number of locals: %d\n", absPds->num_locals);
		printTraceShort(absRes);
	}
	else
	{
		assert (anRes == ABS_RES_NO_ERROR);
		absRes = Malloc(sizeof(struct bddResult));
		absRes->yesno  = RESULT_NO;
		absRes->prefix = NULL;
		absRes->loop   = NULL;
	}
	
	andAbsHashDelete();
	freeGlobalMemory(numSym);
	bddPdsDelete(absPds);

	endTime = clock();
	elapsedTime = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;	
	printf("-----------------------------------------------------\n");
	printf("CPU time used for model checking: %f sec.\n", elapsedTime);
	printf("-----------------------------------------------------\n");
	
	return absRes;
}
