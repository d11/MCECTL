/**CFile***********************************************************************

  FileName    [FlatHierarchy.c]

  PackageName [compile]

  Synopsis    [The class is used to store results of flattening a hierarchy]

  Description [See description in FlatHierarchy.h]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2. 
  Copyright (C) 2005 by ITC-irst. 

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information on NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/

#include "FlatHierarchy.h" 
#include "utils/utils.h" 
#include "utils/assoc.h" 
#include "parser/symbols.h" 


static char rcsid[] UTIL_UNUSED = "$Id: FlatHierarchy.c,v 1.1.2.1.6.7 2007/05/11 08:37:48 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [Data structure used to store the results of compilation.]

  Description []

******************************************************************************/
struct FlatHierarchy {
  node_ptr init_expr;
  node_ptr invar_expr;
  node_ptr trans_expr;
  node_ptr input_expr;
  node_ptr assign_expr;
  node_ptr justice_expr;
  node_ptr compassion_expr;
  node_ptr spec_expr;
  node_ptr ltlspec_expr;
  node_ptr invarspec_expr;
  node_ptr pslspec_expr;
  node_ptr compute_expr;

  node_ptr pred_list;
  node_ptr mirror_list;

  node_ptr var_list; /* variables declared in the given hierarchy */

  hash_ptr assign_hash; /* a hash table that for every variable
  'var_name' the hash contains the following associations:
        init(var_name) -> CONS(rhs, init_list)
                    where rhs is the right handsides of init-assignments of the
                    given variable
		    init-list is a list of INIT expressions of this hierarchy,
		    which contain the variable
        next(var_name) -> CONS( rhs, trans-list)
                     where rhs can be 
                                case module-1.running : rhs-1;
                                case module-2.running : rhs-2;
				...
				case 1: var_name;

		    where rhs-n is the right hand side of 
		    the next-assignment in the process module-n.
		    If there are no processes then the structure degrades
		    to just one 'rhs'.
		    trans-list is a list of TRANS of this hierarchy,
		    which contain var_name or expression next(var_name)
		    
          var_name : the structure is the same as for init(var_name) except 
               that the rhs-n are the right handside of invar-assignment, and
	       init-list is a list of INVAR expressions  */
};


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void flat_hierarchy_init ARGS((FlatHierarchy_ptr self));
static void flat_hierarchy_deinit ARGS((FlatHierarchy_ptr self));
static void flat_hierarchy_copy ARGS((const FlatHierarchy_ptr self, 
				      FlatHierarchy_ptr other));

/* This will be removed when assignment of bit selection is implemented */
void error_bit_selection_assignment_not_supported ARGS((node_ptr name));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [The constructor]

  Description        [The class is used to store information 
  obtained after flattening module hierarchy.
  These class stores:
        the list of TRANS, INIT, INVAR, ASSIGN, SPEC, COMPUTE, LTLSPEC,
	PSLSPEC, INVARSPEC, JUSTICE, COMPASSION,
	a full list of variables declared in the hierarchy,
	a hash table associating variables to their assignments and constrains.
	
  NOTE: this structure is filled in by compileFlatten.c routines. There are 
  a few assumptions about the content stored in this class:
  1. All expressions are stored in the same order as in the input 
     file (in module body or module instantiation order).
  2. Assigns are stored as a list of pairs 
     {process instance name, assignments in it}.
  3. Variable list contains only vars declared in this hierarchy.
  4. The association var->assignments should be for assignments of
     this hierarchy only. 
     Note that var may potentially be from another hierarchy. For
     example, with Games of MBP package an assignment in the body of
     one hierarchy (one player) may have on the left hand side a variable from
     another hierarchy (another player).
     See FlatHierarchy_lookup_assign, FlatHierarchy_insert_assign
  5. The association var->constrains (init, trans, invar) should be
     for constrains of this hierarchy only. Similar to
     var->assignment association (see above) a variable may
     potentially be from another hierarchy.
     See FlatHierarchy_lookup_constrains, FlatHierarchy_add_constrains
  ]

  SideEffects        []

******************************************************************************/
FlatHierarchy_ptr FlatHierarchy_create()
{
  FlatHierarchy_ptr self = ALLOC(struct FlatHierarchy, 1);
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  flat_hierarchy_init(self);
  return self;
}


