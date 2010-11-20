void assert_(int cond, char* file, int line)
{
	if (!(cond))
	{
		printf("\nAssertion violation at %s:%d\n",file,line);
		exit(1);
	}
}

void deref (DdNode **b)
{
	assert (*b != NULL);
	Cudd_RecursiveDeref(mgr,*b);
	*b = NULL;
}

void extendShuffle (bddPds *newPds, void *data)
{
	bddPds *oldPds = (bddPds*)(data);
	int i;
	
 	for (i = 0; i < oldPds->num_globals; i++)
 	{
 		newPds->g0[i] = oldPds->g0[i];
 		newPds->g1[i] = oldPds->g1[i];
 		newPds->g2[i] = oldPds->g2[i];
 	}
	for (i = 0; i < oldPds->num_locals; i++)
	{
		newPds->l0[i] = oldPds->l0[i];
		newPds->l1[i] = oldPds->l1[i];
		newPds->l2[i] = oldPds->l2[i];
		newPds->l3[i] = oldPds->l3[i];
	}

	for (i = oldPds->num_locals; i < newPds->num_locals; i++)
	{
		newPds->l0[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
		newPds->l1[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
		newPds->l2[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
		newPds->l3[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
	}
 	for (i = oldPds->num_globals; i < newPds->num_globals; i++)
 	{
 		newPds->g0[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
 		newPds->g1[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
 		newPds->g2[i] = Cudd_bddNewVarAtLevel(newPds->mgr,0);
 	}
}
		
void updateConcPerm (int **perm)
{
	assert (*perm != NULL);
	int *newPerm;
	bddInitPermutation(mgr,&newPerm);
	memcpy(newPerm, *perm,
	       concPds->helpers->array_size * sizeof(int));
	Free(*perm);
	*perm = newPerm;
}

void updatePerms ()
{
/*	updateConcPerm(&(concPds->helpers->perm_)); */
	updateConcPerm(&(concPds->helpers->perm_g0l0_g1l1));
	updateConcPerm(&(concPds->helpers->perm_g2l2_g1l1));
	updateConcPerm(&(concPds->helpers->perm_g0l0g1_g1l1g2));
	updateConcPerm(&(concPds->helpers->perm_g1l1_g0l0));
	updateConcPerm(&(concPds->helpers->perm_g1g2l3_g0g1l1));
	updateConcPerm(&(concPds->helpers->perm_l2_l0));
	updateConcPerm(&(concPds->helpers->perm_l0_l2));

	updateConcPerm(&(helpers->c_perm_G0_G1));
	updateConcPerm(&(helpers->c_perm_G0G1L1_G1G2L3));
}

void computeConcHelpers (absRefHelpers *helpers)
{
	assert(concPds != NULL);
	DdNode **G0 = concPds->g0;
	DdNode **G1 = concPds->g1;
	DdNode **G2 = concPds->g2;
	DdNode **L0 = concPds->l0;
	DdNode **L1 = concPds->l1;
	DdNode **L2 = concPds->l2;
	DdNode **L3 = concPds->l3;
	int num_globals = concPds->num_globals;
	int num_locals  = concPds->num_locals;

	int *perm;
	/* compute c_perm_G0_G1 */
	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,G0,G1,num_globals);
	helpers->c_perm_G0_G1 = perm;
	/* don't forget the update in updatePerms */
	/* compute c_perm_G0G1L1_G1G2L3 */
	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,G0,G1,num_globals);
	bddAddToPermutation(mgr,perm,G1,G2,num_globals);
	bddAddToPermutation(mgr,perm,L1,L3,num_locals);
	helpers->c_perm_G0G1L1_G1G2L3 = perm;
	/* don't forget the update in updatePerms */

	int i;
	DdNode *bdd, *tmp0, *tmp1;
	/* compute eqv_G0G1_L0L1 */
	Cudd_Ref(bdd = one);
	for (i=num_globals-1; i>=0; i--)
	{
		Cudd_Ref(tmp0 = Cudd_bddXnor(mgr,G0[i],G1[i]));
		Cudd_Ref(tmp1 = Cudd_bddAnd(mgr,bdd,tmp0));
		Cudd_RecursiveDeref(mgr,bdd);
		Cudd_RecursiveDeref(mgr,tmp0);
		bdd = tmp1;
	}
	for (i=num_locals-1; i>=0; i--)
	{
		Cudd_Ref(tmp0 = Cudd_bddXnor(mgr,L0[i],L1[i]));
		Cudd_Ref(tmp1 = Cudd_bddAnd(mgr,bdd,tmp0));
		Cudd_RecursiveDeref(mgr,bdd);
		Cudd_RecursiveDeref(mgr,tmp0);
		bdd = tmp1;
	}
	helpers->eqv_G0G1_L0L1 = bdd;
	/* compute eqv_G1G2_L1L3 */
	Cudd_Ref(bdd = one);
	for (i=num_globals-1; i>=0; i--)
	{
		Cudd_Ref(tmp0 = Cudd_bddXnor(mgr,G1[i],G2[i]));
		Cudd_Ref(tmp1 = Cudd_bddAnd(mgr,bdd,tmp0));
		Cudd_RecursiveDeref(mgr,bdd);
		Cudd_RecursiveDeref(mgr,tmp0);
		bdd = tmp1;
	}
	for (i=num_locals-1; i>=0; i--)
	{
		Cudd_Ref(tmp0 = Cudd_bddXnor(mgr,L1[i],L3[i]));
		Cudd_Ref(tmp1 = Cudd_bddAnd(mgr,bdd,tmp0));
		Cudd_RecursiveDeref(mgr,bdd);
		Cudd_RecursiveDeref(mgr,tmp0);
		bdd = tmp1;
	}
	helpers->eqv_G1G2_L1L3 = bdd;

	int *cube;
	/* compute cube_G0 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G0,num_globals);
	helpers->cube_G0 = bddCreateCube(mgr,cube);
	/* compute cube_G1 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	helpers->cube_G1 = bddCreateCube(mgr,cube);
	/* compute cube_G0L0 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G0,num_globals);
	bddAddVarsToCube(mgr,cube,L0,num_locals);
	helpers->cube_G0L0 = bddCreateCube(mgr,cube);
	/* compute cube_G1L1 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	bddAddVarsToCube(mgr,cube,L1,num_locals);
	helpers->cube_G1L1 = bddCreateCube(mgr,cube);
	/* compute cube_G2L3 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G2,num_globals);
	bddAddVarsToCube(mgr,cube,L3,num_locals);
	helpers->cube_G2L3 = bddCreateCube(mgr,cube);
	/* compute cube_G0L0L2 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G0,num_globals);
	bddAddVarsToCube(mgr,cube,L0,num_locals);
	bddAddVarsToCube(mgr,cube,L2,num_locals);
	helpers->cube_G0L0L2 = bddCreateCube(mgr,cube);
	/* compute cube_G1L1L2 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	bddAddVarsToCube(mgr,cube,L1,num_locals);
	bddAddVarsToCube(mgr,cube,L2,num_locals);
	helpers->cube_G1L1L2 = bddCreateCube(mgr,cube);
	/* compute cube_G2L0L3 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G2,num_globals);
	bddAddVarsToCube(mgr,cube,L0,num_locals);
	bddAddVarsToCube(mgr,cube,L3,num_locals);
	helpers->cube_G2L0L3 = bddCreateCube(mgr,cube);
	/* compute cube_G0G1L1 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G0,num_globals);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	bddAddVarsToCube(mgr,cube,L1,num_locals);
	helpers->cube_G0G1L1 = bddCreateCube(mgr,cube);
	/* compute cube_G1G2L3 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	bddAddVarsToCube(mgr,cube,G2,num_globals);
	bddAddVarsToCube(mgr,cube,L3,num_locals);
	helpers->cube_G1G2L3 = bddCreateCube(mgr,cube);
	/* compute cube_G1G2L1L3 */
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,G1,num_globals);
	bddAddVarsToCube(mgr,cube,G2,num_globals);
	bddAddVarsToCube(mgr,cube,L1,num_locals);
	bddAddVarsToCube(mgr,cube,L3,num_locals);
	helpers->cube_G1G2L1L3 = bddCreateCube(mgr,cube);
}

void initAbsHelpers (absRefHelpers *helpers)
{
	/* helpers->a_perm_l1_l2 = NULL; */
	helpers->cube_g0g1g2l0l1l2l3 = NULL;
	helpers->cube_g2l3 = NULL;
}

void computeAbsHelpers (bddPds *absPds, absRefHelpers *helpers)
{
	assert(absPds != NULL);
	DdNode **g0 = absPds->g0;
	DdNode **g1 = absPds->g1;
	DdNode **g2 = absPds->g2;
	DdNode **l0 = absPds->l0;
	DdNode **l1 = absPds->l1;
	DdNode **l2 = absPds->l2;
	DdNode **l3 = absPds->l3;
	int num_globals = absPds->num_globals;
	int num_locals  = absPds->num_locals;

	int *cube;
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,g0,num_globals);
	bddAddVarsToCube(mgr,cube,g1,num_globals);
	bddAddVarsToCube(mgr,cube,g2,num_globals);
	bddAddVarsToCube(mgr,cube,l0,num_locals);
	bddAddVarsToCube(mgr,cube,l1,num_locals);
	bddAddVarsToCube(mgr,cube,l2,num_locals);
	bddAddVarsToCube(mgr,cube,l3,num_locals);
	if (helpers->cube_g0g1g2l0l1l2l3 != NULL)
		deref(&(helpers->cube_g0g1g2l0l1l2l3));
	helpers->cube_g0g1g2l0l1l2l3 = bddCreateCube(mgr,cube);
	/* see also initAbsHelpers and freeHelpers */
	
	bddInitCube(mgr,&cube);
	bddAddVarsToCube(mgr,cube,g2,num_globals);
	bddAddVarsToCube(mgr,cube,l3,num_locals);
	if (helpers->cube_g2l3 != NULL)
		deref(&(helpers->cube_g2l3));
	helpers->cube_g2l3 = bddCreateCube(mgr,cube);
	/* see also initAbsHelpers and freeHelpers */
	
	/*
	int *perm; 
	// compute a_perm_l1_l2 
	bddInitPermutation(mgr,&perm);
	bddAddToPermutation(mgr,perm,l1,l2,num_locals);
	if (helpers->a_perm_l1_l2 != NULL) Free(helpers->a_perm_l1_l2);
	helpers->a_perm_l1_l2 = perm;
	*/
}

void freeHelpers (absRefHelpers *helpers)
{
	Free(helpers->c_perm_G0_G1);
	Free(helpers->c_perm_G0G1L1_G1G2L3);
	/* Free(helpers->a_perm_l1_l2); */
	if (helpers->cube_g0g1g2l0l1l2l3) deref(&(helpers->cube_g0g1g2l0l1l2l3));
	if (helpers->cube_g2l3) deref(&(helpers->cube_g2l3));

	deref(&(helpers->eqv_G0G1_L0L1));
	deref(&(helpers->eqv_G1G2_L1L3));
	deref(&(helpers->cube_G0));
	deref(&(helpers->cube_G1));
	deref(&(helpers->cube_G0L0));
	deref(&(helpers->cube_G1L1));
	deref(&(helpers->cube_G2L3));
	deref(&(helpers->cube_G0L0L2));
	deref(&(helpers->cube_G1L1L2));
	deref(&(helpers->cube_G2L0L3));
	deref(&(helpers->cube_G0G1L1));
	deref(&(helpers->cube_G1G2L3));
	deref(&(helpers->cube_G1G2L1L3));
	Free(helpers);
}
	

/* concatenate *s1 and s2, write the result again in *s1,
   which is a newly allocated string */
void append (char **s1, char *s2)
{
	int len1 = strlen(*s1);
	int len2 = strlen(s2);
	char *res;
	/* use malloc and free here instead of Malloc and Free, because
	   memory is also allocated via strdup */
	res = malloc(sizeof(char*) * (len1+len2) + 1);
	strcpy(res,*s1);
	strcat(res,s2);
	free(*s1); 
	*s1 = res;
}
		
ruleType rTypeAbs (wRule *absRule)
{
	if (absRule == NULL) return RULE_COMBINE;
	wRule *concRule = (wRule*)(absRule->user);
	return *(ruleType*)(concRule->user);
}

ruleType rTypeConc (wRule *concRule)
{
	if (concRule == NULL) return RULE_COMBINE;
	return *(ruleType*)(concRule->user);
}

char* rTypeString (ruleType rt)
{
	switch (rt)
	{
	case RULE_NORMAL: return "RULE_NORMAL";
	case RULE_PUSH:   return "RULE_PUSH";
	case RULE_POP:    return "RULE_POP";
	case RULE_COMBINE:return "RULE_COMBINE";
	case RULE_EVAL:   return "RULE_EVAL";
	case RULE_ENTRY:  return "RULE_ENTRY";
	default: assert(0); exit(0);
	}
}

void printRule (wRule *r)
{
	printf("rule: <%s,%s> -> <%s",
	       wIdentString(r->from_state),
	       wIdentString(r->from_stack),
	       wIdentString(r->to_state));
	if (r->to_stack1)
		printf(",%s",wIdentString(r->to_stack1));
	if (r->to_stack2)
		printf(" %s",wIdentString(r->to_stack2));
	printf(">\n");
}

int implies (DdNode *A, DdNode *B)
{
	assert (A != NULL);
	assert (B != NULL);
	DdNode *tmp;
	Cudd_Ref(tmp = Cudd_bddOr(mgr,Cudd_Not(A),B));
	int res = (tmp == one);
	Cudd_RecursiveDeref(mgr,tmp);
	return res;
}

			 
void weakenToInterp (DdNode **I, DdNode *J)
/* change *I to the strongest interpolant of *I and J. */
{
	assert (*I != NULL);
	assert ( J != NULL);
	if (DEBUG) assert (implies(*I, J));
	if (J == one || *I == one || *I == zero) return;
	DdNode *common, *onlyI, *onlyJ;
	assert (Cudd_ClassifySupport(mgr, *I, J, &common, &onlyI, &onlyJ));
	Cudd_Ref(common);
	Cudd_Ref(onlyI);
	Cudd_Ref(onlyJ);
	if (onlyI != one)
	{
		DdNode *new;
		Cudd_Ref(new = Cudd_bddExistAbstract(mgr, *I, onlyI));
		if (DEBUG) assert (implies(new, J));
		Cudd_RecursiveDeref(mgr, *I);
		*I = new;
	}
	Cudd_RecursiveDeref(mgr,common);
	Cudd_RecursiveDeref(mgr,onlyI);
	Cudd_RecursiveDeref(mgr,onlyJ);
}

void strengthenToInterp (DdNode **J, DdNode *I)
/* change *J to the weakest interpolant of I and *J. */
{
	assert ( I != NULL);
	assert (*J != NULL);
	if (DEBUG) assert (implies(I, *J));
	if (I == zero || *J == one || *J == zero) return;
	//if (*J == one || *J == zero) return;
	//if (*J == zero) return;
	DdNode *common, *onlyI, *onlyJ;
	assert (Cudd_ClassifySupport(mgr, I, *J, &common, &onlyI, &onlyJ));
	Cudd_Ref(common);
	Cudd_Ref(onlyI);
	Cudd_Ref(onlyJ);
	if (onlyJ != one)
	{
		/* printf("                                     strengthening a J\n"); */
		DdNode *new;
		Cudd_Ref(new = Cudd_bddUnivAbstract(mgr, *J, onlyJ));
		if (DEBUG) assert (implies(I, new));
		Cudd_RecursiveDeref(mgr, *J);
		*J = new;
	}
	/* else printf("not strengthening a J\n"); */
	Cudd_RecursiveDeref(mgr,common);
	Cudd_RecursiveDeref(mgr,onlyI);
	Cudd_RecursiveDeref(mgr,onlyJ);
}

void conciliate (DdNode **I, DdNode **J)
/* iterate weakenToInterp and strengthenToInterp until a fixpoint is reached */
{
	assert (*I != NULL);
	assert (*J != NULL);
	DdNode *oldJ;
	if (DEBUG) assert (implies(*I, *J));
	int i;
	for (i = 0; ; i++)
	{
		weakenToInterp(I,*J);
		oldJ = *J; /* just a temporary copy */
		strengthenToInterp(J,*I);
		if (oldJ == *J) break;
	}
	if (VERBOSE) if (i > 0) fprintf(stderr,
		  "                           Conciliated %d\n",i);
}

void strengthenMaximally (DdNode **J, DdNode *I)
/* strengthen J by (universally) quantifying out a maximal
   number of BDD variables, such that I still implies *J.
*/
{
	assert ( I != NULL);
	assert (*J != NULL);
	if (DEBUG) assert (implies(I, *J));
	if (I == zero || *J == one || *J == zero) return;
	if (*J == I) return;
	DdNode *new, *best, *var;

	int *indices = Cudd_SupportIndex(mgr, *J);
	int i;
	best = *J;
	for (i = Cudd_ReadSize(mgr)-1; i>=0; i--)
	{
		if (best == I) break;
		if (indices[i] == 0) continue;
		var = Cudd_ReadVars(mgr, i);
/*		Cudd_PrintMinterm(mgr,var); */
		assert (var != NULL);
		Cudd_Ref(new = Cudd_bddUnivAbstract(mgr, best, var));
		if (DEBUG) assert (implies(new,best));
 		if ((new != best) && (implies(I, new)))
 		{
 			Cudd_RecursiveDeref(mgr, best);
 			best = new;
 		}
 		else
			Cudd_RecursiveDeref(mgr, new);
	}
	free(indices);
	*J = best;
}

