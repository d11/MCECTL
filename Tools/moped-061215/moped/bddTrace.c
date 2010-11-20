/***************************************************************************/

#include "bdd.h"

extern int NEWTRACES;

/***************************************************************************/

static bddTrans* bddCreateTransition (wTrans *t, bddDomain *values)
{
	bddTrans *path = Calloc(1,sizeof(struct bddTrans));

	path->trans = t;
	path->values = values;
	path->refcount = 1;

	return path;
}

static void bddTransRef (bddTrans* bt)
{
	if (!bt) return;
	bt->refcount++;
}

extern void bddTransDeref (bddTrans *bt)
{
	bddTrans *bt2, *tmp;

	while (bt && --(bt->refcount) == 0)
	{
		bt2 = bt->rest_path;
		while (bt2 && --(bt2->refcount) == 0)
		{
			tmp = bt2->rest_path;
			/* if bt2->trans is NULL, then values was already
			   dereferenced during conversion to bddResult */
			if (bt2->trans) bddDomDeref(bt2->values);
			Free(bt2);
			bt2 = tmp;
		}

		tmp = bt->next_config;
		if (bt->trans) bddDomDeref(bt->values);
		Free(bt);
		bt = tmp;
	}
}

/***************************************************************************/

static bddDomain* bddSelect (bddDomain *v)
{
	/* Select one particular valuation from v->bdd. */
	bddPds *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode *bdd = v->bdd, *tbdd;
	int inv = 0, t_to_one;

	int *select, *cube;

	if (bddEmpty(v))
	{
		Cudd_Ref(pds->zero);
		return bddDomGenerate(v->signature,pds,pds->zero);
	}

	if (v->signature == SIG_PRETRANS)
		select = pds->helpers->select_pretrans;
	else if (v->signature == SIG_POST_INI)
		select = pds->helpers->select_post_ini;
	else if (v->signature == SIG_POST_NON)
		select = pds->helpers->select_post_non;
	else if (v->signature == SIG_STATE)
		select = pds->helpers->select_state;
	else
	{
		fprintf(stderr,"Wrong signature in bddSelect\n");
		exit(1);
	}

	bddInitCube(mgr,&cube);
	memcpy(cube,select,pds->helpers->array_size * sizeof(int));

	while (!Cudd_IsConstant(bdd))
	{
		inv = inv ^ Cudd_IsComplement(bdd);
		bdd = Cudd_Regular(bdd);

		tbdd = Cudd_T(bdd);
		t_to_one = !Cudd_IsConstant(tbdd) ||
			(!!Cudd_V(tbdd) ^ inv ^ Cudd_IsComplement(tbdd));
		if (!t_to_one) cube[bdd->index] = 0;
		bdd = t_to_one? Cudd_T(bdd) : Cudd_E(bdd);
	}

	return bddDomGenerate(v->signature,pds,bddCreateCube(mgr,cube));
}

static bddDomain* bddGetSource (bddDomain *v)
{
	/* Select the 'source-state part' of a BDD attached to a transition. */
	bddPds    *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *bdd, *tmp0, *tmp1, *tmp2;

	/* We use somewhat complicated transformations here to reach the
	   desired results. This is hopefully ok because these operations
	   aren't used often. Another way to resolve this is to declare
	   additional helper structures in the bddPds struct. */
	if (v->signature == SIG_PRETRANS)
	{
		/* g1 l1 g2 -> g1 l1 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l0g2l3));

		/* g1 l1 -> g2 l1 */
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0g1_g1l1g2));
		Cudd_RecursiveDeref(mgr,tmp0);

		/* g2 l1 -> g2 */
		Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp1,
					pds->helpers->cube_g1l1));
		Cudd_RecursiveDeref(mgr,tmp1);
	}

	else if (v->signature == SIG_POST_INI)
	{
		/* g0 l0 g2 l3 -> g0 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l0g2l3));

		/* g0 -> g1 */
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0_g1l1));
		Cudd_RecursiveDeref(mgr,tmp0);

		/* g1 -> g2 */
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp1,
					pds->helpers->perm_g0l0g1_g1l1g2));
		Cudd_RecursiveDeref(mgr,tmp1);
	}

	else if (v->signature == SIG_POST_NON)
	{
		/* g1 l1 l0 g2 l3 -> g1 l1 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l0g2l3));

		/* g1 l1 -> g0 l0 */
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
		Cudd_RecursiveDeref(mgr,tmp0);

		/* g0 l0 -> g0=g2 & l0=l3 */
		Cudd_Ref(tmp2 = Cudd_bddAnd(mgr, tmp1,
					pds->helpers->eqv_g0g2_l0l3));
		Cudd_RecursiveDeref(mgr,tmp1);

		/* g0=g2 & l0=l3 -> g2 l3 */
		Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp2,
					pds->helpers->cube_g0l0));
		Cudd_RecursiveDeref(mgr,tmp2);
	}

	else
	{
		fprintf(stderr,"Wrong signature in bddGetSource\n");
		exit(1);
	}

	return bddDomGenerate(SIG_STATE,pds,bdd);
}

