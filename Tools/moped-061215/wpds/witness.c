/****************************************************************************/
/* Procedures related to the witness graph.				    */

#include "wpdsInt.h"

wWitnessNode* wWNCreate (wFA *fa, wTrans *trans, wSRvalue d)
{
	wWitnessNode *newnode = Malloc(sizeof(struct wWitnessNode));

	newnode->trans = trans;
	fa->sr->ref(newnode->value = d);
	newnode->refcount = 1;
	newnode->edges = NULL;
	newnode->user = NULL;
	newnode->previous = NULL;

	return newnode;
}

void wWNRef (wFA *fa, wWitnessNode *wn)
{
	wn->refcount++;
}

void wWNDeref (wFA *fa, wWitnessNode *wn)
{
	wWitnessEdge *edge, *tmp;
	wWitnessNode *prevNode;

	while (wn)
	{
		if (--wn->refcount) return;

		edge = wn->edges;
		fa->sr->deref(wn->value);
		while (edge)
		{
			tmp = edge->next;
			wWEDelete(fa,edge);
			edge = tmp;
		}
		prevNode = wn->previous;
		Free(wn);
		wn = prevNode;  /* repeat on 'previous' wnode as needed */
	}
}

wWitnessEdge* wWECreate (wFA *fa, wWitnessNode *source, wSRvalue d,
			 wRule *rule, wWitnessNode *wn1, wWitnessNode *wn2)
{
	wWitnessEdge *newedge = Malloc(sizeof(struct wWitnessEdge));

	newedge->source = source;
	fa->sr->ref(newedge->value = d);
	newedge->rule = rule;
	if ((newedge->target1 = wn1)) wWNRef(fa,wn1);
	if ((newedge->target2 = wn2)) wWNRef(fa,wn2);
	newedge->next = source->edges;
	source->edges = newedge;

	return newedge;
}

wWitnessEdge* wWECopy (wFA *fa, wWitnessEdge *edge, wWitnessNode *newsource)
{
	wWitnessEdge *newedge = Malloc(sizeof(struct wWitnessEdge));

	newedge->source = newsource;
	fa->sr->ref(newedge->value = edge->value);
	newedge->rule = edge->rule;
	if ((newedge->target1 = edge->target1)) wWNRef(fa,edge->target1);
	if ((newedge->target2 = edge->target2)) wWNRef(fa,edge->target2);
	newedge->next = newsource->edges;
	newsource->edges = newedge;

	return newedge;
}

void wWEDelete (wFA *fa, wWitnessEdge *edge)
{
	fa->sr->deref(edge->value);
	if (edge->target1) wWNDeref(fa,edge->target1);
	if (edge->target2) wWNDeref(fa,edge->target2);
	Free(edge);
}
