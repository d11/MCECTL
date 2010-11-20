#include "two.h"
#include "bdd.h"

/* Control verbosity of preAnalyse */
int twoVerbose = 0;

/* Constructor for callNodeList */
callNodeList *createCallNodeList(callNode *node, callNodeList *next) {

	callNodeList *list = Malloc(sizeof(struct callNodeList));
	list->node = node;
	list->next = next;

	return list;
}

/* Destructor for callNodeList */
void freeCallNodeList(callNodeList *list) {

	if (!list) return;

	if (list->next) freeCallNodeList(list->next);
	Free(list);

	return;
}

/* Constructor for callNode */
callNode *createCallNode(wIdent name)  {

	callNode *node = Malloc(sizeof(struct callNode));
	node->name = name;
	node->callTo = NULL;
	node->calledBy = NULL;

	return node;
}

/* Destructor for callNode */
void freeCallNode(callNode *node) {

	if (!node) return;

	if (node->callTo) freeCallNodeList(node->callTo);
	if (node->calledBy) freeCallNodeList(node->calledBy);
	Free(node);

	return;
}

/* Destructor for callNodeList which is actually a graph */
void freeCallGraph(callNodeList *list) {

	if (!list) return;

	if (list->next) freeCallNodeList(list->next);
	if (list->node) freeCallNode(list->node);
	Free(list);

	return;
}

/* Find the callNode by name from the graph */
callNode *findCallNode(callGraph *graph, wIdent name) {

	for (; graph; graph = graph->next) {

		if (graph->node->name == name)
			return graph->node;
	}

	printf("Something went wrong. "
		"preAnalyse.findCallNode returned NULL.");
	return NULL;
}

/* Find the module name, which contains label */
wIdent findModuleName(intModule *module, wIdent label) {

	for (; module; module = module->next) {

		if (wSetFind(module->labels, label))
			return module->name;
	}

	printf("Something went wrong. "
		"preAnalyse.findModuleName returned 0.");
	return 0;
}

/* Return 1 if the list contains node, 0 otherwise */
int existCallNode(callNodeList *list, callNode *node) {

	for (; list; list = list->next) {

		if (list->node == node)
			return 1;
	}

	return 0;
}

/* Insert node into the list (if the list doesn't contain the node) */
void insertCallNodeList(callNodeList **list, callNode *node) {

	if (existCallNode(*list, node)) return;

	*list = createCallNodeList(node, *list);
}

/* Construct call graph from ipds */
callGraph *constructCallGraph(intPds *ipds) {

	callNode *node;
	callGraph *graph = NULL;

	/* Create a graph of call nodes. Each node has module->name */
	intModule *module = ipds->modules;
	for (; module; module = module->next) {

		if (!module->name) continue;

		node = createCallNode(module->name);
		graph = createCallNodeList(node, graph);
	}

	/* Look at each push rule, create relation between nodes. */
	wRule *rule = ipds->pds->pds->rules;
	wIdent callerName;
	callNode *callerNode;
	callNode *calledNode;
	for (; rule; rule = rule->next) {

		if (rule->to_stack2 == 0) continue;

		callerName = findModuleName(ipds->modules, rule->from_stack);
		callerNode = findCallNode(graph, callerName);
		calledNode = findCallNode(graph, rule->to_stack1);

		insertCallNodeList(&callerNode->callTo, calledNode);
		insertCallNodeList(&calledNode->calledBy, callerNode);
	}

	return graph;
}

/* Print the call graph */
void printCallGraph(callGraph *graph) {

	callNodeList *list;

	printf("Call graph:\n");
	for (; graph; graph = graph->next) {

		for (list = graph->node->callTo; list; list = list->next)
			printf("%s -> %s\n", 
				wIdentString(graph->node->name), 
				wIdentString(list->node->name));

		for (list = graph->node->calledBy; list; list = list->next)
			printf("%s <- %s\n", 
				wIdentString(graph->node->name), 
				wIdentString(list->node->name));
	}
	printf("\n");
}

/*****************************************************************************/