static bddDomain* bddGetTarget (bddDomain *v)
{
	/* Select the 'target-state part' of a BDD attached to a transition. */
	bddPds    *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *bdd, *tmp0;

	/* The 'one size fits all' approach. This does some unnecessary
	   work in some cases, but works for all relevant transition types
	   (SIG_PRETRANS, SIG_POST_INI, SIG_POST_NON). */
	Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_g0l0));
	Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp0,
					pds->helpers->cube_g1l1));

	Cudd_RecursiveDeref(mgr,tmp0);
	return bddDomGenerate(SIG_STATE,pds,bdd);
}

static bddDomain* bddRestrictSource (bddDomain *v, bddDomain *r)
{
	/* Restrict the BDD of v to those tuples whose 'source' part
	   is contained in r. */
	bddPds    *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *bdd, *rbdd, *tmp0;

	/* v->signature should be a transition code,
	   r->signature should be SIG_STATE. */

	/* Get r into proper shape. */
	if (v->signature == SIG_PRETRANS)
	{
		/* g2 -> g1		(v: g1 / l1 / g2) */
		Cudd_Ref(rbdd = Cudd_bddPermute(mgr, r->bdd,
					pds->helpers->perm_g2l2_g1l1));
	}

	else if (v->signature == SIG_POST_INI)
	{
		/* g2 -> g1 -> g0	(v: g0 / l0 / g2 l3) */
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, r->bdd,
					pds->helpers->perm_g2l2_g1l1));
		Cudd_Ref(rbdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
		Cudd_RecursiveDeref(mgr,tmp0);
	}

	else if (v->signature == SIG_POST_NON)
	{
		/* g2 l3 -> g1 l1	(v: g1 l1 / l0 / g2 l3) */
		Cudd_Ref(rbdd = Cudd_bddPermute(mgr, r->bdd,
					pds->helpers->perm_g1g2l3_g0g1l1));
	}

	else
	{
		fprintf(stderr,"Wrong signature in bddRestrictSource\n");
		exit(1);
	}

	Cudd_Ref(bdd = Cudd_bddAnd(mgr,v->bdd, rbdd));
	Cudd_RecursiveDeref(mgr,rbdd);

	return bddDomGenerate(v->signature,pds,bdd);
}

static bddDomain* bddRestrictTarget (bddDomain *v, bddDomain *r)
{
	/* Restrict the BDD of v to those tuples whose 'target' part
	   is contained in r. */
	bddPds    *pds = v->pds;
	DdNode    *bdd;

	/* v->signature should be a transition code,
	   r->signature should be SIG_STATE. */

	Cudd_Ref(bdd = Cudd_bddAnd(pds->mgr,v->bdd,r->bdd));
	return bddDomGenerate(v->signature,pds,bdd);
}

static bddDomain* bddExcludeTarget (bddDomain *v, bddDomain *r)
{
	/* Restrict the BDD of v to those tuples whose 'target' part
	   is NOT contained in r. */
	bddDomain *complement, *result;
	DdNode    *bdd;

	Cudd_Ref(bdd = Cudd_Not(r->bdd));
	complement = bddDomGenerate(SIG_STATE,r->pds,bdd);
	result = bddRestrictTarget(v,complement);
	bddDomDeref(complement);
	return result;
}

static bddDomain* bddReverseJoin (bddDomain *v1, bddDomain *v2)
{
	bddPds    *pds = v1->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *tmp0, *tmp1, *bdd;
	int	   sig;

	if (v1->signature == SIG_RULE0 && v2->signature == SIG_POST_INI)
	{
		/* g0 l0 g1,		g0 / l0 / g2l3 */
		/* v2: l0 -> l2 (perm_l0_l2), g0 -> g1 (perm_g0l0_g1l1) */
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v2->bdd,
					pds->helpers->perm_l0_l2));
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0_g1l1));
		Cudd_RecursiveDeref(mgr,tmp0);

		/* /\ E g1 (cube_g1l1) */
		Cudd_Ref(bdd = Cudd_bddAndAbstract(mgr, v1->bdd, tmp1, 
					pds->helpers->cube_g1l1));
		Cudd_RecursiveDeref(mgr,tmp1);

		sig = SIG_POST_TWO;
	}

	else if (v1->signature == SIG_RULE1 && v2->signature == SIG_POST_INI)
	{
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v2->bdd,
					pds->helpers->perm_g0l0_g1l1));
		Cudd_Ref(bdd = Cudd_bddAndAbstract(mgr, v1->bdd, tmp0, 
					pds->helpers->cube_g1l1));
		Cudd_RecursiveDeref(mgr,tmp0);

		sig = SIG_POST_INI;
	}

	else if (v1->signature == SIG_RULE2 && v2->signature == SIG_POST_TWO)
	{
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v2->bdd,
					pds->helpers->perm_g0l0_g1l1));
		Cudd_Ref(tmp1 = Cudd_bddAndAbstract(mgr, v1->bdd, tmp0, 
					pds->helpers->cube_g1l1));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp1,
					pds->helpers->cube_l2));
		Cudd_RecursiveDeref(mgr,tmp1);

		sig = SIG_POST_INI;
	}

	else if (v1->signature == SIG_PRETRANS && 
		(v2->signature == SIG_RULE1 || v2->signature == SIG_RULE2))
	{
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v1->bdd,
					pds->helpers->perm_g1l1_g0l0));
		Cudd_Ref(bdd = Cudd_bddAndAbstract(mgr, tmp0, v2->bdd,
					pds->helpers->cube_g0l0));
		Cudd_RecursiveDeref(mgr,tmp0);

		sig = (v2->signature == SIG_RULE1)? SIG_PRETRANS : SIG_PRE_TWO;
	}

	else
	{
		fprintf(stderr,"Wrong signatures in bddReverseJoin\n");
		exit(1);
	}
	
	return bddDomGenerate(sig,pds,bdd);
}

