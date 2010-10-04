/**CFile***********************************************************************

  FileName    [propProp.c]

  PackageName [prop]

  Synopsis    [Main routines for the prop data structure]

  Description [Main routines for the manipulation of the prop data
  structure. A "master" property is also defined to be used to
  represent the whole system, for instance to perform reachability or
  to perform simulation. Moreover a primitives to create, query and
  manipulate a database of property is provided.]

  SeeAlso     []

  Author      [Marco Roveri, Roberto Cavada]

  Copyright   [
  This file is part of the ``prop'' package of NuSMV version 2. 
  Copyright (C) 2000-2001 by ITC-irst. 

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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "prop/prop.h"
#include "propInt.h" 

#include "mc/mc.h"
#include "ltl/ltl.h"
#include "parser/symbols.h"
#include "parser/mbp_symbols.h"
#include "parser/psl/pslNode.h"
#include "enc/enc.h"
#include "compile/compile.h"
#include "utils/utils_io.h"
#include "utils/error.h"


#if HAVE_MBP
#include "mbp/mbp.h"
#endif

#include <string.h>

static char rcsid[] UTIL_UNUSED = "$Id: propProp.c,v 1.27.2.14.2.1.2.26.4.7 2007/04/13 08:15:14 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
/* This variable is used to store various global FSMs */
static Prop_ptr Prop_Master = PROP(NULL);

#if HAVE_MBP
/* This variable is used to store various global Game FSMs */
static Prop_ptr Prop_GameMaster = PROP(NULL);
#endif

EXTERN DdManager* dd_manager;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/* Used to encode that a property MIN/MAX has not yet been checked. */
#define PROP_UNCHECKED -2

/* Used to encode the infinite distanca between two set of states in
   MIN/MAX properties */
#define PROP_INFINITE -1
#define PROP_UNDEFINED -3


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void prop_init ARGS((Prop_ptr self));
static void prop_deinit ARGS((Prop_ptr self));
static void prop_print ARGS((const Prop_ptr self, FILE* file));

static void prop_set_scalar_sexp_fsm 
ARGS((Prop_ptr self, SexpFsm_ptr fsm, const boolean duplicate));
static void prop_set_bool_sexp_fsm 
ARGS((Prop_ptr self, SexpFsm_ptr fsm, const boolean duplicate));
static void prop_set_bdd_fsm 
ARGS((Prop_ptr self, BddFsm_ptr fsm, const boolean duplicate));
static void prop_set_be_fsm 
ARGS((Prop_ptr self, BeFsm_ptr fsm, const boolean duplicate));

#if HAVE_MBP
static void prop_set_game_scalar_sexp_fsm 
ARGS((Prop_ptr self, GameSexpFsm_ptr fsm, const boolean duplicate));
static void prop_set_game_bool_sexp_fsm 
ARGS((Prop_ptr self, GameSexpFsm_ptr fsm, const boolean duplicate));
static void prop_set_game_bdd_fsm 
ARGS((Prop_ptr self, GameBddFsm_ptr fsm, const boolean duplicate));
static void prop_set_game_be_fsm 
ARGS((Prop_ptr self, GameBeFsm_ptr fsm, const boolean duplicate));
#endif

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Initializes the package: master property and 
  property database are allocated]

  Description        [After you had called this, you must also call 
  PropPkg_init_cmd if you need to use the interactive shell for 
  commands]

  SideEffects        []

******************************************************************************/
void PropPkg_init()
{
  nusmv_assert(Prop_Master == PROP(NULL));
  Prop_Master = Prop_create();

#if HAVE_MBP
  nusmv_assert(Prop_GameMaster == PROP(NULL));
  Prop_GameMaster = Prop_create_partial(Nil, Prop_AvoidDeadlock);
#endif

  PropDb_create();
}

/**Function********************************************************************

  Synopsis           [Quits the package]

  Description        []

  SideEffects        []

******************************************************************************/
void PropPkg_quit()
{
  PropDb_destroy();

  Prop_destroy(Prop_Master);
  Prop_Master = PROP(NULL);

#if HAVE_MBP
  Prop_destroy(Prop_GameMaster);
  Prop_GameMaster = PROP(NULL);
#endif
}





/**Function********************************************************************

  Synopsis           [Allocate a property]

  Description        [Allocate a property. If no more room is
  available then a call to <tt>numsv_exit</tt> is performed. All the
  fields of the prop structure are initialized to either NULL or the
  corresponding default type (e.g. Prop_NoType for property type).]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
Prop_ptr Prop_create() 
{
  Prop_ptr self = ALLOC(Prop, 1);

  PROP_CHECK_INSTANCE(self);

  prop_init(self);
  return self;
}


/**Function********************************************************************

  Synopsis           [Free a property]

  Description        [Free a property. Notice that before freeing the
  property all the elements of the property that needs to be freed
  will be automatically freed.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Prop_destroy(Prop_ptr self) 
{
  PROP_CHECK_INSTANCE(self);

  prop_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis [Returns the property as it has been parsed and created]

  Description [Returns the property stored in the prop. If the
  property is PSL, the result should be converted to core symbols
  before model checking (see Prop_get_expr_core or
  PslNode_convert_psl_to_core).

  If the property is a game property, cdr is returned.]

  SideEffects        []

  SeeAlso            [Prop_get_expr_core]

******************************************************************************/
Expr_ptr Prop_get_expr(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  /* Game properties are wrapped into MBP_SPEC_WRAPPER node with player
     name on the left and expression on the right. Return
     only pure expression.
  */
  if (Prop_is_game_prop(self)) return cdr(self->prop);

  /* usual expression */
  return self->prop;
}


