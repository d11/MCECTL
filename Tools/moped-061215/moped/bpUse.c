#include "bp.h"

/* controls whether enforce statements are
   taken into account in liveness/modref analysis */
#define BP_MR_ENFORCE

extern int mc_globals;
extern int mc_elimdead;
extern int mc_queue;
extern wIdent mc_reach_target_label;

/***************************************************************************/

void bp_eds_doexpr (bp_stmt_t* seen, bp_expr_t expr, bp_stmt_t stmt)
{
	switch (expr->token)
	{
	  case BP_EQ:
	  case BP_NE:
	  case BP_AND:
	  case BP_OR:
	  case BP_IMP:
	  case BP_CHOOSE:
		bp_eds_doexpr(seen,expr->right,stmt);
	  case BP_NOT:
		bp_eds_doexpr(seen,expr->left,stmt);
		return;
	  case BP_IDENT: {
		bp_ident_t id = ((bp_idref_t)(expr->left))->ident;
		if (id->fun) seen[id->index] = stmt; }
	  case BP_CONST:
	  case BP_ND:
		return;
	  default:
		bpError("can't happen");
	}	
}

void bp_eds_find (bp_fun_t fun, bp_stmt_t begin)
{
	bp_ident_t var;
	bp_idref_t ref;
	bp_stmt_t stmt;
	bp_stmt_t *lastseen = malloc(fun->num_locals * sizeof(bp_stmt_t));
	bp_stmt_t *enfseen = malloc(fun->num_locals * sizeof(bp_stmt_t));
	char *live, *newlive;
	int *varidx = malloc(fun->num_locals * sizeof(int));
	int i;

	if (!begin) return;

	for (var = fun->locals, i = 0; var; var = var->next)
	{
		varidx[i] = var->index;
		var->index = i++;
	}

	memset(lastseen,0,fun->num_locals * sizeof(bp_stmt_t));
	memset(enfseen,0,fun->num_locals * sizeof(bp_stmt_t));

	for (stmt = begin; stmt; stmt = stmt->next)
	{
		switch (stmt->type)
		{
		  case BP_ASSIGN:
		  case BP_PRINT:
		  case BP_RETURN:
			for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
				bp_eds_doexpr(lastseen,ref->expr,stmt);
			break;
		  case BP_CALL:
			for (ref = stmt->e.a.fmllist; ref; ref = ref->next)
				bp_eds_doexpr(lastseen,ref->expr,stmt);				
			break;
		  case BP_ASSERT:
		  case BP_ASSUME:
		  case BP_CONSTRAIN:
			bp_eds_doexpr(lastseen,stmt->e.c.expr,stmt);
			break;
		  case BP_SKIP:
			  break;
		  case BP_IF:
		  case BP_WHILE:
		  case BP_GOTO:
		  default:
			  bpError("shouldn't happen");
		}
		if (stmt->type == BP_RETURN) break;
	}
	
#ifdef BP_MR_ENFORCE	
	if (fun->enforce) bp_eds_doexpr(enfseen,fun->enforce,begin);
#endif

	live = malloc(fun->num_locals * sizeof(char));
	for (i = 0; i < fun->num_locals; i++)
		live[i] = !!(lastseen[i] || enfseen[i]);

	for (stmt = begin; stmt && stmt->type != BP_RETURN; stmt = stmt->next)
	{
		newlive = malloc(fun->num_locals * sizeof(char));
		memcpy(newlive,live,fun->num_locals * sizeof(char));
		for (i = 0; i < fun->num_locals; i++)
		{
			if (!newlive[i]) continue;
			newlive[i] = (lastseen[i] != stmt || enfseen[i]);
		}
		stmt->live = newlive;
	}

	for (var = fun->locals, i = 0; var; var = var->next)
		var->index = varidx[i++];

	free(varidx);
	free(lastseen);
	free(live);
}

