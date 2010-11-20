/***************************************************************************/

#include "alloc.h"
#include "int.h"

/***************************************************************************/

static void intSetIndices (intVariable *var, int *index)
{	/* Assign the index of a BDD variable to the variables in
	   the list starting at var. The lowest index is given to
	   the last variable in the list. This is so that in nested
	   modules the inherited variables always get the same index. */
	if (!var) return;

	intSetIndices(var->next,index);
	var->bddindex = *index;
	*index += (var->type? var->type : 1)
		* (var->dim? var->dim : 1);
}

static void intSetModule (intModule *module)
{
	intVariable *var;
	for (var = module->vars; var; var = var->next)
		var->module = module;
}

extern intPds* intPdsCreate (wConfig *start, intModule *modules)
{
	intPds *pds = Malloc(sizeof(struct intPds));
	int global_vars = 0, local_vars = 0;
	intModule *proc;

	/* Set the BDD indices for all variables, find out how
	   many global and local variables are needed. */
	pds->modules = modules;
	intSetModule(modules);
	intSetIndices(modules->vars,&global_vars);

	for (proc = pds->modules->next; proc; proc = proc->next)
	{
		int num = 0;
		intSetModule(proc);
		intSetIndices(proc->vars,&num);
		if (num > local_vars) local_vars = num;
	}


	/* fixme: we should add our own shuffle procedure here */
	pds->pds = bddPdsCreate(start,global_vars,local_vars,NULL,NULL);

	return pds;
}

/***************************************************************************/

extern wRule* intPdsInsert (intPds *pds, wIdent p0, wIdent y0,
			wIdent p1, wIdent y1, wIdent y2, intExpr *expr)
{
	DdNode *expr_bdd = intBuild(pds,expr);
	wRule *result = bddPdsInsert(pds->pds,p0,y0,p1,y1,y2,expr_bdd);

	Cudd_RecursiveDeref(pds->pds->mgr,expr_bdd);
	return result;
}

/***************************************************************************/

extern void intPdsDelete (intPds *pds)
{
	bddPdsDelete(pds->pds);
	while (pds->modules)
	{
		intModule *tmp = pds->modules->next;
		while (pds->modules->vars)
		{
			intVariable *tmp = pds->modules->vars->next;
			Free(pds->modules->vars);
			pds->modules->vars = tmp;
		}
		wSetDelete(pds->modules->labels);
		Free(pds->modules);
		pds->modules = tmp;
	}
	Free(pds);
}

/***************************************************************************/
