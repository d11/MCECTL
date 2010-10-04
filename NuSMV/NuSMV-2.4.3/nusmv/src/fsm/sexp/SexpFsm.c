/**CFile*****************************************************************

  FileName    [SexpFsm.c]

  PackageName [fsm.sexp]

  Synopsis    [The SexpFsm implementation]

  Description []

  SeeAlso     [SexpFsm.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``fsm.sexp'' package of NuSMV version 2. 
  Copyright (C) 2003 by ITC-irst. 

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

#include "SexpFsm.h"
#include "sexpInt.h"

/* there are still some variables to be accessed there: */
#include "compile/compile.h" 
#include "compile/PredicateNormaliser.h"

#include "parser/symbols.h"

#include "utils/error.h"
#include "utils/assoc.h"


static char rcsid[] UTIL_UNUSED = "$Id: SexpFsm.c,v 1.1.2.8.4.10.4.18 2007/05/09 15:43:28 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constants declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Type************************************************************************

  Synopsis [A fsm for a single variable. It is represented as a triple
  of Expr_ptr ]

  Description [Private structure, internally used]

  SeeAlso     []

******************************************************************************/
typedef node_ptr VarFsm_ptr;
#define VAR_FSM(x)  ((VarFsm_ptr) x)


/**Type************************************************************************

  Synopsis    [Sexp FSM, the type exported by this package]

  Description []

  SeeAlso     []

******************************************************************************/
typedef struct SexpFsm_TAG {
  BddEnc_ptr enc;
  SymbTable_ptr symb_table;
  SymbLayer_ptr det_layer; /* for booleanization */
  SymbLayer_ptr inl_layer; /* for inlining */

  VarSet_ptr vars_list;
  hash_ptr hash_var_fsm; 
  
  FlatHierarchy_ptr hierarchy; /* contains fsm data */

  int* family_counter; /* for reference counting */

  /* each entry contains list of vertex the entry connects */
  hash_ptr deps_dag_to;

  /* each entry contains list of vertex connecting the entry */
  hash_ptr deps_dag_from;

  /* substitution hash */
  hash_ptr inlining_subst_hash;

  /* list of those nodes that have been removed */
  NodeList_ptr deps_dag_rem_nodes;

} SexpFsm;



/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define NODE_PTR(x)  ((node_ptr) x)  /* upcast from VarFsm_ptr */


#define _PRINT(txt) \
  fprintf(nusmv_stderr, txt); \
  fflush(nusmv_stderr)


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void sexp_fsm_hash_var_fsm_init ARGS((SexpFsm_ptr self));

static void sexp_fsm_init ARGS((SexpFsm_ptr self, 
                                BddEnc_ptr enc, 
                                SymbLayer_ptr det_layer, 
                                SymbLayer_ptr inl_layer, 
                                VarSet_ptr vars_list, 
                                FlatHierarchy_ptr hierarchy));

static void sexp_fsm_deinit ARGS((SexpFsm_ptr self));

static void sexp_fsm_copy ARGS((const SexpFsm_ptr self, SexpFsm_ptr copy));

static Expr_ptr sexp_fsm_simplify_expr ARGS((SexpFsm_ptr self, 
                                             hash_ptr hash, Expr_ptr expr, 
                                             const int group));
			    
static Expr_ptr sexp_fsm_booleanize_expr ARGS((SexpFsm_ptr self, 
                                               Expr_ptr expr));

static hash_ptr simplifier_hash_create ARGS((void));
static void simplifier_hash_destroy ARGS((hash_ptr hash));
static void simplifier_hash_add_expr ARGS((hash_ptr hash, 
                                           Expr_ptr expr, const int group));
static boolean simplifier_hash_query_expr ARGS((hash_ptr hash, Expr_ptr expr, 
                                                const int group));

static void sexp_fsm_hash_var_fsm_destroy ARGS((SexpFsm_ptr self));
static assoc_retval 
sexp_fsm_callback_var_fsm_free ARGS((char *key, char *data, char * arg));
static VarFsm_ptr 
sexp_fsm_hash_var_fsm_lookup_var ARGS((SexpFsm_ptr self, node_ptr var));
static void 
sexp_fsm_hash_var_fsm_insert_var ARGS((SexpFsm_ptr self, 
                                       node_ptr var, VarFsm_ptr varfsm));

static node_ptr 
sexp_fsm_apply_inlining ARGS((SexpFsm_ptr self, node_ptr expr));

static VarFsm_ptr var_fsm_create ARGS((Expr_ptr init, Expr_ptr invar, 
                                       Expr_ptr next));

static void var_fsm_destroy ARGS((VarFsm_ptr self));
static Expr_ptr var_fsm_get_init ARGS((VarFsm_ptr self));
static Expr_ptr var_fsm_get_invar ARGS((VarFsm_ptr self));
static Expr_ptr var_fsm_get_next ARGS((VarFsm_ptr self));
static Expr_ptr var_fsm_get_input ARGS((VarFsm_ptr self));


static void sexp_fsm_add_vertex ARGS((SexpFsm_ptr self, 
                                      node_ptr from, node_ptr to));
static boolean 
sexp_fsm_add_vertices_from_expr ARGS((SexpFsm_ptr self, 
                                      node_ptr name, node_ptr expr, 
                                      VarFilterType filter));
     

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis [Costructor for a scalar and boolean sexp fsm. If det_layer
  is null, then a scalar fsm will be created, otherwise a boolean fsm
  will be created. When the sexp fsm is not booleanizable, det_layer
  *must* be NULL and BddEnc *can* be NULL. inl_layer is a layer used
  for inlining (syntactic simplification) of the FSM (can be NULL for
  no-simplification)]

  Description [If provided, the layer will be filled with the
  determinization variables that might derive from the booleanization.
  hierarchy will be copied, so the caller is responsible for its
  destruction.]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr SexpFsm_create(BddEnc_ptr enc,
                           SymbLayer_ptr det_layer, 
                           SymbLayer_ptr inl_layer, 
                           VarSet_ptr vars_list, 
                           FlatHierarchy_ptr hierarchy)
{
  SexpFsm_ptr self;

  /* allocation: */
  self = ALLOC(SexpFsm, 1);
  SEXP_FSM_CHECK_INSTANCE(self);

  /* initialization: */
  sexp_fsm_init(self, enc, det_layer, inl_layer, vars_list, hierarchy);
  return self;
}