void bp_eds_seq (bp_fun_t fun, bp_stmt_t begin, char exit)
{
	bp_stmt_t stmt = begin;

	while (stmt)
	{
		switch (stmt->type)
		{
		  case BP_GOTO:
			bp_eds_seq(fun,stmt->next,exit);
			return;
		  case BP_IF:
			bp_eds_seq(fun,stmt->e.c.thenstmt,0);
			bp_eds_seq(fun,stmt->e.c.elsestmt,0);
			bp_eds_seq(fun,stmt->next,exit);
			return;
		  case BP_WHILE:
			bp_eds_seq(fun,stmt->e.c.thenstmt,0);
			bp_eds_seq(fun,stmt->next,exit);
			return;
		  case BP_RETURN:
			bp_eds_find(fun,begin);
			bp_eds_seq(fun,stmt->next,exit);
			return;
		  default:
			break;
		}
		stmt = stmt->next;
	}
	if (exit) bp_eds_find(fun,begin);
}

void bp_elim_dead_simple ()
{
	bp_fun_t fun;
	for (fun = bp_functions; fun; fun = fun->next)
		bp_eds_seq(fun,fun->stmt,1);
}

/***************************************************************************/

bp_tree_t bp_mr_tree;
int bp_mr_num_locals;
char *bp_mr_enflive;
bp_flow_t bp_flow_root = NULL;

bp_flow_t bp_mr_get_flownode (bp_fun_t fun, wIdent label)
{
	bp_flow_t res = (bp_flow_t)bp_lookup(bp_mr_tree,label,0);
	if (res) return res;

	res = alloc_bp_flow();
	res->fun = fun;
	res->edges = NULL;
	if (mc_elimdead == 2)
	{
		res->live = malloc(bp_mr_num_locals * sizeof(char));
#ifdef BP_MR_ENFORCE
		memcpy(res->live,bp_mr_enflive,bp_mr_num_locals * sizeof(char));
#else
		memset(res->live,0,bp_mr_num_locals * sizeof(char));
#endif
	}
	else
		res->live = NULL;

	if (label == mc_reach_target_label) bp_flow_root = res;
	res->distance = 0x7ffffff;
	res->in_worklist = 0;
	res->label = label;

	bp_insert_decl(&bp_mr_tree,label,0,(bp_ident_t)res);
	return res;
}

void bp_mr_insert_edge (bp_flow_t fn1, bp_stmt_t stmt, bp_flow_t fn2)
{
	bp_edge_t edge = alloc_bp_edge();
	edge->stmt = stmt;
	edge->target = fn1;
	edge->next = fn2->edges;
	fn2->edges = edge;
	stmt->live = fn2->live;
	stmt->fn1 = fn1;
}

void bp_mr_build_seq (bp_fun_t fun, bp_flow_t fn1, bp_stmt_t stmt, bp_flow_t fn2)
{
	while (stmt)
	{
		bp_flow_t fn3, nextfn = stmt->next? bp_mr_get_flownode(fun,stmt->next->label) : fn2;
		switch (stmt->type)
		{
		  case BP_IF:
			fn3 = bp_mr_get_flownode(fun,stmt->e.c.thenstmt->label);
			bp_mr_insert_edge(fn1,stmt,fn3);
			bp_mr_build_seq(fun,fn3,stmt->e.c.thenstmt,nextfn);
			fn3 = stmt->e.c.elsestmt? bp_mr_get_flownode(fun,stmt->e.c.elsestmt->label) : nextfn;
			bp_mr_insert_edge(fn1,stmt,fn3);
			if (stmt->e.c.elsestmt) bp_mr_build_seq(fun,fn3,stmt->e.c.elsestmt,nextfn);
			stmt->live = NULL;
			break;
		  case BP_WHILE:
			fn3 = bp_mr_get_flownode(fun,stmt->e.c.thenstmt->label);
			bp_mr_insert_edge(fn1,stmt,nextfn);
			bp_mr_insert_edge(fn1,stmt,fn3);
			bp_mr_build_seq(fun,fn3,stmt->e.c.thenstmt,fn1);
			stmt->live = NULL;
			break;
		  case BP_GOTO:
			bp_mr_insert_edge(fn1,stmt,bp_mr_get_flownode(fun,stmt->e.a.label));
			break;
		  case BP_RETURN:
			stmt->fn1 = fn1;
			break;
		  default:
			bp_mr_insert_edge(fn1,stmt,nextfn);
			break;
		}
		fn1 = nextfn;
		stmt = stmt->next;
	}
}

