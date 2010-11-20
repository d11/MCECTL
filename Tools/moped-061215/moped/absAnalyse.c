void bddTransDeref (bddTrans *bt); /* in bddTrace.c */

struct dagEdgeList;

typedef struct dagNode
{
	wWitnessNode *wNode;
	locType type;
	int id;
	DdNode *I0, *I1, *J0, *J1;
	struct dagEdgeList *upList;
	struct dagEdgeList *downList;
} dagNode;

typedef struct dagNodeList
{
	dagNode *n;
	struct dagNodeList *next;
} dagNodeList;

typedef struct dagEdge
{
	wRule *rule;   /* concrete rule */
	DdNode *J;
	dagNode *down;
        dagNode *up1;
	dagNode *up2;
} dagEdge;

typedef struct dagEdgeList
{
	dagEdge *e;
	struct dagEdgeList *next;
} dagEdgeList;

typedef struct dag
{
	dagNodeList *top; /* entry points of this witness dag */
	/* The type of the dagNodes in top is LOC_NORMAL,
	   if it is the starting point of the program. In this case,
	   there is only one dagNode in the list.
	   The type of the dagNodes in top is LOC_ENTRY otherwise,
	   i.e., they are the starting points of the most recently
	   called procedure on this stack level. In this case,
	   there may be multiple dagNodes in the list. */
	dagNode *bottom; /* final point of this witness dag
			    bottom->type == any, if it is the node of
			    the error configuration
			    bottom->type == LOC_CALL, otherwise, i.e., it
			    is the non-popped procedure call on this stack level */
	dagNodeList *pop; /* list of procedure entries popped later
				all those nodes are of type LOC_ENTRY */
	dagNodeList *allNodes; /* list of all dagNodes in this dag
				  (for easy, nonrecursive memory "free"s) */
	dagEdgeList *allEdges; /*  list of all dagEdges in this dag
				   (for easy, nonrecursive memory "free"s) */
	wHeadTable *headTable; /* for collecting all rules used in this dag */	
} dag;

void insertDagNode (dagNode *node, dagNodeList **list)
{
	dagNodeList *old = *list;
	*list = Malloc(sizeof(dagNodeList));
	(*list)->n = node;
	(*list)->next = old;
}

int inDagNodeList (dagNode *node, dagNodeList *list)
{
	for ( ; list; list = list->next)
		if (node == list->n) return 1;
	return 0;
}

void insertDagEdge (dagEdge *edge, dagEdgeList **list)
{
	dagEdgeList *old = *list;
	*list = Malloc(sizeof(dagEdgeList));
	(*list)->e = edge;
	(*list)->next = old;
}

int nodeID;

dagNode *initDagNode (dag *d)
{
	dagNode *res = Malloc(sizeof(dagNode));
	res->wNode = NULL;
	res->type = LOC_NOLOC;
	res->id = ++nodeID;
	res->I0 = res->I1 = res->J0 = res->J1 = NULL;
	res->upList = res->downList = NULL;
	
	insertDagNode(res, &(d->allNodes));
	return res;
}

dagEdge *initDagEdge (dag *d, wWitnessEdge *wEdge)
{
	dagEdge *res = Malloc(sizeof(dagEdge));
	if (wEdge->rule)
	{
		res->rule = (wRule*)(wEdge->rule->user); /* concrete rule */
		assert (res->rule != NULL);
	}
	else res->rule = NULL;
	res->J = NULL;
	res->down = res->up1 = res->up2 = NULL;
	
	insertDagEdge(res, &(d->allEdges));
	return res;
}

dag* initDag ()
{
	dag *res = Malloc(sizeof(dag));
	res->top = NULL;
	res->bottom = NULL;
	res->pop = res->allNodes = NULL;
	res->allEdges = NULL;
	res->headTable = NULL;
	return res;
}

wIdent getStackSymbol (dagNode *node)
{
	return node->wNode->trans->name;
}

/* During the construction phase, the branch being constructed can be
   in one out of two states: ABSO and RELA.
   ABSO means, the nodes witness the values of the ultimate stack symbol,
     i.e., it is the rightmost branch of this dag.
   RELA means the opposite,
     i.e., the witnessed stack symbols are popped,
     i.e., this branch is the up1 field of a combine-edge.
*/
static const int ABSO = 0;
static const int RELA = 1;

bddTrans* getTransList (bddPds *pds, wIdent state, wIdent stack,
		       wFA **fa,
		       wFA **fa_post)
/* pds is an (abstract) PDS, (state,stack) is the head to be reachability-
   checked. Use the result, particularly the witness DAGs, for further
   refinement of the PDS. */
{
	wTrans *t;
	*fa = bddInitialAutomaton(pds);
	*fa_post = bddComputePoststar(pds,*fa,state,stack,&t);
	/* maybe bddComputePoststar will be changed slightly
	   to perform a full poststar search or some sort of breadth-
	   first search */
	bddTrans *result;
	if (t)
	{
		printf("calling bddPickConfig ... ");
		fflush(stdout);
		result = bddPickConfig(*fa_post,t);
		printf("bddPickConfig done\n");
	}
	else result = NULL;
	return result;
}

