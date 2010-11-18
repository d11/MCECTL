/****************************************************************************/
/* Saturation processes.						    */

#include "wpdsInt.h"

/****************************************************************************/
/* Internal functions (functions starting with ws).			    */

/****************************************************************************/
/* tests if one of the witness edges of oldnode is labeled with origin      */
int newParent(wRule *origin, wWitnessNode *oldNode)
{
	wWitnessEdge *edge;
	for (edge = oldNode->edges; edge; edge = edge->next)
	{
		if (edge->rule == origin) return 0;
	}
	return 1;
}

/* This function may substitute the functionality of wsUpdate, see
   below. Here, we want to include witness edges, which do not
   contribute to the semiring value, but are labeled with a wRule that
   no other witness edge is labeled with.  */

/* NOTE: I didn't implement the new trace generation modes in this
	 function - Stefan S. */
static wTrans* wsUpdateSpecial (wSat *prcs, wIdent from, wIdent name, wIdent to, wSRvalue d,
				wRule *origin, wWitnessNode *wn1, wWitnessNode *wn2)
{

	wWitnessNode *wn;
	wWitnessEdge *we;
	int change;

	wTrans *t = wFAInsert(prcs->fa,from,name,to,d,&change);
	
	if (!change && !newParent(origin,t->wnode)) return t;
	if (change) wHeadInsertTrans(prcs->queue,t);
	/* i.e., do not insert, if !change and newParent hold */

	if (prcs->trace == TRACE_NO) return t;

	/* create new witness node and edge */
	wn = wWNCreate(prcs->fa,t,t->label);
	we = wWECreate(prcs->fa,wn,d,origin,wn1,wn2);
	if (t->wnode)
	{
		/* copy the edges from the old witness node unless the
		   old values are subsumed by d */
		wWitnessEdge *edge;
		for (edge = t->wnode->edges; edge; edge = edge->next)
		{
			if (edge->rule != origin)
			{
				wWECopy(prcs->fa,edge,wn);
			}
			else
			/* only in this case, subsumption could occur */
			{
				wSRvalue comb = prcs->sr->combine(d,edge->value);
				if (!prcs->sr->eq(comb,d))
					wWECopy(prcs->fa,edge,wn);
				prcs->sr->deref(comb);
			}
		}
		wWNDeref(prcs->fa,t->wnode);
	}
	t->wnode = wn;
	return t;
}

/* Update the value of transition (from,name,to) with the newly discovered
   semiring value d and update the witness structure (if desired).
   Returns the new or updated transition.				    */
wTrans* wsUpdate (wSat *prcs, wIdent from, wIdent name, wIdent to, wSRvalue d,
		  wRule *origin, wWitnessNode *wn1, wWitnessNode *wn2)
{
	if (prcs->special)
		return wsUpdateSpecial(prcs,from,name,to,d,origin,wn1,wn2);

	wWitnessNode *wn;
	wWitnessEdge *we;
	int change;

	wTrans *t = wFAInsert(prcs->fa,from,name,to,d,&change);
	
	if (!change) return t;
	wHeadInsertTrans(prcs->queue,t);
	if (prcs->trace == TRACE_NO) return t;

	/* create new witness node and edge */
	wn = wWNCreate(prcs->fa,t,t->label);
	we = wWECreate(prcs->fa,wn,d,origin,wn1,wn2);
	if (!t->wnode) { t->wnode = wn; return t; }

	if (prcs->trace == TRACE_COPY)
	{
		/* copy the edges from the old witness node unless the
		   old values are subsumed by d */
		wWitnessEdge *edge;
		for (edge = t->wnode->edges; edge; edge = edge->next)
		{
			wSRvalue comb = prcs->sr->combine(d,edge->value);
			if (!prcs->sr->eq(comb,d)) wWECopy(prcs->fa,edge,wn);
			prcs->sr->deref(comb);
		}
	}
	else if (prcs->trace == TRACE_REF)
	{
		wWNRef(prcs->fa, wn->previous = t->wnode);
	}
	else /* if (prcs->trace == TRACE_REF_TOTAL) */
	{
		if (!prcs->sr->eq(t->label,d))
			wWNRef(prcs->fa, wn->previous = t->wnode);
	}

	wWNDeref(prcs->fa,t->wnode);
	t->wnode = wn;
	return t;
}