static bddDomain *bddJoinTransitions (bddDomain *v1, bddDomain *v2)
{
	/* v1 should be SIG_POST_INI, v2 should be SIG_POST_NON */
	/* v1: go / l0 / g2l3,		v2: g1l1 / l0 / g2l3 */
	/* result: g0 / l0l2 / g2l3 */

	bddPds    *pds = v1->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *tmp0, *tmp1, *bdd;

	Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v1->bdd,
				pds->helpers->perm_g1g2l3_g0g1l1));
	Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, v2->bdd,
				pds->helpers->perm_l0_l2));
	Cudd_Ref(bdd = Cudd_bddAndAbstract(mgr, tmp0, tmp1,
				pds->helpers->cube_g1l1));
	Cudd_RecursiveDeref(mgr,tmp0);
	Cudd_RecursiveDeref(mgr,tmp1);

	return bddDomGenerate(SIG_POST_TWO,pds,bdd);
}

static void bddSplitValues (bddDomain *v, bddDomain **res1, bddDomain **res2)
{
	/* v->sig should be SIG_POST_TWO */
	bddPds    *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode    *tmp0, *tmp1, *bdd;

	if (v->signature == SIG_POST_TWO)
	{
		/* g0 l0 l2 g2 l3 -> g0 l0 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l2));
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1g2l3_g0g1l1));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp1,
					pds->helpers->cube_g1l1));
		Cudd_RecursiveDeref(mgr,tmp1);
		*res1 = bddDomGenerate(SIG_POST_INI,pds,bdd);
		
		/* g0 l0 l2 g2 l3 -> l2 g2 l3 -> l0 g2 l3 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_g0l0));
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_l2_l0));
		Cudd_RecursiveDeref(mgr,tmp0);
		*res2 = bddDomGenerate(SIG_POST_NON,pds,bdd);
	}
	else if (v->signature == SIG_PRE_TWO)
	{
		/* g1 l1 l2 g2 -> g1 l1 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l2));
		Cudd_Ref(bdd = Cudd_bddExistAbstract(mgr, tmp0,
					pds->helpers->cube_l0g2l3));
		Cudd_RecursiveDeref(mgr,tmp0);
		*res1 = bddDomGenerate(SIG_PRETRANS,pds,bdd);

		/* g1 l1 l2 g2 -> l2 g2 -> l1 g2 */
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr ,tmp0,
					pds->helpers->perm_l2_l0));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(bdd = Cudd_bddPermute(mgr, tmp1,
					pds->helpers->perm_g0l0_g1l1));
		Cudd_RecursiveDeref(mgr,tmp1);
		*res2 = bddDomGenerate(SIG_PRETRANS,pds,bdd);
	}

	else
	{
		fprintf(stderr,"Wrong signatures in bddSplitValues\n");
		exit(1);
	}
}

/***************************************************************************/

static void bddStateAdd (bddState **visited, wIdent state, bddDomain *values)
{	/* Add the state/values pair to the visited list. Consumes one
	   reference on values. */

	/* Search for state in the visited list. */
	while (*visited)
	{
		if ((*visited)->state == state)
		{
			/* add new value pairs to the old ones */
			bddDomain *tmp0 = bddUnion((*visited)->values,values);
			bddDomDeref((*visited)->values);
			bddDomDeref(values);
			(*visited)->values = tmp0;
			return;
		}
		visited = &((*visited)->next);
	}

	/* If the state isn't there yet, add it to the list. */
	if (!*visited)
	{
		*visited = Malloc(sizeof(struct bddState));
		(*visited)->state = state;
		(*visited)->values = values;
		(*visited)->next = NULL;
	}
}