dagEdge* buildDagEdge (wWitnessEdge *wEdge, dagNode *source, dag *d, int a_r);
/* forward for buildDagNode */

dagNode* buildDagNode (wWitnessNode *wNode, dagEdge *downEdge, dag *d, int a_r)
/* constructs a dagNode in d corresponding to wNode if not yet present.
   The dag on top is also constructed (recursively) and added to d.
   downEdge is an edge directly below, which is to be added to the downList.
   a_r indicates if this node is ABSO or RELA.
*/
{
	assert (wNode != NULL);
	dagNode *result;
	int newNode;
	newNode = (wNode->user == NULL)||(((dagNode**)(wNode->user))[a_r] == NULL);
 	if (newNode) {
		result = initDagNode(d);
		result->wNode = wNode;
		if (wNode->user == NULL)
		{
			wNode->user = Malloc(2*sizeof(dagNode*));
			((dagNode**)(wNode->user))[1-a_r] = NULL;
		}
		/* pointer back to this dagNode */
		((dagNode**)(wNode->user))[a_r] = result;
	}
	else result = ((dagNode**)(wNode->user))[a_r];
	assert(((dagNode**)(result->wNode->user))[a_r] == result);
	
	if (downEdge) insertDagEdge(downEdge, &(result->downList));
	
	if (!newNode) return result;
	/* else (i.e., it is new) build the DAG on top */
	assert (wNode->edges != NULL); /* email Stefan S. 05-03-08 */
	if (wNode->edges->target1 == NULL)
	{
		assert (wNode->edges->target2 == NULL);
		assert (wNode->edges->rule == NULL);
		if (a_r == ABSO)
		{ /* then this must be (part of) the top of this dag */
			result->type = locInf[getStackSymbol(result)]->type;
			assert (result->type == LOC_NORMAL ||
				result->type == LOC_ENTRY);
			insertDagNode(result,&(d->top));
		}
		else
		{ /* then this must be an entry node */
			result->type = LOC_ENTRY;
			assert(locInf[getStackSymbol(result)]->type==LOC_ENTRY);
			insertDagNode(result, &(d->pop));
		}
		assert (result->upList == NULL);
		return result;
	}
	/* now build the DAG on top */
	dagEdge *upEdge;
	wWitnessEdge *wUpEdge;
	for (wUpEdge = wNode->edges; wUpEdge; wUpEdge = wUpEdge->next)
	{
		upEdge = buildDagEdge(wUpEdge,result,d,a_r);
		insertDagEdge(upEdge, &(result->upList));
	}
	assert (result->type != LOC_NOLOC);
	return result;
}

dagEdge* buildDagEdge (wWitnessEdge *wEdge, dagNode *source, dag *d, int a_r)
/* compute a dagEdge in d corresponding to wEdge.
   The dag on top is also constructed (recursively) and added to d.
   source is the node directly below.
   a_r indicates if this edge is ABSO or RELA.
*/
{
	assert(wEdge != NULL);
	assert(source != NULL);
	dagEdge *result = initDagEdge(d,wEdge);

	switch (rTypeConc(result->rule))
	{
	case RULE_PUSH:
		assert (source->type == LOC_CALL || source->type == LOC_NOLOC);
		source->type = LOC_CALL;
		assert (locInf[getStackSymbol(source)]->type == LOC_CALL);
		break;
	case RULE_POP:
		assert (source->type == LOC_GLOBAL || source->type == LOC_NOLOC);
		source->type = LOC_GLOBAL;
		/* source->wNode witnesses an epsilon transition */
		break;
	case RULE_COMBINE:
		assert (source->type == LOC_EVAL || source->type == LOC_NOLOC);
		source->type = LOC_EVAL;
		if (locInf[getStackSymbol(source)]->type != LOC_CALL)
		{
			printf("\nSome error occurred (multiple initial stack symbols?)  %d  %d\n",
			       locInf[getStackSymbol(source)]->type, LOC_NORMAL);
			printf(" %s\n", wIdentString(getStackSymbol(source)));
		}
		assert (locInf[getStackSymbol(source)]->type==LOC_CALL); /*sic!*/
		break;
	case RULE_NORMAL:
	case RULE_EVAL:
	case RULE_ENTRY:
		assert (source->type == LOC_NORMAL || source->type == LOC_NOLOC);
		source->type = LOC_NORMAL;
		assert (locInf[getStackSymbol(source)]->type==LOC_NORMAL);
		break;
	default: assert(0); exit(0);
	}

	result->down = source;
	assert (wEdge->target1 != NULL);
	if (wEdge->target2)
	{
		assert (source->type == LOC_EVAL);
		result->up2 = buildDagNode(wEdge->target2,result,d,a_r);
		result->up1 = buildDagNode(wEdge->target1,result,d,RELA);
	}
	else
	{
		result->up2 = NULL;
		result->up1 = buildDagNode(wEdge->target1,result,d,a_r);
	}
	return result;
}

