/**CFile***********************************************************************

  FileName    [FsmBuilder.c]

  PackageName [fsm]

  Synopsis    [High level object that can contruct FSMs]

  Description [Defines an high-level object that lives at 
  top-level, that is used to help contruction of FSMs. 
  It can control information that are not shared between 
  lower levels, so it can handle with objects that have not 
  the full knowledge of the whole system]
  
  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``fsm'' package of NuSMV version 2. 
  Copyright (C) 2006 by ITC-irst. 

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

#include "FsmBuilder.h"
#include "fsmInt.h"

#include "fsm/bdd/FairnessList.h"
#include "parser/symbols.h"
#include "parser/idlist/ParserIdList.h" /* to parse trans ordering file */
#include "compile/compile.h"
#include "utils/assoc.h"
#include "utils/error.h"


static char rcsid[] UTIL_UNUSED = "$Id: FsmBuilder.c,v 1.1.2.1.6.10 2007/05/03 15:47:06 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Type************************************************************************

  Synopsis    [FSM builder class constructor]

  Description []

  SeeAlso     []

******************************************************************************/
typedef struct FsmBuilder_TAG 
{
  DdManager*  dd; /* for performance, readability, etc. */

  hash_ptr simpl_hash; /* hash used when removing duplicates */  
} FsmBuilder;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void fsm_builder_init ARGS((FsmBuilder_ptr self, DdManager* dd)); 

static void fsm_builder_deinit ARGS((FsmBuilder_ptr self));

static ClusterList_ptr 
fsm_builder_clusterize_expr ARGS((FsmBuilder_ptr self, 
                                  BddEnc_ptr enc, Expr_ptr expr));

static void 
fsm_builder_clusterize_expr_aux ARGS((const FsmBuilder_ptr self, 
                                      BddEnc_ptr enc, 
                                      ClusterList_ptr clusters, 
                                      Expr_ptr expr_trans, 
                                      boolean is_inside_and));

static JusticeList_ptr
fsm_builder_justice_sexp_to_bdd ARGS((FsmBuilder_ptr self, 
                                      BddEnc_ptr enc, 
                                      node_ptr justice_sexp_list));

static CompassionList_ptr 
fsm_builder_compassion_sexp_to_bdd ARGS((FsmBuilder_ptr self, 
                                         BddEnc_ptr enc, 
                                         node_ptr compassion_sexp_list));

static NodeList_ptr 
fsm_builder_order_vars_list ARGS((const FsmBuilder_ptr self,
                                  const BddEnc_ptr enc, 
                                  const NodeList_ptr vars));

static Expr_ptr 
fsm_builder_remove_dupl ARGS((FsmBuilder_ptr self, Expr_ptr expr));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The constructor creates a BddEnc and self handles it]

  Description        []

  SideEffects        []

******************************************************************************/
FsmBuilder_ptr FsmBuilder_create(DdManager* dd)
{
  FsmBuilder_ptr self = ALLOC(FsmBuilder, 1);

  FSM_BUILDER_CHECK_INSTANCE(self);

  fsm_builder_init(self, dd);
  return self;
}


/**Function********************************************************************

  Synopsis           [Class FsmBuilder destructor]

  Description        []

  SideEffects        []

******************************************************************************/
void FsmBuilder_destroy(FsmBuilder_ptr self)
{
  FSM_BUILDER_CHECK_INSTANCE(self);
  
  fsm_builder_deinit(self);

  FREE(self);
}