/**Function********************************************************************

  Synopsis [Returns the property, but it is converted before in terms
  of core symbols.]

  Description [Returns the property in a form that it can be handled
  by the system (model checking, dependency finder, etc.).  This may
  imply a conversion and a different structure of the resulting
  formula. For example in PSL FORALLs are expanded, SERE are removed,
  global operators G and AG are simplified, etc.

  Use this function at system-level, and Prop_get_expr to get the
  original formula instead]

  SideEffects        []

  SeeAlso            [Prop_get_expr]

******************************************************************************/
Expr_ptr Prop_get_expr_core(const Prop_ptr self)
{
  Expr_ptr res;

  PROP_CHECK_INSTANCE(self);

  /* PSL formulae are converted to SMV LTL or CTL: */
  if (Prop_get_type(self) == Prop_Psl) {
    res = PslNode_convert_psl_to_core(Prop_get_expr(self));
  }
  else res = Prop_get_expr(self); /* usual expression */

  return Compile_pop_distrib_ops(res);
}


/**Function********************************************************************

  Synopsis           [Returns the cone of a property]

  Description        [If the cone of influence of a property has been
  computed, this function returns it.]

  SideEffects        []

******************************************************************************/
Set_t Prop_get_cone(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  return self->cone;
}


/**Function********************************************************************

  Synopsis           [Sets the cone of a property]

  Description        [Stores the cone of influence of the property]

  SideEffects        []

******************************************************************************/
void Prop_set_cone(Prop_ptr self, Set_t cone)
{
  PROP_CHECK_INSTANCE(self);

  self->cone = cone;
}


/**Function********************************************************************

  Synopsis           [Returns the property type]

  Description        [Returns the property kind of the stroed
  property, i.e. CTL, LTL, ...]

  SideEffects        []

******************************************************************************/
Prop_Type Prop_get_type(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  return self->type ;
}


/**Function********************************************************************

  Synopsis           [Returns the status of the property]

  Description        [Returns the status of the property]

  SideEffects        []

******************************************************************************/
Prop_Status Prop_get_status(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  return self->status;
}

/**Function********************************************************************

  Synopsis           [Sets the status of the property]

  Description        [Sets the status of the property]

  SideEffects        []

******************************************************************************/
void Prop_set_status(Prop_ptr self, Prop_Status s)
{
  PROP_CHECK_INSTANCE(self);
  self->status = s;
}


/**Function********************************************************************

  Synopsis           [Returns the number of the property]

  Description        [For COMPUTE properties returns the number
  resulting from the evaluation of the property.]

  SideEffects        []

******************************************************************************/
int Prop_get_number(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->number;
}


/**Function********************************************************************

  Synopsis           [Sets the number of the property]

  Description        [Sets the number resulting from the
  evaluation of the property.]

  SideEffects        []

******************************************************************************/
void Prop_set_number(Prop_ptr self, int n)
{
  PROP_CHECK_INSTANCE(self);
  self->number = n;
}

/**Function********************************************************************

  Synopsis           [Sets the number of the property to INFINITE]

  Description        [Sets the to INFINITE the number resulting from the
  evaluation of the property.]

  SideEffects        []

******************************************************************************/
void Prop_set_number_infinite(Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  self->number = PROP_INFINITE;
}

/**Function********************************************************************

  Synopsis           [Sets the number of the property to UNDEFINED]

  Description        [Sets the to UNDEFINED the number resulting from the
  evaluation of the property.]

  SideEffects        []

******************************************************************************/
void Prop_set_number_undefined(Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  self->number = PROP_UNDEFINED;
}

/**Function********************************************************************

  Synopsis           [Returns the trace number associated to a property]

  Description [For unsatisfied properties, the trace number of the
  asscociated counterexample is returned. 0 is returned if no trace is
  available]

  SideEffects        []

******************************************************************************/
int Prop_get_trace(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->trace;
}


/**Function********************************************************************

  Synopsis           [Sets the trace number]

  Description        [Sets the trace number for an unsatisfied property.]

  SideEffects        []

******************************************************************************/
void Prop_set_trace(Prop_ptr self, int t)
{
  PROP_CHECK_INSTANCE(self);
  self->trace = t;
}