#include "dotDag.c"

void compute_I (dagNode *n, dag *d, int weakenToJ); /* forward for compute_I_edge */

DdNode* compute_I_edge (dagEdge *e, dag *d, int weakenToJ)
/* computes the contribution of e to the I-value of e's son.
   needs the I-values of the parent(s), therefore calls (recursively) compute_I() */
{
	assert(e->down != NULL);
	assert(e->up1 != NULL);

	compute_I(e->up1,d,weakenToJ);
	assert(e->up1->I0 != NULL);
	DdNode *C;
	if (rTypeConc(e->rule) == RULE_COMBINE)
	{
		assert (e->up2 != NULL);
		compute_I(e->up2,d,weakenToJ);
		assert (e->up2->I0 != NULL);
		C = NULL;
	} else
	{
		assert (e->up2 == NULL);
		C = ((bddDomain*)(e->rule->label))->bdd; /* no ref here, it's just*/
	}                                                /* a temporary copy */
	
	DdNode *result;
	switch (rTypeConc(e->rule))
	{
	case RULE_NORMAL:
	case RULE_PUSH:
	case RULE_POP:
		Cudd_Ref(result = cachedAndAbstract(e->up1->I0, C,
						    helpers->cube_G0L0));
		break;
	case RULE_COMBINE:
		Cudd_Ref(result = cachedAndAbstract(e->up1->I0, e->up2->I0,
						    helpers->cube_G1L1));
		break;
	case RULE_EVAL:
		Cudd_Ref(result = Cudd_bddPermute(mgr, e->up1->I0,
				    concPds->helpers->perm_g0l0_g1l1));
		break;
	case RULE_ENTRY:
		Cudd_Ref(result = Cudd_bddAnd(mgr, e->up1->I0, 
					      helpers->eqv_G1G2_L1L3));
		break;
	default: assert(0); exit(1);
	}
	return result;
}

DdNode* collectFatherI (dagNode *n, dag *d, int weakenToJ)
/* compute the I-value of n by "or-ing up" the contributions of the parents.
   (recursively) calls compute_I_edge */
{
	DdNode *res, *tmp, *newContrib;
	dagEdgeList *el;
	Cudd_Ref(res = zero);
	for (el = n->upList; el; el = el->next)
	{
		newContrib = compute_I_edge(el->e,d,weakenToJ); /* ref it there */
		tmp = res;
		Cudd_Ref(res = Cudd_bddOr(mgr,tmp,newContrib));
		Cudd_RecursiveDeref(mgr,tmp);
		Cudd_RecursiveDeref(mgr,newContrib);
	}
	return res;
}

void compute_I (dagNode *n, dag *d, int weakenToJ)
/* compute the I-value of n
   (recursively) calls computeFatherI */
{
	assert (n != NULL && d != NULL);
	if (n->I0 != NULL)
	{
		assert (n->type == LOC_ENTRY || 
			n->type == LOC_EVAL  || n->I1 != NULL);
		return;
	}
	assert (n->I0 == NULL && n->I1 == NULL);
	assert (!inDagNodeList(n,d->pop));
	if (inDagNodeList(n,d->pop))
	{
		assert (n->type == LOC_ENTRY);
		assert (n->upList == NULL);
		Cudd_Ref(n->I0 = one); 
		/* don't set n->I1, since it's an entry node */
		return;
	}
	assert (n->type != LOC_ENTRY);
	DdNode *res = collectFatherI(n,d,weakenToJ); /* ref it there */
	if (weakenToJ == 1)
	{
		assert (WEAK_I_HEUR == 1);
		DdNode *J;
		if (n->type == LOC_EVAL)
			J = n->J0;
		else
		{
			assert (n->type == LOC_NORMAL || n->type == LOC_CALL ||
				n->type == LOC_GLOBAL);
			J = n->J1;
		}
		weakenToInterp(&res, J);
	}
	switch (n->type)
	{
	case LOC_NORMAL:
		n->I1 = res;
		Cudd_Ref(n->I0 = 
			 Cudd_bddPermute(mgr,res,concPds->helpers->perm_g1l1_g0l0));
		break;
	case LOC_EVAL:
		n->I0 = res;
		assert (n->I1 == NULL);
		break;
	case LOC_ENTRY:
		assert(0);
		break;
	case LOC_CALL:
		n->I1 = res;
		Cudd_Ref(n->I0 =
			 Cudd_bddPermute(mgr,res,concPds->helpers->perm_l2_l0));
		break;
	case LOC_GLOBAL:
		n->I1 = res;
		Cudd_Ref(n->I0 =
		     Cudd_bddPermute(mgr,res,concPds->helpers->perm_g1g2l3_g0g1l1));
		break;
	default: assert(0); exit(1);
	}
	assert (n->I0 != NULL);	       
}