/* Returns the name of the automaton state that post* uses as a new state
   for the given state/stack pair. 					    */
wIdent wsGetMidstate (wIdent state, wIdent stack)
{
	wIdent name;
	char* c1 = wIdentString(state);
	char* c2 = wIdentString(stack);
	char* b = Malloc(strlen(c1) + strlen(c2) + 4);

	sprintf(b,"<%s,%s>",c1,c2);
	name = wIdentCreate(b);
	Free(b);

	return name;
}

/****************************************************************************/
/* Various cases for the augmentation procedure				    */
/* The rv/tv arguments are the value of the rules and transitions	    */
/* being propagated, i.e. full values in batch mode and delta values        */
/* in differential mode.						    */

/* pre*: from <p,y> -> <p',empty> produce (p,y,p') */

void wsPreApply0 (wSat *prcs, wHeadEdge *rule, wSRvalue rv)
{
	wsUpdate(prcs,rule->from->state,rule->from->name,
			rule->to->state,rule->label,
			rule->origin,NULL,NULL);

	/* Having applied the rule once, we remove it from the head table.
	   The following lines assume that rule is the first entry in the
	   rule->to chain (which is guaranteed for the calls in wsPreRule
	   and wsPreProcess), and that we can safely disrupt the chain at
	   rule->from->post (which is the case during pre*).		   */
	rule->to->pre = rule->prenext;
	prcs->sr->deref(rv);
	Free(rule);
}

/* pre*: from <p,y> -> <p',y'> and (p',y',q) produce (p,y,q) */

void wsPreApply1 (wSat *prcs, wHeadEdge *rule, wTrans *t,
				wSRvalue rv, wSRvalue tv)
{
	/* Concerning references, we use the assumption that the extender
	   creates a reference, and that wsUpdate does so as needed.	   */
	wSRvalue d = prcs->sr->extend(rv,tv);
	wsUpdate(prcs,rule->from->state,rule->from->name,t->to,d,
			rule->origin,rule->wnode? rule->wnode : t->wnode,
				     rule->wnode? t->wnode : NULL);
	prcs->sr->deref(d);
}

/* pre*: from <p,y> -> <p',y'y''> and (p',y',q) produce <p,y> -> <q,y''>
	 and (p,y,q') for all (q,y'',q') */

void wsPreApply2 (wSat *prcs, wHeadEdge *rule, wTrans *t,
				wSRvalue rv, wSRvalue tv)
{
	/* Compute the weight for the new rule. */
	wSRvalue d = prcs->sr->extend(rv,tv);

	/* Create the new rule. */
	wHead *head = wHTInsert(prcs->heads,t->to,rule->tail);
	wHeadEdge *newrule = wHTInsertEdge(prcs->heads,rule->from,head,0,d,
						rule->origin,t->wnode);
	prcs->sr->deref(d);

	/* Apply new rule to transitions (q,y'',?). */
	rv = prcs->diff? newrule->delta : newrule->label;
	for (t = head->trans; t; t = t->stack)
		wsPreApply1(prcs,newrule,t,rv,t->label);
	if (prcs->diff)
	{
		prcs->sr->deref(newrule->delta);
		newrule->delta = prcs->sr->zero();
	}
}

/* post*: from t1 = (p,eps,q) and t2 = (q,y,q') produce (p,y,q') */

void wsPostContract (wSat *prcs, wTrans* t1, wTrans* t2,
				 wSRvalue d1, wSRvalue d2)
{
	/* Once we extend this with trace information, we need to do
	   this for every hyperedge leaving (t1,t1->label). */
	wSRvalue d = prcs->sr->extend(d2,d1);
	wsUpdate(prcs,t1->from,t2->name,t2->to,d,
			NULL,t1->wnode,t2->wnode);
	prcs->sr->deref(d);
}

/* post*: from <p,y> -> <p',eps> and (p,y,q) produce (p',eps,q)
   or: from <p,y> -> <p',y'> and (p,y,q) produce (p',y',q) */

void wsPostApply01 (wSat *prcs, wHeadEdge *rule, wTrans *t,
				wSRvalue rv, wSRvalue tv)
{
	wSRvalue d = prcs->sr->extend(tv,rv);
	wsUpdate(prcs,rule->to->state,rule->to->name,t->to,d,
			rule->origin,t->wnode,NULL);
	prcs->sr->deref(d);
}