/**Function********************************************************************

  Synopsis           [Creates a new sexp fsm, taking into account of the 
  current variables ordering, or the trans ordering file when specified by 
  the user. When used, the latter overrides the former.]

  Description [The caller becomes the owner of the returned object.
  If det_layer is NULL, then a scalar fsm will be created, otherwise
  returned fsm will be boolean. An exception may occur if the trans
  cluster ordering is specified and an error occurs while parsing it.]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr 
FsmBuilder_create_sexp_fsm(const FsmBuilder_ptr self, BddEnc_ptr enc, 
                           SymbLayer_ptr det_layer, SymbLayer_ptr inl_layer, 
                           VarSet_ptr vars_list)
{
  FlatHierarchy_ptr fh;
  SexpFsm_ptr res;
  NodeList_ptr input_vars;
  NodeList_ptr sorted_vars = NODE_LIST(NULL);

  FSM_BUILDER_CHECK_INSTANCE(self);

  /* prepares a new empty hierarchy. Allows for the original assigns
     that are needed to construct the single vars' fsm. Also,
     preserves all the other fields. */
  fh = FlatHierarchy_copy(mainFlatHierarchy);
  FlatHierarchy_set_init(fh, Nil);
  FlatHierarchy_set_invar(fh, Nil);
  FlatHierarchy_set_trans(fh, Nil);
  FlatHierarchy_set_input(fh, Nil);

  /* retrieves an ordered vars list */
  input_vars = NodeList_create_from_list((node_ptr) vars_list);
  
  CATCH {
    sorted_vars = fsm_builder_order_vars_list(self, enc, input_vars);
  }
  FAIL {
    if (input_vars != NODE_LIST(NULL)) NodeList_destroy(input_vars);
    nusmv_exit(1); /* re-throws it */
  }

  res = SexpFsm_create(enc, det_layer, inl_layer, 
                       NodeList_to_node_ptr(sorted_vars), fh); 

  NodeList_destroy(sorted_vars);
  NodeList_destroy(input_vars);
  FlatHierarchy_destroy(fh);
  return res;
}


/**Function********************************************************************

  Synopsis           [Creates a BddFsm instance from a given SexpFsm]

  Description        [
  Note: all variables from provided encoding will go to the BDD FSM.
  Use FsmBuilder_create_bdd_fsm_of_vars if only SOME variables should be taken
  into account.]

  SideEffects        []

******************************************************************************/
BddFsm_ptr FsmBuilder_create_bdd_fsm(const FsmBuilder_ptr self, 
                                     BddEnc_ptr enc, 
                                     const SexpFsm_ptr sexp_fsm, 
                                     const TransType trans_type)
{
  BddFsm_ptr bddfsm;
  BddVarSet_ptr state_vars_cube, input_vars_cube, next_state_vars_cube;
  
  state_vars_cube = BddEnc_get_state_vars_cube(enc);
  input_vars_cube = BddEnc_get_input_vars_cube(enc);
  next_state_vars_cube = BddEnc_get_next_state_vars_cube(enc);

  bddfsm = FsmBuilder_create_bdd_fsm_of_vars(self, enc, sexp_fsm, trans_type,
                                             state_vars_cube,  input_vars_cube, 
                                             next_state_vars_cube);

  bdd_free(self->dd, (bdd_ptr) state_vars_cube);
  bdd_free(self->dd, (bdd_ptr) input_vars_cube);
  bdd_free(self->dd, (bdd_ptr) next_state_vars_cube);

  return bddfsm;
}


