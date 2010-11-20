#include <stdlib.h>
#include "bp.h"

extern int bp_declcount;
extern int mc_sepvar;

signed char bp_insert_decl (bp_tree_t* tree,
				wIdent name, short fun, bp_ident_t id)
{
	bp_tree_t tmp;
	signed char bal;

	if (!*tree)
	{
		(*tree) = alloc_bp_tree();
		(*tree)->name = name;
		(*tree)->bal = 0;
		(*tree)->fun = fun;
		(*tree)->id = id;
		(*tree)->left = (*tree)->right = NULL;
		return 1;
	}

	if ((*tree)->name == name)
	{
		if (fun == -2) return 0;	/* labels */

		if (!(*tree)->fun || (*tree)->fun == fun)
		{
			bpError("conflicting declarations");
			return 0;
		}

		(*tree)->fun = fun;
		(*tree)->id = id;
		return 0;
	}

	if ((*tree)->name < name)
		(*tree)->bal +=
			(bal = bp_insert_decl(&((*tree)->right),name,fun,id));
	else
		(*tree)->bal -=
			(bal = bp_insert_decl(&((*tree)->left),name,fun,id));

	if (!bal || !(*tree)->bal) return 0;
	if (bal && ((*tree)->bal == 1 || (*tree)->bal == -1)) return 1;

	if ((*tree)->bal == -2 && (*tree)->left->bal == -1)
	{	/* (T1 B T2) A T3 -> T1 B (T2 A T3) */
		tmp = (*tree)->left;
		(*tree)->left = tmp->right;
		tmp->right = *tree;
		*tree = tmp;

		(*tree)->bal = 0;
		(*tree)->right->bal = 0;
		return 0;
	}

	if ((*tree)->bal == -2 && (*tree)->left->bal == 1)
	{	/* (T1 B (T2 C T3)) A T4 -> (T1 B T2) C (T3 A T4) */
		tmp = (*tree)->left->right;
		(*tree)->left->right = tmp->left;
		tmp->left = (*tree)->left;
		(*tree)->left = tmp->right;
		tmp->right = (*tree);
		*tree = tmp;

		(*tree)->left->bal = ((*tree)->bal+1)/(-2);
		(*tree)->right->bal = ((*tree)->bal-1)/(-2);
		(*tree)->bal = 0;
		return 0;
	}

	if ((*tree)->bal == 2 && (*tree)->right->bal == 1)
	{
		tmp = (*tree)->right;
		(*tree)->right = tmp->left;
		tmp->left = *tree;
		*tree = tmp;
		
		(*tree)->bal = 0;
		(*tree)->left->bal = 0;
		return 0;
	}

	if ((*tree)->bal == 2 && (*tree)->right->bal == -1)
	{
		tmp = (*tree)->right->left;
		(*tree)->right->left = tmp->right;
		tmp->right = (*tree)->right;
		(*tree)->right = tmp->left;
		tmp->left = (*tree);
		*tree = tmp;

		(*tree)->left->bal = ((*tree)->bal+1)/(-2);
		(*tree)->right->bal = ((*tree)->bal-1)/(-2);
		(*tree)->bal = 0;
		return 0;
	}

	bpError("this shouldn't happen");
	return 0;
}

bp_ident_t bp_lookup (bp_tree_t tree, wIdent name, short fun)
{
	if (!tree) return NULL;
	if (tree->name == name)
	{
		if (tree->fun == fun) return tree->id;
		if (tree->fun == 0 && fun > 0) return tree->id;
		return NULL;
	}

	if (tree->name < name) return bp_lookup(tree->right,name,fun);
	return bp_lookup(tree->left,name,fun);
}

bp_ident_t bp_find_var (bp_tree_t tree, wIdent name, short fun)
{
	bp_ident_t res = bp_lookup(tree,name,fun);
	if (!res) bpError("name lookup failure (%s/%d)",wIdentString(name),fun);
	return res;
}

bp_ident_t bp_find_decl (wIdent name, short fun)
{
	bp_ident_t res = bp_lookup(bp_nametree,name,fun);
	if (res) return res;

	res = alloc_bp_ident();
	res->varname = name;
	res->index = bp_assign_loc_index(name,bp_funcount);
	res->fun = fun;
	res->next = bp_decls;
	bp_decls = res;

	bp_insert_decl(&bp_nametree,name,fun,res);

	return res;
}

int globalctr = 0;
int localctr = 0;
bp_tree_t bp_indextree = NULL;

int bp_assign_glob_index (wIdent varname)
{
	int retval = 0;

	if (mc_sepvar == 0) retval = bp_declcount;
	if (mc_sepvar == 1) retval = globalctr++;
	if (mc_sepvar == 2) retval = bp_declcount;
	if (mc_sepvar == 3) retval = bp_declcount;

	if (retval >= bp_maxrets)
	{
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = varname;
		newid->index = retval;
		newid->next = bp_globids;
		bp_allglobids = bp_globids = newid;
		bp_maxrets = retval+1;
	}

	bp_declcount++;
	return retval;
}

int bp_assign_loc_index (wIdent varname, short fun)
{
	int retval = 0;

	if (mc_sepvar == 0) retval = bp_declcount;
	if (mc_sepvar == 1) retval = localctr++;
	if (mc_sepvar == 2 || mc_sepvar == 3)
	{
		bp_ident_t res = bp_lookup(bp_indextree,varname,0);
		if (res) retval = res->index;
		else
		{
			res = alloc_bp_ident();
			res->index = retval = localctr++;
			res->varname = varname;
			bp_insert_decl(&bp_indextree,varname,0,res);
		}
	}

	if (retval >= bp_maxlocs)
	{
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = varname;
		newid->index = retval;
		newid->next = bp_locids;
		bp_locids = newid;
		bp_maxlocs = retval+1;
	}

	bp_declcount++;
	return retval;
}

int bp_assign_parm_index (wIdent varname, short fun)
{
	if (mc_sepvar == 3)
	{
		char tmp[32];
		sprintf(tmp,"*param%d",bp_declcount+1);
		varname = wIdentCreate(tmp);
	}

	return bp_assign_loc_index(varname,fun);
}

int bp_assign_ret_index (wIdent varname, int i)
{
	int retval = 0;

	if (mc_sepvar == 0)	retval = bp_numglobals + i;
	if (mc_sepvar == 1) retval = globalctr++;
	if (mc_sepvar == 2)	retval = bp_numglobals + i;
	if (mc_sepvar == 3)	retval = bp_numglobals + i;

	if (retval >= bp_maxrets)
	{
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = varname;
		newid->index = retval;
		newid->next = bp_allglobids;
		bp_allglobids = newid;
		bp_maxrets = retval+1;
	}

	return retval;
}

int bp_assign_cb_index (wIdent varname, short fun, int i)
{
	int retval = 0;

	if (mc_sepvar == 0)	retval = bp_numglobals + i;
	if (mc_sepvar == 1) retval = globalctr++;
	if (mc_sepvar == 2 || mc_sepvar == 3)
	{
		bp_ident_t res = bp_lookup(bp_indextree,varname,0);
		if (res) retval = res->index;
		else
		{
			res = alloc_bp_ident();
			res->index = retval = bp_maxrets;
			res->varname = varname;
			bp_insert_decl(&bp_indextree,varname,0,res);
		}
	}

	if (retval >= bp_maxrets)
	{
		bp_ident_t newid = alloc_bp_ident();
		newid->varname = varname;
		newid->index = retval;
		newid->next = bp_allglobids;
		bp_allglobids = newid;
		bp_maxrets = retval+1;
	}

	return retval;
}
