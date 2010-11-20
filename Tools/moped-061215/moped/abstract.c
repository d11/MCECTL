void collAddPred (DdNode **coll, DdNode *conc, DdNode *abs)
/* *coll = *coll AND (conc <-> abs) */
{
	assert(conc != NULL && abs != NULL && coll != NULL && *coll != NULL);
	DdNode *equiv, *collOld;
	Cudd_Ref(equiv = Cudd_bddXnor(mgr,conc,abs));
	collOld = *coll;
	Cudd_Ref(*coll = Cudd_bddAnd(mgr,collOld,equiv));
	Cudd_RecursiveDeref(mgr,collOld);
	Cudd_RecursiveDeref(mgr,equiv);
}
	
void collRelabel (DdNode *collOld, DdNode **collNew, int *perm1, int *perm2)
/* *collNew = collOld [perm1][perm2] */
{
	assert(perm1 != NULL);
	DdNode *beta;
	Cudd_RecursiveDeref(mgr,*collNew);
	Cudd_Ref(beta = Cudd_bddPermute(mgr,collOld,perm1));
	if (perm2)
	{
		Cudd_Ref(*collNew = Cudd_bddPermute(mgr,beta,perm2));
		Cudd_RecursiveDeref(mgr,beta);
	}
	else *collNew = beta;
}

void collect (locationInfo *li, bddPds *absPds)
{
	assert(li != NULL);
	assert(li->type == LOC_GLOBAL || li->nPreds <= maxLPreds);
	int i;
	predicateList *preds = li->preds;
	if (li->collFlag == li->nPreds) return; /* this line is optional */
	for (i = li->nPreds - 1; i >= li->collFlag; i--)
	{
		assert (preds != NULL);
		assert (i == preds->id);
		collAddPred(&(li->coll0), preds->J,
			    li->type == LOC_GLOBAL ? absPds->g0[i] : absPds->l0[i]);
		preds = preds->next;
	}		
	li->collFlag = li->nPreds;

	switch(li->type)
	{
	case LOC_NORMAL:
		collRelabel(li->coll0, &(li->coll1),
			    absPds->helpers->perm_g0l0_g1l1,
			    concPds->helpers->perm_g0l0_g1l1);
		break;
	case LOC_ENTRY:
		collRelabel(li->coll0, &(li->coll1),
			    absPds->helpers->perm_g0l0_g1l1, NULL);
		break;
	case LOC_CALL:
		collRelabel(li->coll0, &(li->coll1),
			    absPds->helpers->perm_l0_l2,
			    concPds->helpers->perm_l0_l2);
		break;
	case LOC_GLOBAL:
		collRelabel(li->coll0, &(li->coll1),
			    absPds->helpers->perm_g0l0_g1l1,
			    helpers->c_perm_G0G1L1_G1G2L3);
		break;
	default: assert(0);
	}
	assert(li->coll0 != zero && li->coll1 != zero);
}

void collectAll (bddPds *absPds)
{
	int i;
	/* collect local preds */
	for (i=0; i<NLocs; i++) collect(locInf[locs[i]], absPds);
	/* collect global preds */
	collect(globInf, absPds);
}

