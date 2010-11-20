#include "bp.h"
#include "bdd.h"

extern int bp_labelcount;
extern int mc_elimdead;
extern int mc_globals;

DdNode **bddvars;
DdNode **pvars, **qvars, **ppvars;
DdNode **yvars, **ypvars, **yppvars, **ypppvars;
int *mc_shuffle, *mc_rev_shuffle;
int GLOBALS,LOCALS;

int mc_globals = 0;
int mc_encoding = 0;
int mc_elimdead = 0;
int mc_sepvar = 2;
int mc_queue = 0;
wIdent mc_reach_target_label;

DdNode **bp_gequal, **bp_lequal, **bp_l2equal;
DdNode *bp_enforce_expr, *bp_skip_expr;
DdNode *bp_main_enforce;

int *bp_savedglobals;

/* Translation table for assignments
   bpa_perm has one entry for each global and local of the current function.
     The entries themselves are an index into the bddvars array and are sorted
     according to the variable ordering.
   bpa_gxlat and bpa_lxlat translate a var->index into the corresponding
     index in bpaperm (for globals and locals, respectively). */

int *bpa_perm, *bpa_gxlat, *bpa_lxlat;
int bpa_size;

/* Another translation table for calls
   Here we have translation tables for globals, locals, and stack variables. */

int *bpc_perm, *bpc_gxlat, *bpc_lxlat, *bpc_sxlat;
int bpc_size;

/* bp_flag has, for each global and local, a flag saying what to do with
     that variable when building an expression. Indices correspond to bpa_perm
     or bpc_perm.
 	   0 = ignore, 1 = take expression fro bpa_expr (and dereference), 2 = equality
   This are commonly used by the bpa and bpc structures
     since we can always assume that bpc_size >= bpa_size */

char *bp_flag;
DdNode **bp_expr;

enum { BP_IGNORE, BP_HAVE_EXPR, BP_EQUIV };

/* translates a var->index into the position in the liveness information */
int *bp_livexlat;

int bp_stepsize = 4;
int bp_fun_numlocals;
wIdent bp_qstate;
bddPds *mc_pds;

void bp_create_sequence (bp_fun_t,bp_stmt_t,wIdent,wIdent);

/***************************************************************************/

/* set to 1 to kill dead vars on skip and goto statements */
#define BP_USE_VARS_ON_SKIP 0

/***************************************************************************/

wIdent bp_create_label (bp_fun_t fun, bp_stmt_t stmt, wIdent label)
{
	char tmp[256];	/* fixme */
	wIdent ret;

	if (stmt && stmt->no_label_given)
		sprintf(tmp,"%s/%ld",wIdentString(fun->funname),stmt->label);
	else if (stmt && stmt->label > 0)
		sprintf(tmp,"%s:%s",wIdentString(fun->funname),
				    wIdentString(stmt->label));
	else if (label)
		sprintf(tmp,"%s:%s",wIdentString(fun->funname),
				    wIdentString(label));
	else
		sprintf(tmp,"%s/%d",wIdentString(fun->funname),++bp_labelcount);

	ret = wIdentCreate(tmp);
	bp_insert_decl(&bp_labeltree,ret,-2,(bp_ident_t)fun);
	return ret;
}

/***************************************************************************/

DdNode* bp_get_var (bp_idref_t idref)
{
	bp_ident_t ident = idref->ident;

	if (ident->fun) return (idref->primed? mc_pds->l1 : mc_pds->l0)[ident->index];
	return (idref->primed? mc_pds->g1 : mc_pds->g0)[ident->index];
}

/***************************************************************************/

#define bpa_clearflags(fun) (memset(bp_flag,BP_IGNORE,bpa_size))
#define bpc_clearflags(fun) (memset(bp_flag,BP_IGNORE,bpc_size))
#define bp_set_expr(index,expr) \
			bp_expr[index] = expr, bp_flag[index] = BP_HAVE_EXPR;