/***************************************************************************/

void bp_mr_startuse (int i, bp_flow_t fn)
{
	bp_edge_t e = fn->edges;
	if (fn->live[i]) return;
	fn->live[i] = 1;
	while (e)
	{
		bp_idref_t ref;

		if (e->stmt->type == BP_ASSIGN || e->stmt->type == BP_CALL)
			for (ref = e->stmt->e.a.asgnlist; ref; ref = ref->next)
				if (ref->ident->fun && ref->ident->index == i) goto nextone;
		bp_mr_startuse(i,e->target);
nextone:
		e = e->next;
	}

}

void bp_mr_doexpr (bp_expr_t expr, bp_stmt_t stmt)
{
	switch (expr->token)
	{
	  case BP_EQ:
	  case BP_NE:
	  case BP_AND:
	  case BP_OR:
	  case BP_IMP:
	  case BP_CHOOSE:
		bp_mr_doexpr(expr->right,stmt);
	  case BP_NOT:
		bp_mr_doexpr(expr->left,stmt);
		return;
	  case BP_IDENT: {
		bp_ident_t id = ((bp_idref_t)(expr->left))->ident;
		if (id->fun) bp_mr_startuse(id->index,stmt->fn1);}
	  case BP_CONST:
	  case BP_ND:
		return;
	  default:
		bpError("can't happen");
	}	
}

void bp_mr_doseq (bp_stmt_t stmt)
{
	while (stmt)
	{
		bp_idref_t ref;

		switch (stmt->type)
		{
		  case BP_ASSIGN:
		  case BP_PRINT:
		  case BP_RETURN:
			for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
				bp_mr_doexpr(ref->expr,stmt);
			break;
		  case BP_CALL:
			for (ref = stmt->e.a.fmllist; ref; ref = ref->next)
				bp_mr_doexpr(ref->expr,stmt);				
			break;
		  case BP_ASSERT:
		  case BP_ASSUME:
		  case BP_CONSTRAIN:
			bp_mr_doexpr(stmt->e.c.expr,stmt);
			break;
		  case BP_SKIP:
		  case BP_GOTO:
			break;
		  case BP_IF:
			bp_mr_doexpr(stmt->e.c.expr,stmt);
			bp_mr_doseq(stmt->e.c.thenstmt);
			bp_mr_doseq(stmt->e.c.elsestmt);
			break;
		  case BP_WHILE:
			bp_mr_doexpr(stmt->e.c.expr,stmt);
			bp_mr_doseq(stmt->e.c.thenstmt);
			break;
		  default:
			  bpError("can't happen");
		}
		stmt = stmt->next;
	}
}

void bp_mr_fun (bp_fun_t fun)
{
	bp_ident_t var;
	int i, *varidx = malloc(fun->num_locals * sizeof(int));
	
	for (var = fun->locals, i = 0; var; var = var->next)
	{
		varidx[i] = var->index;
		var->index = i++;
	}

	bp_mr_doseq(fun->stmt);

	for (var = fun->locals, i = 0; var; var = var->next)
		var->index = varidx[i++];

	free(varidx);
}

void bp_mod_ref ()
{
	bp_fun_t fun;
	for (fun = bp_functions; fun; fun = fun->next)
		bp_mr_fun(fun);
}

/***************************************************************************/