/**Function********************************************************************

  Synopsis           [Copy costructor]

  Description        []

  SideEffects        []

******************************************************************************/
SexpFsm_ptr SexpFsm_copy(const SexpFsm_ptr self)
{
  SexpFsm_ptr copy;

  SEXP_FSM_CHECK_INSTANCE(self);

  copy = ALLOC(SexpFsm, 1);
  SEXP_FSM_CHECK_INSTANCE(copy);

  sexp_fsm_copy(self, copy);
  return copy;
}


/**Function********************************************************************

  Synopsis           [Copy the Sexp FSM and perform predicate-normalisation
  on all the expressions.]

  Description        [Predicate-normalisations means that an expression is
  modified in such a way that at the end the subexpressions of a
  not-boolean expression can be only not-boolean. This is performed by
  changing boolean expression "exp" (which is a subexpression of a
  not-boolean expression) to "ITE(exp, 1, 0)", and then pushing all
  ITE up to the root of not-boolean expressions.

  Constrain: the given Sexp FSM has to be NOT boolean. Otherwise,
  it is meaningless to apply normalisation functions, since all the exporessions
  are already boolean.
  ]

  SideEffects        [SexpFsm_copy]

******************************************************************************/
SexpFsm_ptr 
SexpFsm_create_predicate_normalised_copy(const SexpFsm_ptr self,
                                         PredicateNormaliser_ptr normaliser)
{
  SexpFsm_ptr copy;

  SEXP_FSM_CHECK_INSTANCE(self);
  nusmv_assert(SYMB_LAYER(NULL) == self->det_layer); /* NOT boolean */

  copy = SexpFsm_copy(self);

  FlatHierarchy_set_init(copy->hierarchy, 
                         PredicateNormaliser_normalise_expr(normaliser, 
                                 FlatHierarchy_get_init(copy->hierarchy)));

  FlatHierarchy_set_invar(copy->hierarchy, 
                         PredicateNormaliser_normalise_expr(normaliser, 
                                 FlatHierarchy_get_invar(copy->hierarchy)));

  FlatHierarchy_set_trans(copy->hierarchy, 
                          PredicateNormaliser_normalise_expr(normaliser, 
                                 FlatHierarchy_get_trans(copy->hierarchy)));

  FlatHierarchy_set_input(copy->hierarchy, 
                          PredicateNormaliser_normalise_expr(normaliser, 
                                 FlatHierarchy_get_input(copy->hierarchy)));

  FlatHierarchy_set_justice(copy->hierarchy, 
                            PredicateNormaliser_normalise_expr(normaliser,
                                 FlatHierarchy_get_justice(copy->hierarchy)));

  FlatHierarchy_set_compassion(copy->hierarchy, 
                    PredicateNormaliser_normalise_expr(normaliser,
                         FlatHierarchy_get_compassion(copy->hierarchy)));

  return copy;
}


/**Function********************************************************************

  Synopsis           [Destructor]

  Description        []

  SideEffects        []

******************************************************************************/
void SexpFsm_destroy(SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);

  sexp_fsm_deinit(self); 
  FREE(self);
}


/**Function********************************************************************

  Synopsis           [Returns the BoolEnc instance connected to self]

  Description [This method can be called only when a valid BddEnc was
  passed to the class constructor (not NULL). Returned instance do not
  belongs to the caller and must _not_ be destroyed]

  SideEffects        []

******************************************************************************/
BoolEnc_ptr SexpFsm_get_bool_enc(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(self->enc));
}


/**Function********************************************************************

  Synopsis           [Returns the symbol table that is connected to the 
  BoolEnc instance connected to self]

  Description [This method can be called only when a valid BddEnc was
  passed to the class constructor (not NULL). Returned instance do not
  belongs to the caller and must _not_ be destroyed]

  SideEffects        []

******************************************************************************/
SymbTable_ptr SexpFsm_get_symb_table(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return BaseEnc_get_symbol_table(BASE_ENC(self->enc));  
}



/**Function********************************************************************

  Synopsis           [Construct a boolean fsm from a scalar one]

  Description [Given layer det_layer will be filled with the
  determinization variables coming from the booleanization
  process. Returns a copy if self is already booleanized. Optional
  inl_layer is used by the inlining simplification (can be NULL)]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr SexpFsm_scalar_to_boolean(const SexpFsm_ptr self, 
                                      SymbLayer_ptr det_layer, 
                                      SymbLayer_ptr inl_layer)
{
  SexpFsm_ptr result;
  
  SEXP_FSM_CHECK_INSTANCE(self);
  SYMB_LAYER_CHECK_INSTANCE(det_layer); /* must be a valid layer */

  if (SexpFsm_is_boolean(self)) {
    result = SexpFsm_copy(self);
  }
  else {
    /* creates a new booleanized copy */    
    result = ALLOC(SexpFsm, 1);
    SEXP_FSM_CHECK_INSTANCE(result);
    sexp_fsm_init(result, self->enc, det_layer, inl_layer, 
                  self->vars_list, self->hierarchy);
  }

  return result;
}


/**Function********************************************************************

  Synopsis           [Returns true if self if a booleanized fsm, false if 
  it is scalar]

  Description        []

  SideEffects        []

******************************************************************************/
boolean SexpFsm_is_boolean(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return (self->det_layer != SYMB_LAYER(NULL));
}



/**Function********************************************************************

  Synopsis           [Returns a copy of the complete hierarchy]

  Description        [Returned hierarchy can be freely changed to create 
  a new sexp fsm derived from self. In any case the caller *must* destroy the 
  returned instance, as a copy of the internal hierarchy is returned. 
  Also, notice that the SexpFsm constructor copies the passed hierarchy.]

  SideEffects        []

******************************************************************************/
FlatHierarchy_ptr SexpFsm_get_hierarchy(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_copy(self->hierarchy);
}


