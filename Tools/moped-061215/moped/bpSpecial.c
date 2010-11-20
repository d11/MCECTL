/***************************************************************************/
/* contains all the messy call/return code				   */
/***************************************************************************/

int *bp_me_xlat2;
int bp_me_fun;
char *bp_sc_flag;

extern int mc_encoding;

void bp_fnv_recur (bp_expr_t expr)
{
	switch (expr->token)
	{
	    case BP_EQ:
	    case BP_NE:
	    case BP_AND:
	    case BP_OR:
	    case BP_IMP:
	    case BP_CHOOSE:
			bp_fnv_recur(expr->right);
	    case BP_NOT:
			bp_fnv_recur(expr->left);
			return;
		case BP_IDENT: {
			bp_ident_t id = ((bp_idref_t)(expr->left))->ident;
			/* if it's a local, mark it */
			if (id->fun > 0) bp_sc_flag[bpa_lxlat[id->index]]++;
			/* if it's a shifted global, mark the original local */
			if (!id->fun && bp_me_xlat2[id->index] != -1)
				bp_sc_flag[bpa_lxlat[bp_me_xlat2[id->index]]]++;
			if (id->fun < 0) bp_sc_flag[bpa_gxlat[-id->fun-1]]++;
			return; }
	    case BP_CONST:
	    case BP_ND:
			return;
	    default:
			bpError("can't happen");
	}
}

void bp_find_needed_vars (bp_fun_t fun, bp_idref_t ref)
{
	memset(bp_sc_flag,0,bpa_size);
	while (ref)
	{
		bp_fnv_recur(ref->expr);
		ref = ref->next;
	}
}

/***************************************************************************/

void bp_re_recur (bp_expr_t expr)
{
	switch (expr->token)
	{
	    case BP_EQ:
	    case BP_NE:
	    case BP_AND:
	    case BP_OR:
	    case BP_IMP:
	    case BP_CHOOSE:
			bp_re_recur(expr->right);
	    case BP_NOT:
			bp_re_recur(expr->left);
			return;
		case BP_IDENT: {
			bp_ident_t id = ((bp_idref_t)(expr->left))->ident;
			if (id->fun || bp_me_xlat2[id->index] == -1) return;
			id->fun = bp_me_fun; id->index = bp_me_xlat2[id->index];
			return; }
	    case BP_CONST:
	    case BP_ND:
			return;
	    default:
			bpError("can't happen");
	}
}

void bp_restore_exprs (bp_idref_t ref)
{
	while (ref)
	{
		bp_re_recur(ref->expr);
		ref = ref->next;
	}
}

/***************************************************************************/

void bp_call_extra (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
			wIdent fromstate, wIdent restorelabel)
{
	DdNode *tmp0;
	bp_ident_t var;

	memset(bp_flag,BP_EQUIV,bpa_size);
	Cudd_Ref(tmp0 = mc_pds->one);
	tmp0 = bpa_build_expr(fun,tmp0);
	for (var = target->returns; var; var = var->next)
	{
	    DdNode *tmp1;
	    Cudd_Ref(tmp1 = Cudd_bddXnor
			    (mc_pds->mgr,mc_pds->g1[var->index],mc_pds->g0[var->index]));
	    tmp0 = bp_add_clause(tmp0,tmp1);
	    Cudd_RecursiveDeref(mc_pds->mgr,tmp1);
	}

	bddPdsInsert(mc_pds,fromstate,restorelabel,
			bp_qstate,restorelabel,0,tmp0);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
}

wIdent bp_tmpstate(bp_fun_t fun, bp_fun_t target)
{
	wIdent tmpstate;
	static int count = 0;
	char *s = Malloc(strlen(wIdentString(fun->funname))
			+ strlen(wIdentString(target->funname)) + 10);
	sprintf(s,"%s-%s-%d",wIdentString(fun->funname),
			     wIdentString(target->funname),++count);
	tmpstate = wIdentCreate(s);
	Free(s);
	return tmpstate;
}

/***************************************************************************/