/**Function********************************************************************

  Synopsis           [Creates a BddFsm instance from a given SexpFsm]

  Description        [It is the same as FsmBuilder_create_bdd_fsm except that 
  the cubes of state, input and next-state variables is given explicitly.
  
  Note: The functions will take a copy of provided cubes.]

  SideEffects        []

******************************************************************************/
BddFsm_ptr FsmBuilder_create_bdd_fsm_of_vars(const FsmBuilder_ptr self, 
                                             BddEnc_ptr enc, 
                                             const SexpFsm_ptr sexp_fsm, 
                                             const TransType trans_type,
                                             BddVarSet_ptr state_vars_cube,
                                             BddVarSet_ptr input_vars_cube,
                                             BddVarSet_ptr next_state_vars_cube)
{
  /* to construct Bdd Fsm: */
  BddFsm_ptr bddfsm;
  BddTrans_ptr trans;
  JusticeList_ptr justice;
  CompassionList_ptr compassion;
  bdd_ptr init_bdd, invar_bdd, input_bdd;

  /* to construct trans: */
  ClusterList_ptr clusters;
  ClusterOptions_ptr cluster_options;

  FSM_BUILDER_CHECK_INSTANCE(self);


  /* ---------------------------------------------------------------------- */
  /* Trans construction                                                     */
  /* ---------------------------------------------------------------------- */
  cluster_options = ClusterOptions_create(options);
  
  clusters = fsm_builder_clusterize_expr(self, enc, 
                                         SexpFsm_get_trans(sexp_fsm));
  
  trans = BddTrans_create(self->dd, 
                          clusters, 
                          (bdd_ptr) state_vars_cube, 
                          (bdd_ptr) input_vars_cube, 
                          (bdd_ptr) next_state_vars_cube, 
                          trans_type, 
                          cluster_options);

  ClusterOptions_destroy(cluster_options); /* this is no longer needed */ 


  /* ---------------------------------------------------------------------- */
  /* Bdd Fsm construction                                                   */
  /* ---------------------------------------------------------------------- */
  justice = fsm_builder_justice_sexp_to_bdd(self, enc, 
                                            SexpFsm_get_justice(sexp_fsm));

  compassion = fsm_builder_compassion_sexp_to_bdd(self, enc, 
                                        SexpFsm_get_compassion(sexp_fsm));
  
  /* init */
  init_bdd = BddEnc_expr_to_bdd(enc, SexpFsm_get_init(sexp_fsm), Nil);
  /* invar */
  invar_bdd = BddEnc_expr_to_bdd(enc, SexpFsm_get_invar(sexp_fsm), Nil);
  /* input */
  input_bdd = BddEnc_expr_to_bdd(enc, SexpFsm_get_input(sexp_fsm), Nil);
  
  bddfsm = BddFsm_create(enc, 
                         BDD_STATES(init_bdd), 
                         BDD_INVAR_STATES(invar_bdd), 
                         BDD_INVAR_INPUTS(input_bdd), 
                         trans, 
                         justice, compassion);

  bdd_free(self->dd, input_bdd);
  bdd_free(self->dd, invar_bdd);
  bdd_free(self->dd, init_bdd);

  return bddfsm;
}


/**Function********************************************************************

  Synopsis [Given an expression, returns a bdd ClusterList with
  each conjuction occurring into expr contained in each cluster of
  the list. ]

  Description [Each cluster into the list represents a piece of
  transition relation. If the given expression contains
  duplicates, they will not occur into the returned cluster
  list. Returned list should be destroyed by the caller.]

  SideEffects        []

******************************************************************************/
ClusterList_ptr FsmBuilder_clusterize_expr(FsmBuilder_ptr self, 
                                           BddEnc_ptr enc, 
                                           Expr_ptr expr)
{
  Expr_ptr simp_expr;
  FSM_BUILDER_CHECK_INSTANCE(self);
   
  clear_assoc(self->simpl_hash);
  simp_expr = fsm_builder_remove_dupl(self, expr);
  clear_assoc(self->simpl_hash);
   
   return fsm_builder_clusterize_expr(self, enc, simp_expr);
}