#define bpa_add_equivs(fun,live) bp_add_equivs(fun,live,bpa_gxlat,bpa_lxlat)
#define bpc_add_equivs(fun,live) bp_add_equivs(fun,live,bpc_gxlat,bpc_lxlat)
#define bp_prio(stmt) ((mc_queue == 2 && stmt->next)? stmt->next->fn1->distance : 0)

/***************************************************************************/

DdNode* bp_add_clause (DdNode *base, DdNode *add)
{
	DdNode *tmp0;

	Cudd_Ref(tmp0 = Cudd_bddAnd(mc_pds->mgr,base,add));
	Cudd_RecursiveDeref(mc_pds->mgr,base);
	return tmp0;
}

/***************************************************************************/

void bpa_setup (bp_fun_t fun)
{
	bp_ident_t var;
	int i = 0, j;

	bpa_size = fun->num_globals + fun->num_locals;
	bpa_perm = malloc(bpa_size * sizeof(int));
	bpa_gxlat = malloc(bp_maxrets * sizeof(int));
	bpa_lxlat = malloc(bp_maxlocs * sizeof(int));
	bp_livexlat = malloc(bp_maxlocs * sizeof(int));

	for (var = bp_globals; var; var = var->next)
		if (!mc_globals || fun->funnode->usedglob[var->index])
			bpa_perm[i++] = mc_rev_shuffle[var->index];

	for (var = fun->locals; var; var = var->next)
		bpa_perm[i++] = mc_rev_shuffle[GLOBALS + var->index];

	sort_int(bpa_perm,0,bpa_size - 1);
	for (i = 0; i < bpa_size; i++)
	{
		j = bpa_perm[i] = mc_shuffle[bpa_perm[i]];
		if (j < GLOBALS)
			bpa_gxlat[j] = i;
		else
			bpa_lxlat[j-GLOBALS] = i;
	}

	for (var = fun->locals, j = 0; var; var = var->next, j++)
		bp_livexlat[var->index] = j;
}

void bpc_setup (bp_fun_t fun)
{
	bp_ident_t var, cb;
	bp_funedge_t edge;
	int i, j;

	bpc_size = bp_maxrets + 2*bp_maxlocs;
	bpc_perm = malloc(bpc_size * sizeof(int));
	bpc_gxlat = malloc(bp_maxrets * sizeof(int));
	bpc_lxlat = malloc(bp_maxlocs * sizeof(int));
	bpc_sxlat = malloc(bp_maxlocs * sizeof(int));

	bp_flag = malloc(bpc_size * sizeof(char));
	bp_expr = malloc(bpc_size * sizeof(DdNode*));

	memset(bpc_gxlat,0xff,bp_maxrets * sizeof(int));
	memset(bpc_lxlat,0xff,bp_maxlocs * sizeof(int));
	memset(bpc_sxlat,0xff,bp_maxlocs * sizeof(int));

	for (var = bp_globals; var; var = var->next)
		if (!mc_globals || fun->funnode->usedglob[var->index])
			bpc_gxlat[var->index] = 1;

	for (var = fun->locals; var; var = var->next)
		bpc_lxlat[var->index] =	bpc_sxlat[var->index] = 1;

	for (edge = fun->funnode->edges; edge; edge = edge->next)
	{
		bp_fun_t target = edge->fn->fun;

		if (call_crosses_comp(fun,target))
		{
			for (var = bp_globals; var; var = var->next)
			{
				if (!fun->funnode->usedglob[var->index]) continue;
				if (target->funnode->usedglob[var->index]) continue;
				bpc_lxlat[bp_savedglobals[var->index]] = 1;
				bpc_sxlat[bp_savedglobals[var->index]] = 1;
			}
		}

		memset(bp_flag,0,bp_maxlocs * sizeof(char));
		for (var = target->parms; var; var = var->next)
		{
			bpc_lxlat[var->index] = 1;
			bp_flag[var->index] = 1;
		}

		if (stepwise(target->num_args))
		{
			for (var = fun->locals, cb = fun->callbuf; var; 
				 var = var->next, cb = cb->next)
					 if (bp_flag[var->index]) bpc_gxlat[cb->index] = 1;
		}

		if (stepwise(target->num_args) || stepwise(target->num_returns))
			for (var = target->returns; var; var = var->next)
				bpc_gxlat[var->index] = 1;
	}

	for (var = fun->returns; var; var = var->next)
		bpc_gxlat[var->index] = 1;

	i = 0;
	for (j = 0; j < bp_maxrets; j++)
		if (bpc_gxlat[j] > 0) bpc_perm[i++] = mc_rev_shuffle[pvars - bddvars + j];
	for (j = 0; j < bp_maxlocs; j++)
		if (bpc_lxlat[j] > 0) bpc_perm[i++] = mc_rev_shuffle[yvars - bddvars + j];
	for (j = 0; j < bp_maxlocs; j++)
		if (bpc_sxlat[j] > 0) bpc_perm[i++] = mc_rev_shuffle[yppvars - bddvars + j];
	bpc_size = i;

	sort_int(bpc_perm,0,bpc_size - 1);
	for (i = 0; i < bpc_size; i++)
	{
		j = bpc_perm[i] = mc_shuffle[bpc_perm[i]];
		if (j < yvars - bddvars)
			bpc_gxlat[j - (pvars - bddvars)] = i;
		else if (j < yppvars - bddvars)
			bpc_lxlat[j - (yvars - bddvars)] = i;
		else
			bpc_sxlat[j - (yppvars - bddvars)] = i;
	}
}