void bp_build_fun_graph (bp_fun_t fun)
{
	bp_ident_t var;
	bp_flow_t fn1, fn2;
	int i, *varidx = NULL;

#ifdef BP_MR_ENFORCE
	bp_stmt_t *seen;

	if (mc_elimdead == 2)
	{
		varidx = malloc(fun->num_locals * sizeof(int));
		for (var = fun->locals, i = 0; var; var = var->next)
		{
			varidx[i] = var->index;
			var->index = i++;
		}
	}
#endif

	bp_mr_tree = NULL;
	bp_mr_num_locals = fun->num_locals;

#ifdef BP_MR_ENFORCE
	bp_mr_enflive = malloc(fun->num_locals * sizeof(char));
	memset(bp_mr_enflive,0,fun->num_locals * sizeof(char));
	seen = malloc(fun->num_locals * sizeof(bp_stmt_t));
	memset(seen,0,fun->num_locals * sizeof(bp_stmt_t));
	if (fun->enforce)
		bp_eds_doexpr(seen,fun->enforce,(bp_stmt_t)fun);
	for (i = 0; i < fun->num_locals; i++)
		bp_mr_enflive[i] = !!seen[i];
#endif

	fn1 = bp_mr_get_flownode(fun,fun->stmt->label);
	fn2 = bp_mr_get_flownode(fun,-1);

	bp_mr_build_seq(fun,fn1,fun->stmt,fn2);

#ifdef BP_MR_ENFORCE
	if (mc_elimdead == 2)
	{
		for (var = fun->locals, i = 0; var; var = var->next)
			var->index = varidx[i++];
		free(varidx);
	}

	// fixme: release memory in bp_mr_tree
	free(bp_mr_enflive);
	free(seen);
#endif
}

void bp_build_flow_graph ()
{
	bp_fun_t fun;
	for (fun = bp_functions; fun; fun = fun->next)
		bp_build_fun_graph(fun);
}

/***************************************************************************/

#define bp_glob_doasgn(id,glob) \
	{	if (mc_globals && !(id)->fun) glob[(id)->index] = 1; }

#define bp_glob_doexpr(expr,glob) \
	{	if (mc_globals) bp_glob_doexpr_recur(expr,glob); }

char *bpg_flag;

void bp_glob_doexpr_recur (bp_expr_t expr, char *glob)
{
	switch (expr->token)
	{
	  case BP_EQ:
		if (expr->left->token == BP_IDENT 
			&& expr->right->token == BP_IDENT)
		{
			bp_idref_t id1 = (bp_idref_t)(expr->left->left);
			bp_idref_t id2 = (bp_idref_t)(expr->right->left);
			if ((id1->primed ^ id2->primed) 
				&& id1->ident->index == id2->ident->index)
			break;
		}
	  case BP_NE:
	  case BP_AND:
	  case BP_OR:
	  case BP_IMP:
	  case BP_CHOOSE:
		bp_glob_doexpr_recur(expr->right,glob);
	  case BP_NOT:
		bp_glob_doexpr_recur(expr->left,glob);
		return;
	  case BP_IDENT:
		  bp_glob_doasgn(((bp_idref_t)(expr->left))->ident,glob);
		  break;
	  case BP_CONST:
	  case BP_ND:
		return;
	  default:
		bpError("can't happen");
	}	
}

void bp_glob_addcall (bp_fun_t fun, bp_stmt_t stmt)
{
	bp_fun_t target = (bp_fun_t)bp_lookup(bp_funtree,stmt->e.a.label,-1);
	bp_funedge_t edge;
	bp_ident_t var;
	char alreadythere = 0;

	if (!target) bpError("function %s does not exist",
					wIdentString(stmt->e.a.label));

	for (edge = fun->funnode->edges; edge; edge = edge->next)
		if (edge->fn == target->funnode) alreadythere = 1;

	if (alreadythere && mc_queue != 2) return;

	edge = alloc_bp_funedge();
	edge->stmt = stmt;
	edge->fn = target->funnode;
	if (!alreadythere)
	{
		edge->next = fun->funnode->edges;
		fun->funnode->edges = edge;
	}
	edge->nextcall = target->funnode->calls;
	target->funnode->calls = edge;

	if (stepwise(target->num_args))
		for (var = target->parms; var; var = var->next)
			bpg_flag[var->index] = 1;
}