/**Function********************************************************************

  Synopsis           [Returns an Expr that collects init states for all 
  variables handled by self]

  Description        []

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_init(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_get_init(self->hierarchy);
}


/**Function********************************************************************

  Synopsis           [Returns an Expr that collects invar states for all 
  variables handled by self]

  Description        []

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_invar(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_get_invar(self->hierarchy);
}


/**Function********************************************************************

  Synopsis           [Returns an Expr that collects all next states for all 
  variables handled by self]

  Description        []

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_trans(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_get_trans(self->hierarchy);
}


/**Function********************************************************************

  Synopsis           [Returns an Expr that collects all input states for all 
  variables handled by self]

  Description        []

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_input(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  /* Currently no constraints over input are allowed, thus we return
     true to inidicate this. */
  return FlatHierarchy_get_input(self->hierarchy);
}


/**Function********************************************************************

  Synopsis      [ Gets the sexp expression defining the initial state for
                  the variable "v". ]

  Description   [ Gets the sexp expression defining the initial state for
                  the variable "v". ]

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_var_init(const SexpFsm_ptr self, node_ptr var_name)
{
  VarFsm_ptr var_fsm;
  SEXP_FSM_CHECK_INSTANCE(self);

  var_fsm = find_assoc(self->hash_var_fsm, var_name);
  return var_fsm_get_init(var_fsm);
}


/**Function********************************************************************

  Synopsis      [ Gets the sexp expression defining the state constraints
                  for the variable "v". ]

  Description   [ Gets the sexp expression defining the state constraints
                  for the variable "v". ]

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_var_invar(const SexpFsm_ptr self, node_ptr var_name)
{
  VarFsm_ptr var_fsm;

  SEXP_FSM_CHECK_INSTANCE(self);

  var_fsm = find_assoc(self->hash_var_fsm, var_name);
  return var_fsm_get_invar(var_fsm);
}


/**Function********************************************************************

  Synopsis      [ Gets the sexp expression defining the transition relation
                  for the variable "v". ]

  Description   [ Gets the sexp expression defining the transition relation
                  for the variable "v". ]

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_var_trans(const SexpFsm_ptr self, node_ptr var_name)
{
  VarFsm_ptr var_fsm;

  SEXP_FSM_CHECK_INSTANCE(self);

  var_fsm = find_assoc(self->hash_var_fsm, var_name);
  return var_fsm_get_next(var_fsm);
}


/**Function********************************************************************

  Synopsis      [ Gets the sexp expression defining the input relation
                  for the variable "v". ]

  Description   []

  SideEffects        []

******************************************************************************/
Expr_ptr SexpFsm_get_var_input(const SexpFsm_ptr self, node_ptr var_name)
{
  VarFsm_ptr var_fsm;

  SEXP_FSM_CHECK_INSTANCE(self);

  var_fsm = find_assoc(self->hash_var_fsm, var_name);
  return var_fsm_get_input(var_fsm);
}




/**Function********************************************************************

  Synopsis      [ Gets the list of sexp expressions defining the set of justice 
                  constraints for this machine. ]

  Description   [ Gets the list of sexp expressions defining the set of justice 
                  constraints for this machine. ]

  SideEffects        []

******************************************************************************/
node_ptr SexpFsm_get_justice(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_get_justice(self->hierarchy);
}


/**Function********************************************************************

  Synopsis      [ Gets the list of sexp expressions defining the set of
                  compassion constraints for this machine. ]

  Description   [ Gets the list of sexp expressions defining the set of 
                  compassion constraints for this machine. ]

  SideEffects        []

******************************************************************************/
node_ptr SexpFsm_get_compassion(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return FlatHierarchy_get_compassion(self->hierarchy);
}


/**Function********************************************************************

  Synopsis      [Returns the list of variables in the FSM]

  Description   [Returned instance belongs to self]

  SideEffects   []

******************************************************************************/
VarSet_ptr SexpFsm_get_vars_list(const SexpFsm_ptr self)
{
  SEXP_FSM_CHECK_INSTANCE(self);
  return self->vars_list;
}


/*---------------------------------------------------------------------------*/
/* Static function definitions                                               */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis      []

  Description   []

  SideEffects   []

******************************************************************************/
static void sexp_fsm_create_vars_dag(SexpFsm_ptr self)
{
  SymbTable_ptr st;
  node_ptr iter;
  
  if (self->inl_layer == SYMB_LAYER(NULL)) {
    self->deps_dag_to = (hash_ptr) NULL;
    self->deps_dag_from = (hash_ptr) NULL;  
    self->deps_dag_rem_nodes = NODE_LIST(NULL);
    return;
  }
  
  if (opt_verbose_level_gt(options, 2)) {
    _PRINT("SexpFsm: Creating scalar inlining DAG... ");
  }
  st = SexpFsm_get_symb_table(self);
  self->deps_dag_to = new_assoc();
  nusmv_assert(self->deps_dag_to != (hash_ptr) NULL);
  self->deps_dag_from = new_assoc();
  nusmv_assert(self->deps_dag_from != (hash_ptr) NULL);

  for (iter = NODE_PTR(self->vars_list); iter != Nil; iter = cdr(iter)) {
    node_ptr expr, name;
    
    name = car(iter);
   
    /* invar */
    expr = SexpFsm_get_var_invar(self, name);   
    if (!sexp_fsm_add_vertices_from_expr(self, name, expr, VFT_ALL)) {
      sexp_fsm_add_vertex(self, name, Nil);
    }
    
    /* next */
    {
      node_ptr nname = find_node(NEXT, name, Nil);
      expr = SexpFsm_get_var_trans(self, name);
      boolean added_c = sexp_fsm_add_vertices_from_expr(self, nname, expr, 
                                                        VFT_CURRENT | VFT_INPUT);
      boolean added_n = sexp_fsm_add_vertices_from_expr(self, nname, expr, 
                                                        VFT_NEXT);
      if (!(added_c || added_n)) { sexp_fsm_add_vertex(self, nname, Nil); }
    }
  }

  /* removed nodes list */
  self->deps_dag_rem_nodes = NODE_LIST(NULL);

  if (opt_verbose_level_gt(options, 2)) {
    _PRINT(" Done\n");
  }
}