void bp_special_call (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
						wIdent label1, wIdent retlabel)
{
	DdNode *tmp1, *tmp2;
	wIdent label2, tmplabel;
	wIdent targ1 = 0, targ2;
	bp_idref_t ref = stmt->e.a.fmllist;
	bp_ident_t fml = target->parms;
	bp_ident_t var, cb;
	int i, base = 0;
	int stepsize = bp_stepsize;
	int *occbyfmls = Malloc(bp_maxlocs * sizeof(int));

	bp_sc_flag = Malloc(bpa_size * sizeof(char));
	bp_me_xlat2 = Malloc(bp_maxrets * sizeof(int));
	memset(occbyfmls,0xff,bp_maxlocs * sizeof(int));
	memset(bp_me_xlat2,0xff,bp_maxrets * sizeof(int));

	while (base < target->num_args)
	{
		bpc_clearflags(fun);
	
		/* add clause to set one parm */
		for (i = 0; i < stepsize; i++)
		{
			if (!ref) break;
			tmp1 = bp_make_choose_expr(mc_pds->l1[fml->index],ref->expr);
			bp_set_expr(bpc_lxlat[fml->index],tmp1);

			/* indicate that the parm's index is now used */
			occbyfmls[fml->index] = base;
			ref = ref->next;
			fml = fml->next;
		}

		/* mark all local variables in the remaining expressions */
		bp_find_needed_vars(fun,ref);

		/* save the locals we still need */
		for (var = fun->locals, cb = fun->callbuf; var;
				var = var->next, cb = cb->next)
		{
			if (!bp_sc_flag[bpa_lxlat[var->index]]) continue;

			if (occbyfmls[var->index] >= 0
					&& occbyfmls[var->index] < base)
			{
			   /* if the var has been overwritten in an earlier
			    * iteration and still needed, keep it in globals */		
				bp_flag[bpc_gxlat[cb->index]] = BP_EQUIV;
			}
			else if (occbyfmls[var->index] == base)
			{
				/* if the var has just been overwritten,
				 * shift it into the globals */
				Cudd_Ref(tmp1 = Cudd_bddXnor
				  (mc_pds->mgr,mc_pds->g1[cb->index],mc_pds->l0[var->index]));
				bp_set_expr(bpc_gxlat[cb->index],tmp1);

				/* set up index translation for restoring
				 * the expressions */
				bp_me_xlat2[cb->index] = var->index;
				bp_me_fun = var->fun;
				/* manipulate expressions to reflect shift */
				var->fun = 0;
				var->index = cb->index;
			}
			else
			{
				/* if it's not been overwritten yet,
				 * keep it in the locals */
				bp_flag[bpc_lxlat[var->index]] = BP_EQUIV;
			}
		}

		/* take care of saved globals that have been shifted
		 * into locals earlier */
		if (call_crosses_comp(fun,target))
		{
			for (var = bp_globals; var; var = var->next)
			{
				if (!var->fun) continue;

				/* in the first iteration, put saved globals
				 * onto the stack */
				if (!base)
				    bp_flag[bpc_sxlat[var->index]] = BP_EQUIV;

				/* keep saved globals around if still needed;
				   these are not caught by the previous loop */
				if (!bp_sc_flag[bpa_gxlat[-var->fun-1]])
					continue;
				bp_flag[bpc_lxlat[var->index]] = BP_EQUIV;
			}
		}

		/* keep all parms that were set in previous iterations */
		for (var = target->parms; occbyfmls[var->index] < base;
				var = var->next)
			bp_flag[bpc_lxlat[var->index]] = BP_EQUIV;

		/* save the globals which are also in the called function */
		for (var = bp_globals; var; var = var->next)
			if (!mc_globals ||
			      (!var->fun && fun->funnode->usedglob[var->index]))
				bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

		/* in the first iteration, save locals on stack */
		if (!base)
			for (var = fun->locals; var; var = var->next)
				bp_flag[bpc_sxlat[var->index]] = BP_EQUIV;

		Cudd_Ref(tmp2 = mc_pds->one);
		tmp2 = bpc_build_expr(fun,tmp2);
		
		/* now just add the rule */
		label2 = ref? bp_create_label(fun,NULL,0)
			: bp_create_label(target,target->stmt,0);
		tmplabel = base? bp_create_label(fun,NULL,0) : retlabel;

		bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,
			tmplabel, tmp2);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);

		/* determine control locations used return statements */
		targ2 = targ1;
		targ1 = !mc_encoding? bp_qstate :
			(ref? bp_tmpstate(fun,target) : target->funname);
		if (!base && mc_encoding)
			bp_call_extra(fun,stmt,target,targ1,retlabel);

		label1 = label2;
		base += stepsize;

		/* in the first iteration, do not create an additional return */
		if (base == stepsize) continue;

		/* on additional returns, just save the returns and globals */
		bpc_clearflags(fun);

		for (var = bp_globals; var; var = var->next)
			if (!mc_globals ||
			      (!var->fun && fun->funnode->usedglob[var->index]))
				bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

		for (var = target->returns; var; var = var->next)
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

		Cudd_Ref(tmp2 = mc_pds->one);
		tmp2 = bpc_build_expr(fun,tmp2);
		bddPdsInsert(mc_pds,targ1,tmplabel,targ2,0,0,tmp2);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
	}

	bp_restore_exprs(stmt->e.a.fmllist);
	Free(occbyfmls);
	Free(bp_me_xlat2);
	Free(bp_sc_flag);
}