/**Function********************************************************************

  Synopsis           [Returns the scalar FSM of a property]

  Description        [Resturns the scalar FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr Prop_get_scalar_sexp_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->scalar_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game scalar FSM of a property]

  Description        [Resturns the Game scalar FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
GameSexpFsm_ptr Prop_get_game_scalar_sexp_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  return self->game_scalar_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Sets the scalar FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_scalar_sexp_fsm(Prop_ptr self, SexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  prop_set_scalar_sexp_fsm(self, fsm, true);
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Sets the Game scalar FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_game_scalar_sexp_fsm(Prop_ptr self, GameSexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  prop_set_game_scalar_sexp_fsm(self, fsm, true);
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the boolean FSM of a property]

  Description        [Resturns the boolean FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr Prop_get_bool_sexp_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->bool_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game boolean FSM of a property]

  Description        [Resturns the Game boolean FSM in sexp associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
GameSexpFsm_ptr Prop_get_game_bool_sexp_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  return self->game_bool_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Sets the boolean FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_bool_sexp_fsm(Prop_ptr self, SexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  prop_set_bool_sexp_fsm(self, fsm, true);
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Sets the Game boolean FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_game_bool_sexp_fsm(Prop_ptr self, GameSexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  prop_set_game_bool_sexp_fsm(self, fsm, true);
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the BDD FSM of a property]

  Description        [Returns the BDD FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
BddFsm_ptr Prop_get_bdd_fsm(Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->bdd_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game BDD FSM of a property]

  Description        [Returns the Game BDD FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
GameBddFsm_ptr Prop_get_game_bdd_fsm(Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  return self->game_bdd_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Sets the boolean FSM in BDD of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_bdd_fsm(Prop_ptr self, BddFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  prop_set_bdd_fsm(self, fsm, true);
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Sets the Game BDD FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_game_bdd_fsm(Prop_ptr self, GameBddFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  prop_set_game_bdd_fsm(self, fsm, true);
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the BE FSM  of a property]

  Description        [Returns the boolean BE FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
BeFsm_ptr Prop_get_be_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  return self->be_fsm;
}



#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game BE FSM in  of a property]

  Description        [Returns the Game boolean BE FSM associated to
  the property. Self keeps the ownership of the given fsm]

  SideEffects        []

******************************************************************************/
GameBeFsm_ptr Prop_get_game_be_fsm(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  return self->game_be_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Sets the boolean BE FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_be_fsm(Prop_ptr self, BeFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  prop_set_be_fsm(self, fsm, true);
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Sets the Game boolean BE FSM of a property]

  Description        [The given fsm will be duplicated, so the caller keeps 
  the ownership of fsm]

  SideEffects        []

******************************************************************************/
void Prop_set_game_be_fsm(Prop_ptr self, GameBeFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(self);
  nusmv_assert(Prop_is_game_prop(self));
  prop_set_game_be_fsm(self, fsm, true);
}
#endif


/**Function********************************************************************

  Synopsis           [Applies cone of influence to the given property]

  Description        [The COI is applied only for BDD-based model checking. 
  To apply for BMC, use Prop_apply_coi_for_bmc]

  SideEffects        [Internal FSMs are computed]

******************************************************************************/
void Prop_apply_coi_for_bdd(Prop_ptr self, FsmBuilder_ptr helper) 
{
  SexpFsm_ptr scalar_fsm;
  BddFsm_ptr  bdd_fsm;
  SymbTable_ptr symb_table;
  boolean applied = false;


  PROP_CHECK_INSTANCE(self);

  /* COI of game properties are not implemented yet */
  nusmv_assert(!Prop_is_game_prop(self));

  symb_table = Compile_get_global_symb_table();
  scalar_fsm = Prop_get_scalar_sexp_fsm(self);
  bdd_fsm    = Prop_get_bdd_fsm(self);

  /* scalar sexp fsm */
  if (scalar_fsm == SEXP_FSM(NULL)) {
    VarSet_ptr vars; 

    Set_t spec_dep = Formulae_GetDependencies(symb_table, 
		      Prop_get_expr_core(self), 
		      FlatHierarchy_get_justice(mainFlatHierarchy),
		      FlatHierarchy_get_compassion(mainFlatHierarchy));

    Set_t cone = ComputeCOI(symb_table, spec_dep);

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Using cone of influence\n");
    }
        
    vars = Set_Set2List(cone);

    scalar_fsm = FsmBuilder_create_sexp_fsm(helper, Enc_get_bdd_encoding(),  
					    SYMB_LAYER(NULL), /* scalar */
					    SYMB_LAYER(NULL), /*no inlining*/
					    vars);
    Prop_set_cone(self, cone);
    prop_set_scalar_sexp_fsm(self, scalar_fsm, false); /* does not dup */

    applied = true;
  }

  /* bdd fsm */
  if (bdd_fsm == BDD_FSM(NULL)) {
    bdd_fsm = FsmBuilder_create_bdd_fsm(helper, Enc_get_bdd_encoding(), 
					scalar_fsm, 
					get_partition_method(options));
    prop_set_bdd_fsm(self, bdd_fsm, false); /* does not dup */
    applied = true;
  }

  if (! applied) {
    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Using previously built model for COI...\n");
    }
  }
  
}


/**Function********************************************************************

  Synopsis           [Applies cone of influence to the given property]

  Description        [The COI is applied only for BMC-based model checking. 
  To apply for BDD, use Prop_apply_coi_for_bdd. This method creates a new layer 
  for those determinization vars that derives from the booleanization of 
  the fsm deriving from the property cone. That layer will be committed to the 
  BoolEnc and BeEnc encodings only, not to the BddEnc. The newly created layer 
  will be assigned to a name that depends on the property number within the 
  database DbProp.]

  SideEffects        [Internal FSMs are computed]

******************************************************************************/
void Prop_apply_coi_for_bmc(Prop_ptr self, FsmBuilder_ptr helper) 
{
  BeEnc_ptr be_enc;
  SexpFsm_ptr bool_fsm;
  BeFsm_ptr be_fsm;
  SymbTable_ptr symb_table;

  PROP_CHECK_INSTANCE(self);

  /* COI of game properties are not implemented yet */
  nusmv_assert(!Prop_is_game_prop(self));

  bool_fsm   = Prop_get_bool_sexp_fsm(self);
  be_fsm    = Prop_get_be_fsm(self);
  symb_table = Compile_get_global_symb_table();

  /* boolean sexp fsm */
  if (bool_fsm == SEXP_FSM(NULL)) {
    char* layer_name = (char*) NULL;
    int layer_name_dim;
    SymbLayer_ptr det_layer;
    VarSet_ptr vars; 

    Set_t spec_dep = 
      Formulae_GetDependencies(symb_table, Prop_get_expr_core(self), 
			       FlatHierarchy_get_justice(mainFlatHierarchy), 
			       FlatHierarchy_get_compassion(mainFlatHierarchy));

    Set_t cone = ComputeCOI(symb_table, spec_dep);

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Using cone of influence\n");
    }
    
    vars = Set_Set2List(cone);

    layer_name_dim = strlen(DETERM_LAYER_NAME) + 6;
    layer_name = ALLOC(char, layer_name_dim);
    nusmv_assert(layer_name != (char*) NULL);

    snprintf(layer_name, layer_name_dim, "%s_%03d", 
	     DETERM_LAYER_NAME, Prop_get_index(self));

    det_layer = SymbTable_create_layer(symb_table, layer_name, 
				       SYMB_LAYER_POS_BOTTOM);

    be_enc = Enc_get_be_encoding();
    {  /* commits the layer: */

      /* BddEnc is required as bdds are used when producing
	 counter-examples: */
      BddEnc_ptr bdd_enc; 
      BoolEnc_ptr bool_enc;
      
      bool_enc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(be_enc));
      bdd_enc = Enc_get_bdd_encoding();

      /* creates the boolean FSM */
      bool_fsm = FsmBuilder_create_sexp_fsm(helper, Enc_get_bdd_encoding(), 
					    det_layer, /* boolean */
					    SYMB_LAYER(NULL), /*no inlining*/
					    vars); 

      BaseEnc_commit_layer(BASE_ENC(bool_enc), layer_name);
      BaseEnc_commit_layer(BASE_ENC(be_enc), layer_name);   
      BaseEnc_commit_layer(BASE_ENC(bdd_enc), layer_name);   
    }

    Prop_set_cone(self, cone);
    prop_set_bool_sexp_fsm(self, bool_fsm, false); /* does not dup */
    
    /* creates the BE FSM */
    nusmv_assert(be_fsm == BE_FSM(NULL));

    /* Notice that currently a single variable manager instance
       exists, and it is handled by the BMC package as a public global
       variable. Current implementation is temporary kept in this
       format. */     
    be_fsm = BeFsm_create_from_sexp_fsm(be_enc, bool_fsm);
    prop_set_be_fsm(self, be_fsm, false); /* does not dup */

    FREE(layer_name);
  } 
  else {
    nusmv_assert(be_fsm != BE_FSM(NULL));
    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Using previously built model for COI...\n");
    }
  }
}