/**Function********************************************************************

  Synopsis      [Initializes either the boolean or scalar sexp fsm]

  Description [hierarchy is copied into an independent FlatHierarchy
  instance. If the new sexp must be based only on a set of variables, the 
  hierarchy must be empty]

  SideEffects        []

******************************************************************************/
static void sexp_fsm_init(SexpFsm_ptr self, 
                          BddEnc_ptr enc, 
                          SymbLayer_ptr det_layer, 
                          SymbLayer_ptr inl_layer, 
                          VarSet_ptr vars_list, 
                          FlatHierarchy_ptr hierarchy)
{
  /* -------------------------------------------------------------------- */ 
  /* 0. Initialization                                                    */
  /* -------------------------------------------------------------------- */ 

  /* inits some private members */
  self->enc = enc;
  self->symb_table = BaseEnc_get_symbol_table(BASE_ENC(enc));
  self->det_layer = det_layer;
  self->inl_layer = inl_layer;
  self->vars_list = copy_list(vars_list);
  self->hash_var_fsm = new_assoc();  
  self->hierarchy = FlatHierarchy_copy(hierarchy);

  self->deps_dag_to = (hash_ptr) NULL;
  self->deps_dag_from = (hash_ptr) NULL;
  self->deps_dag_rem_nodes = NODE_LIST(NULL);
  self->inlining_subst_hash = (hash_ptr) NULL;

  self->family_counter = ALLOC(int, 1);
  nusmv_assert(self->family_counter != (int*) NULL);
  *(self->family_counter) = 1; /* this is adam for this family */

  /* sets up the single variable FSMs */
  sexp_fsm_hash_var_fsm_init(self);


  /* -------------------------------------------------------------------- */ 
  /* 1. Semplification                                                    */
  /* -------------------------------------------------------------------- */ 
  {
    hash_ptr hash = simplifier_hash_create();
    node_ptr iter;

    /* init */
    FlatHierarchy_set_init(self->hierarchy, 
                           sexp_fsm_simplify_expr(self, hash,
                                                  FlatHierarchy_get_init(self->hierarchy), INIT));
    
    /* invar */
    FlatHierarchy_set_invar(self->hierarchy, 
                            sexp_fsm_simplify_expr(self, hash,
                                                   FlatHierarchy_get_invar(self->hierarchy), INVAR));

    /* trans */
    FlatHierarchy_set_trans(self->hierarchy, 
                            sexp_fsm_simplify_expr(self, hash, 
                                                   FlatHierarchy_get_trans(self->hierarchy), TRANS));


    /* now integrates information coming from the variables FSMs
       (assign and constraints) */    
    for (iter = NODE_PTR(self->vars_list); iter != Nil; iter = cdr(iter)) {
      node_ptr var = car(iter);
      VarFsm_ptr varfsm = sexp_fsm_hash_var_fsm_lookup_var(self, var);
    
      if (varfsm != VAR_FSM(NULL)) {
        Expr_ptr tmp;

        /* inits */
        tmp = var_fsm_get_init(varfsm);
        if (tmp != Nil) {
          tmp = sexp_fsm_simplify_expr(self, hash, tmp, INIT);
          FlatHierarchy_set_init(self->hierarchy, 
                                 Expr_and(FlatHierarchy_get_init(self->hierarchy),
                                          tmp));
        }

        /* invars */
        tmp = var_fsm_get_invar(varfsm);
        if (tmp != Nil) {
          tmp = sexp_fsm_simplify_expr(self, hash, tmp, INVAR);
          FlatHierarchy_set_invar(self->hierarchy, 
                                  Expr_and(FlatHierarchy_get_invar(self->hierarchy),
                                           tmp));
        }
      
        /* next */
        tmp = var_fsm_get_next(varfsm);
        if (tmp != Nil) {
          tmp = sexp_fsm_simplify_expr(self, hash, tmp, TRANS);
          FlatHierarchy_set_trans(self->hierarchy, 
                                  Expr_and(FlatHierarchy_get_trans(self->hierarchy),
                                           tmp));
        }
      }
    } /* loop over vars */
    
    simplifier_hash_destroy(hash); 
  }

  /* -------------------------------------------------------------------- */ 
  /* 2. Inlining                                                          */
  /* -------------------------------------------------------------------- */ 
  if (opt_symb_inlining(options)) {
    sexp_fsm_create_vars_dag(self); /* dependencies DAG */

    /* init */
    FlatHierarchy_set_init(self->hierarchy, 
                           sexp_fsm_apply_inlining(self, 
                                                   FlatHierarchy_get_init(self->hierarchy)));
    
    /* invar */
    FlatHierarchy_set_invar(self->hierarchy, 
                            sexp_fsm_apply_inlining(self, 
                                                    FlatHierarchy_get_invar(self->hierarchy)));

    /* trans */
    FlatHierarchy_set_trans(self->hierarchy, 
                            sexp_fsm_apply_inlining(self,
                                                    FlatHierarchy_get_trans(self->hierarchy)));
  }

  /* -------------------------------------------------------------------- */ 
  /* 3. Booleanization                                                    */
  /* -------------------------------------------------------------------- */ 
  {
    /* We set the verbose level to 0 and then we restore the original
       value. This because booleanization uses eval */  
    int curr_verbosity = get_verbose_level(options);
    set_verbose_level(options, 0);

    /* init */
    FlatHierarchy_set_init(self->hierarchy, 
                           sexp_fsm_booleanize_expr(self, 
                                                    FlatHierarchy_get_init(self->hierarchy)));
    
    /* invar */
    FlatHierarchy_set_invar(self->hierarchy, 
                            sexp_fsm_booleanize_expr(self, 
                                                     FlatHierarchy_get_invar(self->hierarchy)));

    /* trans */
    FlatHierarchy_set_trans(self->hierarchy, 
                            sexp_fsm_booleanize_expr(self,
                                                     FlatHierarchy_get_trans(self->hierarchy)));

    /* justice */
    FlatHierarchy_set_justice(self->hierarchy,
                              sexp_fsm_booleanize_expr(self,
                                                       FlatHierarchy_get_justice(self->hierarchy)));

    /* compassion */
    FlatHierarchy_set_compassion(self->hierarchy,
                                 sexp_fsm_booleanize_expr(self,
                                                          FlatHierarchy_get_compassion(self->hierarchy)));

    /* restores the verbosity level */
    set_verbose_level(options, curr_verbosity); 
  }
}