/**Function********************************************************************

  Synopsis           [Class FlatHierarcy destructorUtility constructor]

  Description        [Use this constructor to set the main hierarchy members]

  SideEffects        [FlatHierarchy_create]

******************************************************************************/
EXTERN FlatHierarchy_ptr 
FlatHierarchy_create_from_members(node_ptr init, node_ptr invar, 
				  node_ptr trans, node_ptr input, 
				  node_ptr justice, node_ptr compassion)
{
  FlatHierarchy_ptr self = FlatHierarchy_create();
 
  FlatHierarchy_set_init(self, init);
  FlatHierarchy_set_invar(self, invar);
  FlatHierarchy_set_trans(self, trans);
  FlatHierarchy_set_input(self, input);
  FlatHierarchy_set_justice(self, justice);
  FlatHierarchy_set_compassion(self, compassion);

  return self;
}


/**Function********************************************************************

  Synopsis           [Class FlatHierarcy destructor]

  Description        [The destoructor does not destroy the nodes 
  given to it with access functions.]

  SideEffects        [FlatHierarchy_create]

******************************************************************************/
void FlatHierarchy_destroy(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  flat_hierarchy_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis           [Returns a newly created instance that is a copy of self]

  Description        []

  SideEffects        []

******************************************************************************/
FlatHierarchy_ptr FlatHierarchy_copy(const FlatHierarchy_ptr self)
{
  FlatHierarchy_ptr res;

  FLAT_HIERARCHY_CHECK_INSTANCE(self);

  res = FlatHierarchy_create();  
  flat_hierarchy_copy(self, res);
  return res;  
}


/**Function********************************************************************

  Synopsis           [A set of functions accessing the fields of the class]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr FlatHierarchy_get_init(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->init_expr);
}
void FlatHierarchy_set_init(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->init_expr = n;
}

node_ptr FlatHierarchy_get_invar(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->invar_expr);
}
void FlatHierarchy_set_invar(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->invar_expr = n;
}

node_ptr FlatHierarchy_get_trans(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->trans_expr);
}
void FlatHierarchy_set_trans(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->trans_expr = n;
}

node_ptr FlatHierarchy_get_input(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->input_expr);
}
void FlatHierarchy_set_input(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->input_expr = n;
}

node_ptr FlatHierarchy_get_assign(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->assign_expr);
}
void FlatHierarchy_set_assign(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->assign_expr = n;
}

node_ptr FlatHierarchy_get_justice(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->justice_expr);
}
void FlatHierarchy_set_justice(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->justice_expr = n;
}

node_ptr FlatHierarchy_get_compassion(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->compassion_expr);
}
void FlatHierarchy_set_compassion(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->compassion_expr = n;
}

node_ptr FlatHierarchy_get_spec(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->spec_expr);
}
void FlatHierarchy_set_spec(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->spec_expr = n;
}

node_ptr FlatHierarchy_get_ltlspec(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->ltlspec_expr);
}
void FlatHierarchy_set_ltlspec(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->ltlspec_expr = n;
}

node_ptr FlatHierarchy_get_invarspec(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->invarspec_expr);
}
void FlatHierarchy_set_invarspec(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->invarspec_expr = n;
}

node_ptr FlatHierarchy_get_pslspec(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->pslspec_expr);
}
void FlatHierarchy_set_pslspec(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->pslspec_expr = n;
}

node_ptr FlatHierarchy_get_compute(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return(self->compute_expr);
}
void FlatHierarchy_set_compute(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->compute_expr = n;
}


/**Function********************************************************************

  Synopsis [Returns a list of variables declared in the given
  hierarchy]

  Description        []

  SideEffects        []

  SeeAlso            [FlatHierarchy_set_var_list]

******************************************************************************/
node_ptr FlatHierarchy_get_var_list(FlatHierarchy_ptr self)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  return self->var_list;
}

/**Function********************************************************************

  Synopsis           [Add a variable name to the list of variables 
  declared in the given hierarchy]

  Description        []

  SideEffects        []

  SeeAlso            [FlatHierarchy_get_var_list]

******************************************************************************/
void FlatHierarchy_add_to_var_list(FlatHierarchy_ptr self, node_ptr n)
{
  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  self->var_list = cons(n, self->var_list);
}