/***************************************************************************/

void bp_special_eval (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
						wIdent label1, wIdent retlabel)
{
	DdNode *tmp1, *tmp2;
	wIdent label2;
	bp_idref_t ref = stmt->e.a.asgnlist;
	bp_ident_t ret = target->returns, ret2;
	int i, index, base = 0;
	int stepsize = bp_stepsize;

	while (base < target->num_returns)
	{
		bpc_clearflags(fun);

		for (i = 0; i < stepsize; i++)
		{
			if (!ref) break;
			
			/* evaluate return values */
			Cudd_Ref(tmp1
				= Cudd_bddXnor(mc_pds->mgr,bp_get_var(ref),
					mc_pds->g0[ret->index]));
			index = (ref->ident->fun? bpc_lxlat : bpc_gxlat)
						[ref->ident->index];
			bp_set_expr(index,tmp1);

			ref = ref->next;
			ret = ret->next;
		}

		/* save the return values which are yet to be evaluated */
		for (ret2 = ret; ret2; ret2 = ret2->next)
			bp_flag[bpc_gxlat[ret2->index]] = BP_EQUIV;

		/* keep the other globals and live locals */
		bpc_add_equivs(fun,stmt->live);
		
		Cudd_Ref(tmp2 = mc_pds->one);
		tmp2 = bpc_build_expr(fun,tmp2);

		/* if it's the last eval rule, apply the enforce condition */
		if (!ref) tmp2 = bp_add_clause(tmp2,bp_enforce_expr);

		label2 = ref? bp_create_label(fun,NULL,0) : retlabel;
		bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,0,tmp2);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);

		label1 = label2;
		base += stepsize;
	}
}

/***************************************************************************/

void bp_special_return (bp_fun_t fun, bp_stmt_t stmt, wIdent label1)
{
	DdNode *tmp1, *tmp2;
	wIdent targetstate,label2;
	bp_idref_t ref = stmt->e.a.asgnlist;
	bp_ident_t var, mark, ret = fun->returns;
	int i, base = 0;
	int stepsize = bp_stepsize;

	bp_sc_flag = Malloc(bpa_size * sizeof(char));

	while (base < fun->num_returns)
	{
		bpc_clearflags(fun);

		mark = ret;
		for (i = 0; i < stepsize; i++)
		{
			if (!ref) break;

			/* set a return value */
			tmp1 = bp_make_choose_expr(mc_pds->g1[ret->index],ref->expr);
			bp_set_expr(bpc_gxlat[ret->index],tmp1);	

			ref = ref->next;
			ret = ret->next;
		}

		bp_find_needed_vars(fun,ref);

		/* keep the locals we still need to compute the other returns */
		for (var = fun->locals; var; var = var->next)
		{
			if (!bp_sc_flag[bpa_lxlat[var->index]]) continue;
			bp_flag[bpc_lxlat[var->index]] = BP_EQUIV;
		}

		/* keep the return values that have already been set */
		for (var = fun->returns; var != mark; var = var->next)
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

		/* keep the globals */
		bp_add_equivs(fun,NULL,bpc_gxlat,NULL);

		Cudd_Ref(tmp2 = mc_pds->one);
		tmp2 = bpc_build_expr(fun,tmp2);	

		label2 = ref? bp_create_label(fun,NULL,0) : 0;
		targetstate = (mc_encoding && !ref)? fun->funname : bp_qstate;

		bddPdsInsert(mc_pds,bp_qstate,label1,targetstate,label2,0,tmp2);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
		
		label1 = label2;
		base += stepsize;
	}

	Free(bp_sc_flag);
}

/***************************************************************************/