/**Function********************************************************************

  Synopsis [private service used as callback by
  sexp_fsm_destroy_vars_dag]

  Description   []

  SideEffects   []

******************************************************************************/
static enum st_retval __free_nodelist(char* key, char* val, char* data)
{ if (val != NULL) NodeList_destroy(NODE_LIST(val)); return ASSOC_DELETE; }


/**Function********************************************************************

  Synopsis      [Destroyer for the vars dag, used by inlining]

  Description   []

  SideEffects   []

******************************************************************************/
static void sexp_fsm_destroy_vars_dag(SexpFsm_ptr self)
{
  if (self->deps_dag_to != (hash_ptr) NULL) {
    if (*(self->family_counter) == 0) { 
      clear_assoc_and_free_entries(self->deps_dag_to, __free_nodelist);
    }
    free_assoc(self->deps_dag_to);
    self->deps_dag_to = (hash_ptr) NULL;
  }

  if (self->deps_dag_from != (hash_ptr) NULL) {
    if (*(self->family_counter) == 0) { 
      clear_assoc_and_free_entries(self->deps_dag_from, __free_nodelist);
    }
    free_assoc(self->deps_dag_from);
    self->deps_dag_from = (hash_ptr) NULL;
  }

  if (self->inlining_subst_hash != (hash_ptr) NULL) {
    free_assoc(self->inlining_subst_hash);
    self->inlining_subst_hash = (hash_ptr) NULL;
  }

  /* this should be NULL, but it might happen to be not in case of
     interruption during inlining */
  if (self->deps_dag_rem_nodes != NODE_LIST(NULL)) {
    NodeList_destroy(self->deps_dag_rem_nodes);
  }
}


/**Function********************************************************************

  Synopsis      [Initializes the vars fsm hash]

  Description   []

  SideEffects        []

******************************************************************************/
static void sexp_fsm_deinit(SexpFsm_ptr self)
{
  nusmv_assert(*(self->family_counter) > 0);
  *(self->family_counter) -= 1; /* self de-ref */

  sexp_fsm_hash_var_fsm_destroy(self);
  sexp_fsm_destroy_vars_dag(self); /* dependencies DAG */
  FlatHierarchy_destroy(self->hierarchy);

  if (*(self->family_counter) == 0) {
    FREE(self->family_counter);
    self->family_counter = (int*) NULL;
    free_list(self->vars_list);
  }
}


/**Function********************************************************************

  Synopsis      [Copies members from self to copy]

  Description   []

  SideEffects        []

******************************************************************************/
static void sexp_fsm_copy(const SexpFsm_ptr self, SexpFsm_ptr copy)
{
  /* inits some private members */
  copy->enc        = self->enc;
  copy->symb_table = self->symb_table;
  copy->det_layer  = self->det_layer;
  copy->inl_layer  = self->inl_layer;
  copy->vars_list = self->vars_list;
  copy->hash_var_fsm = copy_assoc(self->hash_var_fsm);
  copy->hierarchy = FlatHierarchy_copy(self->hierarchy);

  if (self->deps_dag_to != (hash_ptr) NULL) {
    copy->deps_dag_to = copy_assoc(self->deps_dag_to);
  } else copy->deps_dag_to = (hash_ptr) NULL;

  if (self->deps_dag_from != (hash_ptr) NULL) {
    copy->deps_dag_from = copy_assoc(self->deps_dag_from);
  } else copy->deps_dag_from = (hash_ptr) NULL;

  if (self->inlining_subst_hash != (hash_ptr) NULL) {
    copy->inlining_subst_hash = copy_assoc(self->inlining_subst_hash);
  } else copy->inlining_subst_hash = (hash_ptr) NULL;

  copy->deps_dag_rem_nodes = NODE_LIST(NULL);

  /* increments family members */
  copy->family_counter = self->family_counter;
  *(self->family_counter) += 1;  
}


/**Function********************************************************************

  Synopsis      [Initializes the vars fsm hash]

  Description   []

  SideEffects        []

******************************************************************************/
static void sexp_fsm_hash_var_fsm_init(SexpFsm_ptr self)
{
  node_ptr iter;

  iter = NODE_PTR(self->vars_list);
  while (iter != Nil) {
    int saved_yylineno = yylineno;

    VarFsm_ptr var_fsm;

    node_ptr var_name = car(iter);
    node_ptr var_name_i = find_node(SMALLINIT, var_name, Nil);
    node_ptr var_name_n = find_node(NEXT, var_name, Nil);

    node_ptr init_a = 
      FlatHierarchy_lookup_assign(self->hierarchy, var_name_i);

    node_ptr invar_a = 
      FlatHierarchy_lookup_assign(self->hierarchy, var_name);

    node_ptr next_a = 
      FlatHierarchy_lookup_assign(self->hierarchy, var_name_n);

    node_ptr init_c = 
      FlatHierarchy_lookup_constrains(self->hierarchy, var_name_i);

    node_ptr invar_c = 
      FlatHierarchy_lookup_constrains(self->hierarchy, var_name);

    node_ptr next_c = 
      FlatHierarchy_lookup_constrains(self->hierarchy, var_name_n);
    
    Expr_ptr init_sexp  = Expr_true();
    Expr_ptr invar_sexp = Expr_true();
    Expr_ptr trans_sexp = Expr_true();
    
    /* add all the constrains */
    if (Nil != init_c) init_sexp = Expr_and(init_sexp, EXPR(init_c));
    if (Nil != invar_c) invar_sexp = Expr_and(invar_sexp, EXPR(invar_c));
    if (Nil != next_c)  trans_sexp = Expr_and(trans_sexp, EXPR(next_c));
    
    /* add all assignments */
    if (Nil != init_a) {
      yylineno = init_a->lineno;
      init_sexp = Expr_and(init_sexp,
                           EXPR(find_node(EQDEF, var_name_i, init_a)));
    }
    
    if (Nil != invar_a) {
      yylineno = invar_a->lineno;
      invar_sexp = Expr_and(invar_sexp,
                            EXPR(new_node(EQDEF, var_name, invar_a)));
    }

    if (Nil != next_a) {
      yylineno = next_a->lineno;
      trans_sexp = Expr_and(trans_sexp,
                            EXPR(new_node(EQDEF, var_name_n, next_a)));
    }
 
    /* inserts the var fsm inside the hash table */
    var_fsm = var_fsm_create(init_sexp, invar_sexp, trans_sexp);
    sexp_fsm_hash_var_fsm_insert_var(self, var_name, var_fsm);

    iter = cdr(iter);
    yylineno = saved_yylineno;
  } /* loop */
}