/***************************************************************************/

DdNode* bp_build_expr (bp_fun_t fun, DdNode *base,
						int size, int *perm, char *flag, DdNode **expr)
{
	int i, j;

	for (i = size - 1; i >= 0; i--)
	{
		switch (flag[i])
		{
		  case BP_IGNORE:
			break;
		  case BP_HAVE_EXPR:
			base = bp_add_clause(base,expr[i]);
			Cudd_RecursiveDeref(mc_pds->mgr,expr[i]);
			break;
		  case BP_EQUIV:
			j = perm[i];
			if (j < mc_pds->num_globals)
				base = bp_add_clause(base,bp_gequal[j]);
			else if (j < yppvars - bddvars)
				base = bp_add_clause(base,bp_lequal[j - (yvars-bddvars)]);
			else
				base = bp_add_clause(base,bp_l2equal[j - (yppvars-bddvars)]);
			break;
		  default:
			break;
		}
	}
	
	return base;
}


DdNode* bpa_build_expr (bp_fun_t fun, DdNode *base)
{
	base = bp_build_expr(fun,base,bpa_size,bpa_perm,bp_flag,bp_expr);
	return bp_add_clause(base,bp_enforce_expr);
}

DdNode* bpc_build_expr (bp_fun_t fun, DdNode *base)
{
	return bp_build_expr(fun,base,bpc_size,bpc_perm,bp_flag,bp_expr);
}

/***************************************************************************/

void bp_add_equivs (bp_fun_t fun, char *live, int *gxlat, int *lxlat)
{
	bp_ident_t var;
	int aindex;

	if (gxlat) for (var = bp_globals; var; var = var->next)
	{
		if (mc_globals && !fun->funnode->usedglob[var->index]) continue;
		aindex = gxlat[var->index];
		if (bp_flag[aindex] == BP_IGNORE) bp_flag[aindex] = BP_EQUIV;
	}

	if (lxlat) for (var = fun->locals; var; var = var->next)
	{
		aindex = lxlat[var->index];
		
		if (!live || live[bp_livexlat[var->index]])
		{
			if (bp_flag[aindex] == BP_IGNORE) bp_flag[aindex] = BP_EQUIV;
		}
		else if (bp_flag[aindex] == BP_HAVE_EXPR)
		{
			bp_flag[aindex] = BP_IGNORE;
			Cudd_RecursiveDeref(mc_pds->mgr,bp_expr[aindex]);
		}
	}
}

/***************************************************************************/

/* add/remove primes in all occurrences of variables in an enforce condition */