void lazy (dagNode *n)
{
	if (n->type == LOC_CALL) return; /* because if we weaken a call node,
	    we might on the other hand require  from the epsilon node more 
	    than the procedure effect */
	if (n->type == LOC_EVAL) return; /* because we don't have predicates here */
	assert (n->wNode != NULL);
	DdNode *c, *cC, *C;
	c = ((bddDomain*)(n->wNode->value))->bdd; /* no ref, just temporary */
	assert (c != NULL);
	assert (c != zero);
	if (n->type == LOC_GLOBAL)
	{
		cC = globInf->coll0; /* no ref, just temporary */
	}
	else
	{
		if (locInf[getStackSymbol(n)] == NULL)
			printf("\n%s\n",wIdentString(getStackSymbol(n)));
		assert (locInf[getStackSymbol(n)] != NULL);
		cC = locInf[getStackSymbol(n)]->coll0; /* no ref, just temporary */
	}
	assert (cC != NULL);
	assert (helpers->cube_g0g1g2l0l1l2l3 != NULL);
	Cudd_Ref(C = cachedAndAbstract(
			 c, cC, helpers->cube_g0g1g2l0l1l2l3));

	/* This implication might not always hold: 
	if (!implies(n->I0, C))
		printf("\n%s\n",wIdentString(getStackSymbol(n)));
	assert (implies(n->I0, C));
	*/

	assert (n->J0 != NULL);
	DdNode *lazyBDD;
	Cudd_Ref(lazyBDD = Cudd_bddOr(mgr,Cudd_Not(C),n->J0));
	/* conciliate(&(n->J0), &lazyBDD); */
	/*
	deref(&(n->J0));
	n->J0 = lazyBDD;
	deref(&C);
	return;
	*/
	if (1) /* (lazyBDD == one) */
	/* (lazyBDD == one && C != zero && n->J0 != one 
	   && n->type == LOC_ENTRY) */
	{
		/* printf("Lazy symbol: %s\n",wIdentString(getStackSymbol(n)));*/
		/* dotDag(d); */
		/*
 		if (n->J0 != one)
 			printf("LAZY CUTOFF at %s\n",wIdentString(getStackSymbol(n)));
		*/

		deref(&(n->J0));
		n->J0 = lazyBDD; /* has been refed above */
	}
	else deref(&lazyBDD);
	deref(&C);
}

int differenceEmpty (wHead *pdsHead, wHead *dagHead)
/* check if the set difference of the rules in pdsHead and dagHead is empty */
{
	wHeadEdge *pdsHedge, *dagHedge;
	int found;
	for (pdsHedge = pdsHead->post; pdsHedge; pdsHedge = pdsHedge->postnext)
	{
		found = 0;
		for (dagHedge = dagHead->post; dagHedge; dagHedge = dagHedge->postnext)
		{
			if (dagHedge->origin == pdsHedge->origin)
			{
				found = 1;
				break;
			}
		}
		if (!found) return 0;
	}
	return 1;
}

int allRulesInDag (wIdent from_state, wIdent from_stack, dag *d)
/* check if all pds rules going out from (state,stack) are represented in d */
{
	wHead *pdsHead, *dagHead;
	pdsHead = wHTInsert(concHeadTable, from_state, from_stack);
	dagHead = wHTInsert(d->headTable,  from_state, from_stack);
	return differenceEmpty(pdsHead,dagHead);
}

int checkPreserveCandidate (DdNode *candidate, dagEdge *e, dag *d)
{
	DdNode *check, *C;
	int ok;
	int Logging = 0;
	switch (rTypeConc(e->rule))
	{
	case RULE_NORMAL:
		assert (e->up1->I0 != NULL);
		if (Logging)
			printf("checking for %s -- %s ... ",
			       wIdentString(getStackSymbol(e->down)),
			       wIdentString(getStackSymbol(e->up1)));
		if (Logging == 1) printf("\n");
		if (PRESERVE_HEUR == 1 && candidate == zero)
		{
			if (Logging>1) printf("no, because it's FALSE\n");
			return 0;
		}
		if (PRESERVE_HEUR == 2)
		{
			if (!allRulesInDag(e->rule->from_state,
					   e->rule->from_stack,d))
			{
				if (Logging>1) 
					printf("no, because not all rules in DAG\n");
				return 0;
			}
		}
		if (!implies(e->up1->I0, candidate))
		{
			if (Logging>1)
				printf("no, because not implied by strongest interpolant\n");
			return 0;
		}
		C = ((bddDomain*)(e->rule->label))->bdd;
		Cudd_Ref(check = cachedAndAbstract(candidate,C,
						   helpers->cube_G0L0));
		ok = implies(check, e->down->J1);
		Cudd_RecursiveDeref(mgr,check);
		if (ok)
		{
			if (Logging>1) printf("yes\n");
		}
		else
		{
			if (Logging>1) printf("no, because post does not imply next pred\n");
		}
		return ok;
		break;
	case RULE_COMBINE:
		assert (e->up2->I0 != NULL);
		if (!implies(e->up2->I0, candidate)) return 0;
		Cudd_Ref(check = cachedAndAbstract(e->up1->I0,candidate,
						   helpers->cube_G1L1));
		ok = implies(check, e->down->J0);
		Cudd_RecursiveDeref(mgr,check);		
		return ok;
		break;
	default: assert (0); return 0;
	}
	assert (0); return 0;
}