static bddDomain* bddStateFind (bddState *visited, wIdent state)
{	/* Returns the value pairs already contained in the visited list
	   for state `state'. Generates a reference to the returned BDD. */
	bddPds *pds = visited->values->pds;

	while (visited)
	{
		if (visited->state == state)
		{
			bddDomRef(visited->values);
			return visited->values;
		}
		visited = visited->next;
	}

	Cudd_Ref(pds->zero);
	return bddDomGenerate(SIG_STATE,pds,pds->zero);
}

static void bddStateDelete (bddState *visited)
{	/* Delete the chain of visited states starting at visited. */
	while (visited)
	{
		bddState *tmp = visited->next;
		bddDomDeref(visited->values);
		Free(visited);
		visited = tmp;
	}
}

/* Checks queue and appends subsequent transitions to the end of the queue.
   Return 1 if transition to final state was found, 0 otherwise. */
static int bddPickExtendQueue (wFA *fa, bddTrans *queue,
				bddTrans **queue_end, bddState **visited)
{
	/* Find transition/BDD pairs in fa such that
	   (i) the source data agrees with the target of queue
	   (ii) the target data does not yet appear in visited.
	   Note: Going through the list of all transitions in the FA is
	   fairly inefficient. We should exploit the head table of the
	   sat process for this (but we have deleted the table at this
	   point... sigh). Fix this at a later stage. */
	wTrans *t;
	bddTrans *result;

	wIdent target_state = queue->trans->to;
	bddDomain *target_bdd = bddGetTarget(queue->values);
	bddDomain *avail, *taken, *tmp;

	for (t = fa->transitions; t; t = t->next)
	{
		/* Make sure that condition (i) is met. */
		if (t->from != target_state) continue;
		avail = bddRestrictSource(t->label,target_bdd);

		/* Make sure that condition (ii) is met. */
		taken = bddStateFind(*visited,t->to);
		tmp = bddExcludeTarget(avail,taken);
		bddDomDeref(taken); bddDomDeref(avail);
		avail = tmp;

		/* Is the result empty? */
		if (bddEmpty(avail))
		{
			bddDomDeref(avail);
			continue;
		}

		/* Otherwise generate the result and append it to the queue. */
		result = bddCreateTransition(t,avail);
		result->next_config = NULL;
		result->rest_path = queue;	/* point back to parent */
		(*queue_end)->next_config = result;
		*queue_end = result;

		/* Mark the selected values as `visited'. */
		bddStateAdd(visited,t->to,bddGetTarget(result->values));

		if (wIsFinal(t->to))
		{
			bddDomDeref(target_bdd);
			return 1;
		}
	}

	bddDomDeref(target_bdd);
	return 0;
}

/* Select a path from the BFS tree by going backwards along rest_path */
static bddTrans* bddPickPath (bddTrans *queue)
{
	bddTrans *result = NULL, *newtrans;
	bddDomain *avail, *src, *lowest, *suitable;
	wWitnessNode *wn;

	while (queue)
	{
		if (result)
		{
			src = bddGetSource(result->values);
			avail = bddRestrictTarget(queue->values,src);
			bddDomDeref(src);
		}
		else
			bddDomRef(avail = queue->values);

		/* pick from the lowest wnode possible */
		lowest = NULL;
		for (wn = queue->trans->wnode; wn; wn = wn->previous)
		{
			suitable = bddIntersect(avail,wn->value);
			if (!bddEmpty(suitable))
			{
				if (lowest) bddDomDeref(lowest);
				lowest = suitable;
			}
			else
				bddDomDeref(suitable);
		}
		
		newtrans = bddCreateTransition(queue->trans,bddSelect(lowest));
		newtrans->rest_path = result;
		newtrans->next_config = NULL;
		newtrans->rule = NULL;
		result = newtrans;

		bddDomDeref(avail);
		bddDomDeref(lowest);
		queue = queue->rest_path;
	}

	return result;
}

extern bddTrans* bddPickConfig (wFA* fa, wTrans *start)
{
	/* Pick a loop-free path beginning with start and leading to an
	   accepting state. visited contains the state/value pairs already
	   visited during the search for an accepting state. */

	bddTrans *queue_start, *queue, *queue_end, *result;
	bddState *visited = NULL;

	/* Create a FIFO queue of transitions to conduct breadth-first search */
	bddDomRef(start->label);
	queue_start = queue = queue_end
		= bddCreateTransition(start,start->label);
	queue->next_config = queue->rest_path = NULL;

	bddStateAdd(&visited,start->from,bddGetSource(start->label));
	bddStateAdd(&visited,start->to,bddGetTarget(start->label));

	/* if we're already done... */
	/* using a goto here to avoid unnecessary indentation */
	if (wIsFinal(queue->trans->to)) goto skip_loop;

	while (queue)
	{
		/* returns 1 if final state found */
		if (bddPickExtendQueue(fa,queue,&queue_end,&visited)) break;
		queue = queue->next_config;
	}

	if (!queue)
	{
		fprintf(stderr,"Could not pick configuration!?\n");
		exit(1);
	}

	/* go to last element in queue (which ends in final state) */
	while (queue->next_config) queue = queue->next_config;

	skip_loop:

	/* reconstruct path from the BFS queue */
	result = bddPickPath(queue);

	/* cleanup: */
	/* delete the queue chain manually (can't rely on refs) */
	while ((queue = queue_start))
	{
		queue_start = queue_start->next_config;
		bddDomDeref(queue->values);
		Free(queue);
	}

	bddStateDelete(visited);

	return result;
}