/**Function********************************************************************

  Synopsis           [removes duplicates from expression containing AND nodes]

  Description        [group identifies INVAR, TRANS or INIT group.]

  SideEffects        []

******************************************************************************/
static Expr_ptr 
sexp_fsm_simplify_expr(SexpFsm_ptr self, hash_ptr hash, Expr_ptr expr, 
                       const int group)
{
  Expr_ptr result;
  
  if ((expr == EXPR(NULL)) || simplifier_hash_query_expr(hash, expr, group)) {
    result = Expr_true();
  }
  else {
    switch (node_get_type(NODE_PTR(expr))) {
    case AND:
      {
        Expr_ptr left = sexp_fsm_simplify_expr(self, hash, car(NODE_PTR(expr)),
                                               group);
        Expr_ptr right = sexp_fsm_simplify_expr(self, hash, cdr(NODE_PTR(expr)),
                                                group);
        result = Expr_and(left, right);
        break;
      }

    default:
      result = expr;
    } /* switch */
    
    simplifier_hash_add_expr(hash, expr, group);
  }
  
  return result;
}


/**Function********************************************************************

  Synopsis [Booleanizes the given expression, keeping each top level
  part of a possible conjuction]

  Description [If the fsm is not boolean, the input expression is
  returned]

  SideEffects        []

******************************************************************************/
static Expr_ptr sexp_fsm_booleanize_expr(SexpFsm_ptr self, Expr_ptr expr)
{
  Expr_ptr result;

  if (expr == NODE_PTR(NULL)) return NODE_PTR(NULL);
  if (self->det_layer == SYMB_LAYER(NULL)) return expr;

  switch (node_get_type(NODE_PTR(expr))) {
  case AND:
    {
      Expr_ptr left  = sexp_fsm_booleanize_expr(self, car(NODE_PTR(expr)));
      Expr_ptr right = sexp_fsm_booleanize_expr(self, cdr(NODE_PTR(expr)));
      result = Expr_and(left, right);      
      break;
    }

  default:
    result = EXPR(Compile_expr2bexpr(self->enc, self->det_layer, 
                                     NODE_PTR(expr)));
  } /* switch */  

  return result;
}


/**Function********************************************************************

  Synopsis [Substitutes all expressions of subst keys c inside expr by
  the cooresponding value into subst, ignoring left side of EQDEF]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static node_ptr _subst_expr(SymbTable_ptr st, node_ptr expr, hash_ptr subst)
{
  node_ptr dest;

  if (expr == Nil) return Nil;
  dest = find_assoc(subst, expr);
  if (dest != (node_ptr) NULL) return dest;
  
  switch (node_get_type(expr)) {
  case FAILURE:
  case TRUEEXP:
  case FALSEEXP:
  case NUMBER: 
  case NUMBER_WORD: 
  case NUMBER_FRAC: 
  case NUMBER_REAL:
  case NUMBER_EXP:
  case BIT:
    return expr;

  case DOT:
  case ATOM:
  case ARRAY: 
    {
      node_ptr name = CompileFlatten_resolve_name(st, expr, Nil);
      node_ptr param = lookup_param_hash(name);

      /* parameter */
      if (param != (node_ptr) NULL) {
        /* process the value of the parameter */
        return _subst_expr(st, param, subst);
      }
      
      /* defines are expanded */
      if (SymbTable_is_symbol_define(st, name)) {
        /* define (rhs must be boolean, recur to check) */
        node_ptr body = SymbTable_get_define_flatten_body(st, name);
        if (body == (node_ptr) NULL) error_undefined(name); 	
        return _subst_expr(st, body, subst);
      }

      /* variable */
      return expr;
    }

  case EQDEF: /* substitutes only the right side */
    return find_node(node_get_type(expr), 
                     car(expr),
                     _subst_expr(st, cdr(expr), subst));

  default:
    return find_node(node_get_type(expr), 
                     _subst_expr(st, car(expr), subst), 
                     _subst_expr(st, cdr(expr), subst));   
  }
}


/**Function********************************************************************

  Synopsis      [Private service of sexp_fsm_add_vertex]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static void __add_vertex_aux(hash_ptr hash, node_ptr from, node_ptr to)
{
  NodeList_ptr list = NODE_LIST(find_assoc(hash, from));
  if (list == NODE_LIST(NULL)) {
    list = NodeList_create();
    insert_assoc(hash, from, (node_ptr) list);
  }
  if (to != Nil && !NodeList_belongs_to(list, to)) NodeList_append(list, to);
}

/**Function********************************************************************

  Synopsis      []

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static void sexp_fsm_add_vertex(SexpFsm_ptr self, node_ptr from, node_ptr to)
{
  __add_vertex_aux(self->deps_dag_to, from, to);
  if (to != Nil) { __add_vertex_aux(self->deps_dag_from, to, from); }
}



/**Function********************************************************************

  Synopsis [returns the roots in the graph. Returned list must be
  destroyed]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static NodeList_ptr sexp_fsm_dag_get_roots(SexpFsm_ptr self)
{
  NodeList_ptr res = NodeList_create();
  node_ptr keys = assoc_get_keys(self->deps_dag_to, true);
  node_ptr iter;
  for (iter=keys; iter != Nil; iter=cdr(iter)) {
    NodeList_ptr list_from, list_to;
    boolean has_sons;
    node_ptr name = car(iter);
    
    /* is it a dead node? */
    if (NodeList_belongs_to(self->deps_dag_rem_nodes, name)) continue;

    list_from = NODE_LIST(find_assoc(self->deps_dag_from, name));
    list_to = NODE_LIST(find_assoc(self->deps_dag_to, name));
    has_sons = false;

    if (list_to != NODE_LIST(NULL) && NodeList_get_length(list_to) > 0) {
      /* checks if sons have been removed */      
      ListIter_ptr it;
      for (it=NodeList_get_first_iter(list_to); !ListIter_is_end(it); 
           it=ListIter_get_next(it)) {
        node_ptr son = NodeList_get_elem_at(list_to, it);
        if (!NodeList_belongs_to(self->deps_dag_rem_nodes, son)) {
          has_sons = true;
          break;
        }
      }
    }

    if (!has_sons) continue;

    if (list_from == NODE_LIST(NULL) || NodeList_get_length(list_from) == 0) {
      NodeList_append(res, name);
    }
    else { /* checks if removed */ 
      boolean is_top = true;
      ListIter_ptr it;
      for (it=NodeList_get_first_iter(list_from); !ListIter_is_end(it); 
           it=ListIter_get_next(it)) {
        node_ptr parent = NodeList_get_elem_at(list_from, it);
        if (!NodeList_belongs_to(self->deps_dag_rem_nodes, parent)) {
          is_top = false; 
          break;
        }
      } /* for all parents */
      if (is_top) NodeList_append(res, name);
    }
  }

  return res;
}