/* Return a copy of config */
wConfig *copyConfig(wConfig *config) {

	int stackLength = sizeof(config->stack);
	wIdent *stack = Calloc(sizeof(wIdent), stackLength);
	int i;
	for (i = 0; i < stackLength; i++)
		stack[i] = config->stack[i];

	wConfig *c = Malloc(sizeof(struct wConfig));
	c->state = config->state;
	c->stack = stack;

	return c;
}

/* Return a copy of set */
wSet *copyWSet(wSet *set) {

	if (!set) return NULL;

	wSet *s = Malloc(sizeof(struct wSet));
	s->ident = set->ident;
	s->next = copyWSet(set->next);

	return s;
}

/* Return a copy of module, but the new module has no variables */
intModule *moduleNoVars(intModule *module) {

	if (!module) return NULL;

	intModule *m = Malloc(sizeof(struct intModule));
	m->name = module->name;
	m->labels = copyWSet(module->labels);
	m->vars = NULL;
	m->next = moduleNoVars(module->next);

	return m;
}

/* Copy all rules from oldPds to pds, but the new pds has no variables */
void rulesNoVars(intPds *pds, intPds *oldPds) {

	wRule *r = oldPds->pds->pds->rules;
	for (; r; r = r->next)
		intPdsInsert(pds, r->from_state, r->from_stack,
			r->to_state, r->to_stack1, r->to_stack2,
			intExprConst(1));
}

/* Return a copy of oldPds, but the new pds has no variables */
intPds *pdsNoVars(intPds *oldPds) {

	intPds *pds;
	wConfig *start;
	intModule *modules;

	start = copyConfig(oldPds->pds->start);
	modules = moduleNoVars(oldPds->modules);
	pds = intPdsCreate(start, modules);
	rulesNoVars(pds, oldPds);

	return pds;
}

/* Return 1 if the transition state -label-> finalstate or
   state -label-> state was flagged in the automaton fa. */
char isLabelFlagged(wFA *fa, wIdent state, wIdent finalstate, wIdent label) {

	wTrans *t;

	t = wFAFind(fa, state, label, finalstate);
	if (t && t->user) return 1;

	t = wFAFind(fa, state, label, state);
	if (t && t->user) return 1;

	return 0;
}

/* Print the transition t */
void printTransition(wTrans *t) {

	printf("%s -%s-> %s\n", wIdentString(t->from), 
		wIdentString(t->name), wIdentString(t->to));
}

/* Go through the witness node of t, set all transition->user to 1
   if transitions were involved when constructing t in fa. */
void createTransFlag(wFA *fa, wTrans *t) {

	if (!t) return;
	if (t->user) return;

	t->user = (void *) 1;
	wWitnessEdge *edge = t->wnode->edges;
	for (; edge; edge = edge->next) {
		if (edge->target1) {
			if (twoVerbose > 2) {
				printTransition(t);
				printf("   edge->target1->trans: ");
				printTransition(edge->target1->trans);
			}
			createTransFlag(fa, edge->target1->trans);
		}
		if (edge->target2) {
			if (twoVerbose > 2) {
				printTransition(t);
				printf("   edge->target2->trans: ");
				printTransition(edge->target2->trans);
			}
			createTransFlag(fa, edge->target2->trans);
		}
	}

	if (fa->dir == SAT_PRE) return;

	/* in case of post*, if t->to is not yet a final state,
	   we have to flag all transitions that leave t->to */

	if (wIsFinal(t->to)) return;

	wTrans *tmp = fa->transitions;
	for (; tmp; tmp = tmp->next)
		if (tmp->from == t->to) createTransFlag(fa, tmp);
}

/* Print the automaton fa with optional name s */
void print_automaton (wFA *fa, char *s)
{
	wTrans *t;

	printf("Automaton %s:\n",s);
	for (t = fa->transitions; t; t = t->next)
		printf("  %s -- %s --> %s (%s)\n",
			wIdentString(t->from),wIdentString(t->name), 
			wIdentString(t->to), 
			bddEmpty(t->label) ? "empty" : "NOT empty");
	printf("\n");
}