/*---------------------------------------------------------------------------*/
/* Static function definitions                                               */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void fsm_builder_init(FsmBuilder_ptr self, DdManager* dd)
{
  self->dd = dd;
  self->simpl_hash = new_assoc();
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void fsm_builder_deinit(FsmBuilder_ptr self)
{
  free_assoc(self->simpl_hash);
}


/**Function********************************************************************

  Synopsis           [Converts an expression into a list of clusters. 
  This list can be used to create a BddFsm]

  Description        [Each cluster into the list represents a piece of 
  transition relation. One important requirement is that the given expr should 
  not contain duplicates. See for example SexpFsm_get_{init, invar, trans} on 
  how to obtain a well formed expression]

  SideEffects        []

******************************************************************************/
static ClusterList_ptr fsm_builder_clusterize_expr(FsmBuilder_ptr self, 
                                                   BddEnc_ptr enc, 
                                                   Expr_ptr expr)
{
  ClusterList_ptr clusters;
  
  clusters = ClusterList_create(self->dd); 
  fsm_builder_clusterize_expr_aux(self, enc, clusters, expr, false);

  return clusters;
}



/**Function********************************************************************

  Synopsis           [Auxiliary function to recursively traverse the 
  given expression, clusterizing each node as bdd. If called from outside, 
  parameter is_inside_and is false.]

  Description        []

  SideEffects        [given cluster list will change]

******************************************************************************/
static void
fsm_builder_clusterize_expr_aux(const FsmBuilder_ptr self, 
                                BddEnc_ptr enc,
                                ClusterList_ptr clusters, 
                                Expr_ptr expr_trans, 
                                boolean is_inside_and)
{
  add_ptr tmp; 
  node_ptr node = (node_ptr) expr_trans;

  if (node != Nil) {
    yylineno = node_get_lineno(node);

    switch (node_get_type(node)) {
    case AND:
      fsm_builder_clusterize_expr_aux(self, enc, clusters, car(node), true);
      fsm_builder_clusterize_expr_aux(self, enc, clusters, cdr(node), true);

      if (!is_inside_and && (ClusterList_length(clusters) == 0)) {
        /* Due to lazy evaluation, the list is going to be empty (and
           the call is over). Adds a single true cluster */
        bdd_ptr one = bdd_one(self->dd);
        Cluster_ptr cluster = Cluster_create(self->dd);
        Cluster_set_trans(cluster, self->dd, one);
        ClusterList_append_cluster(clusters, cluster);
      }

      break;
      
    default:
      tmp = BddEnc_expr_to_add(enc, expr_trans, Nil); 

      if (! (add_is_one(self->dd, tmp) && is_inside_and)) {
        Cluster_ptr cluster = Cluster_create(self->dd);
        bdd_ptr tmp2 = add_to_bdd(self->dd, tmp);
        Cluster_set_trans(cluster, self->dd, tmp2);
        bdd_free(self->dd, tmp2);

        ClusterList_append_cluster(clusters, cluster);
      }

      add_free(self->dd, tmp);

    } /* switch */
  }
}



/**Function********************************************************************

  Synopsis           [Converts a list of expressions into a list of 
  bdds, wrapped inside a justice list object]

  Description        [The caller becomes the wner of the returned object. 
  Internally used by the bdd fsm building code]

  SideEffects        []

******************************************************************************/
static JusticeList_ptr
fsm_builder_justice_sexp_to_bdd(FsmBuilder_ptr self, 
                                BddEnc_ptr enc, 
                                node_ptr justice_sexp_list)
{
  JusticeList_ptr res;
  node_ptr iter; 

  res = JusticeList_create(self->dd);

  iter = justice_sexp_list;
  while (iter != Nil) {
    Expr_ptr expr = EXPR( car(iter) );
    bdd_ptr  p = BddEnc_expr_to_bdd(enc, expr, Nil);
    JusticeList_append_p(res, BDD_STATES(p));

    bdd_free(self->dd, p);
    iter = cdr(iter);
  } /* loop */
  
  return res;
}


/**Function********************************************************************

  Synopsis [Converts a list of couple of expressions into a list of couple of 
  bdds, wrapped inside a compassion list object]

  Description        [The caller becomes the wner of the returned object. 
  Internally used by the bdd fsm building code]

  SideEffects        []

******************************************************************************/
static CompassionList_ptr 
fsm_builder_compassion_sexp_to_bdd(FsmBuilder_ptr self, 
                                   BddEnc_ptr enc,
                                   node_ptr compassion_sexp_list)
{
  CompassionList_ptr res;
  node_ptr iter; 

  res = CompassionList_create(self->dd);

  iter = compassion_sexp_list;
  while (iter != Nil) {
    Expr_ptr expr;
    bdd_ptr  p, q;
    node_ptr couple = car(iter);

    expr = car(couple);
    p = BddEnc_expr_to_bdd(enc, expr, Nil);
    expr = cdr(couple);
    q = BddEnc_expr_to_bdd(enc, expr, Nil);

    CompassionList_append_p_q(res, BDD_STATES(p), BDD_STATES(q));

    bdd_free(self->dd, q);
    bdd_free(self->dd, p);

    iter = cdr(iter);
  } /* loop */
  
  return res;
}


void print_ids_list(FILE* fout, const NodeList_ptr list)
{
  ListIter_ptr iter = NodeList_get_first_iter(list);
  int len = 0;
  while (!ListIter_is_end(iter)) {
    char* name = sprint_node(NodeList_get_elem_at(list, iter));
    int l = strlen(name) + 1;
    if (len + l > 70) { fprintf(fout, "\n"); len = 0; }
    fprintf(fout, "%s ", name);
    FREE(name);
    len += l;
    iter = ListIter_get_next(iter);
  }
  fprintf(fout, "\n");
}


/**Function********************************************************************

  Synopsis           [Private service used by SexpFsm builders]

  Description        [This method orders the given vars list wrt to the trans
  ordering file as a first attempt, then wrt the current bdd variables
  ordering.]

  SideEffects        []

******************************************************************************/
static NodeList_ptr fsm_builder_order_vars_list(const FsmBuilder_ptr self,
                                                const BddEnc_ptr enc, 
                                                const NodeList_ptr vars)
{
  NodeList_ptr res = NODE_LIST(NULL);

  if (opt_trans_order_file(options)) {
    /* The user has specified a trans ordering file */
    FILE* fl;
    ParserIdList_ptr parser;

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, 
              "Reading the trans variable ordering from file '%s'\n", 
              get_trans_order_file(options));
    }

    fl = fopen(get_trans_order_file(options), "r");
    if (fl == (FILE*) NULL) {
      fprintf(nusmv_stderr, 
              "While opening the specified trans ordering file:\n");
      error_file_not_found(get_trans_order_file(options));
    }

    parser = ParserIdList_create();
    
    CATCH {
      ParserIdList_parse_from_file(parser, fl);
    }
    FAIL {
      fclose(fl);
      ParserIdList_destroy(parser);
      nusmv_exit(1); /* re-throws the exception */
    }

    fclose(fl);
    
    res = Compile_make_sorted_vars_list_from_order(
                           BaseEnc_get_symbol_table(BASE_ENC(enc)), 
                           vars, ParserIdList_get_id_list(parser));

    ParserIdList_destroy(parser);
  }
  else {
    /* reads the vars ordering from the internal BDD vars ordering */
    NodeList_ptr order;

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, 
              "Reading the trans variable ordering from the BDD encoder\n");
    }

    order = BddEnc_get_var_ordering(enc, DUMP_SCALARS_ONLY);
    res = Compile_make_sorted_vars_list_from_order(
                   BaseEnc_get_symbol_table(BASE_ENC(enc)), vars, order);

    NodeList_destroy(order);
  }
  
  return res;
}


/**Function********************************************************************

  Synopsis           [Removes duplicates from expression containing AND nodes]

  Description        []

  SideEffects        []

******************************************************************************/
static Expr_ptr fsm_builder_remove_dupl(FsmBuilder_ptr self, Expr_ptr expr)
{
  node_ptr result;
  
  if ((expr == EXPR(NULL)) || 
      find_assoc(self->simpl_hash, (node_ptr) expr) != Nil) {
    return Expr_true();
  }

  switch (node_get_type(NODE_PTR(expr))) {
  case AND:
    {
      Expr_ptr left = fsm_builder_remove_dupl(self, car(NODE_PTR(expr)));
      Expr_ptr right = fsm_builder_remove_dupl(self, cdr(NODE_PTR(expr)));
      result = Expr_and(left, right);
      break;
    }

  default:
    result = expr;
  } /* switch */
    
  insert_assoc(self->simpl_hash, (node_ptr) expr, (node_ptr) true);
  return result;
}