void bp_glob_save (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
						wIdent label1, wIdent savelabel)
{
	DdNode *tmp1, *tmp2;
	bp_ident_t var;

	bpc_clearflags(fun);

	/* for all globals that aren't needed in target, save them to a local
	   variable that's going to be kept on the stack in the next step */

	for (var = bp_globals; var; var = var->next)
	{
		if (!fun->funnode->usedglob[var->index]) continue;
		if (target->funnode->usedglob[var->index])
		{
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;
			continue;
		}
		Cudd_Ref(tmp1 = Cudd_bddXnor(mc_pds->mgr,
			mc_pds->g0[var->index],mc_pds->l1[bp_savedglobals[var->index]]));
		bp_set_expr(bpc_lxlat[bp_savedglobals[var->index]],tmp1);

		/* manipulate indices in case variable is needed as argument */
		var->fun = -var->index-1;
		var->index = bp_savedglobals[var->index];
	}

	for (var = fun->locals; var; var = var->next)
		bp_flag[bpc_lxlat[var->index]] = BP_EQUIV;

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpc_build_expr(fun,tmp2);
	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,savelabel,0,
			tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}

void bp_glob_restore (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
					  wIdent restorelabel, wIdent retlabel)
{
	DdNode *tmp1, *tmp2;
	bp_ident_t var;

	bpc_clearflags(fun);

	/* restore the globals from the stack */
	for (var = bp_globals; var; var = var->next)
	{
		/* keep those which were used in the callee */
		if (!var->fun)
		{
			if (!fun->funnode->usedglob[var->index]) continue;
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;
			continue;
		}

		/* restore indices to original state */
		var->index = -var->fun-1;
		var->fun = 0;

		Cudd_Ref(tmp1 = Cudd_bddXnor(mc_pds->mgr,
			mc_pds->g1[var->index],mc_pds->l0[bp_savedglobals[var->index]]));
		bp_set_expr(bpc_gxlat[var->index],tmp1);
	}

	/* keep the return values */
	for (var = target->returns; var; var = var->next)
		bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

	/* keep the locals */
	bp_add_equivs(fun,stmt->live,NULL,bpc_lxlat);

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpc_build_expr(fun,tmp2);
	/* if there are no returns, then we must add the enforce clause
	   since there will be no eval rule behind this that adds it */
	if (!target->num_returns)
		tmp2 = bp_add_clause(tmp2,bp_enforce_expr);

	bddPdsInsert(mc_pds,bp_qstate,restorelabel,bp_qstate,retlabel,0,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}

/***************************************************************************/

void bp_normal_call_save (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
					  wIdent savelabel, wIdent restorelabel)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;
	bp_ident_t var, parm;

	bpc_clearflags(fun);

	/* produce clauses to set parms */
	for (ref = stmt->e.a.fmllist, parm = target->parms; ref;
			ref = ref->next, parm = parm->next)
	{
		tmp1 = bp_make_choose_expr(mc_pds->l1[parm->index],ref->expr);
		bp_set_expr(bpc_lxlat[parm->index],tmp1);
	}

	/* add equivalences for the globals used in the callee */
	/* and save the others on the stack */
	for (var = bp_globals; var; var = var->next)
	{
		if (!fun->funnode->usedglob[var->index]) continue;
		if (target->funnode->usedglob[var->index])
		{
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;
			continue;
		}
		Cudd_Ref(tmp1 = Cudd_bddXnor(mc_pds->mgr,
		    mc_pds->g0[var->index],mc_pds->l2[bp_savedglobals[var->index]]));
		bp_set_expr(bpc_sxlat[bp_savedglobals[var->index]],tmp1);

		/* manipulate indices so the restore functions work correctly */
		var->fun = -var->index-1;
		var->index = bp_savedglobals[var->index];
	}

	/* save locals on stack */
		for (var = fun->locals; var; var = var->next)
			bp_flag[bpc_sxlat[var->index]] = BP_EQUIV;

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpc_build_expr(fun,tmp2);

	bddPdsInsert(mc_pds,bp_qstate,savelabel,bp_qstate,
		    bp_create_label(target,target->stmt,0),restorelabel,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);

	if (mc_encoding)
		bp_call_extra(fun,stmt,target,target->funname,restorelabel);
}