/**Function********************************************************************

  Synopsis           [Returns the right handside of an assignment which has
  "name" as the left handside.]

  Description        [The name can be a usual variable name, init(variable
  name) or next(variable name). The name should be fully resolved (and
  created with find_node).

  NB: All returned assignments are supposed to be declared in the
  given hierarchy.]

  SideEffects        []

  SeeAlso            [FlatHierarchy_insert_assign]

******************************************************************************/
node_ptr FlatHierarchy_lookup_assign(FlatHierarchy_ptr self, node_ptr name)
{
  node_ptr res;
  nusmv_assert(self != NULL);

  /* Currently, bit selection is not allowed on the left side of
     assignments */
  if (node_get_type(name) == BIT_SELECTION || 
      ((node_get_type(name) == NEXT || node_get_type(name) == SMALLINIT) &&
       node_get_type(car(name)) == BIT_SELECTION)) {
    error_bit_selection_assignment_not_supported(name);
  }

  /* common consistency check : the name should be correctly formed */
  nusmv_assert(SMALLINIT == node_get_type(name) || 
	       NEXT == node_get_type(name) ||
	       DOT == node_get_type(name) || ATOM == node_get_type(name) ||
	       ARRAY == node_get_type(name) || BIT == node_get_type(name));
  
  res = find_assoc(self->assign_hash, name);
  if (Nil == res) return Nil;

  nusmv_assert(CONS == node_get_type(res));

  return car(res);
}


/**Function******************************are supposed to be**************************************

  Synopsis           [Insert the right handside of an assignment which
  has "name" as the left handside]

  Description        [The name can be a usual variable name, init(var-name) or
  next(var-name). 
  The variable name should be fully resolved (and created  with find_node).
 
  NB: All given assignments should have been declared in the given hierarchy.]

  SideEffects        []

  SeeAlso            [FlatHierarchy_lookup_assign]

******************************************************************************/
void FlatHierarchy_insert_assign(FlatHierarchy_ptr self, node_ptr name,
				 node_ptr assign)
{
  node_ptr cont;

  nusmv_assert(self != NULL);
  cont = find_assoc(self->assign_hash, name);
  
  if (Nil == cont) { /* there was no container before => create a new one */
    cont = cons(Nil, Nil);
    insert_assoc(self->assign_hash, name, cont);
  }
  
  /* [AT] I think, this is true:
     name and init(name) can have only one assignment 
  */
  nusmv_assert( (node_get_type(name) != SMALLINIT &&
		 node_get_type(name) != DOT) || Nil == car(cont));
  
  setcar(cont, assign);
  return;
}


/**Function********************************************************************

  Synopsis           [Returns  a list of constrains which contain
  a variable of the given name.]

  Description        [
  If the parameter "name" is a usual variable name then
  the INVAR expressions are returned.
  If the parameter "name" has a form init(var-name) then
  the INIT expressions are returned.
  If the parameter "name" has a form next(var-name) then
  the TRANS expressions are returned.

  The name should be fully resolved (and created with find_node).

  NB: All returned expressions are supposed to be declared in the
  given hierarchy.]

  SideEffects        []

  SeeAlso            [FlatHierarchy_add_constrains]

******************************************************************************/
node_ptr FlatHierarchy_lookup_constrains(FlatHierarchy_ptr self,
					     node_ptr name)
{
  node_ptr res;
  nusmv_assert(self != NULL);

  /* common consistency check : the name should be correctly formed */
  nusmv_assert(SMALLINIT == node_get_type(name) || NEXT == node_get_type(name) ||
	       DOT == node_get_type(name) ||
	       ARRAY == node_get_type(name) || BIT == node_get_type(name));
  
  res = find_assoc(self->assign_hash, name);
  if (Nil == res) return Nil;

  nusmv_assert(CONS == node_get_type(res));

  return cdr(res);
}