/* post*: from <p,y> -> <p',y'y''> and (p,y,q)
	  produce (p',y',(p'y')) and ((p'y'),y'',q) */

void wsPostApply2 (wSat *prcs, wHeadEdge *rule, wTrans *t,
				wSRvalue rv, wSRvalue tv)
{
	wSRvalue d2 = prcs->sr->extend(tv,rv);
	wSRvalue d1 = prcs->sr->quasione?
			prcs->sr->quasione(d2) : prcs->sr->one();
	wsUpdate(prcs,rule->to->state,rule->to->name,rule->midstate,d1,
			NULL,NULL,NULL);
	wsUpdate(prcs,rule->midstate,rule->tail,t->to,d2,
			rule->origin,t->wnode,NULL);
	prcs->sr->deref(d1);
	prcs->sr->deref(d2);
}

/****************************************************************************/
/* Augmentation procedure for pushdown rules				    */

void wsPreRule (wSat *prcs, wHeadEdge *rule, wSRvalue pv)
{
	wTrans *t;

	/* If the rule is a pop rule (e.g. <p,y> -> <p',empty>) and there
	   already is a transition starting at p', then apply the rule.
	   Otherwise, application of the rule is postponed until such a
	   transition appears; this is to prevent the process from doing
	   unnecessary work by creating automaton parts that are not
	   connected to a final state.					    */
	if (!rule->to->name)
	{
		if (rule->to->trans) wsPreApply0(prcs,rule,pv);
		return;
	}

	/* At this point, the rule is either <p,y> -> <p',y'> or
	   <p,y> -> <p',y'y''>, so we use it with all transitions (p',y',?). */
	for (t = rule->to->trans; t; t = t->stack)
		(rule->tail? wsPreApply2 : wsPreApply1)
				(prcs,rule,t,pv,t->label);
}

void wsPostRule (wSat *prcs, wHeadEdge *rule, wSRvalue pv)
{
        wHead *phead = wHTInsert(prcs->heads,rule->from->state,0);
	wTrans *t;

	/* Suppose the left-hand side of the rule is <p,y>. If this is the
	   first such rule we encounter, we might have transitions lingering
	   in the (p,0) bucket (i.e. transitions starting at p.) Now that we
	   know that p is a control state, we need to move these to their
	   (p,y) lists. */

        while ((t = phead->trans))
		wHeadInsertTrans(wHTInsert(prcs->heads,t->from,t->name),t);

	/* If we have a pop rule, e.g. <p,y> -> <p',y'y''>, then create
	   a new automaton state q_{p',y'}.				*/
	if (rule->tail)
		rule->midstate = wsGetMidstate(rule->to->state,rule->to->name);

	/* Apply the rule to all transitions (p,y,?). */
	for (t = rule->from->trans; t; t = t->stack)
		(rule->tail? wsPostApply2 : wsPostApply01)
			(prcs,rule,t,pv,t->label);
}

/****************************************************************************/
/* Augmentation procedure for automaton transitions			    */

void wsPreProcess (wSat *prcs, wTrans *t, wSRvalue pv)
{
	wHead *head = wHTInsert(prcs->heads,t->from,t->name);
	wHead *head0 = wHTInsert(prcs->heads,t->from,0);
	wHeadEdge *rule;

	/* Put transition into the (p,y,*)-list. */
	wHeadInsertTrans(head,t);

	/* This is to indicate that there is a transition starting at t->from
	   (this is used by wsPreRule to decide whether a pop rule should be
	   applied immediately). */
	head0->trans = t;

	/* Apply any postponed pop rules (see wsPreRule for more details). */
	while (head0->pre) wsPreApply0(prcs,head0->pre,head0->pre->label);

	/* If the transition is (p,y,q), apply all rules starting with
	   <p,y> on the right-hand side. */
	for (rule = head->pre; rule; rule = rule->prenext)
		(rule->tail? wsPreApply2 : wsPreApply1)
				(prcs,rule,t,rule->label,pv);
}