static wPath* bddTransToPath (wFA *fa, bddTrans *current)
{
	int length = 0;
	bddTrans *tmp;
	wTrans **array;
	wPath *result;

	for (tmp = current; tmp; tmp = tmp->rest_path) length++;
	array = Calloc((length+1),sizeof(wTrans*));
	length = 0;
	for (tmp = current; tmp; tmp = tmp->rest_path)
		array[length++] = tmp->trans;

	result = wPathFromArray(fa,array);
	Free(array);
	return result;
}

/***************************************************************************/

/* Check whether the chosen valuation in current has a predecessor
	valuation in `predecessor'; if so, construct it and return 1. */

static int bddGetPred0 (wFA *fa, bddTrans **current, wRule *rule, wPath *pred)
{
	/* Compute the possible predecessor values, then split them up. */
	bddDomain *twoval = bddReverseJoin(rule->label,(*current)->values);

	bddDomain *val1, *val2, *val1int, *val2int;
	bddDomain *tgtval, *srcval, *common_states, *sel_state;
	bddTrans *t1, *t2;

	bddSplitValues(twoval,&val1,&val2);
	bddDomDeref(twoval);

	/* Now val1 and val2 are the potential predecessor values
	   for both transitions. Now we need to find values for the
	   state in the middle. */

	/* Find the values that are actually there in both transitions,
	   extract the `target-state values' from the first and the
	   `source-state values' from the second transition. */
	val1int = bddIntersect(val1,pred->wnode->value);
	val2int = bddIntersect(val2,pred->next_path->wnode->value);
	tgtval = bddGetTarget(val1int);
	srcval = bddGetSource(val2int);
	bddDomDeref(val1); bddDomDeref(val2);

	/* select a value from the intersection of target and source */
	common_states = bddIntersect(tgtval,srcval);
	sel_state = bddSelect(common_states);
	bddDomDeref(tgtval); bddDomDeref(srcval); bddDomDeref(common_states);

	/* pick values with a middle state from the intersection */
	val1 = bddRestrictTarget(val1int,sel_state);
	val2 = bddRestrictSource(val2int,sel_state);
	bddDomDeref(val1int); bddDomDeref(val2int); bddDomDeref(sel_state);

	if (bddEmpty(val1) || bddEmpty(val2))
	{
		bddDomDeref(val1); bddDomDeref(val2);
		return 0;
	}

	t1 = bddCreateTransition(pred->wnode->trans,bddSelect(val1));
	t2 = bddCreateTransition(pred->next_path->wnode->trans,bddSelect(val2));
	bddDomDeref(val1); bddDomDeref(val2);

	t1->rest_path = t2;	/* the reference counter for t2 was already
				   increased by bddCreateTransition */
	bddTransRef(t2->rest_path = (*current)->rest_path);
	t1->next_config = *current;
	t1->rule = rule;
	*current = t1;

	return 1;
}

static int bddGetPred1 (wFA *fa, bddTrans **current, wRule *rule, wPath *pred)
{
	/* Compute the 'potential' predecessors of the values. */
	bddDomain *ppval = bddReverseJoin(rule->label,(*current)->values);
	/* Restrict to those that are actually present in the wnode. */
	bddDomain *predval = bddIntersect(ppval,pred->wnode->value);
	bddTrans *newtrans;

	bddDomDeref(ppval);
	
	if (bddEmpty(predval))
	{
		bddDomDeref(predval);
		return 0;
	}

	/* Construct a possible predecessor. */
	newtrans = bddCreateTransition(pred->wnode->trans,bddSelect(predval));
	bddDomDeref(predval);

	bddTransRef(newtrans->rest_path = (*current)->rest_path);
	newtrans->next_config = *current;
	newtrans->rule = rule;
	*current = newtrans;

	return 1;
}