DdNode* abstractRule (wRule *concRule, bddPds *absPds)
/* return the bdd of the abstract rule */
{
	assert(locInf[concRule->from_stack] != NULL);
	DdNode *p1, *p2, *p3;
	switch (rTypeConc(concRule))
	{
	case RULE_NORMAL:
		Cudd_Ref(p1 = cachedAndAbstract(
				 locInf[concRule->from_stack]->coll0,
				 ((bddDomain*)(concRule->label))->bdd,/* conc. rule*/
				 helpers->cube_G0L0));
		Cudd_Ref(p2 = cachedAndAbstract(
				 p1,
				 locInf[concRule->to_stack1]->coll1,
				 helpers->cube_G1G2L1L3));
		assert(concRule->to_stack2 == 0);
		Cudd_RecursiveDeref(mgr,p1);
		return p2;
	case RULE_PUSH:
		Cudd_Ref(p1 = cachedAndAbstract(
				 locInf[concRule->from_stack]->coll0,
				 locInf[concRule->to_stack2]->coll1,
				 helpers->cube_G2L3));
		Cudd_Ref(p2 = cachedAndAbstract(
				 p1,
				 ((bddDomain*)(concRule->label))->bdd,/* conc. rule*/
				 helpers->cube_G0L0L2));
		Cudd_Ref(p3 = cachedAndAbstract(
				 p2,
				 locInf[concRule->to_stack1]->coll1,
				 helpers->cube_G1L1));
		Cudd_RecursiveDeref(mgr,p1);
		Cudd_RecursiveDeref(mgr,p2);
		return p3;
	case RULE_EVAL:
		Cudd_Ref(p1 = Cudd_bddPermute(mgr,
					      locInf[concRule->to_stack1]->coll0,
					      absPds->helpers->perm_g0l0_g1l1));
		Cudd_Ref(p2 = cachedAndAbstract(
				 locInf[concRule->from_stack]->coll0,
				 p1,
				 helpers->cube_G2L0L3));
		Cudd_Ref(p3 = cachedAndAbstract(
				 p2,
				 globInf->coll0,
				 helpers->cube_G0G1L1));
		Cudd_RecursiveDeref(mgr,p1);
		Cudd_RecursiveDeref(mgr,p2);
		return p3;
	case RULE_ENTRY:
		Cudd_Ref(p1 = cachedAndAbstract(
				 locInf[concRule->to_stack1]->coll1,
				 helpers->eqv_G1G2_L1L3,
				 helpers->cube_G2L3));
		Cudd_Ref(p2 = cachedAndAbstract(
				 p1,
				 locInf[concRule->from_stack]->coll0,
				 helpers->cube_G1L1));
		Cudd_RecursiveDeref(mgr,p1);
		return p2;
	case RULE_POP:
		Cudd_Ref(p1 = cachedAndAbstract(
				 locInf[concRule->from_stack]->coll0,
				 ((bddDomain*)(concRule->label))->bdd,/* conc. rule*/
				 helpers->cube_G0L0));
		Cudd_Ref(p2 = cachedAndAbstract(
				 p1,
				 globInf->coll1,
				 helpers->cube_G1G2L3));
		Cudd_RecursiveDeref(mgr,p1);
		return p2;
	default: assert(0); exit(0);
	}
}
	

bddPds* abstract (bddPds *old_absPds)
{
	bddPds *absPds;
	assert(concPds != NULL);

	printf("abstract(): maxLPreds = %d\n", maxLPreds);
	printf("            globInf->nPreds = %d\n", globInf->nPreds);	
	if (VERBOSE) printf("bddPdsCreate\n");
	if (old_absPds == NULL)
		absPds = bddPdsCreate(concPds->start,globInf->nPreds,maxLPreds,
				      NULL,NULL);
	else
		absPds = bddPdsCreate(concPds->start,globInf->nPreds,maxLPreds,
				      extendShuffle,old_absPds);
	if (VERBOSE) printf("Cudd_ReadSize() == %d\n",Cudd_ReadSize(mgr));
	computeAbsHelpers(absPds,helpers);
	updatePerms();
	assert(absPds->num_globals == globInf->nPreds);
	assert(absPds->num_locals  == maxLPreds);
	printf("            collectAll\n");
	collectAll(absPds);
	printf("            create abstract rules ... ");
	wRule *rule, *absRule;
	DdNode *absBdd;
	for (rule = concPds->pds->rules; rule; rule = rule->next)
	{
		absBdd = abstractRule(rule,absPds);
		absRule = bddPdsInsert(
			absPds,
			rule->from_state,rule->from_stack,
			rule->to_state,rule->to_stack1,rule->to_stack2,
			absBdd);
		Cudd_RecursiveDeref(mgr,absBdd);
		absRule->user = rule;/* pointer to the corresponding concrete rule*/
/*		printRule(absRule);
 		Cudd_PrintMinterm(mgr,absBdd);
*/
		/* assert(absBdd != zero); */
	}
	printf("done\n");
	return absPds;
}