void wsPostProcess (wSat *prcs, wTrans *t, wSRvalue pv)
{
	wHead *head = wHTInsert(prcs->heads,t->from,t->name);
	wHeadEdge *rule;
	wTrans *t1;

	/* Suppose t = (p,y,q). If there are any rules with (p,y) on their
	   left-hand sides, we can assume that p is a PDS control-state;
	   therefore p should not be the target of any transition.	   */
	if (t->name && head->post)
	{
		/* Put t into the (p,y) list (used in wsPostRule). */
		wHeadInsertTrans(head,t);

		/* Apply all rules with (p,y) on their left-hand side. */
		for (rule = head->post; rule; rule = rule->postnext)
			(rule->tail? wsPostApply2 : wsPostApply01)
						(prcs,rule,t,rule->label,pv);
	}
	/* Otherwise, if y isn't epsilon, p might be a non-initial state... */
	else if (t->name)
	{
		/* ... therefore, we might have to iterate over the
		   transitions starting at p at a later stage, so insert t
		   into the (p,0) bucket. */
		head = wHTInsert(prcs->heads,t->from,0);
		wHeadInsertTrans(head,t);

		/* Connect t with any epsilon transition leading to p. */
		head = wHTInsert(prcs->heads,0,t->from);
		for (t1 = head->trans; t1; t1 = t1->stack)
			wsPostContract(prcs,t1,t,t1->label,pv);
	}
	else
	{
		/* t is an epsilon transition, put it into the (0,q) bucket. */
		head = wHTInsert(prcs->heads,0,t->to);
		wHeadInsertTrans(head,t);

		/* Combine t with any transition starting at q. */
		head = wHTInsert(prcs->heads,t->to,0);
		for (t1 = head->trans; t1; t1 = t1->stack)
			wsPostContract(prcs,t,t1,pv,t1->label);
	}
}

/****************************************************************************/
/* External functions for saturation processes.				    */

wSat* wSatCreate (wSemiring *sr, char direction, char trace)
{
	wSat *prcs = Malloc(sizeof(struct wSat));

	prcs->sr = wSRCopy(sr);
	prcs->dir = direction;
	prcs->fa = wFACreate(sr);
	prcs->fa->dir = direction;
	prcs->trace = trace;
	prcs->heads = wHTCreate(prcs);
	prcs->queue = wHTInsert(prcs->heads,0,0);
	prcs->diff = !!sr->diff;

	return prcs;
}

void wSatAugRule (wSat *prcs, wIdent p0, wIdent y0,
		wIdent p1, wIdent y1, wIdent y2, wSRvalue d, wRule *orig)
{
	wHead *from = wHTInsert(prcs->heads,p0,y0);
	wHead *to   = wHTInsert(prcs->heads,p1,y1);
	wHeadEdge *rule = wHTInsertEdge(prcs->heads,from,to,y2,d,orig,NULL);

	wSRvalue pv;		/* the value being propagated */

	if (prcs->diff)
	{
		pv = rule->delta;
		prcs->sr->ref(rule->delta = prcs->sr->zero());
	}
	else prcs->sr->ref(pv = rule->label);

	(prcs->dir == SAT_PRE? wsPreRule : wsPostRule)(prcs,rule,pv);
	prcs->sr->deref(pv);
}

wTrans* wSatAddTrans (wSat *prcs, wIdent from, wIdent name, wIdent to,
			wSRvalue d)
{
	return wsUpdate(prcs,from,name,to,d,NULL,NULL,NULL);
}

void wSatAugTrans (wSat *prcs, wTrans *t)
{
	wSRvalue pv;		/* the value being propagated */

	if (prcs->diff)
	{
		pv = t->delta;
		prcs->sr->ref(t->delta = prcs->sr->zero());
	}
	else prcs->sr->ref(pv = t->label);

	(prcs->dir == SAT_PRE? wsPreProcess : wsPostProcess)(prcs,t,pv);
	prcs->sr->deref(pv);
}

wTrans* wSatPopTrans (wSat *prcs)
{
	wTrans *t = prcs->queue->trans;
	if (!t) return NULL;
	wHeadRemoveTrans(t);
	return t;
}

wFA* wSatDelete (wSat *prcs)
{
	wFA *result = prcs->fa;
	wHTDelete(prcs->heads);
	Free(prcs->sr);
	Free(prcs);
	return result;
}