void bp_prime_enforce (bp_expr_t expr, char value)
{
	switch (expr->token)
	{
	    case BP_EQ:
	    case BP_NE:
	    case BP_AND:
	    case BP_OR:
	    case BP_IMP:
	    case BP_CHOOSE:
			bp_prime_enforce(expr->right,value);
	    case BP_NOT:
			bp_prime_enforce(expr->left,value);
			return;
	    case BP_IDENT:
			((bp_idref_t)(expr->left))->primed = value;
	    case BP_CONST:
	    case BP_ND:
			return;
	    default:
			bpError("can't happen");
	}
}

/***************************************************************************/

DdNode* bp_make_expr (bp_expr_t expr)
{
	DdNode* (*fn)(DdManager*,DdNode*,DdNode*) = NULL;
	DdNode *tmp0, *tmp1, *tmp2;

	switch (expr->token)
	{
	    case BP_EQ:
			fn = Cudd_bddXnor;
	    case BP_NE:
			if (!fn) fn = Cudd_bddXor;
	    case BP_AND:
			if (!fn) fn = Cudd_bddAnd;
	    case BP_OR:
	    case BP_IMP:
			if (!fn) fn = Cudd_bddOr;
			tmp1 = bp_make_expr(expr->left);
			tmp2 = bp_make_expr(expr->right);
			if (expr->token == BP_IMP) tmp1 = Cudd_Not(tmp1);
			Cudd_Ref(tmp0 = fn(mc_pds->mgr,tmp1,tmp2));
			Cudd_RecursiveDeref(mc_pds->mgr,tmp1);
			Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
			return tmp0;
	    case BP_NOT:
			return Cudd_Not(bp_make_expr(expr->left));
	    case BP_IDENT:
			Cudd_Ref(tmp0 = bp_get_var((bp_idref_t)(expr->left)));
			return tmp0;
	    case BP_CONST:
			Cudd_Ref(expr->left? mc_pds->one : mc_pds->zero);
			return expr->left? mc_pds->one : mc_pds->zero;
	    default:
			bpError("can't happen");
	  		return NULL;
	}
}

/***************************************************************************/

DdNode* bp_make_choose_expr (DdNode *var, bp_expr_t expr)
{
	DdNode *tmp0, *tmp1, *tmp2, *tmp3;

	if (expr->token == BP_CHOOSE)
	{
		tmp0 = bp_make_expr(expr->left);
		tmp1 = bp_make_expr(expr->right);

		Cudd_Ref(tmp3 = Cudd_bddAnd(mc_pds->mgr,Cudd_Not(tmp0),Cudd_Not(tmp1)));

		Cudd_Ref(tmp2 = Cudd_bddAnd(mc_pds->mgr,Cudd_Not(tmp0),tmp1));
		Cudd_RecursiveDeref(mc_pds->mgr,tmp1);
		Cudd_Ref(tmp1 = Cudd_bddAnd(mc_pds->mgr,tmp2,Cudd_Not(var)));
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
		Cudd_Ref(tmp2 = Cudd_bddOr(mc_pds->mgr,tmp1,tmp3));
		Cudd_RecursiveDeref(mc_pds->mgr,tmp1);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp3);

		Cudd_Ref(tmp1 = Cudd_bddAnd(mc_pds->mgr,var,tmp0));
		Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
		Cudd_Ref(tmp0 = Cudd_bddOr(mc_pds->mgr,tmp1,tmp2));

		Cudd_RecursiveDeref(mc_pds->mgr,tmp1);
		Cudd_RecursiveDeref(mc_pds->mgr,tmp2);

		return tmp0;
	}

	tmp0 = bp_make_expr(expr);
	Cudd_Ref(tmp1 = Cudd_bddXnor(mc_pds->mgr,var,tmp0));
	Cudd_RecursiveDeref(mc_pds->mgr,tmp0);

	return tmp1;
}

/***************************************************************************/

#include "bpSpecial.c"

/***************************************************************************/

