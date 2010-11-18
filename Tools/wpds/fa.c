/*************************************************************************/
/* Functions for dealing with automaton transitions.			 */

#include "wpdsInt.h"

wFA *tmpFA;

/************************ Automaton transitions **************************/

wTrans* wTransCreate (wIdent from, wIdent name, wIdent to)
{
	wTrans *new_transition;

	new_transition = Calloc(1,sizeof(struct wTrans));

	new_transition->hash = from*from + name*name*2 + to*to*6;

	new_transition->from = from;
	new_transition->name = name;
	new_transition->to   = to;

	return new_transition;
}

int wTransCompare (void *vt1, void *vt2)
{
	int cmp;
	wTrans *t1 = vt1, *t2 = vt2;
	if ((cmp = wIdentCompare(t1->from,t2->from))) return cmp;
	if ((cmp = wIdentCompare(t1->name,t2->name))) return cmp;
	return wIdentCompare(t1->to,t2->to);
}

void wTransDelete (void *vt)
{
	wTrans *t = vt;
	tmpFA->sr->deref(t->label);
	if (tmpFA->sr->diff) tmpFA->sr->deref(t->delta);
	if (t->wnode) wWNDeref(tmpFA,t->wnode);
	Free(t);
}

/************************ Functions on automata **************************/

wFA* wFACreate (wSemiring *sr)
{
	wFA *fa = Malloc(sizeof(struct wFA));
	fa->sr = wSRCopy(sr);
	fa->hash = wHashCreate(wTransCompare);
	fa->transitions = NULL;
	fa->paths = NULL;
	fa->user = NULL;
	return fa;
}

void wFADelete (wFA *fa)
{
	tmpFA = fa;	/* set tmpFA so that wTransDelete can access it */
	wHashDelete(fa->hash,wTransDelete);
	if (fa->paths) wHashDelete(fa->paths,wPathDeleteHash);
	Free(fa->sr);
	Free(fa);
}

wTrans* wFAInsert (wFA *fa, wIdent from, wIdent name, wIdent to, wSRvalue d,
			int *change)
{
	wTrans *newtrans = wTransCreate(from,name,to);
	wTrans *trans = wHashInsert(fa->hash,newtrans);
	char differential = !!fa->sr->diff;

	if (newtrans == trans)
	{
		trans->next = fa->transitions;
		fa->transitions = trans;
		fa->sr->ref(trans->label = d);
		if (differential) fa->sr->ref(trans->delta = d);
		if (change) *change = 1;
		return trans;
	}
 	Free(newtrans);

	wSRvalue old = trans->label;
	trans->label = fa->sr->combine(trans->label,d);
	if (change) *change = !fa->sr->eq(old,trans->label);
	if (differential)
	{
		wSRvalue delta = fa->sr->diff(d,old);
		wSRvalue olddelta = trans->delta;
		trans->delta = fa->sr->combine(olddelta,delta);
		fa->sr->deref(olddelta);
		fa->sr->deref(delta);
	}
	fa->sr->deref(old);
	return trans;
}

wTrans* wFAFind (wFA *fa, wIdent from, wIdent name, wIdent to)
{
	wTrans *trans = wTransCreate(from,name,to);
	wTrans *result = wHashFind(fa->hash,trans);
	Free(trans);
	return result;
}

/********************* wReadout and auxiliary functions *********************/

int wStateCompare (void *vs1, void *vs2)
{
	wState *s1 = vs1, *s2 = vs2;
	return wIdentCompare(s1->name,s2->name);
}

void wStateDelete (void *vs)
{
	wState *state = vs;
	tmpFA->sr->deref(state->value);
	if (tmpFA->sr->diff) tmpFA->sr->deref(state->delta);
	Free(state);
}

/* Put a state into the hash. Initial states are added to init_queue, final
   states to ni_queue (see also the comments in wpdsInt.h and at wReadout. */
wState* wStateInsert (wFA *fa, wHash *states, wIdent name,
				wState **init_queue, wState **ni_queue)
{
	/* Create state, checking if it is already present. */
	wState *state, *newstate = Calloc(1,sizeof(struct wState));

	newstate->hash = newstate->name = name;
	state = wHashInsert(states,newstate);
	if (state != newstate) { Free(newstate); return state; }

	/* If not, initialize its values and place it in appropriate queues. */
	if (wIsFinal(name))
	{
		state->value = fa->sr->one();
		state->next_state = *ni_queue;
		*ni_queue = state;
		state->in_queue = 1;
	}
	else
		state->value = fa->sr->zero();

	if (fa->sr->diff) fa->sr->ref(state->delta = state->value);

	if (wIsInitial(name))
	{
		state->next_state = *init_queue;
		*init_queue = state;
	}

	return state;
}

/* Return the state associated with the given name. */
wState* wStateFind (wHash *hash, wIdent name)
{
	wState *result, *newstate = Calloc(1,sizeof(struct wState));
	newstate->hash = newstate->name = name;
	result = wHashFind(hash,newstate);
	Free(newstate);
	return result;
}

wFA* wReadout (wFA *fa)
{
	wHash *states = wHashCreate(wStateCompare);
	wState *init_queue = NULL, *ni_queue = NULL;
	wState *state, *from, *to;
	wIdent finalstate;
	wTrans *t;
	wFA *newfa;

	/* Create a hash table of the transitions in fa. A transition
	   t = (p,y,q) is stored with p if p is an initial state, and
	   with q otherwise. */
	for (t = fa->transitions; t; t = t->next)
	{
		from = wStateInsert(fa,states,t->from,&init_queue,&ni_queue);
		to = wStateInsert(fa,states,t->to,&init_queue,&ni_queue);

		state = wIsInitial(t->from)? from : to;
		t->stack = state->trans;
		state->trans = t;
	}

	/* Execute the saturation process (see Algorithms 4 and 7
	   in the journal paper). */
	while ((state = ni_queue))
	{
		wSRvalue pv;

		if (fa->sr->diff)
		{
			pv = state->delta;
			state->delta = fa->sr->zero();
		}
		else
			fa->sr->ref(pv = state->value);

		ni_queue = ni_queue->next_state;
		state->in_queue = 0;

		for (t = state->trans; t; t = t->stack)
		{
			wSRvalue nv = (fa->dir == SAT_POST)?
				fa->sr->extend(pv,t->label) :
				fa->sr->extend(t->label,pv);
			wState *from = wStateFind(states,t->from);
			wSRvalue comb = fa->sr->combine(from->value,nv);

			fa->sr->deref(nv);
			if (!fa->sr->eq(comb,from->value) && !from->in_queue)
			{
				from->next_state = ni_queue;
				ni_queue = from;
				from->in_queue = 1;
			}
			fa->sr->deref(from->value);
			from->value = comb;
		}

		fa->sr->deref(pv);
	}

	/* Assemble the resulting automaton. */
	newfa = wFACreate(fa->sr);
	newfa->dir = fa->dir;
	finalstate = wIdentCreate("__end__");

	for (state = init_queue; state; state = state->next_state)
		for (t = state->trans; t; t = t->stack)
		{
			wState *to = wStateFind(states,t->to);
			wSRvalue d = (fa->dir == SAT_POST)
					? fa->sr->extend(to->value,t->label)
					: fa->sr->extend(t->label,to->value);
			wFAInsert(newfa,t->from,t->name,finalstate,d,NULL);
			fa->sr->deref(d);
		}

	/* Clean up and return. */
	tmpFA = fa;
	wHashDelete(states,wStateDelete);

	return newfa;
}