void bp_glob_addreturn (bp_fun_t fun, bp_stmt_t stmt)
{
	bp_edge_t edge = alloc_bp_edge();
	edge->stmt = stmt;
	edge->next = fun->funnode->returns;
	fun->funnode->returns = edge;
}

void bp_glob_seq (bp_fun_t fun, bp_stmt_t stmt, char* glob)
{
	bp_idref_t ref;

	while (stmt)
	{
		switch (stmt->type)
		{
		  case BP_ASSIGN:
			for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
			{
				bp_glob_doasgn(ref->ident,glob);
				bp_glob_doexpr(ref->expr,glob);
			}
			break;
		  case BP_RETURN:
			bp_glob_addreturn(fun,stmt);
		  case BP_PRINT:
			for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
				bp_glob_doexpr(ref->expr,glob);
			break;
		  case BP_CALL:
			for (ref = stmt->e.a.asgnlist; ref; ref = ref->next)
				bp_glob_doasgn(ref->ident,glob);
			for (ref = stmt->e.a.fmllist; ref; ref = ref->next)
				bp_glob_doexpr(ref->expr,glob);	
			bp_glob_addcall(fun,stmt);
			break;
		  case BP_ASSERT:
		  case BP_ASSUME:
		  case BP_CONSTRAIN:
			bp_glob_doexpr(stmt->e.c.expr,glob);
			break;
		  case BP_SKIP:
		  case BP_GOTO:
			break;
		  case BP_IF:
			bp_glob_doexpr(stmt->e.c.expr,glob);
			bp_glob_seq(fun,stmt->e.c.thenstmt,glob);
			bp_glob_seq(fun,stmt->e.c.elsestmt,glob);
			break;
		  case BP_WHILE:
			bp_glob_doexpr(stmt->e.c.expr,glob);
			bp_glob_seq(fun,stmt->e.c.thenstmt,glob);
			break;
		  default:
			  bpError("can't happen");
		}
		stmt = stmt->next;
	}
}

bp_funnode_t bp_glob_stack;
int bp_glob_count, bp_glob_comp;

void bp_glob_dfs (bp_funnode_t v)
{
	bp_funedge_t edge;
	bp_funnode_t w;
	int i;
	char *c, *d;

	v->stack = bp_glob_stack;
	bp_glob_stack = v;
	v->lowpt = v->dfsnum = ++bp_glob_count;

	for (edge = v->edges; edge; edge = edge->next)
	{
		w = edge->fn;
		if (!w->dfsnum)
		{
			bp_glob_dfs(w);
			if (w->lowpt < v->lowpt) v->lowpt = w->lowpt;
		}
		if (!w->comp && w->dfsnum < v->lowpt) v->lowpt = w->dfsnum;

		if (mc_globals)
			for (i = 0, c = v->usedglob, d = w->usedglob; 
					i < bp_numglobals; i++)
				*c++ |= *d++;
	}

	if (v->lowpt == v->dfsnum)
	{
		int weight = 0;
		bp_funnode_t new_stack = v->stack;
		v->stack = NULL;

		if (mc_globals)
			for (i = 0; i < bp_numglobals; i++)
				if (v->usedglob[i]) weight++;

		bp_glob_comp++;
		for (w = bp_glob_stack; w; w = w->stack)
		{
			w->comp = bp_glob_comp;
			if (!mc_globals) continue;
			w->fun->num_globals = weight;
			memcpy(w->usedglob,v->usedglob,bp_numglobals * sizeof(char));
		}

		bp_glob_stack = new_stack;
	}
}