/**Function********************************************************************

  Synopsis           [Prints a property with info or its position and status 
  within the database]

  Description        [Prints a property on the specified FILE
  stream. Some of the information stored in the property structure are
  printed out (e.g. property, property kind, property status, ...).]

  SideEffects        []

******************************************************************************/
void Prop_print_db(Prop_ptr self, FILE* file) 
{
  PROP_CHECK_INSTANCE(self);

  fprintf(file, "%.3d : ", self->index);
  prop_print(self, file); 
  fprintf(file, "\n");

  fprintf(file, "\t[%-15s", Prop_get_type_as_string(self));
  if (self->type == Prop_Compute) {
    char* str_number = Prop_get_number_as_string(self);
    fprintf(file, "%-15s", str_number);
    FREE(str_number);
  }
  else fprintf(file, "%-15s", Prop_get_status_as_string(self));
  fprintf(file, (self->trace == 0 ? "N/A ]\n" : "T%-3d]\n"), self->trace);
}


/**Function********************************************************************

  Synopsis           [Prints a property]

  Description        [Prints a property. 
  PSL properties are specially handled.]

  SideEffects        []

******************************************************************************/
void Prop_print(Prop_ptr self, FILE* file) 
{
  PROP_CHECK_INSTANCE(self);
  prop_print(self, file); 
}


/**Function********************************************************************

  Synopsis           [Returns the number value as a string (only for compute 
  types)]

  Description        [Returns a number, 'Inifinite' or 'Unchecked'. 
  The returned string is dynamically created, and caller must free it.]

  SideEffects        []

******************************************************************************/
char* Prop_get_number_as_string(const Prop_ptr self)
{
  char buf[16];
  char* ret = NULL;
  int n; 

  PROP_CHECK_INSTANCE(self);

  nusmv_assert(Prop_get_type(self) == Prop_Compute); /* compute type only */

  n = Prop_get_number(self);
  if (n == PROP_UNCHECKED) snprintf(buf, sizeof(buf), "Unchecked");
  else if (n == PROP_INFINITE) snprintf(buf, sizeof(buf), "Infinite");
  else if (n == PROP_UNDEFINED) snprintf(buf, sizeof(buf), "Undefined");
  else snprintf(buf, sizeof(buf), "%d", n);
  
  ret = ALLOC(char, strlen(buf)+sizeof(char));
  nusmv_assert(ret != NULL);

  strcpy(ret, buf);
  return ret;
}


/**Function********************************************************************

  Synopsis           [Returns the context name of a property]

  Description        [If the property has no explicit context, 'Main' will
  be returned. The returned string must be deleted by the caller.]

  SideEffects        []

******************************************************************************/
char* Prop_get_context_text(const Prop_ptr self) 
{
  char* cntx = (char *)NULL;
  char* EMTPY_CONTEXT_STR = "Main";
  node_ptr context;

  PROP_CHECK_INSTANCE(self);

  context = (node_ptr) self->prop;

  if (node_get_type(context) == CONTEXT) { 
    context = car(context);
    if (context != Nil) {
      cntx = sprint_node(context);
    }
    else {
      cntx = ALLOC(char, strlen(EMTPY_CONTEXT_STR)+1);
      nusmv_assert(cntx != NULL);
      strcpy(cntx, EMTPY_CONTEXT_STR);  
    }    
  }
  else {
    cntx = ALLOC(char, strlen(EMTPY_CONTEXT_STR)+1);
    nusmv_assert(cntx != NULL);
    strcpy(cntx, EMTPY_CONTEXT_STR);
  }

  return cntx;
}


/**Function********************************************************************

  Synopsis           [Returns the property text, with no explicit context]

  Description        [The returned string must be deleted by the caller.]

  SideEffects        []

******************************************************************************/
char* Prop_get_text(const Prop_ptr self) 
{
  node_ptr p;

  PROP_CHECK_INSTANCE(self);

  p = (node_ptr) Prop_get_expr(self);
  if (node_get_type(p) == CONTEXT) p = cdr(p);  /* skip context */

  return sprint_node(p);
}


/**Function********************************************************************

  Synopsis           [Returns the scalar FSM]

  Description        [Returns the scalar FSM stored in the master prop]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr Prop_master_get_scalar_sexp_fsm()
{
  PROP_CHECK_INSTANCE(Prop_Master);
  return Prop_Master->scalar_fsm; 
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game scalar FSM]

  Description        [Returns the Game scalar FSM stored in the master prop]

  SideEffects        []

******************************************************************************/
GameSexpFsm_ptr Prop_master_get_game_scalar_sexp_fsm()
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  return Prop_GameMaster->game_scalar_fsm; 
}
#endif


