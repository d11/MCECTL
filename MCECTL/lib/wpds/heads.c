/***********************************************************************/
/* Data structures and functions for heads (or surface configurations) */

#include "wpdsInt.h"

wHeadTable *tmpHT;

/***********************************************************************/

wHead* wHeadCreate (wIdent state, wIdent name)
{
	wHead *newhead = Calloc(1,sizeof(struct wHead));
	newhead->state  = state;
	newhead->name   = name;
	newhead->hash  = state * state + name * name * 2;
	return newhead;
}

int wHeadCompare (void *vh1, void *vh2)
{
	int cmp;
	wHead *h1 = vh1, *h2 = vh2;
	if ((cmp = wIdentCompare(h1->state,h2->state))) return cmp;
	return wIdentCompare(h1->name,h2->name);
}

/* This function is used while deleting the whole hash table; therefore,
   it suffices to delete just the pre-chain of edges. (The post-chain
   entries will be caught by the targeted head.)			*/

void wHeadDelete (void *vh)
{
	wHead *head = vh;
	wSemiring *sr = tmpHT->prcs->sr;

	while (head->pre)
	{
		wHeadEdge *e = head->pre->prenext;
		sr->deref(head->pre->label);
		if (tmpHT->prcs->diff) sr->deref(head->pre->delta);
		if (head->pre->wnode) wWNDeref(tmpHT->prcs->fa,
						head->pre->wnode);
		Free(head->pre);
		head->pre = e;
	}
	Free(head);
}

/***********************************************************************/

wHeadTable* wHTCreate (wSat *prcs)
{
	wHeadTable *table = Malloc(sizeof(struct wHeadTable));
	table->prcs = prcs;
	table->hash = wHashCreate(wHeadCompare);
	return table;
}

void wHTDelete (wHeadTable *table)
{
	tmpHT = table;	/* set tmpHT so that wHeadDelete can access it */
	wHashDelete(table->hash,wHeadDelete);
	Free(table);
}

wHead* wHTInsert (wHeadTable *table, wIdent state, wIdent name)
{
	wHead *head = wHeadCreate(state,name);
	wHead *result = wHashInsert(table->hash,head);
	if (result != head) Free(head);
	return result;
}

/***********************************************************************/

wHeadEdge* wHTInsertEdge (wHeadTable *table, wHead *from, wHead *to,
				wIdent tail, wSRvalue label,
				wRule *origin, wWitnessNode *wnode)
{
	wHeadEdge *edge;
	wSemiring *sr = table->prcs->sr;
	char differential = table->prcs->diff;

	if (table->prcs->trace == TRACE_NO) { origin = NULL; wnode = NULL; }

	for (edge = to->pre; edge; edge = edge->prenext)
	    if (edge->from == from && edge->tail == tail)
	    {
	   	wSRvalue nv;

		/* Check whether the origin of the new edge is compatible with
		   a previously recorded edge, i.e. whether they originated
		   from the same rule (and transition, if applicable). If that
		   is the case, the new edge is `merged' into the old one. */
		if (edge->origin != origin) continue;
		if (edge->wnode && !wnode) continue;
		if (!edge->wnode && wnode) continue;
		if (wnode && edge->wnode->trans != wnode->trans) continue;

		if (wnode)
		{
			wWNRef(table->prcs->fa,wnode);
			wWNDeref(table->prcs->fa,edge->wnode);
			edge->wnode = wnode;
		}

		nv = sr->combine(edge->label,label);

		if (sr->eq(nv,edge->label))
			sr->deref(nv);
		else
		{
			if (differential)
			{
			    wSRvalue delta = sr->diff(label,edge->label);
			    wSRvalue olddelta = edge->delta;
			    edge->delta = sr->combine(olddelta,delta);
			    sr->deref(olddelta);
			    sr->deref(delta);
			}
			sr->deref(edge->label);
			edge->label = nv;
		}
		return edge;
	    }

	edge = Calloc(1,sizeof(struct wHeadEdge));
	edge->from = from;
	edge->to = to;
	edge->tail = tail;
	edge->prenext = to->pre;
	edge->postnext = from->post;
	sr->ref(edge->label = label);
	if (differential) sr->ref(edge->delta = label);

	edge->origin = origin;
	if (wnode) wWNRef(table->prcs->fa,edge->wnode = wnode);

	return to->pre = from->post = edge;
}

/***********************************************************************/

void wHeadRemoveTrans (wTrans *t)
{
	if (!t->stack_prev) return;
	*(t->stack_prev) = t->stack;
	if (t->stack) t->stack->stack_prev = t->stack_prev;
	t->stack_prev = NULL;
}

void wHeadInsertTrans (wHead *head, wTrans *t)
{
	wHeadRemoveTrans(t);
	t->stack_prev = &(head->trans);
	t->stack = head->trans;
	if (head->trans) head->trans->stack_prev = &(t->stack);
	head->trans = t;
}