/* Print the transition flags with */
void printTransFlag(wFA *fa) {

	wTrans *t = fa->transitions;

	printf("*** Transition flags ***\n");
	for (; t; t = t->next) {
		if (t->user)
			printTransition(t);
	}
	printf("*** End of transition flags ***\n");
	printf("\n");
}

/*****************************************************************************/

/* A variant of bddComputePrestar. It has an extra argument pre1.
   pre1 is the automaton which is computed from the first pre*.
   If t->user is NULL, where t is a transition of pre1,
   it means that the transition can be ignored when computing pre*. */
static wFA* twoComputePrestar (bddPds *pds, wFA *fa, wTrans **found, wFA *pre1)
{
        wSat *prcs = wSatCreate(pds->pds->sr,SAT_PRE,TRACE_YES);
	prcs->special = (pre1) ? 0 : 1;
	wIdent state = pds->start->state;
	wIdent stack = *(pds->start->stack);
	wIdent finalstate = bddFinalState();
        wRule *pt;
	wTrans *t;

	/* Add all the rules */
        for (pt = pds->pds->rules; pt; pt = pt->next)
	{

		/* do not copy rules which their flags of
		left-hand-side symbols are not on.*/
		if (pre1 && !isLabelFlagged(pre1, state, finalstate, pt->from_stack))
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

	/* Add all initial transitions (to the queue) */
        for (t = fa->transitions; t; t = t->next)
                 wSatAddTrans(prcs,t->from,t->name,t->to,t->label);

	/* Process all the transitions */
        while ((t = wSatPopTrans(prcs)))
	{
		if (twoVerbose > 1) printTransition(t);

		/* skip, if t was not flagged */
		if (pre1) {
			wTrans *old = wFAFind(pre1, t->from, t->name, t->to);
			if (old && !old->user) continue;
		}

		/* found target with non-empty BDD? */
		if (!*found && t->from == state && t->name == stack &&
			t->to == finalstate && !bddEmpty(t->label)) {
			*found = t;
			if(pre1) break;
		}

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	if (twoVerbose > 1) printf("\n");
        return wSatDelete(prcs);
}

/* Perform pre* analysis when problem->type == PROBLEM_REACHABILITY_PRE_2 */
bddResult* twoPreReachability (intPds *pds, wIdent state, wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));

        wTrans *t1 = NULL, *t2 = NULL;
	wFA *fa1, *fa2, *fa_pre1, *fa_pre2;
	intPds *pds1;

	/* first pre*: pds without variables */
	if (twoVerbose > 1) printf("First pre*\n");
	pds1 = pdsNoVars(pds);
	fa1 = bddTargetAutomaton(pds1->pds, state, stack);
	fa_pre1 = twoComputePrestar(pds1->pds, fa1, &t1, NULL);
	if (twoVerbose > 2) print_automaton(fa_pre1, "fa_pre1");

	/* return if the initial configuration
	   cannot be reached in the first pre* */
	if (!t1) {
		result->yesno = RESULT_NO;
		result->prefix = NULL;
		result->loop = NULL;

		bddFaDelete(fa1);
		bddFaDelete(fa_pre1);
		intPdsDelete(pds1);

		return result;
	}

	/* create the flag which keeps transitions that were involved 
	   in reaching the initial configuration in the first pre*. */
	if (twoVerbose > 2) printf("Constructing transitions flag ..\n");
	createTransFlag(fa_pre1, t1);
	if (twoVerbose > 2) printf("Constructing transitions flag completed.\n\n");
	if (twoVerbose > 1) printTransFlag(fa_pre1);

	/* second pre* */
	if (twoVerbose > 1) printf("Second pre*\n");
	fa2 = bddTargetAutomaton(pds->pds, state, stack);
	fa_pre2 = twoComputePrestar(pds->pds, fa2, &t2, fa_pre1);
	if (twoVerbose > 2) print_automaton(fa_pre2, "fa_pre2");

	/* pre* is complete; now compute the result and an error trace. */

	result->yesno = t2 ? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;

	if (t2) bddGetPreTrace(pds->pds,fa_pre2,t2,result);

	bddFaDelete(fa1);
	bddFaDelete(fa2);
	bddFaDelete(fa_pre1);
	bddFaDelete(fa_pre2);
	intPdsDelete(pds1);
	
	return result;
}