static int bddGetPred2 (wFA *fa, bddTrans **current, wRule *rule, wPath *pred)
{
	/* Make sure there are at least two transitions in the current path. */
	if (!(*current)->rest_path) return 0;

	/* First join the first two values, then proceed as in bddGetPred1. */
	bddDomain *join = bddJoinTransitions((*current)->values,
				(*current)->rest_path->values);
	/* Compute the 'potential' predecessors of the values. */
	bddDomain *ppval = bddReverseJoin(rule->label,join);
	/* Restrict to those that are actually present in the wnode. */
	bddDomain *predval = bddIntersect(ppval,pred->wnode->value);
	bddTrans *newtrans;

	bddDomDeref(join);
	bddDomDeref(ppval);
	
	if (bddEmpty(predval))
	{
		bddDomDeref(predval);
		return 0;
	}

	/* Construct a possible predecessor. */
	newtrans = bddCreateTransition(pred->wnode->trans,bddSelect(predval));
	bddDomDeref(predval);

	bddTransRef(newtrans->rest_path = (*current)->rest_path->rest_path);
	newtrans->next_config = *current;
	newtrans->rule = rule;
	*current = newtrans;

	return 1;
}

static int bddGetPredecessor (wFA *fa, bddTrans **current, wRule *rule,
					wPath *predecessor)
{
	/* Check whether we have arrived at an initial configuration. */
	if (!predecessor) return 1;

	if (rule->to_stack2)
		return bddGetPred2(fa,current,rule,predecessor);

	else if (rule->to_stack1)
		return bddGetPred1(fa,current,rule,predecessor);

	else /* if (!rule->to_stack1) */
		return bddGetPred0(fa,current,rule,predecessor);
}

/***************************************************************************/

/* Check whether the chosen valuation in current has a successor
	valuation in `successor'; if so, construct it and return 1. */

static int bddGetSucc0 (wFA *fa, bddTrans **current, wRule *rule, wPath *succ)
{
	bddTrans *newtrans;
	/* Make sure there are at least two transitions in the current path. */
	if (!(*current)->rest_path) return 0;

	/* Make a copy of the second transition in the current path.
	   In principle, it would be enough to start a new path by
	    just linking to this second transition, but that gets us
	    into trouble during bddConvertConfig. */
	bddDomRef((*current)->rest_path->values);
	newtrans = bddCreateTransition((*current)->rest_path->trans,
					(*current)->rest_path->values);
	bddTransRef(newtrans->rest_path = (*current)->rest_path->rest_path);

	(*current)->next_config = newtrans;
	(*current)->rule = rule;
	(*current) = newtrans;
	
	return 1;
}

static int bddGetSucc1 (wFA *fa, bddTrans **current, wRule *rule, wPath *succ)
{
	/* Compute the 'potential' successors of the values. */
	bddDomain *ssval = bddReverseJoin((*current)->values,rule->label);

	/* Restrict to those that are actually present in the wnode. */
	bddDomain *succval = bddIntersect(ssval,succ->wnode->value);
	bddTrans *newtrans;

	bddDomDeref(ssval);
	
	if (bddEmpty(succval))
	{
		bddDomDeref(succval);
		return 0;
	}

	/* Construct a possible successor. */
	newtrans = bddCreateTransition(succ->wnode->trans,bddSelect(succval));
	bddDomDeref(succval);

	bddTransRef(newtrans->rest_path = (*current)->rest_path);
	(*current)->next_config = newtrans;
	(*current)->rule = rule;
	*current = newtrans;

	return 1;
}

static int bddGetSucc2 (wFA *fa, bddTrans **current, wRule *rule, wPath *succ)
{
	/* Compute the 'potential' predecessors of the values. */
	bddDomain *ssval = bddReverseJoin((*current)->values,rule->label);

	bddDomain *val1, *val2, *val1int, *val2int;
	bddDomain *tgtval, *srcval, *common_states, *sel_state;
	bddTrans *t1, *t2;

	/* Split ssval up into the values for the two successor transitions. */
	bddSplitValues(ssval,&val1,&val2);
	bddDomDeref(ssval);

	/* Now val1 and val2 are the potential predecessor values
	   for both transitions. Now we need to find values for the
	   state in the middle. */

	/* Find the values that are actually there in both transitions,
	   extract the `target-state values' from the first and the
	   `source-state values' from the second transition. */
	val1int = bddIntersect(val1, succ->wnode->value);
	val2int = bddIntersect(val2, succ->next_path->wnode->value);
	tgtval = bddGetTarget(val1int);
	srcval = bddGetSource(val2int);
	bddDomDeref(val1); bddDomDeref(val2);

	/* select a value from the intersection of target and source */
	common_states = bddIntersect(tgtval,srcval);
	sel_state = bddSelect(common_states);
	bddDomDeref(tgtval); bddDomDeref(srcval); bddDomDeref(common_states);

	/* pick values with a middle state from the intersection */
	val1 = bddRestrictTarget(val1int,sel_state);
	val2 = bddRestrictSource(val2int,sel_state);
	bddDomDeref(val1int); bddDomDeref(val2int); bddDomDeref(sel_state);

	if (bddEmpty(val1) || bddEmpty(val2))
	{
		bddDomDeref(val1); bddDomDeref(val2);
		return 0;
	}

	t1 = bddCreateTransition(succ->wnode->trans,bddSelect(val1));
	t2 = bddCreateTransition(succ->next_path->wnode->trans,bddSelect(val2));
	bddDomDeref(val1); bddDomDeref(val2);

	t1->rest_path = t2;	/* the reference counter for t2 was already
				   increased by bddCreateTransition */
	bddTransRef(t2->rest_path = (*current)->rest_path);
	(*current)->next_config = t1;
	(*current)->rule = rule;
	*current = t1;

	return 1;
}