DdNode* preserveHeur (dagEdge *e, dag *d)
{
	DdNode *candidate;
	switch (rTypeConc(e->rule))
	{
	case RULE_NORMAL:
		Cudd_Ref(candidate = e->down->J0);
		break;
	case RULE_COMBINE:
		Cudd_Ref(candidate = Cudd_bddPermute(mgr,e->down->J0,
						     helpers->c_perm_G0_G1));
		break;
	default:
		return NULL;
	}
	assert (candidate != NULL);
	if (checkPreserveCandidate(candidate,e,d))
		return candidate;
	else
	{
		Cudd_RecursiveDeref(mgr,candidate);
		return NULL;
	}
	/* should not reach */
}

void compute_J (dagNode *n, dag *d); /* forward for compute_J_edge */

DdNode* compute_J_edge (dagEdge *e, dag *d)
/* computes the contribution of e to the J-value of e's (right) parent.
   needs the child's J-value, therefore calls (recursively) compute_J() */
{
	assert (e->down != NULL);
	assert (e->up1 != NULL);
	assert (e->up1->I0 != NULL);

	if (e->J != NULL) return e->J;

	compute_J(e->down,d);
	assert (e->down->J0 != NULL);
	DdNode *C;
	if (rTypeConc(e->rule) == RULE_COMBINE)
	{
		assert (e->up2 != NULL);
		assert (e->down->J1 == NULL);
		C = NULL;
	} else
	{
		assert (e->up2 == NULL);
		assert (e->down->J1 != NULL);
		C = ((bddDomain*)(e->rule->label))->bdd; /* no ref here, it's just*/
	}						 /* a temporary copy */
	assert (e->J == NULL);
	if (PRESERVE_HEUR) e->J = preserveHeur(e,d);
	if (e->J == NULL)
	{
		switch (rTypeConc(e->rule))
		{
		case RULE_NORMAL:
			Cudd_Ref(e->J = Cudd_Not(cachedAndAbstract(
			     C, Cudd_Not(e->down->J1), helpers->cube_G1L1)));
			break;
		case RULE_PUSH:
			Cudd_Ref(e->J = Cudd_Not(cachedAndAbstract(
			     C, Cudd_Not(e->down->J1), helpers->cube_G1L1L2)));
			break;
		case RULE_POP:
			Cudd_Ref(e->J = Cudd_Not(cachedAndAbstract(
			     C, Cudd_Not(e->down->J1), helpers->cube_G1)));
			break;
		case RULE_COMBINE:
			/* this is the contribution to the right (call) parent */
			Cudd_Ref(e->J = Cudd_Not(cachedAndAbstract(
			     e->up1->I0, Cudd_Not(e->down->J0),helpers->cube_G0)));
			break;
		case RULE_EVAL:
			Cudd_Ref(e->J = e->down->J0);
			break;
		case RULE_ENTRY:
			Cudd_Ref(e->J = Cudd_Not(cachedAndAbstract(
			      helpers->eqv_G1G2_L1L3, Cudd_Not(e->down->J1),
			      helpers->cube_G2L3)));
			break;
		default: assert(0); exit(1);
		}
	}
	if (STRONG_J_HEUR >= 1)
		strengthenToInterp(&(e->J), rTypeConc(e->rule)==RULE_COMBINE?
				   e->up2->I0 : e->up1->I0);
	return e->J;
}

DdNode* compute_J_leftParent (dagEdge *e, dag *d)
/* computes the contribution of e to the J-value of e's left parent.
   e is a combine-edge.
   needs the child's value and the contribution to the right (call) parent,
   therefore calls (recursively) compute_J_edge() */
{
	assert (rTypeConc(e->rule) == RULE_COMBINE);
	assert (e->down != NULL);
	assert (e->up1 != NULL && e->up2 != NULL);
	
	compute_J_edge(e,d);
	assert (e->J != NULL); /* the contribution to the right (call) parent */
	assert (e->down->J0 != NULL && e->down->J1 == NULL);

	DdNode *rightParent;
	rightParent = e->J;
	/* optional: */
	if (e->up2->J0 != NULL) rightParent = e->up2->J0;

	DdNode *result;
	Cudd_Ref(result = Cudd_Not(
			 cachedAndAbstract(rightParent, Cudd_Not(e->down->J0),
					   helpers->cube_G2L0L3)));
	if (STRONG_J_HEUR >= 1) strengthenToInterp(&result, e->up1->I0);
	return result;
}