void bp_normal_eval_restore (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
						 wIdent retlabel, wIdent label2)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;
	bp_ident_t var, parm;
	int index;

	bpa_clearflags(fun);

	/* restore the globals from the stack */
	for (var = bp_globals; var; var = var->next)
	{
		if (!var->fun)
		{
			/* don't handle those that aren't in this function */
			if (!fun->funnode->usedglob[var->index]) continue;

			/* keep those which were used in the callee */
			bp_flag[bpa_gxlat[var->index]] = BP_EQUIV;
			continue;
		}

		/* restore indices to original state */
		var->index = -var->fun-1;
		var->fun = 0;

		Cudd_Ref(tmp1 = Cudd_bddXnor(mc_pds->mgr,
			mc_pds->g1[var->index],mc_pds->l0[bp_savedglobals[var->index]]));
		bp_set_expr(bpa_gxlat[var->index],tmp1);
	}

	/* put return values into receiving variables */
	for (ref = stmt->e.a.asgnlist, parm = target->returns; ref; 
		ref = ref->next, parm = parm->next)
	{
		Cudd_Ref(tmp1
			= Cudd_bddXnor(mc_pds->mgr,bp_get_var(ref),
				mc_pds->g0[parm->index]));
		index = (ref->ident->fun? bpa_lxlat : bpa_gxlat)
					[ref->ident->index];
		bp_set_expr(index,tmp1);
	}

	/* keep the remaining locals */
	bp_add_equivs(fun,stmt->live,NULL,bpa_lxlat);

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpa_build_expr(fun,tmp2);
	bddPdsInsert(mc_pds,bp_qstate,retlabel,bp_qstate,label2,0,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}

/***************************************************************************/

void bp_normal_call (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
					wIdent savelabel, wIdent restorelabel)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;
	bp_ident_t var, parm;

	bpc_clearflags(fun);

	/* produce clauses to set parms */
	for (ref = stmt->e.a.fmllist, parm = target->parms; ref;
			ref = ref->next, parm = parm->next)
	{
		tmp1 = bp_make_choose_expr(mc_pds->l1[parm->index],ref->expr);
		bp_set_expr(bpc_lxlat[parm->index],tmp1);
	}

	/* add equivalences for globals that are carried over into the callee */
	for (var = bp_globals; var; var = var->next)
		if (!mc_globals ||
		    (!var->fun && fun->funnode->usedglob[var->index]))
			bp_flag[bpc_gxlat[var->index]] = BP_EQUIV;

	/* save locals on stack */
		for (var = fun->locals; var; var = var->next)
			bp_flag[bpc_sxlat[var->index]] = BP_EQUIV;

	/* add clauses to keep saved globals on the stack */
	if (call_crosses_comp(fun,target))
	{
		for (var = bp_globals; var; var = var->next)
		{
			if (!var->fun) continue;
			/* we can use var->index here because the index
			   has already been changed! */
			bp_flag[bpc_sxlat[var->index]] = BP_EQUIV;
		}
	}

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpc_build_expr(fun,tmp2);

	bddPdsInsert(mc_pds,bp_qstate,savelabel,bp_qstate,
		    bp_create_label(target,target->stmt,0),restorelabel,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);

	if (mc_encoding)
		bp_call_extra(fun,stmt,target,target->funname,restorelabel);
}

void bp_normal_eval (bp_fun_t fun, bp_stmt_t stmt, bp_fun_t target,
					 wIdent retlabel, wIdent label2)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;
	bp_ident_t parm;
	int index;

	bpa_clearflags(fun);

	/* put return values into receiving variables */
	for (ref = stmt->e.a.asgnlist, parm = target->returns; ref; 
		ref = ref->next, parm = parm->next)
	{
		Cudd_Ref(tmp1
			= Cudd_bddXnor(mc_pds->mgr,bp_get_var(ref),
				mc_pds->g0[parm->index]));
		index = (ref->ident->fun? bpa_lxlat : bpa_gxlat)
					[ref->ident->index];
		bp_set_expr(index,tmp1);
	}

	/* keep all other variables */
	bpa_add_equivs(fun,stmt->live);

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpa_build_expr(fun,tmp2);
	bddPdsInsert(mc_pds,bp_qstate,retlabel,bp_qstate,label2,0,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}

void bp_normal_return (bp_fun_t fun, bp_stmt_t stmt, wIdent label1)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;
	bp_ident_t parm;
	wIdent targetstate = mc_encoding? fun->funname : bp_qstate;

	bpc_clearflags(fun);
	for (ref = stmt->e.a.asgnlist, parm = fun->returns; ref; 
		ref = ref->next, parm = parm->next)
	{
		tmp1 = bp_make_choose_expr(mc_pds->g1[parm->index],ref->expr);
		bp_set_expr(bpc_gxlat[parm->index],tmp1);
	}

	bp_add_equivs(fun,NULL,bpc_gxlat,NULL);

	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpc_build_expr(fun,tmp2);
	
	bddPdsInsert(mc_pds,bp_qstate,label1,targetstate,0,0,tmp2);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}