static int bddGetSuccessor (wFA *fa, bddTrans **current, wRule *rule,
					wPath *successor)
{
	/* Check whether we have arrived at an initial configuration. */
	if (!successor) return 1;

	if (rule->to_stack2)
		return bddGetSucc2(fa,current,rule,successor);

	else if (rule->to_stack1)
		return bddGetSucc1(fa,current,rule,successor);

	else /* if (!rule->to_stack1) */
		return bddGetSucc0(fa,current,rule,successor);
}

/***************************************************************************/

static DdNode* bddGetStateValue (bddDomain *v)
{
	bddPds *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode *tmp0, *result;

	if (v->signature == SIG_POST_INI)
	{
		Cudd_Ref(result = Cudd_bddExistAbstract(mgr, v->bdd,
				pds->helpers->cube_l0g2l3));
	}
	else if (v->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddPermute(mgr, v->bdd,
				pds->helpers->perm_g1l1_g0l0));
		Cudd_Ref(result = Cudd_bddExistAbstract(mgr, tmp0,
				pds->helpers->cube_l0g2l3));
		Cudd_RecursiveDeref(mgr,tmp0);
	}

	return result;
}

static DdNode* bddGetStackValue (bddDomain *v)
{
	bddPds *pds = v->pds;
	DdManager *mgr = pds->mgr;
	DdNode *result, *tmp0, *tmp1;

	if (v->signature == SIG_POST_INI || v->signature == SIG_POST_NON)
	{
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_g1l1));
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr,tmp0,
					pds->helpers->perm_l0_l2));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr,tmp1,
					pds->helpers->cube_g0l0));
		Cudd_RecursiveDeref(mgr,tmp1);
		Cudd_Ref(tmp1 = Cudd_bddExistAbstract(mgr,tmp0,
					pds->helpers->cube_l0g2l3));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(result = Cudd_bddPermute(mgr,tmp1,
					pds->helpers->perm_l2_l0));
		Cudd_RecursiveDeref(mgr,tmp1);
	}
	else if (v->signature == SIG_PRETRANS)
	{
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, v->bdd,
					pds->helpers->cube_l0g2l3));
		Cudd_Ref(tmp1 = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g0l0g1_g1l1g2));
		Cudd_RecursiveDeref(mgr,tmp0);
		Cudd_Ref(tmp0 = Cudd_bddExistAbstract(mgr, tmp1,
					pds->helpers->cube_l0g2l3));
		Cudd_RecursiveDeref(mgr,tmp1);
		Cudd_Ref(result = Cudd_bddPermute(mgr, tmp0,
					pds->helpers->perm_g1l1_g0l0));
		Cudd_RecursiveDeref(mgr,tmp0);
	}

	return result;
}

static bddPath* bddConvertPath (bddTrans *current)
{
	bddPath *result;

	if (!current) return NULL;
	/* has result been computed before? */
	if (!current->trans)
	{
		result = (bddPath*)(current->values);
		result->refcount++;
		return result;
	}

	result = Malloc(sizeof(struct bddPath));
	result->stack = current->trans->name;
	result->bdd = bddGetStackValue(current->values);
	result->refcount = 1;
	result->user = NULL;
	result->rest_path = bddConvertPath(current->rest_path);

	/* store result for future use (see above) */
	bddDomDeref(current->values);
	current->trans = NULL;
	current->values = (bddDomain*) result;

	return result;
}

static bddConfig* bddConvertConfig (bddTrans *current)
{
	bddConfig *result, **ptr = &result;

	while (current)
	{
		*ptr = Malloc(sizeof(struct bddConfig));
		(*ptr)->state = current->trans->from;
		(*ptr)->rule = current->rule;
		(*ptr)->prefix = bddConvertPath(current->prefix);

		(*ptr)->bdd = bddGetStateValue(current->values);

		(*ptr)->stack = Malloc(sizeof(struct bddPath));
		(*ptr)->stack->stack = current->trans->name;
		(*ptr)->stack->bdd = bddGetStackValue(current->values);
		(*ptr)->stack->refcount = 1;
		(*ptr)->stack->user = NULL;
		(*ptr)->stack->rest_path = bddConvertPath(current->rest_path);

		ptr = &((*ptr)->next_config);
		current = current->next_config;
	}

	*ptr = NULL;
	return result;
}