/**Function********************************************************************

  Synopsis           [Set the scalar FSM]

  Description        [Set the scalar FSM of the master prop. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void  Prop_master_set_scalar_sexp_fsm(SexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_Master);
  if (Prop_Master->scalar_fsm != SEXP_FSM(NULL)) {
    SexpFsm_destroy(Prop_Master->scalar_fsm);
  }

  Prop_Master->scalar_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Set the Gamescalar FSM]

  Description        [Set the Gamescalar FSM of the master prop. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void  Prop_master_set_game_scalar_sexp_fsm(GameSexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  if (Prop_GameMaster->game_scalar_fsm != GAME_SEXP_FSM(NULL)) {
    GameSexpFsm_destroy(Prop_GameMaster->game_scalar_fsm);
  }

  Prop_GameMaster->game_scalar_fsm = fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the boolean FSM in sexp]

  Description [Returns the boolean FSM in sexp stored in the master
  prop.  The prop package becomes the owner of the given fsm. The
  returned value may be NULL when coi is enabled]

  SideEffects        []

******************************************************************************/
SexpFsm_ptr Prop_master_get_bool_sexp_fsm()
{
  PROP_CHECK_INSTANCE(Prop_Master);
  return Prop_Master->bool_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game boolean FSM in sexp]

  Description        [Returns theGame  boolean FSM in sexp stored in 
  the master prop.  The prop package becomes the owner of the given  fsm]

  SideEffects        []

******************************************************************************/
GameSexpFsm_ptr Prop_master_get_game_bool_sexp_fsm()
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  return Prop_GameMaster->game_bool_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Set the boolean FSM in sexp]

  Description        [Set the boolean FSM in sexp of the master prop. The 
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_bool_sexp_fsm(SexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_Master);
  if (Prop_Master->bool_fsm != SEXP_FSM(NULL)) {
    SexpFsm_destroy(Prop_Master->bool_fsm);
  }

  Prop_Master->bool_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Set the Game boolean FSM in sexp]

  Description        [Set the Game boolean FSM in sexp of the master prop. The 
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_game_bool_sexp_fsm(GameSexpFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  if (Prop_GameMaster->game_bool_fsm != GAME_SEXP_FSM(NULL)) {
    GameSexpFsm_destroy(Prop_GameMaster->game_bool_fsm);
  }

  Prop_GameMaster->game_bool_fsm = fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the boolean FSM in BDD]

  Description [Returns the boolean FSM in BDD stored in the master
  prop.  The returned value may be NULL when coi is enabled]

  SideEffects        []

******************************************************************************/
BddFsm_ptr Prop_master_get_bdd_fsm()
{
  PROP_CHECK_INSTANCE(Prop_Master);
  return Prop_Master->bdd_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game boolean FSM in BDD]

  Description        [Returns the Game boolean FSM in BDD stored in the
  master prop]

  SideEffects        []

******************************************************************************/
GameBddFsm_ptr Prop_master_get_game_bdd_fsm()
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  return Prop_GameMaster->game_bdd_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Set the boolean FSM in BDD]

  Description        [Set the boolean FSM in BDD of the master prop. The 
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_bdd_fsm(BddFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_Master);

  if (Prop_Master->bdd_fsm != BDD_FSM(NULL)) {
    BddFsm_destroy(Prop_Master->bdd_fsm);
  }

  Prop_Master->bdd_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Set the Game boolean FSM in BDD]

  Description        [Set the Game boolean FSM in BDD of the master prop. The 
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_game_bdd_fsm(GameBddFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  if (Prop_GameMaster->game_bdd_fsm != GAME_BDD_FSM(NULL)) {
    GameBddFsm_destroy(Prop_GameMaster->game_bdd_fsm);
  }

  Prop_GameMaster->game_bdd_fsm = fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Returns the boolean FSM in BE]

  Description [Returns the boolean FSM in BE stored in the master
  prop.  The returned value may be NULL when coi is enabled]

  SideEffects        []

******************************************************************************/
BeFsm_ptr Prop_master_get_be_fsm()
{
  PROP_CHECK_INSTANCE(Prop_Master);
  return Prop_Master->be_fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Returns the Game boolean FSM in BE]

  Description        [Returns the Game boolean FSM in BE stored in the
  master prop]

  SideEffects        []

******************************************************************************/
GameBeFsm_ptr Prop_master_get_game_be_fsm()
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  return Prop_GameMaster->game_be_fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Set the boolean FSM in BE]

  Description        [Set the boolean FSM in BE of the master prop. The 
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_be_fsm(BeFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_Master);
  if (Prop_Master->be_fsm != (BeFsm_ptr) NULL) {
    BeFsm_destroy(Prop_Master->be_fsm);
  }

  Prop_Master->be_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           [Set the Game boolean FSM in BE]

  Description [Set the Game boolean FSM in BE of the master prop. The
  prop package becomes the owner of the given fsm. This method
  destroys the previously set FSM if any.]

  SideEffects        []

******************************************************************************/
void Prop_master_set_game_be_fsm(GameBeFsm_ptr fsm)
{
  PROP_CHECK_INSTANCE(Prop_GameMaster);
  if (Prop_GameMaster->game_be_fsm != (GameBeFsm_ptr) NULL) {
    GameBeFsm_destroy(Prop_GameMaster->game_be_fsm);
  }

  Prop_GameMaster->game_be_fsm = fsm;
}
#endif


/**Function********************************************************************

  Synopsis           [Copies master prop FSM data into prop]

  Description        [Copies the FSM informations stored in the master
  prop into the corresponding fields of the given prop structure.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Prop_set_fsm_to_master(Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  /* set the usual or game FSM of properties to the global ones */
  if (Prop_is_game_prop(self)) {
#if HAVE_MBP
    Prop_set_game_scalar_sexp_fsm(self,Prop_master_get_game_scalar_sexp_fsm());
    Prop_set_game_bool_sexp_fsm(self, Prop_master_get_game_bool_sexp_fsm());
    Prop_set_game_bdd_fsm(self, Prop_master_get_game_bdd_fsm());
    Prop_set_game_be_fsm(self, Prop_master_get_game_be_fsm());
#else
    internal_error("Setting up Game FSM whereas MBP package is no enabled");
#endif
  }
  else {
    Prop_set_scalar_sexp_fsm(self, Prop_master_get_scalar_sexp_fsm());
    Prop_set_bool_sexp_fsm(self, Prop_master_get_bool_sexp_fsm());
    Prop_set_bdd_fsm(self, Prop_master_get_bdd_fsm());
    Prop_set_be_fsm(self, Prop_master_get_be_fsm());
  }
}