/**Function********************************************************************

  Synopsis      [Removes the given node from deps dag]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static void sexp_fsm_remove_dep_dag_node(SexpFsm_ptr self, node_ptr from)
{
  if (NodeList_belongs_to(self->deps_dag_rem_nodes, from)) return;
  NodeList_append(self->deps_dag_rem_nodes, from);
}


/**Function********************************************************************

  Synopsis      [private service of create_vars_dag]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static boolean sexp_fsm_add_vertices_from_expr(SexpFsm_ptr self, 
                                               node_ptr name, node_ptr expr, 
                                               VarFilterType filter) 
{
  NodeList_ptr deps;
  boolean added = false;

  switch (node_get_type(expr)) {
  case EQDEF: 
    { /* creates the vertices by iterating found dependencies */
      ListIter_ptr iter;    

      deps = Compile_get_expression_dependencies_by_type(
                        SexpFsm_get_symb_table(self), cdr(expr), filter);

      for (iter = NodeList_get_first_iter(deps); !ListIter_is_end(iter);
           iter = ListIter_get_next(iter)) {      
        node_ptr name2 = NodeList_get_elem_at(deps, iter);
        if (VFT_NEXT == filter) name2 = find_node(NEXT, name2, Nil);
        if (name != name2) { 
          sexp_fsm_add_vertex(self, name, name2); 
          added = true;
        }
      }
      NodeList_destroy(deps);
      break;
    }

  case AND:
  case CONS:
    {
      boolean lr = sexp_fsm_add_vertices_from_expr(self, name, car(expr), 
                                                   filter);
      boolean rr = sexp_fsm_add_vertices_from_expr(self, name, cdr(expr), 
                                                   filter);
      added = lr || rr;
      break;
    }

  default:
    added = false;
  }
   
  return added;
}


/**Function********************************************************************

  Synopsis [returns an EQDEF node assigning name, searching into
 expr. expr can be either a conjunction or a EQDEF node]

  Description   [Used during inlining]

  SideEffects   []

******************************************************************************/
static node_ptr __search_eqdef(node_ptr expr, node_ptr name)
{
  node_ptr res = Nil;

  switch (node_get_type(expr)) {
  case EQDEF:
    if (car(expr) == name) return expr;
    break;

  case AND:
    res = __search_eqdef(car(expr), name);
    if (res == Nil) res = __search_eqdef(cdr(expr), name);
    break;

  default:
    break;
  }
  
  return res;
}


/**Function********************************************************************

  Synopsis [returns an hash table for variable inlining substitution. The 
  hash is created by using a scheduling.]

  Description   [Used during inlining. Returned hash must be NOT freed]

  SideEffects   []

******************************************************************************/
static hash_ptr sexp_fsm_dag_get_subst_hash(SexpFsm_ptr self)
{
  boolean keep_on;
  hash_ptr subst_hash;

  /* DAG had to be created by constructor */
  nusmv_assert(self->deps_dag_to != (hash_ptr) NULL);
  
  if (self->inlining_subst_hash != (hash_ptr) NULL) {
    return self->inlining_subst_hash;
  }

  if (opt_verbose_level_gt(options, 3)) {
    _PRINT("SexpFsm: Calculating substitution hash:");
  }

  if (self->deps_dag_rem_nodes != NODE_LIST(NULL)) {
    NodeList_destroy(self->deps_dag_rem_nodes);
  }
  self->deps_dag_rem_nodes = NodeList_create();
  
  /* Creates and fills the substitution hash */
  subst_hash = new_assoc();
  do {    
    NodeList_ptr list = sexp_fsm_dag_get_roots(self);    
    ListIter_ptr iter;

    keep_on = false;

    for (iter = NodeList_get_first_iter(list); !ListIter_is_end(iter); 
         iter=ListIter_get_next(iter)) {
      node_ptr name = NodeList_get_elem_at(list, iter);
      node_ptr invar, trans, eqdef;      

      if (node_get_type(name) == NEXT) {
        invar = SexpFsm_get_var_invar(self, car(name));
        trans = SexpFsm_get_var_trans(self, car(name));
      }
      else {
        invar = SexpFsm_get_var_invar(self, name);
        trans = SexpFsm_get_var_trans(self, name);
      }
      
      /* substitutes invar and next */
      eqdef = __search_eqdef(invar, name);
      if (eqdef != Nil) {
        nusmv_assert(node_get_type(eqdef) == EQDEF);
        nusmv_assert(find_assoc(subst_hash, name) == (node_ptr) NULL);
        insert_assoc(subst_hash, name, cdr(eqdef));
      }
      eqdef = __search_eqdef(trans, name);
      if (eqdef != Nil) {
        nusmv_assert(node_get_type(eqdef) == EQDEF);
        nusmv_assert(find_assoc(subst_hash, name) == (node_ptr) NULL);
        insert_assoc(subst_hash, name, cdr(eqdef));
      }
      
      if (!keep_on) { /* termination condition */
        NodeList_ptr sons = NODE_LIST(find_assoc(self->deps_dag_to, name));
        keep_on = (sons != NODE_LIST(NULL) && NodeList_get_length(sons) > 0);
      }

      sexp_fsm_remove_dep_dag_node(self, name);
      if (opt_verbose_level_gt(options, 4)) {
        _PRINT(".");
      }
    }
    NodeList_destroy(list);

    if (opt_verbose_level_gt(options, 4)) {
      _PRINT("-");
    }
  } while (keep_on);
  NodeList_destroy(self->deps_dag_rem_nodes);
  self->deps_dag_rem_nodes = NODE_LIST(NULL);
  
  self->inlining_subst_hash = subst_hash;
  if (opt_verbose_level_gt(options, 3)) {
    _PRINT(" Done\n");
  }

  return subst_hash;
}