/***************************************************************************/

static void bddFindSuitableConfiguration
		(wFA *fa, wPath **path, bddTrans **current,
		int (*succFn)(wFA*,bddTrans**,wRule*,wPath*))
{
	bddTrans *trans = *current;
	bddTrans *newTrans = *current;
	wPathTrans *pt;

	/* Here we populate path->transitions with all possible successors
	   (including multiple copies of the same successor if TRACE_REF
	   is on). */
	wPathTraceStep(fa,*path);

	/* Find some suitable successor. */
	for (pt = (*path)->transitions; pt; pt = pt->next)
		if (succFn(fa,&trans,pt->rule,pt->target))
			break;

	if (NEWTRACES && pt && pt->target)
	{
		/* "Move down" in the "previous wnodes" chain as far
		   as possible. */
		wPathTrans *chosenPt = pt;
		for (pt = pt->next; pt; pt = pt->next)
			if (succFn(fa,&newTrans,pt->rule,pt->target))
			{
				/* Ref current because it didn't get a
				   reference from succFn for being the
				   next_config of trans. */
				if (succFn == bddGetPredecessor)
					bddTransRef(*current);
				bddTransDeref(trans);

				trans = newTrans;
				chosenPt = pt;
				newTrans = *current;
			}
			else
				break;

		pt = chosenPt;
	}

	if (pt)
	{
		wPath *pnext;
		wPathRef(fa,pnext = pt->target);
		wPathDeref(fa,*path);
		*path = pnext;
		*current = trans;
	}
	else
	{
		fprintf(stderr,"Could not find %scessor!?\n",
			(succFn == bddGetPredecessor)? "prede" : "suc");
		exit(1);
	}
}

/***************************************************************************/

extern void bddGetPostTrace (bddPds *pds, wFA *fa,
				wTrans *start, bddResult *result)
{
	bddTrans *current = bddPickConfig(fa,start);
	wPath *path = bddTransToPath(fa,current);
	
	do {
		bddFindSuitableConfiguration(fa,&path,&current,
						bddGetPredecessor);
	} while (path);

	wPathDeref(fa,path);
	result->prefix = bddConvertConfig(current);
	bddTransDeref(current);
}

/***************************************************************************/

extern void bddGetPreTrace (bddPds *pds, wFA *fa,
				wTrans *start, bddResult *result)
{
	bddTrans *current = bddPickConfig(fa,start);
	bddTrans *first = current;
	wPath *path = bddTransToPath(fa,current);

	do {
		current->prefix = NULL;
		bddFindSuitableConfiguration(fa,&path,&current,bddGetSuccessor);
	} while (path);

	wPathDeref(fa,path);
	result->prefix = bddConvertConfig(first);
	bddTransDeref(first);
}

extern void bddGetDpnTrace (bddPds *pds, wFA *fa,
				wTrans *t1, wTrans *t2, bddResult *result)
{
	bddTrans *bt2 = Malloc(sizeof(struct bddTrans));
	bt2->trans = t2;
	bt2->values = bddSelect(t2->label);
	bt2->rule = NULL;
	bt2->refcount = 1;
	bt2->rest_path = NULL;
	bt2->next_config = NULL;
	
	bddTrans *bt1 = Malloc(sizeof(struct bddTrans));
	bt1->trans = t1;
	bt1->values = bddSelect(t1->label);
	bt1->rule = NULL;
	bt1->refcount = 1;
	bt1->rest_path = bt2;
	bt1->next_config = NULL;

	bddTrans *first = bt1, *current = bt1;
	wPath *path = bddTransToPath(fa,bt1), *pnext, *cp;
	wPathTrans *pt;
	bddTrans *prefix = NULL;
	
	do {
		current->prefix = prefix;

		for (cp = path, bt2 = current;
			!cp->wnode->edges->rule;
			cp = cp->next_path, current = current->rest_path)
		{
			bddTrans *newtrans = Malloc(sizeof(struct bddTrans));
			newtrans->trans = cp->wnode->trans;
			bddDomRef(newtrans->values = bt2->values);
			newtrans->rule = NULL;
			newtrans->refcount = 1;
			newtrans->rest_path = prefix;
			newtrans->next_config = NULL;
			prefix = newtrans;
		}

		wPathTraceStep(fa,path);
		for (pt = path->transitions; pt; pt = pt->next)
			if (bddGetSuccessor(fa,&current,pt->rule,pt->target))
			{
				wPathRef(fa,pnext = pt->target);
				wPathDeref(fa,path);
				path = pnext;
				break;
			}

		if (!pt)
		{
			fprintf(stderr,"Could not find successor!?\n");
			exit(1);
		}
	} while (path);

	wPathDeref(fa,path);
	result->prefix = bddConvertConfig(first);
	bddTransDeref(first);
}