void bp_create_assign (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	DdNode *tmp1, *tmp2;
	bp_idref_t ref;

	bpa_clearflags(fun);

	for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
	{
		int index = (ref->ident->fun? bpa_lxlat : bpa_gxlat)
							[ref->ident->index];

		tmp1 = bp_make_choose_expr(bp_get_var(ref),ref->expr);
		bp_set_expr(index,tmp1);
	}

	bpa_add_equivs(fun,stmt->live);
	Cudd_Ref(tmp2 = mc_pds->one);
	tmp2 = bpa_build_expr(fun,tmp2);

	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,0,tmp2);

	Cudd_RecursiveDeref(mc_pds->mgr,tmp2);
}


/***************************************************************************/

void bp_create_call (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	wIdent retlabel, savelabel, restorelabel;
	char do_save_in_call = 0;
	char do_restore_in_eval = 0;
	bp_fun_t target;

	target = (bp_fun_t)bp_lookup(bp_funtree,stmt->e.a.label,-1);
	
	/* create the call statement, possibly preceded by a saving rule */

	retlabel = target->num_returns? bp_create_label(fun,NULL,0) : label2;
	savelabel = label1;
	restorelabel = retlabel;

	if (call_crosses_comp(fun,target))
	{
		do_save_in_call = !stepwise(target->num_args);
		do_restore_in_eval = target->num_returns && !stepwise(target->num_returns);

		if (!do_save_in_call)
		{
			savelabel = bp_create_label(fun,NULL,0);
			bp_glob_save(fun,stmt,target,label1,savelabel);
		}
		if (!do_restore_in_eval)
			restorelabel = bp_create_label(fun,NULL,0);
	}

	if (stepwise(target->num_args))
		/* treat huge call statements specially */
		bp_special_call(fun,stmt,target,savelabel,restorelabel);
	else if (do_save_in_call)
		bp_normal_call_save(fun,stmt,target,savelabel,restorelabel);
	else
		bp_normal_call(fun,stmt,target,savelabel,restorelabel);

	if (call_crosses_comp(fun,target) && !do_restore_in_eval)
		bp_glob_restore(fun,stmt,target,restorelabel,retlabel);

	/* create the return/evaluation statement; unnecessary if
		no values are returned */

	if (!target->num_returns) return;

	if (stepwise(target->num_returns))
		/* treat huge numbers of returns values specially */
		bp_special_eval(fun,stmt,target,retlabel,label2);
	else if (do_restore_in_eval)
		bp_normal_eval_restore(fun,stmt,target,retlabel,label2);
	else
		bp_normal_eval(fun,stmt,target,retlabel,label2);
}

/***************************************************************************/

void bp_create_assert (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	DdNode *tmp0 = mc_pds->one;
	bp_expr_t expr = stmt->e.c.expr;

	if (expr->token != BP_ND) tmp0 = bp_make_expr(expr);
	else Cudd_Ref(tmp0);

	if (stmt->live)
	{
		bpa_clearflags(fun);
		bpa_add_equivs(fun,stmt->live);
		tmp0 = bpa_build_expr(fun,tmp0);
	}
	else
		tmp0 = bp_add_clause(tmp0,bp_skip_expr);

	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,0,tmp0);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
}

/***************************************************************************/

void bp_create_constrain(bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	DdNode *tmp0 = mc_pds->one;
	bp_expr_t expr = stmt->e.c.expr;

	if (expr->token != BP_ND) tmp0 = bp_make_expr(expr);
	else Cudd_Ref(tmp0);

	tmp0 = bp_add_clause(tmp0,bp_enforce_expr);

	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,0,tmp0);
	Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
}

/***************************************************************************/

void bp_create_if (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	bp_stmt_t tmp = stmt->next;
	bp_expr_t expr = stmt->e.c.expr;

	stmt->type = BP_ASSUME;
	stmt->next = stmt->e.c.thenstmt;
	bp_create_sequence(fun,stmt,label1,label2);

	if (expr->token != BP_ND)
		new_bp_expr(stmt->e.c.expr,BP_NOT,expr,NULL);
	stmt->next = stmt->e.c.elsestmt;
	bp_create_sequence(fun,stmt,label1,label2);

	if (expr->token != BP_ND) Free(stmt->e.c.expr);
	stmt->e.c.expr = expr;
	stmt->type = BP_IF;
	stmt->next = tmp;
}