void compute_J (dagNode *n, dag *d)
/* compute the J-value of n by "and-ing up" the contributions of the children */
{
	assert (n != NULL && d != NULL);
	if (n->J0 != NULL)
	{
		assert (n->type == LOC_ENTRY || 
			n->type == LOC_EVAL  || n->J1 != NULL);
		return;
	}
	assert (n->J0 == NULL && n->J1 == NULL);
	assert (n->I0 != NULL);
	assert (n->type == LOC_ENTRY || 
		n->type == LOC_EVAL  || n->I1 != NULL);

	if (SHORTCUT && (n->I0 == zero))
	{
		Cudd_Ref(n->J0 = zero);
		if (n->I1)
		{
			assert (n->I1 == zero);
			Cudd_Ref(n->J1 = zero);
		}
		return;
	}
 
	dagEdgeList *el;
	DdNode *tmp, *newContrib;
	DdNode *res;
	Cudd_Ref(res = one);
	for (el = n->downList; el; el = el->next)
	{
		if (n->type == LOC_GLOBAL)
			newContrib = compute_J_leftParent(el->e,d);/*ref it there */
		else
			Cudd_Ref(newContrib = compute_J_edge(el->e,d));
		tmp = res;
		Cudd_Ref(res = Cudd_bddAnd(mgr,tmp,newContrib));
		Cudd_RecursiveDeref(mgr,tmp);
		Cudd_RecursiveDeref(mgr,newContrib);
	}
	n->J0 = res; /* in all cases */
	if (LAZY) lazy(n);
	if (STRONG_J_HEUR >= 1) strengthenToInterp(&(n->J0), n->I0);
	if (STRONG_J_HEUR == 2) conciliate(&(n->I0), &(n->J0));
	if (STRONG_J_HEUR == 3) strengthenMaximally(&(n->J0), n->I0);
	/*
	printf("compute_J");
	printf(" %s ", wIdentString(getStackSymbol(n)));
	fflush(stdout);
	*/
	switch (n->type)
	{
	case LOC_NORMAL:
		Cudd_Ref(n->J1 =
			 Cudd_bddPermute(mgr,n->J0,concPds->helpers->perm_g0l0_g1l1));
		break;
	case LOC_EVAL:
	case LOC_ENTRY:
		assert (n->J1 == NULL);
		break;
	case LOC_CALL:
		Cudd_Ref(n->J1 =
			 Cudd_bddPermute(mgr,n->J0,concPds->helpers->perm_l0_l2));
		break;
	case LOC_GLOBAL:
		Cudd_Ref(n->J1 =
		     Cudd_bddPermute(mgr,n->J0,helpers->c_perm_G0G1L1_G1G2L3));
		break;
	default: assert(0); exit(1);
	}
	/*
	printf(" %i\n", Cudd_DagSize(n->J0));
	fflush(stdout);
	*/
	assert ((n->I1 == NULL) == (n->J1 == NULL));
	if (DEBUG) assert(implies(n->I0,n->J0));
}

void cutBackLinks (dagNode *n)
/* clear the n->wNode->user fields, since the construction has been completed. */
{
	if (n == NULL) return;
	if (n->wNode->user == NULL) return;
	Free(n->wNode->user);
	n->wNode->user = NULL;
	dagEdgeList *el;
	for (el = n->upList; el; el = el->next)
	{
		cutBackLinks(el->e->up1);
		cutBackLinks(el->e->up2);
	}
}

void freeDagNodeList (dagNodeList *nl)
/* free the list, but not the nodes */
{
	dagNodeList *next;
	while (nl)
	{
		next = nl->next;
		Free(nl);
		nl = next;
	}
}

void freeDagEdgeList (dagEdgeList *el)
/* free the list, but not the edges */
{
	dagEdgeList *next;
	while (el)
	{
		next = el->next;
		Free(el);
		el = next;
	}
}

void freeDagNode (dagNode *n)
{
	assert (n != NULL);
	if (n->I0 != NULL) Cudd_RecursiveDeref(mgr,n->I0);
	if (n->I1 != NULL) Cudd_RecursiveDeref(mgr,n->I1);
	if (n->J0 != NULL) Cudd_RecursiveDeref(mgr,n->J0);
	if (n->J1 != NULL) Cudd_RecursiveDeref(mgr,n->J1);
	freeDagEdgeList(n->upList);
	freeDagEdgeList(n->downList);
	Free(n);
}