void bp_build_call_graph ()
{
	bp_fun_t fun;
	bp_funnode_t fn;
	bp_ident_t var, cb;
	int j;


	for (fun = bp_functions; fun; fun = fun->next)
	{
		fun->funnode = fn = alloc_bp_funnode();

		fn->fun = fun;
		fn->lowpt = fn->dfsnum = fn->comp = 0;
		fn->stack = NULL;
		fn->edges = NULL;
		fn->returns = NULL;
		fn->calls = NULL;
		if (!mc_globals) continue;
		fn->usedglob = malloc(bp_numglobals * sizeof(char));
		memset(fn->usedglob,0,bp_numglobals * sizeof(char));
	}

	bpg_flag = malloc(bp_maxlocs * sizeof(char));

	for (fun = bp_functions; fun; fun = fun->next)
	{
#ifdef BP_MR_ENFORCE
		if (fun->enforce) bp_glob_doexpr(fun->enforce,fun->funnode->usedglob);
#endif
		memset(bpg_flag,0,bp_maxlocs * sizeof(char));
		bp_glob_seq(fun,fun->stmt,fun->funnode->usedglob);

		for (var = fun->locals, j = 0, cb = fun->callbuf; var; 
				var = var->next, cb = cb->next)
			if (bpg_flag[var->index])
			{
				char tmp[16];

				sprintf(tmp,"*cb%d",var->index);
				cb->varname = wIdentCreate(tmp);
				cb->index = bp_assign_cb_index(cb->varname,bp_funcount,j++);
			}
	}

	free(bpg_flag);
	fun = (bp_fun_t)bp_lookup(bp_funtree,wIdentCreate("main"),-1);
	if (!fun) bpError("main function does not exist");
	bp_glob_count = bp_glob_comp = 0;
	bp_glob_stack = NULL;
	bp_glob_dfs(fun->funnode);
}

/***************************************************************************/

bp_flow_t bp_cd_wl;
bp_flow_t *bp_cd_last;

void bp_cd_add (bp_flow_t fn, int dist)
{
	if (fn->in_worklist) return;

//if (fn->label >= 0) printf("added %s in function %s with distance %d\n",wIdentString(fn->label),wIdentString(fn->fun->funname),dist);
	fn->in_worklist = 1;
	fn->distance = dist;
	fn->next_in_wl = NULL;
	*bp_cd_last = fn;
	bp_cd_last = &(fn->next_in_wl);
}

void bp_cd_donode (bp_flow_t fn)
{
	bp_edge_t edge;

	for (edge = fn->edges; edge;  edge = edge->next)
		if (edge->stmt->type != BP_CALL)
			bp_cd_add(edge->target,fn->distance + 1);
		else
		{
			bp_fun_t target = (bp_fun_t)bp_lookup(bp_funtree,edge->stmt->e.a.label,-1);
			bp_edge_t retedge = target->funnode->returns;
			while (retedge)
			{
				bp_cd_add(retedge->stmt->fn1,fn->distance + 1);
				retedge = retedge->next;
			}
		}

	if (!fn->edges)
	{
		bp_funedge_t retedge = fn->fun->funnode->calls;

//if (fn->fun->stmt->fn1 != fn) *(int*)NULL = 1;
//		printf(" function %s\n",wIdentString(fn->fun->funname));
		while (retedge)
		{ 
			/* if this call hasn't been made yet, ignore it */
//			if (!retedge->stmt->next->fn1->in_worklist) return;
//		printf("  called from %s\n",wIdentString(retedge->stmt->fn1->fun->funname));
			bp_cd_add(retedge->stmt->fn1,fn->distance + 1);
			retedge = retedge->nextcall;
		}
	}
}

void bp_compute_distances ()
{
	if (!bp_flow_root)
		bpError("target label not found!");

	bp_cd_last = &bp_cd_wl;
	bp_cd_add(bp_flow_root,0);

	while (bp_cd_wl)
	{
		bp_cd_donode(bp_cd_wl);
		bp_cd_wl = bp_cd_wl->next_in_wl;
	}
}