/**Function********************************************************************

  Synopsis           [Returns the a string associated to a property type]

  Description        [Returns the string corresponding to a property
  type for printing it. Returned string must NOT be deleted]

  SideEffects        []

******************************************************************************/
const char* Prop_get_type_as_string(Prop_ptr self) 
{
  PROP_CHECK_INSTANCE(self);
  return PropType_to_string(Prop_get_type(self));
}

/**Function********************************************************************

  Synopsis           [Returns the a string associated to a property status]

  Description        [Returns the string corresponding to a property
  status for printing it. The caller must NOT free the returned string, 
  dince it is a constant.]

  SideEffects        []

******************************************************************************/
const char* Prop_get_status_as_string(const Prop_ptr self) 
{
  char* res = (char*) NULL;
  Prop_Status t;

  PROP_CHECK_INSTANCE(self);

  t = Prop_get_status(self);

  switch (t) {
  case Prop_NoStatus:    res = PROP_NOSTATUS_STRING; break; 
  case Prop_Unchecked:   res = PROP_UNCHECKED_STRING; break; 
  case Prop_True:        res = PROP_TRUE_STRING; break; 
  case Prop_False:       res = PROP_FALSE_STRING; break; 
  case Prop_Wrong:       res = PROP_WRONG_STRING; break; 
  case Prop_Number:      res = PROP_NUMBER_STRING; break; 

  default:  nusmv_assert(false); /* invalid status */
  }

  return res;
}



/**Function********************************************************************

  Synopsis           [Check if a property in the database is of a given type]

  Description        [Checks if a property in the database is of a given type.
  If the type is correct, value 0 is returned, otherwise an error message 
  is emitted and value 1 is returned.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int Prop_check_type(const Prop_ptr self, Prop_Type type) 
{ 
  PROP_CHECK_INSTANCE(self);

  if (Prop_get_type(self) != type) {
    fprintf(nusmv_stderr,
            "Error: specified property type is %s, "
	    "but type %s was expected.\n", 
            Prop_get_type_as_string(self), PropType_to_string(type));
    return 1;
  }

  return 0;
}




/**Function********************************************************************

  Synopsis           [Verifies a given property]

  Description        [Depending the property, different model checking
  algorithms are called. The status of the property is updated
  accordingly to the result of the verification process.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Prop_verify(Prop_ptr self)
{
  if (Prop_get_status(self) != Prop_Wrong) {
    if (Prop_get_status(self) == Prop_Unchecked)  {
      switch (Prop_get_type(self)) {
	
      case Prop_Ctl: 
	if (opt_ag_only(options)) {
	  if ( opt_forward_search(options)) { Mc_CheckAGOnlySpec(self); }
	  else {
	    /* Cannot use AG-only since reachables must be calculated before */
	    warning_ag_only_without_reachables();
	    Mc_CheckCTLSpec(self);
	  }
	}
	else { Mc_CheckCTLSpec(self); }
	break;
	
      case Prop_Compute:  Mc_CheckCompute(self); break;

      case Prop_Invar:    Mc_CheckInvar(self); break;

      case Prop_Ltl:      Ltl_CheckLtlSpec(self); break;

      case Prop_Psl:
	if (Prop_is_psl_ltl(self)) { Ltl_CheckLtlSpec(self); }
	else {
	  if (Prop_is_psl_obe(self)) {
	    if (opt_ag_only(options)) {
	      if ( opt_forward_search(options)) { Mc_CheckAGOnlySpec(self); }
	      else {
		/* Cannot use AG-only since reachables must be calculated before */
		warning_ag_only_without_reachables();
		Mc_CheckCTLSpec(self);
	      }
	    }
	    else { Mc_CheckCTLSpec(self); }
	  }
	  else { error_psl_not_supported_feature(); }
	}
	break; 

#if HAVE_MBP
      case Prop_ReachTarget:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckReachTargetSpec(self);
	break;

      case Prop_AvoidTarget:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckAvoidTargetSpec(self);
	break;

      case Prop_ReachDeadlock:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckReachDeadlockSpec(self);
	break;

      case Prop_AvoidDeadlock:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckAvoidDeadlockSpec(self);
	break;

      case Prop_BuchiGame:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckBuchiGameSpec(self);
	break;

      case Prop_GenReactivity:
	nusmv_assert(opt_mbp_game(options)); /* the input file contains game */
	Mbp_CheckGenReactivitySpec(self);
	break;
#else
      case Prop_ReachTarget:
      case Prop_AvoidTarget:
      case Prop_ReachDeadlock:
      case Prop_AvoidDeadlock:
      case Prop_BuchiGame:
      case Prop_GenReactivity:
	nusmv_assert(false);/*realizability property cannot be here if MBP is off*/
#endif

      default:  nusmv_assert(false); /* invalid type */
      }
    }
  }
}


