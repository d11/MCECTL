/***************************************************************************/

#include "alloc.h"
#include "int.h"
#include "two.h"

/***************************************************************************/

static int intDetermine (int *cube, intVariable *var,
				DdNode **bddvars, int offset)
{
	int total = 0, bitvalue, index, i;
	if (var->type == 0)	/* boolean */
	{
		return cube[bddvars[var->bddindex + offset]->index];
	}

	/* integer */
	bitvalue = (1 << (var->type-1));
	index = var->bddindex + offset;
	for (i = 0; i < var->type; i++)
	{
		if (cube[bddvars[index++]->index]) total += bitvalue;
		bitvalue /= 2;
	}
	return total;
}

static intValuation* intConvertValues (intPds *pds, DdNode *valuation,
					intVariable *ivars, DdNode **bddvars)
{
	intValuation *result = NULL;
	intValuation **resptr = &result;
	int *cube, i;

	bddInitCube(pds->pds->mgr,&cube);
        Cudd_BddToCubeArray(pds->pds->mgr,valuation,cube);

	while (ivars)
	{
		*resptr = Malloc(sizeof(intValuation));
		(*resptr)->var = ivars;
		(*resptr)->next = NULL;

		if (ivars->dim)
		{
			(*resptr)->value.array = Malloc(ivars->dim*sizeof(int));
			for (i = 0; i < ivars->dim; i++)
				(*resptr)->value.array[i] = 
					intDetermine(cube, ivars, bddvars,
							i * ivars->type);
		}
		else
		{
			(*resptr)->value.integer =
				intDetermine(cube,ivars,bddvars,0);
		}
		
		ivars = ivars->next;
		resptr = &((*resptr)->next);
	}

	Free(cube);
	return result;
}

static void intConvertPathElem (intPds *pds, intPath **isptr, bddPath *bpath)
{
	intModule *im;

	*isptr = Malloc(sizeof(struct intPath));
	(*isptr)->stack = bpath->stack;

	/* find the module to which bpath->stack belongs */
	for (im = pds->modules->next; im; im = im->next)
		if (wSetFind(im->labels,bpath->stack)) break;
	(*isptr)->values = im? intConvertValues(pds,bpath->bdd,
						im->vars,pds->pds->l0) : NULL;
	(*isptr)->refcount = 1;
	(*isptr)->rest_path = NULL;

	/* remember the result */
	bpath->user = *isptr;
}

static void intConvertConfig (intPds *pds, intConfig *icfg, bddConfig *bcfg)
{
	intPath **isptr;
	bddPath *bpath;

	icfg->state = bcfg->state;
	icfg->values = intConvertValues(pds,bcfg->bdd,
					pds->modules->vars,pds->pds->g0);
	
	isptr = &(icfg->stack);
	bpath = bcfg->stack;
	while (bpath)
	{
		/* is the result already known? */
		if (bpath->user)
		{
			*isptr = (intPath*)(bpath->user);
			(*isptr)->refcount++;
			break;
		}
		/* otherwise, convert new assignment */
		intConvertPathElem(pds,isptr,bpath);
		isptr = &((*isptr)->rest_path);
		bpath = bpath->rest_path;
	}
}

static intResult *intConvertResult (intPds *pds, bddResult *bres)
{
	intResult *result = Malloc(sizeof(struct intResult));
	intConfig **icfgptr;
	bddConfig *bcfg;

	result->yesno = bres->yesno;
	result->prefix = result->loop = NULL;
	if (result->yesno == RESULT_NO) return result;

	icfgptr = &(result->prefix);
	bcfg = bres->prefix;
	while (bcfg)
	{
		*icfgptr = Malloc(sizeof(struct intConfig));
		intConvertConfig(pds,*icfgptr,bcfg);
		icfgptr = &((*icfgptr)->next_config);
		bcfg = bcfg->next_config;
	}
	*icfgptr = NULL;

	return result;
}

/***************************************************************************/

extern intResult* intAnalyse (intPds *pds, intProblem *problem)
{

	intResult *result;
	bddResult *bres;

	if (problem->type == PROBLEM_REACHABILITY_PRE_2
			|| problem->type == PROBLEM_REACHABILITY_2)
		bres = twoAnalyse(pds, problem);
	else
		bres = bddAnalyse(pds->pds,problem);
	result = intConvertResult(pds,bres);
	bddDeleteResult(pds->pds,bres);
	return result;
}

static void intPrintValues (intValuation *values)
{
	int counter = 0;
	while (values)
	{
		intVariable *var = values->var;
		printf("\t%s = ",wIdentString(var->name));
		if (var->dim)
		{
			int i;
			printf("[");
			for (i = 0; i < var->dim; i++)
			{
				if (i) printf(",");
				printf("%d",values->value.array[i]);
			}
			printf("]");
		}
		else if (var->type)
			printf("%d",values->value.integer);
		else
			printf("%s",values->value.integer? "true" : "false");
		if ((counter++)%6 == 5) printf("\n");
		values = values->next;
	}
	printf("\n");
}

static void intPrintConfig (intPds *pds, intConfig *config)
{
	intPath *path;
	int i = 0;

	printf("<%s,",wIdentString(config->state));
	for (path = config->stack; path; path = path->rest_path)
		printf(" %s",wIdentString(path->stack));
	printf(">\n");

	if (config->values)
	{
		printf("Global variables:\n");
		intPrintValues(config->values);
	}

	for (path = config->stack; path; path = path->rest_path)
	{
		if (!path->values) continue;
		printf("Local variables at stack position %d:\n",++i);
		intPrintValues(path->values);
	}
}

extern void intPrintResult (intPds *pds, intResult *result)
{
	intConfig *config;

	if (result->yesno == RESULT_NO)
	{
		printf("Not reachable.\n");
		return;
	}

	printf("Reachable. Trace:\n");
	printf("\n-------------------\n\n");

	for (config = result->prefix; config; config = config->next_config)
	{
		intPrintConfig(pds,config);
		printf("\n-------------------\n\n");
	}
}

/***************************************************************************/

static void intDeleteValues (intValuation *val)
{
	while (val)
	{
		intValuation *next = val->next;
		if (val->var->dim) Free(val->value.array);
		Free(val);
		val = next;
	}
}

static void intDeletePath (intPath *path)
{
	while (path)
	{
		intPath *next = path->rest_path;
		if (--path->refcount) break;
		intDeleteValues(path->values);
		Free(path);
		path = next;
	}
}

static void intDeleteConfig (intConfig *config)
{
	while (config)
	{
		intConfig *next = config->next_config;
		intDeleteValues(config->values);
		intDeletePath(config->stack);
		Free(config);
		config = next;
	}
}

extern void intDeleteResult (intPds *pds, intResult *result)
{
	intDeleteConfig(result->prefix);
	intDeleteConfig(result->loop);
	Free(result);
}

/***************************************************************************/