/***************************************************************************/

void bp_create_while (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	bp_stmt_t tmp = stmt->next;
	bp_expr_t expr = stmt->e.c.expr;

	stmt->type = BP_ASSUME;
	stmt->next = stmt->e.c.thenstmt;
	bp_create_sequence(fun,stmt,label1,label1);

	if (expr->token != BP_ND)
		new_bp_expr(stmt->e.c.expr,BP_NOT,expr,NULL);
	stmt->next = NULL;
	bp_create_sequence(fun,stmt,label1,label2);

	if (expr->token != BP_ND) Free(stmt->e.c.expr);
	stmt->e.c.expr = expr;
	stmt->type = BP_WHILE;
	stmt->next = tmp;
}

/***************************************************************************/

void bp_create_return (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	if (stmt->e.a.asgnlist && stepwise(fun->num_returns))
		/* treat large numbers of returns specially */
		bp_special_return(fun,stmt,label1);
	else
		bp_normal_return(fun,stmt,label1);
}

/***************************************************************************/

void bp_create_skip (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	DdNode *tmp0;

	if (BP_USE_VARS_ON_SKIP && stmt->live)
	{
		bpa_clearflags(fun);
		bpa_add_equivs(fun,stmt->live);
		Cudd_Ref(tmp0 = mc_pds->one);
		tmp0 = bpa_build_expr(fun,tmp0);
	}
	else
		tmp0 = bp_skip_expr;
	
	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,label2,0,tmp0);

	if (BP_USE_VARS_ON_SKIP && stmt->live)
		Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
}

/***************************************************************************/

void bp_create_goto (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	wIdent target = bp_create_label(fun,NULL,stmt->e.a.label);

	DdNode *tmp0;

	if (BP_USE_VARS_ON_SKIP && stmt->live)
	{
		bpa_clearflags(fun);
		bpa_add_equivs(fun,stmt->live);
		Cudd_Ref(tmp0 = mc_pds->one);
		tmp0 = bpa_build_expr(fun,tmp0);
	}
	else
		tmp0 = bp_skip_expr;
	
	bddPdsInsert(mc_pds,bp_qstate,label1,bp_qstate,target,0,tmp0);

	if (BP_USE_VARS_ON_SKIP && stmt->live)
		Cudd_RecursiveDeref(mc_pds->mgr,tmp0);
}

/***************************************************************************/

void bp_create_sequence (bp_fun_t fun, bp_stmt_t stmt, wIdent label1, wIdent label2)
{
	void (*cfun)(bp_fun_t,bp_stmt_t,wIdent,wIdent) = NULL;
	wIdent nextlabel;

	while (stmt)
	{
	    switch (stmt->type)
	    {
			case BP_ASSIGN:	   cfun = bp_create_assign; break;
			case BP_CALL:      cfun = bp_create_call; break;
			case BP_ASSERT:    cfun = bp_create_assert; break;
			case BP_ASSUME:    cfun = bp_create_assert; break;
			case BP_CONSTRAIN: cfun = bp_create_constrain; break;
			case BP_PRINT:     cfun = bp_create_skip; break;
			case BP_IF:        cfun = bp_create_if; break;
			case BP_WHILE:     cfun = bp_create_while; break;
			case BP_RETURN:    cfun = bp_create_return; break;
			case BP_SKIP:      cfun = bp_create_skip; break;
			case BP_GOTO:      cfun = bp_create_goto; break;
			default:		   bpError("can't happen");
	    }

	    nextlabel = stmt->next? bp_create_label(fun,stmt->next,0)
				  : label2;
	    cfun(fun,stmt,label1,nextlabel);
	    label1 = nextlabel;
	    stmt = stmt->next;
	}
}

/***************************************************************************/

/* translate a single function into pushdown rules */