void freeDagEdge (dagEdge *e)
{
	assert (e != NULL);
	if (e->J != NULL) deref(&(e->J));
	Free(e);
}

void freeDag (dag *d)
{
	dagNodeList *nl;
	for (nl = d->allNodes; nl; nl = nl->next)
	{
		freeDagNode(nl->n); nl->n = NULL;
	}
	dagEdgeList *el;
	for (el = d->allEdges; el; el = el->next)
	{
		freeDagEdge(el->e); el->e = NULL;
	}
	freeDagNodeList(d->top);
	freeDagNodeList(d->pop);
	freeDagNodeList(d->allNodes);
	freeDagEdgeList(d->allEdges);
	freeHeadTable(d->headTable);
	Free(d);
}

void nullifyI (dag **d, int height)
{
	int i;
	dagNodeList *nl;
	dagNode *n;
	for (i=0; i<height; i++)
		for (nl=d[i]->allNodes; nl; nl=nl->next)
		{
			n = nl->n;
			if (n->I0)
			{
				Cudd_RecursiveDeref(mgr, n->I0);
				n->I0 = NULL;
			}
			if (n->I1)
			{
				Cudd_RecursiveDeref(mgr, n->I1);
				n->I1 = NULL;
			}
		}
}

void registerUsedRules (dag *d)
{
	assert (d->headTable == NULL);
	d->headTable = wHTCreate(NULL);
	dagEdgeList *el;
	wRule *rule;
	wHead *head;
	wHeadEdge *edge;
	for (el = d->allEdges; el; el = el->next)
	{
		rule = el->e->rule;
		if (rule == NULL) continue;
		head = wHTInsert(d->headTable,rule->from_state,rule->from_stack);
		edge = Calloc(1,sizeof(struct wHeadEdge));
		edge->origin = rule; /* origin is just a field with a rule */
		edge->postnext = head->post;
		head->post = edge;
	}
}

dag ** rawDags (int height, wTrans **transArray)
/* compute a skeleton of the DAG (without predicates) */
{
	nodeID = 0;
	dag **d = Malloc(height * sizeof(dag*));
	int i;
	/* dotHeader(); */
	for (i = height-1; i>=0; i--) /* from right to left, i.e., bottom to top */
	{
		d[i] = initDag();
		d[i]->bottom = buildDagNode(transArray[i]->wnode,NULL,
					    d[i],ABSO); /* construct the DAG */
		assert (d[i]->top != NULL);
		/* dotDag(d[i]); */
		/*dotNodes(d[i]);
		  dotEdges(d[i]); */
		cutBackLinks(d[i]->bottom); /* clear the user field of */
		                            /* the witness nodes */
		registerUsedRules(d[i]);
	}
	/* dotFooter(); */
	return d;
}

void setEntryValues (dagNodeList *dnl, DdNode *value)
{
	for ( ; dnl; dnl = dnl->next)
	{
		assert (dnl->n->I0 == NULL);
		Cudd_Ref(dnl->n->I0 = value);
		assert (dnl->n->I1 == NULL);
		if (dnl->n->type == LOC_NORMAL)
		{
			assert (value == one);
			Cudd_Ref(dnl->n->I1 = one);
		}
		else
			assert (dnl->n->type == LOC_ENTRY);
	}
}

int forward_I (dag **d, int height, int weakenToJ)
/* calculate the I-values.
   return -1 iff counterexample is real
          0  if counterexample is spurious and SHORTCUT is not set
          level (0<=level<height) otherwise, where level is the stack level,
                       where the counterexample got spurious
*/
{
	int i;
	DdNode *carry;
	Cudd_Ref(carry = one);
	
 	for (i = height-1; i>=0; i--) /* from right to left, i.e., bottom to top */
	{
		setEntryValues(d[i]->top,carry);
		deref(&carry);
		setEntryValues(d[i]->pop,one);
		/* now, compute all I-values of d[i] recursively: */
		compute_I(d[i]->bottom, d[i], weakenToJ);
		Cudd_Ref(carry = Cudd_bddExistAbstract(mgr, d[i]->bottom->I0,
						       helpers->cube_G2L0L3));
		if (carry == zero && (SHORTCUT || i==0)) break;
	}
	deref(&carry);
	return i;
}