/*****************************************************************************/

wFA* twoComputePoststar (bddPds *pds, wFA *fa,
		wIdent state, wIdent stack, wTrans **found, wFA *post1)
{
        wSat *prcs = wSatCreate(pds->pds->sr,SAT_POST,TRACE_YES);
	prcs->special = (post1) ? 0 : 1;
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
		if (twoVerbose > 1) printTransition(t);

		/* skip, if t was not flagged */
		if (post1) {
			wTrans *old = wFAFind(post1, t->from, t->name, t->to);
			if (twoVerbose > 1) printf("   (flag=%d)\n", (int) old->user);
			if (old && !old->user) continue;
		}

		/* found target with non-empty BDD? */
		if (!*found && t->from == state && t->name == stack 
				&& !bddEmpty(t->label)) {
			*found = t;
			if (post1) break;
		}

		/* fixme: we should prevent propagation of 'false' values
			  at this point (for efficiency) */
                wSatAugTrans(prcs,t);
	}

	if (twoVerbose > 1) printf("\n");
        return wSatDelete(prcs);
}

/* Perform post* analysis when problem->type == PROBLEM_REACHABILITY_2 */
bddResult* twoPostReachability (intPds *pds, wIdent state, wIdent stack)
{
	bddResult *result = Malloc(sizeof(struct bddResult));

        wTrans *t1 = NULL, *t2 = NULL;
	wFA *fa1, *fa2, *fa_post1, *fa_post2;
	intPds *pds1;

	/* first post*: pds without variables */
	if (twoVerbose > 1) printf("First post*\n");
	pds1 = pdsNoVars(pds);
	fa1 = bddInitialAutomaton(pds1->pds);
	fa_post1 = twoComputePoststar(pds1->pds, fa1, state, stack, &t1, NULL);
	if (twoVerbose > 2) print_automaton(fa_post1, "fa_post1");

	/* return if the initial configuration
	   cannot be reached in the first post* */
	if (!t1) {
		result->yesno = RESULT_NO;
		result->prefix = NULL;
		result->loop = NULL;

		bddFaDelete(fa1);
		bddFaDelete(fa_post1);
		intPdsDelete(pds1);

		return result;
	}

	/* create the flag which keeps transitions that were involved 
	   in reaching the initial configuration in the first post*. */
	if (twoVerbose > 2) printf("Constructing transitions flag ..\n");
	createTransFlag(fa_post1, t1);
	if (twoVerbose > 2) printf("Constructing transitions flag completed.\n\n");
	if (twoVerbose > 1) printTransFlag(fa_post1);

	/* second post* */
	if (twoVerbose > 1) printf("Second post*\n");
	fa2 = bddInitialAutomaton(pds->pds);
	fa_post2 = twoComputePoststar(pds->pds, fa2, state, stack, &t2, fa_post1);
	if (twoVerbose > 2) print_automaton(fa_post2, "fa_post2");

	/* post* is complete; now compute the result and an error trace. */

	result->yesno = t2 ? RESULT_YES : RESULT_NO;
	result->prefix = NULL;
	result->loop = NULL;

	if (t2) printTransition(t2);
	if (t2) bddGetPostTrace(pds->pds,fa_post2,t2,result);

	bddFaDelete(fa1);
	bddFaDelete(fa2);
	bddFaDelete(fa_post1);
	bddFaDelete(fa_post2);
	intPdsDelete(pds1);
	
	return result;
}

/*****************************************************************************/

/* External function. It is meant to be called from intAnalyse() when
   problem->type == PROBLEM_REACHABILITY_PRE_2 or
   problem->type == PROBLEM_REACHABILITY_2. */
bddResult *twoAnalyse (intPds *pds, intProblem *problem) {

	if (problem->type == PROBLEM_REACHABILITY_PRE_2)
		return twoPreReachability(pds, problem->state, problem->stack);
	else if (problem->type == PROBLEM_REACHABILITY_2)
		return twoPostReachability(pds, problem->state, problem->stack);
	else fprintf(stderr, "unknown problem\n");

	return NULL;
}