/**Function********************************************************************

  Synopsis           [Adds the given expressions to the list 
  of constrains associated to the given variable]

  Description        [
  The parameter "name" can be a usual variable name then 
  an expression is expected to be INVAR body.
  The parameter "name" can have a form init(var-name) then 
  an expression is expected to be INIT body.
  The parameter "name" can have a form next(var-name) then 
  an expression is expected to be TRANS body.

  In any case the variable name should be fully resolved (and created
  with find_node).

  NB: All given expressions should have been declared in the given hierarchy.]


  SideEffects        []

  SeeAlso            [FlatHierarchy_lookup_constrains]

******************************************************************************/
void FlatHierarchy_add_constrains(FlatHierarchy_ptr self, node_ptr name,
				  node_ptr expr)
{
  node_ptr cont;

  FLAT_HIERARCHY_CHECK_INSTANCE(self);
  cont = find_assoc(self->assign_hash, name);
  
  if (Nil == cont) { /* there was no container before => create a new one */
    cont = cons(Nil, Nil);
    insert_assoc(self->assign_hash, name, cont);
  }
  
  if (Nil == cdr(cont)) setcdr(cont, expr);
  else setcdr(cont, find_node(AND, cdr(cont), expr));
  
  return;
}

node_ptr FlatHierarchy_get_preds(FlatHierarchy_ptr cmp)
{
  return cmp->pred_list;
}

void FlatHierarchy_add_pred(FlatHierarchy_ptr cmp, node_ptr n)
{
  cmp->pred_list = cons(n, cmp->pred_list);
}

void FlatHierarchy_set_pred(FlatHierarchy_ptr cmp, node_ptr n)
{
  cmp->pred_list = n;
}

node_ptr FlatHierarchy_get_mirrors(FlatHierarchy_ptr cmp)
{
  return cmp->mirror_list;
}

void FlatHierarchy_add_mirror(FlatHierarchy_ptr cmp, node_ptr n)
{
  cmp->mirror_list = cons(n, cmp->mirror_list);
}

void FlatHierarchy_set_mirror(FlatHierarchy_ptr cmp, node_ptr n)
{
  cmp->mirror_list = n;
}


/*---------------------------------------------------------------------------*/
/* Static function definitions                                               */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [initialisation function used by the constructor]

  Description        []

  SideEffects        []

******************************************************************************/
static void flat_hierarchy_init(FlatHierarchy_ptr self)
{
  self->init_expr            = Nil;
  self->invar_expr           = Nil;
  self->trans_expr           = Nil;
  self->input_expr           = Nil;
  self->assign_expr          = Nil;
  self->justice_expr         = Nil;
  self->compassion_expr      = Nil;
  self->spec_expr            = Nil;
  self->compute_expr         = Nil;
  self->ltlspec_expr         = Nil;
  self->pslspec_expr         = Nil;
  self->invarspec_expr       = Nil;
  self->var_list             = Nil;
  self->pred_list            = Nil;
  self->mirror_list          = Nil;

  self->assign_hash = new_assoc();
}


/**Function********************************************************************

  Synopsis           [de-initialisation function used by the destructor]

  Description        []

  SideEffects        []

******************************************************************************/
static void flat_hierarchy_deinit(FlatHierarchy_ptr self)
{
  free_assoc(self->assign_hash);
}


/**Function********************************************************************

Synopsis           [Copies self's data into other, so that other contains 
the same information]

  Description        []

  SideEffects        []

******************************************************************************/
static void 
flat_hierarchy_copy(const FlatHierarchy_ptr self, FlatHierarchy_ptr other)
{
  other->init_expr            = self->init_expr;
  other->invar_expr           = self->invar_expr;
  other->trans_expr           = self->trans_expr;
  other->input_expr           = self->input_expr;
  other->assign_expr          = self->assign_expr;
  other->justice_expr         = self->justice_expr;
  other->compassion_expr      = self->compassion_expr;
  other->spec_expr            = self->spec_expr;
  other->compute_expr         = self->compute_expr;
  other->ltlspec_expr         = self->ltlspec_expr;
  other->pslspec_expr         = self->pslspec_expr;
  other->invarspec_expr       = self->invarspec_expr;
  other->var_list             = self->var_list;
  other->pred_list            = self->pred_list;
  other->mirror_list          = self->mirror_list;

  free_assoc(other->assign_hash);
  other->assign_hash = copy_assoc(self->assign_hash);
}