/**Function********************************************************************

  Synopsis           [Returns the a string associated to a property type]

  Description        [Returns the string corresponding to a property
  type for printing it. Returned string must NOT be deleted]

  SideEffects        []

******************************************************************************/
const char* PropType_to_string(const Prop_Type type) 
{
  char* res = (char*) NULL;
  
  switch (type) {
  case Prop_NoType:  res = PROP_NOTYPE_STRING; break;
  case Prop_Ctl:     res = PROP_CTL_STRING; break;
  case Prop_Ltl:     res = PROP_LTL_STRING; break;
  case Prop_Psl:     res = PROP_PSL_STRING; break;
  case Prop_Invar:   res = PROP_INVAR_STRING; break; 
  case Prop_Compute: res = PROP_COMPUTE_STRING; break;
  case Prop_ReachTarget:   res = PROP_REACH_TARGET_STRING; break;
  case Prop_AvoidTarget:   res = PROP_AVOID_TARGET_STRING; break;
  case Prop_ReachDeadlock: res = PROP_REACH_DEADLOCK_STRING; break;
  case Prop_AvoidDeadlock: res = PROP_AVOID_DEADLOCK_STRING; break;
  case Prop_BuchiGame:     res = PROP_BUCHI_GAME_STRING; break;
  case Prop_GenReactivity: res = PROP_GEN_REACTIVITY_STRING; break;

  default: nusmv_assert(false); /* unknown type! */
  }

  return res;
}


/**Function********************************************************************

  Synopsis           [Returns true if the property is PSL property and it 
  is LTL compatible]

  Description        []

  SideEffects        []

******************************************************************************/
boolean Prop_is_psl_ltl(const Prop_ptr self)
{
  return (Prop_get_type(self) == Prop_Psl) && 
    PslNode_is_handled_psl(PslNode_remove_forall_replicators(self->prop));
}

/**Function********************************************************************

  Synopsis           [Returns true if the property is PSL property and it 
  is CTL compatible]

  Description        []

  SideEffects        []

******************************************************************************/
boolean Prop_is_psl_obe(const Prop_ptr self)
{
  return (Prop_get_type(self) == Prop_Psl) && PslNode_is_obe(self->prop);
}


/**Function********************************************************************

  Synopsis           [Returns true if the property is a game specifications,
  i.e. one of REACHTARGET, AVOIDTARGET, REACHDEADLOCK, AVOIDDEADLOCK,
  BUCHIGAME, GENREACTIVITY.]

  Description        []

  SideEffects        []

******************************************************************************/
boolean Prop_is_game_prop(const Prop_ptr self)
{
  return self->type >= Prop_ReachTarget && self->type <= Prop_GenReactivity;
}



/**Function********************************************************************

  Synopsis           [Returns true if the property is a game specifications,
  i.e. one of REACHTARGET, AVOIDTARGET, REACHDEADLOCK, AVOIDDEADLOCK,
  BUCHIGAME, GENREACTIVITY.]

  Description        []

  SideEffects        []

******************************************************************************/
string_ptr Prop_get_game_player(const Prop_ptr self)
{
  nusmv_assert(Prop_is_game_prop(self) && Nil != self->prop);
  return (string_ptr)car(car(self->prop));
}

/**Function********************************************************************

  Synopsis           [Creates a property, but does not insert it within 
  the database, so the property can be used on the fly.]

  Description [Creates a property structure filling only the property
  and property type fields. The property index within the db is not set.]

  SideEffects        []

******************************************************************************/
Prop_ptr Prop_create_partial(Expr_ptr expr, Prop_Type type)
{
  Prop_ptr self = Prop_create();
  PROP_CHECK_INSTANCE(self);

  /* DEBUGGING: game(realizability) properties must be wrapped into MBP_SPEC_WRAPPER */
  if (Nil != expr && Prop_ReachTarget <= type && type <= Prop_GenReactivity) {
    nusmv_assert(MBP_SPEC_WRAPPER == node_get_type(expr));
  }

  self->index = -1;
  self->status = Prop_Unchecked;
  self->prop = expr;
  self->type = type;
  return self;
}


#if HAVE_MATHSAT
/**Function********************************************************************

  Synopsis           [Returns the associated concrete trace structure]

  Description        [This function is used by CEGAR, and is available only 
  when mathsat is enabled]

  SideEffects        []

******************************************************************************/
struct ConcTrace_TAG* Prop_get_ctrace(const Prop_ptr self) 
{
  PROP_CHECK_INSTANCE(self);
  return self->ctrace;
}

/**Function********************************************************************

  Synopsis           [Associates given concrete trace structure to the
  property]

  Description        [This function is used by CEGAR, and is available only 
  when mathsat is enabled]

  SideEffects        []

******************************************************************************/
void Prop_set_ctrace(Prop_ptr self, struct ConcTrace_TAG* ctrace)
{
  PROP_CHECK_INSTANCE(self);
  self->ctrace = ctrace;
}
#endif

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/



/**Function********************************************************************

  Synopsis           [Returns the index of a property]

  Description        [Returns the unique identifier of a property]

  SideEffects        []

******************************************************************************/
int Prop_get_index(const Prop_ptr self)
{
  PROP_CHECK_INSTANCE(self);

  return self->index;
}


/**Function********************************************************************

  Synopsis           [Sets the index of a property]

  Description        [Sets the unique identifier of a property]

  SideEffects        []

******************************************************************************/
void Prop_set_index(Prop_ptr self, const int index)
{
  PROP_CHECK_INSTANCE(self);
  self->index = index;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the property]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void prop_init(Prop_ptr self) 
{
  self->index = 0;
  self->prop = EXPR(NULL);
  self->cone = (node_ptr) NULL;
  self->type = Prop_NoType;
  self->status = Prop_NoStatus;
  self->number = PROP_UNCHECKED;
  self->trace = 0;
  self->scalar_fsm = SEXP_FSM(NULL);
  self->bool_fsm = SEXP_FSM(NULL);
  self->bdd_fsm = BDD_FSM(NULL);
  self->be_fsm = (BeFsm_ptr) NULL;

#if HAVE_MATHSAT
  self->ctrace = (struct ConcTrace_TAG*)NULL;
#endif
}