/**Function********************************************************************

  Synopsis      [Called during initialization to perform inlining.]

  Description [Top level function to apply inlining to the given
  expression. It has no effects if inl_layer has not been passed]

  SideEffects   []

******************************************************************************/
static node_ptr sexp_fsm_apply_inlining(SexpFsm_ptr self, node_ptr expr) 
{
  hash_ptr subst_hash;
  node_ptr res;

  if (self->inl_layer == SYMB_LAYER(NULL)) return expr;

  if (opt_verbose_level_gt(options, 1)) {
    _PRINT("SexpFsm: Applying inlining...\n");
  }
  subst_hash = sexp_fsm_dag_get_subst_hash(self);

  /* performs now the substitution */
  res = _subst_expr(SexpFsm_get_symb_table(self), expr, subst_hash);

  return res;
}


/**Function********************************************************************

  Synopsis           [This is used when creating cluster list from vars list]

  Description        []

  SideEffects        []

******************************************************************************/
static hash_ptr simplifier_hash_create()
{
  hash_ptr result;

  result = st_init_table(st_ptrcmp, st_ptrhash);
  nusmv_assert(result != ((hash_ptr) NULL));

  return result;
}

/**Function********************************************************************

  Synopsis           [Call after sexp_fsm_cluster_hash_create]

  Description        []

  SideEffects        []

******************************************************************************/
static void simplifier_hash_destroy(hash_ptr hash)
{
  nusmv_assert(hash != (hash_ptr) NULL);
  st_free_table(hash);  
}


/**Function********************************************************************

  Synopsis           [To insert a new node in the hash]

  Description        [group is INIT, INVAR or TRANS]

  SideEffects        [The hash can change]

******************************************************************************/
static void 
simplifier_hash_add_expr(hash_ptr hash, Expr_ptr expr, const int group)
{
  int res;
  
  res = st_add_direct(hash, (char*) expr, PTR_FROM_INT(char*, group));
  nusmv_assert(res != ST_OUT_OF_MEM);
}


/**Function********************************************************************

  Synopsis           [Queries for an element in the hash, returns True if 
  found]

  Description        []

  SideEffects        []

******************************************************************************/
static boolean 
simplifier_hash_query_expr(hash_ptr hash, Expr_ptr expr, 
                           const int group)
{
  nusmv_ptrint hashed_group; 
  boolean result;

  result = st_lookup(hash, (char*) expr, (char**) &hashed_group);

  /* groups are checked consecutively, i.e. at first, *all* INIT expressions 
     are checked, then *all* INVAR, and then *all* TRANS. So hash_group
     will not interfere with each other
  */
  return (result && ((int) hashed_group == group));
}


/**Function********************************************************************

  Synopsis           [Call to destroy the var fsm hash]

  Description        [Private method, used internally]

  SideEffects        []

******************************************************************************/
static void sexp_fsm_hash_var_fsm_destroy(SexpFsm_ptr self) 
{
  nusmv_assert(self->hash_var_fsm != (hash_ptr) NULL);

  if (*(self->family_counter) == 0) {
    clear_assoc_and_free_entries(self->hash_var_fsm, 
                                 sexp_fsm_callback_var_fsm_free);
  }
  else free_assoc(self->hash_var_fsm);
}


/**Function********************************************************************

  Synopsis [Private callback that destroys a single variable fsm
  contained into the var fsm hash]

  Description        []

  SideEffects        []

******************************************************************************/
static assoc_retval sexp_fsm_callback_var_fsm_free(char *key, 
                                                   char *data, char * arg)
{
  VarFsm_ptr varfsm = VAR_FSM(data);

  var_fsm_destroy(varfsm);  
  return ASSOC_DELETE;
}


/**Function********************************************************************

  Synopsis [Given a variable name, returns the corresponding variable
  fsm, or NULL if not found]

  Description        []

  SideEffects        []

******************************************************************************/
static VarFsm_ptr 
sexp_fsm_hash_var_fsm_lookup_var(SexpFsm_ptr self, node_ptr var) 
{
  nusmv_assert(self->hash_var_fsm != (hash_ptr) NULL);

  return VAR_FSM( find_assoc(self->hash_var_fsm, var) );
}


/**Function********************************************************************

  Synopsis           [Adds a var fsm to the internal hash. Private.]

  Description        []

  SideEffects        []

******************************************************************************/
static void 
sexp_fsm_hash_var_fsm_insert_var(SexpFsm_ptr self, 
                                 node_ptr var, VarFsm_ptr varfsm)
{
  nusmv_assert(self->hash_var_fsm != (hash_ptr) NULL);
  
  insert_assoc(self->hash_var_fsm, var, varfsm);
}


/**Function********************************************************************

  Synopsis           [Creates a var fsm]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static VarFsm_ptr var_fsm_create(Expr_ptr init, 
                                 Expr_ptr invar, 
                                 Expr_ptr next)
{
  return VAR_FSM( cons(NODE_PTR(init), 
                       cons(NODE_PTR(invar), NODE_PTR(next))) );
}


/**Function********************************************************************

  Synopsis           [It does not destroy the init, trans and invar nodes. 
  It destroys only the support nodes]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void var_fsm_destroy(VarFsm_ptr self)
{
  node_ptr node = NODE_PTR(self);

  free_node(cdr(node));
  free_node(node);
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static Expr_ptr var_fsm_get_init(VarFsm_ptr self)
{
  return EXPR( car(NODE_PTR(self)) );
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static Expr_ptr var_fsm_get_invar(VarFsm_ptr self)
{
  return EXPR( car(cdr(NODE_PTR(self))) );
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static Expr_ptr var_fsm_get_next(VarFsm_ptr self)
{
  return EXPR( cdr(cdr(NODE_PTR(self))) );
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static Expr_ptr var_fsm_get_input(VarFsm_ptr self)
{
  /* Currently no constraints over input are allowed, thus we return
     true to inidicate this. */
  return Expr_true();
}