void backward_J (dag **d, int height, int level)
/* calculate the J-values from level to bottom */
{
	assert (level >= 0); 
	assert (level < height);
	int i;
	dagNodeList *pop, *top;
	DdNode *carry = NULL;
	for (i = level; i<height; i++) /* from left to right, i.e., top to bottom */
	{
		if (i == level)
		{
			assert (carry == NULL);
			assert (d[i]->bottom->I0 == zero);
			Cudd_Ref(d[i]->bottom->J0 = zero);
			if (d[i]->bottom->I1 != NULL)
				Cudd_Ref(d[i]->bottom->J1 = zero);
		}
		else
		{
			assert (carry != NULL);
			assert (d[i]->bottom->type == LOC_CALL);
			assert (d[i]->bottom->J0 == NULL);
			assert (d[i]->bottom->J1 == NULL);
			d[i]->bottom->J0 = carry; /* it's ref-ed once */
			Cudd_Ref(d[i]->bottom->J1 = carry);
			if (DEBUG) assert (implies(d[i]->bottom->I0,
						   d[i]->bottom->J0));
			if (DEBUG) assert (implies(d[i]->bottom->I1,
						   d[i]->bottom->J1));
		}
		assert (d[i]->bottom->J0 != NULL);
		/* now, compute all J-values of d[i] recursively: */
		Cudd_Ref(carry = one);
		DdNode *tmp;
		for (top = d[i]->top; top; top = top->next)
		{
			assert (top->n->type == LOC_ENTRY ||
				top->n->type == LOC_NORMAL);
			compute_J(top->n, d[i]);
			assert (top->n->J0 != NULL);
			Cudd_Ref(tmp = Cudd_bddAnd(mgr,carry,top->n->J0));
			deref(&carry);
			carry = tmp;
		}
		assert (carry != NULL); /* carry is properly refed */
		/* now do the popped calls, too */
		for (pop = d[i]->pop; pop; pop = pop->next)
		{
			assert (pop->n->type == LOC_ENTRY);
			compute_J(pop->n, d[i]);
			assert (pop->n->J0 == one);
		}
	}
	deref(&carry);
}

void addPreds (dagNodeList *nl)	
/* go through the node list and add the J0-predicates to the
   appropriate locationInfo */
{
	dagNode *n;
	wIdent s;
	for ( ; nl; nl = nl->next)
	{
		n = nl->n;
		if (WEAK_I_HEUR == 2)
			weakenToInterp(&(n->I0), n->J0);
		switch (n->type)
		{
		case LOC_EVAL:
			break;
		case LOC_NORMAL:
		case LOC_ENTRY:
		case LOC_CALL:
			s = getStackSymbol(n);
			assert (s < wident_globals_used); /* actually, s < numSym */
			assert (locInf[s] != NULL);
if (I_VALUES)           addPred(n->I0, locInf[s], 1);
if (J_VALUES)		addPred(n->J0, locInf[s], 1);
			break;
		case LOC_GLOBAL:
if (I_VALUES)   	addPred(n->I0, globInf, 0);
if (J_VALUES)		addPred(n->J0, globInf, 0);
			break;
		default: assert(0); exit(1);
		}
	}			
}

analyseResult extractPreds (bddTrans *bt)
{
	assert (bt != NULL);
	analyseResult res;
	/* compute transArray */
	int height = 0;
	bddTrans *trans;
	for (trans = bt; trans; trans = trans->rest_path) height++;
	assert(height>0);
	wTrans **transArray = Malloc(sizeof(wTrans*) * height);
	int i = 0;
	for (trans = bt; trans; trans = trans->rest_path)
		transArray[i++] = trans->trans;
	assert(i == height);
	/* compute the DAGs */
	dag **d = rawDags(height, transArray);
	Free(transArray); transArray = NULL;
	printf("computing I-Values\n");
	int level = forward_I(d, height, 0);
	if (level == -1)
	{
		printf("-----------------------------------------------------\n");
		printf("There is some concrete path. Reachable.\n");
		printf("-----------------------------------------------------\n");
		res = ABS_RES_REAL_CE;
	}
	else
	{
		res = ABS_RES_SPURIOUS_CE;
		if (J_VALUES)
		{
			printf("computing J-Values\n");
			/* compute the J-values */
			backward_J(d, height, level);
		}
		if (WEAK_I_HEUR == 1)
		{
			assert (J_VALUES);
			nullifyI(d, height);
			assert (forward_I(d, height, 1) <= level);
		}
		/* add the predicates to future abstractions */
 		for (i=0; i<height; i++) addPreds(d[i]->allNodes);
	}
	/* free everything */
	for (i=0; i<height; i++) freeDag(d[i]);
	Free(d);
	return res;
}

analyseResult absAnalyse (bddPds *absPds, wIdent state, wIdent stack)
{
	analyseResult res;
	wFA *fa1, *fa2;
	bddTrans *bt = getTransList(absPds,state,stack,&fa1,&fa2);
	if (bt == NULL)
	{
		res = ABS_RES_NO_ERROR;
		printf("-----------------------------------------------------\n");
		printf("No path found in abstract system. ");
		printf("Not reachable.\n");
		printf("-----------------------------------------------------\n");
	}
	else
	{
		res = extractPreds(bt);
		bddTransDeref(bt);
	}
	bddFaDelete(fa1);
	bddFaDelete(fa2);
	return res;
}