/**Function********************************************************************

  Synopsis           [Destroy the elements of a property]

  Description        [Destroy the elements of a property]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void prop_deinit(Prop_ptr self) 
{
  /* depending on whether this is a game property or not , destroy
     the associated game or usual FSMs
  */
  if (Prop_is_game_prop(self)) {
#if HAVE_MBP 
    if (self->game_be_fsm != GAME_BE_FSM(NULL)) GameBeFsm_destroy(self->game_be_fsm);
    if (self->game_bdd_fsm != GAME_BDD_FSM(NULL)) GameBddFsm_destroy(self->game_bdd_fsm);
    if (self->game_bool_fsm != GAME_SEXP_FSM(NULL)) GameSexpFsm_destroy(self->game_bool_fsm);
    if (self->game_scalar_fsm != GAME_SEXP_FSM(NULL)) GameSexpFsm_destroy(self->game_scalar_fsm);
#else
    internal_error("Setting down Game FSM whereas MBP package is no enabled");
#endif
  }
  else {
    if (self->be_fsm != NULL) BeFsm_destroy(self->be_fsm);
    if (self->bdd_fsm != BDD_FSM(NULL)) BddFsm_destroy(self->bdd_fsm);
    if (self->bool_fsm != SEXP_FSM(NULL)) SexpFsm_destroy(self->bool_fsm);
    if (self->scalar_fsm != SEXP_FSM(NULL)) SexpFsm_destroy(self->scalar_fsm);
  }
}


/**Function********************************************************************

  Synopsis           [Prints a property]

  Description        [Prints a property. 
  PSL properties and game propeties are specially handled.]

  SideEffects        []

******************************************************************************/
static void prop_print(const Prop_ptr self, FILE* file) 
{
  node_ptr p;
  node_ptr context;
    
  p = Prop_get_expr(self);
  context = Nil;
  
  if (p != Nil && node_get_type(p) == CONTEXT) {
    context = car(p);
    p = cdr(p);
  }

  /* For game (MBP) specifications also the specification type and
     the player responsible for the specification are printed. */
  if (Prop_is_game_prop(self)) {  
    indent(file);
    fprintf(file, " %s %s ", Prop_get_type_as_string(self),
	    get_text(Prop_get_game_player(self)));
  }

  indent_node(file, "", p, " ");

  if (context != Nil) {
    fprintf(file, "IN ");
    print_node(file, context);
  }
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_scalar_sexp_fsm(Prop_ptr self, SexpFsm_ptr fsm, 
				     const boolean duplicate)
{
  if (self->scalar_fsm != SEXP_FSM(NULL)) SexpFsm_destroy(self->scalar_fsm);
  if (duplicate && (fsm != SEXP_FSM(NULL))) { 
    self->scalar_fsm = SexpFsm_copy(fsm);
  }
  else self->scalar_fsm = fsm; 
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_game_scalar_sexp_fsm(Prop_ptr self, GameSexpFsm_ptr fsm, 
					  const boolean duplicate)
{
  if (self->game_scalar_fsm != GAME_SEXP_FSM(NULL)) {
    GameSexpFsm_destroy(self->game_scalar_fsm);
  }
  if (duplicate && (fsm != GAME_SEXP_FSM(NULL))) { 
    self->game_scalar_fsm = GameSexpFsm_copy(fsm);
  }
  else self->game_scalar_fsm = fsm; 
}
#endif


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_bool_sexp_fsm(Prop_ptr self, SexpFsm_ptr fsm, 
				   const boolean duplicate)
{
  if (self->bool_fsm != SEXP_FSM(NULL)) SexpFsm_destroy(self->bool_fsm);
  if (duplicate && (fsm != SEXP_FSM(NULL))) {
    self->bool_fsm = SexpFsm_copy(fsm);
  }
  else self->bool_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_game_bool_sexp_fsm(Prop_ptr self, GameSexpFsm_ptr fsm, 
					const boolean duplicate)
{
  if (self->game_bool_fsm != GAME_SEXP_FSM(NULL)) {
    GameSexpFsm_destroy(self->game_bool_fsm);
  }
  if (duplicate && (fsm != GAME_SEXP_FSM(NULL))) {
    self->game_bool_fsm = GameSexpFsm_copy(fsm);
  }
  else self->game_bool_fsm = fsm;
}
#endif


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_bdd_fsm(Prop_ptr self, BddFsm_ptr fsm, 
			     const boolean duplicate)
{
  if (self->bdd_fsm != BDD_FSM(NULL)) BddFsm_destroy(self->bdd_fsm);
  if (duplicate && (fsm != BDD_FSM(NULL))) {
    self->bdd_fsm = BddFsm_copy(fsm);
  }
  else self->bdd_fsm = fsm; 
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_game_bdd_fsm(Prop_ptr self, GameBddFsm_ptr fsm, 
				  const boolean duplicate)
{
  if (self->game_bdd_fsm != GAME_BDD_FSM(NULL)) {
    GameBddFsm_destroy(self->game_bdd_fsm);
  }
  if (duplicate && (fsm != GAME_BDD_FSM(NULL))) {
    self->game_bdd_fsm = GameBddFsm_copy(fsm);
  }
  else self->game_bdd_fsm = fsm; 
}
#endif


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_be_fsm(Prop_ptr self, BeFsm_ptr fsm, 
			    const boolean duplicate)
{
  if (self->be_fsm != (BeFsm_ptr) NULL) BeFsm_destroy(self->be_fsm);
  if (duplicate && (fsm != (BeFsm_ptr) NULL)) {
    self->be_fsm = BeFsm_copy(fsm);
  }
  else self->be_fsm = fsm;
}


#if HAVE_MBP
/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
static void prop_set_game_be_fsm(Prop_ptr self, GameBeFsm_ptr fsm, 
				 const boolean duplicate)
{
  if (self->game_be_fsm != (GameBeFsm_ptr) NULL) {
    GameBeFsm_destroy(self->game_be_fsm);
  }
  if (duplicate && (fsm != (GameBeFsm_ptr) NULL)) {
    self->game_be_fsm = GameBeFsm_copy(fsm);
  }
  else self->game_be_fsm = fsm;
}
#endif