void bp_create_fun (bp_fun_t fun)
{
	wIdent label1, label2;
	bp_stmt_t ret;

	if (!fun->funnode->dfsnum) return;			

	bpa_setup(fun);
	bpc_setup(fun);

	if (fun->enforce)
	{
		if (fun->funname == wIdentCreate("main"))
		{
			bp_prime_enforce(fun->enforce,0);
			Cudd_RecursiveDeref(mc_pds->mgr,bp_main_enforce);
			Cudd_Ref(bp_main_enforce = bp_make_expr(fun->enforce));
		}
		bp_prime_enforce(fun->enforce,1);
		Cudd_Ref(bp_enforce_expr = bp_make_expr(fun->enforce));
	}
	else	Cudd_Ref(bp_enforce_expr = mc_pds->one);

	/* set equality for all vars */
	memset(bp_flag,BP_EQUIV,bpa_size);
	Cudd_Ref(bp_skip_expr = mc_pds->one);
	bp_skip_expr = bpa_build_expr(fun,bp_skip_expr);
	
	label1 = bp_create_label(fun,fun->stmt,0);
	label2 = bp_create_label(fun,NULL,0);

	bp_create_sequence(fun,fun->stmt,label1,label2);

	new_bp_stmt(ret,BP_RETURN);
	ret->e.a.asgnlist = NULL;
	bp_create_return(fun,ret,label2,0);
	Free(ret);

	Free(bpa_perm); Free(bpa_gxlat); Free(bpa_lxlat);
	Free(bpc_perm); Free(bpc_gxlat); Free(bpc_lxlat); Free(bpc_sxlat);
	Free(bp_flag);	Free(bp_expr);
	Free(bp_livexlat);

	Cudd_RecursiveDeref(mc_pds->mgr,bp_skip_expr);
	if (fun->enforce) Cudd_RecursiveDeref(mc_pds->mgr,bp_enforce_expr);
}

/***************************************************************************/

/* translate the boolean program tree into a pushdown system */

bddPds* bp_create_pds ()
{
	bp_fun_t fun;
	wConfig *start;
	int i;

	if (mc_globals)
	{
		bp_ident_t var;
		bp_savedglobals = Malloc(bp_numglobals * sizeof(int));
		for (var = bp_globals; var; var = var->next)
			bp_savedglobals[var->index] = bp_maxlocs++;
	}

	bp_build_call_graph();
	if (mc_elimdead == 2) bp_build_flow_graph();
	if (mc_elimdead == 1) bp_elim_dead_simple();
	if (mc_elimdead == 2) bp_mod_ref();

	bp_labeltree = NULL;
	bp_qstate = wIdentCreate("q");

	fun = (bp_fun_t)bp_lookup(bp_funtree,wIdentCreate("main"),-1);
	if (!fun)
	{
		fprintf(stderr,"Error in Boolean Program: "
				"main function does not exist\n");
		exit(1);
	}

	start = wConfigCreate(bp_qstate, bp_create_label(fun,fun->stmt,0), 0);
	mc_pds = bddPdsCreate (start, bp_maxrets, bp_maxlocs,
					bp_reorder_dep, NULL);

	/* create equality expressions like 'x=x' */
	bp_gequal = Malloc(bp_maxrets * sizeof(DdNode*));
	bp_lequal = Malloc(bp_maxlocs * sizeof(DdNode*));
	bp_l2equal = Malloc(bp_maxlocs * sizeof(DdNode*));
	for (i = 0; i < bp_maxrets; i++)
	    Cudd_Ref(bp_gequal[i] =
			Cudd_bddXnor(mc_pds->mgr,mc_pds->g0[i],mc_pds->g1[i]));
	for (i = 0; i < bp_maxlocs; i++)
	    Cudd_Ref(bp_lequal[i] =
			Cudd_bddXnor(mc_pds->mgr,mc_pds->l0[i],mc_pds->l1[i]));
	for (i = 0; i < bp_maxlocs; i++)
	    Cudd_Ref(bp_l2equal[i] =
			Cudd_bddXnor(mc_pds->mgr,mc_pds->l0[i],mc_pds->l2[i]));

	Cudd_Ref(bp_main_enforce = mc_pds->one);

	for (fun = bp_functions; fun; fun = fun->next)
		bp_create_fun(fun);

// to do
//	mc_pds.start_bdd = bp_main_enforce;

	return mc_pds;
}
