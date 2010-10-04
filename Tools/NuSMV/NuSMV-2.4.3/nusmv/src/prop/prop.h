/**CHeaderFile*****************************************************************

  FileName    [prop.h]

  PackageName [prop]

  Synopsis    [Manager of generic properties]

  Description [This file is responsible of manipulate all the
  informations associated to a given property, i.e. the kind of
  property, the property itself, its cone of influence, if the
  property is not satisfied the associated copunter-example, the
  associated FSM in different formats (flatten sexp, flatten boolean
  sexp, bdd, and BE).]

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

  Revision    [$Id: prop.h,v 1.14.2.6.4.15.4.2 2007/05/16 15:25:32 nusmv Exp $]

******************************************************************************/

#ifndef __PROP_H__
#define __PROP_H__

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "compile/symb_table/SymbTable.h"

#include "fsm/FsmBuilder.h"
#include "fsm/sexp/SexpFsm.h"
#include "fsm/bdd/BddFsm.h"
#include "fsm/be/BeFsm.h"

#if HAVE_MBP
# include "mbp/fsm/GameSexpFsm.h"
# include "mbp/fsm/GameBddFsm.h"
# include "mbp/fsm/GameBeFsm.h"
#endif

#include "utils/utils.h"
#include "utils/ustring.h"
#include "set/set.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [The status of a property]

  Description [The status of a property, i.e. If it is checked,
  unchecked, satisifed or unsatisfied.]

  SeeAlso     [optional]

******************************************************************************/
enum _Prop_Status {Prop_NoStatus, Prop_Unchecked, Prop_True, Prop_False, Prop_Wrong, Prop_Number};
#define PROP_NOSTATUS_STRING "NoStatus"
#define PROP_UNCHECKED_STRING "Unchecked"
#define PROP_TRUE_STRING "True"
#define PROP_FALSE_STRING "False"
#define PROP_WRONG_STRING "Wrong"
#define PROP_NUMBER_STRING "Number"
 
/**Enum************************************************************************

  Synopsis    [Enumerates the different types of a specification]

  Description [Enumerates the different types of a specification]

******************************************************************************/
enum _Prop_Type {Prop_NoType, 
		 Prop_Ctl, 
		 Prop_Ltl, 
		 Prop_Psl, 
		 Prop_Invar, 
		 Prop_Compute,
		 /* Game specifications: ReachTarget should be the first and 
		    GenReactivity should be the last in game specs.
		 */
		 Prop_ReachTarget,
		 Prop_AvoidTarget,
		 Prop_ReachDeadlock,
		 Prop_AvoidDeadlock,
		 Prop_BuchiGame,
		 Prop_GenReactivity};

#define PROP_NOTYPE_STRING "NoType"
#define PROP_CTL_STRING "CTL"
#define PROP_LTL_STRING "LTL"
#define PROP_PSL_STRING "PSL"
#define PROP_INVAR_STRING "Invar"
#define PROP_COMPUTE_STRING "Quantitative"
#define PROP_REACH_TARGET_STRING "ReachTarget"
#define PROP_AVOID_TARGET_STRING "AvoidTarget"
#define PROP_REACH_DEADLOCK_STRING "ReachDeadlock"
#define PROP_AVOID_DEADLOCK_STRING "AvoidDeadlock"
#define PROP_BUCHI_GAME_STRING "BuchiGame"
#define PROP_GEN_REACTIVITY_STRING "GenReactivity"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef enum _Prop_Status Prop_Status;
typedef enum _Prop_Type Prop_Type;

typedef struct Prop_TAG* Prop_ptr;
#define PROP(x) \
         ((Prop_ptr) x)

#define PROP_CHECK_INSTANCE(x) \
         (nusmv_assert(PROP(x) != PROP(NULL)))


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/* package handling */
EXTERN void PropPkg_init_cmd ARGS((void));
EXTERN void PropPkg_quit_cmd ARGS((void));

EXTERN void PropPkg_init ARGS((void));
EXTERN void PropPkg_quit ARGS((void));

/* Property interface */
EXTERN Prop_ptr Prop_create ARGS((void));
Prop_ptr Prop_create_partial ARGS((Expr_ptr expr, Prop_Type type));

EXTERN void Prop_destroy ARGS((Prop_ptr self));

EXTERN Expr_ptr Prop_get_expr ARGS((const Prop_ptr self));
EXTERN Expr_ptr Prop_get_expr_core ARGS((const Prop_ptr self));
EXTERN Set_t    Prop_get_cone ARGS((const Prop_ptr self));

EXTERN Prop_Type Prop_get_type ARGS((const Prop_ptr self));
EXTERN const char* Prop_get_type_as_string ARGS((Prop_ptr self));

EXTERN Prop_Status Prop_get_status ARGS((const Prop_ptr self));
EXTERN const char* Prop_get_status_as_string ARGS((const Prop_ptr self));

EXTERN int Prop_get_number ARGS((const Prop_ptr self));
EXTERN char* Prop_get_number_as_string ARGS((const Prop_ptr self));

EXTERN int Prop_get_trace ARGS((const Prop_ptr self));


EXTERN void Prop_set_cone ARGS((Prop_ptr self, Set_t cone));
EXTERN void Prop_set_status ARGS((Prop_ptr self, Prop_Status s));
EXTERN void Prop_set_number ARGS((Prop_ptr self, int n));
EXTERN void Prop_set_number_infinite ARGS((Prop_ptr self));
EXTERN void Prop_set_number_undefined ARGS((Prop_ptr self));
EXTERN void Prop_set_trace ARGS((Prop_ptr self, int t));

/* FSMs getters: */
EXTERN SexpFsm_ptr Prop_master_get_scalar_sexp_fsm ARGS((void));
EXTERN SexpFsm_ptr Prop_master_get_bool_sexp_fsm ARGS((void));
EXTERN BddFsm_ptr  Prop_master_get_bdd_fsm ARGS((void));
EXTERN BeFsm_ptr Prop_master_get_be_fsm ARGS((void));

EXTERN SexpFsm_ptr Prop_get_scalar_sexp_fsm ARGS((const Prop_ptr self));
EXTERN SexpFsm_ptr Prop_get_bool_sexp_fsm ARGS((const Prop_ptr self));
EXTERN BddFsm_ptr  Prop_get_bdd_fsm ARGS((const Prop_ptr self));
EXTERN BeFsm_ptr Prop_get_be_fsm ARGS((const Prop_ptr self));

/* FSMs setters: */
EXTERN void Prop_master_set_scalar_sexp_fsm ARGS((SexpFsm_ptr fsm));
EXTERN void Prop_master_set_bool_sexp_fsm ARGS((SexpFsm_ptr fsm));
EXTERN void Prop_master_set_bdd_fsm ARGS((BddFsm_ptr fsm));
EXTERN void Prop_master_set_be_fsm ARGS((BeFsm_ptr fsm));

EXTERN void Prop_set_scalar_sexp_fsm ARGS((Prop_ptr self, SexpFsm_ptr fsm));
EXTERN void Prop_set_bool_sexp_fsm ARGS((Prop_ptr self, SexpFsm_ptr fsm));
EXTERN void Prop_set_bdd_fsm ARGS((Prop_ptr self, BddFsm_ptr fsm));
EXTERN void Prop_set_be_fsm ARGS((Prop_ptr self, BeFsm_ptr fsm));

#if HAVE_MBP
/* Game FSMs getters: available only if MBP is enable */
EXTERN GameSexpFsm_ptr Prop_master_get_game_scalar_sexp_fsm ARGS((void));
EXTERN GameSexpFsm_ptr Prop_master_get_game_bool_sexp_fsm ARGS((void));
EXTERN GameBddFsm_ptr  Prop_master_get_game_bdd_fsm ARGS((void));
EXTERN GameBeFsm_ptr Prop_master_get_game_be_fsm ARGS((void));

EXTERN GameSexpFsm_ptr Prop_get_game_scalar_sexp_fsm ARGS((const Prop_ptr self));
EXTERN GameSexpFsm_ptr Prop_get_game_bool_sexp_fsm ARGS((const Prop_ptr self));
EXTERN GameBddFsm_ptr  Prop_get_game_bdd_fsm ARGS((const Prop_ptr self));
EXTERN GameBeFsm_ptr Prop_get_game_be_fsm ARGS((const Prop_ptr self));

/* Game FSMs setters: available only if MBP is enable*/
EXTERN void Prop_master_set_game_scalar_sexp_fsm ARGS((GameSexpFsm_ptr fsm));
EXTERN void Prop_master_set_game_bool_sexp_fsm ARGS((GameSexpFsm_ptr fsm));
EXTERN void Prop_master_set_game_bdd_fsm ARGS((GameBddFsm_ptr fsm));
EXTERN void Prop_master_set_game_be_fsm ARGS((GameBeFsm_ptr fsm));

EXTERN void Prop_set_game_scalar_sexp_fsm ARGS((Prop_ptr self, GameSexpFsm_ptr fsm));
EXTERN void Prop_set_game_bool_sexp_fsm ARGS((Prop_ptr self, GameSexpFsm_ptr fsm));
EXTERN void Prop_set_game_bdd_fsm ARGS((Prop_ptr self, GameBddFsm_ptr fsm));
EXTERN void Prop_set_game_be_fsm ARGS((Prop_ptr self, GameBeFsm_ptr fsm));
#endif

EXTERN void Prop_set_fsm_to_master ARGS((Prop_ptr self));

EXTERN void
Prop_apply_coi_for_bdd ARGS((Prop_ptr self, FsmBuilder_ptr helper));

EXTERN void
Prop_apply_coi_for_bmc ARGS((Prop_ptr self, FsmBuilder_ptr helper));

EXTERN void Prop_verify ARGS((Prop_ptr self));

EXTERN void Prop_print ARGS((Prop_ptr self, FILE*));
EXTERN void Prop_print_db ARGS((Prop_ptr self, FILE*));

EXTERN int Prop_check_type ARGS((const Prop_ptr self, Prop_Type type));

EXTERN char* Prop_get_text ARGS((const Prop_ptr self));
EXTERN char* Prop_get_context_text ARGS((const Prop_ptr self));


/* Property database */
EXTERN void PropDb_create ARGS((void));
EXTERN void PropDb_destroy ARGS((void));
EXTERN void PropDb_clean ARGS((void));

EXTERN int  
PropDb_fill ARGS((SymbTable_ptr symb_table, node_ptr, node_ptr, node_ptr, 
		  node_ptr, node_ptr));
EXTERN int  
PropDb_fill_game ARGS((SymbTable_ptr symb_table,
		       node_ptr reachtarget, node_ptr avoidtarget,
		       node_ptr reachdeadlock, node_ptr avoiddeadlock,
		       node_ptr buchigame, node_ptr genreactivity));

EXTERN boolean PropDb_add ARGS((Prop_ptr));

EXTERN int 
PropDb_prop_create_and_add ARGS((SymbTable_ptr symb_table, node_ptr, 
				 Prop_Type));

EXTERN int PropDb_get_size ARGS((void));

EXTERN Prop_ptr PropDb_get_prop_at_index ARGS((const int num));
EXTERN int PropDb_get_prop_index ARGS((const Prop_ptr property));

EXTERN Prop_ptr PropDb_get_last ARGS((void));


EXTERN void PropDb_print_list_header ARGS((FILE* file));
int PropDb_print_prop_at_index ARGS((FILE* file, const int index));
EXTERN void PropDb_print_all ARGS((FILE* file));
EXTERN void PropDb_print_all_type ARGS((FILE* file, Prop_Type type));
EXTERN void PropDb_print_all_status ARGS((FILE* file, Prop_Status status));
EXTERN void PropDb_print_all_status_type ARGS((FILE* file, Prop_Status status,
					       Prop_Type type));


EXTERN lsList PropDb_get_props_of_type ARGS((const Prop_Type type));

EXTERN int 
PropDb_prop_parse_and_add ARGS((SymbTable_ptr symb_table, 
				const char* str, const Prop_Type type));

EXTERN int PropDb_get_prop_index_from_string ARGS((const char* idx));

EXTERN int PropDb_get_prop_index_from_trace_index ARGS((const int trace_idx));

EXTERN void PropDb_verify_all ARGS((void));
EXTERN void PropDb_verify_all_type ARGS((Prop_Type));
EXTERN void PropDb_verify_prop_at_index ARGS((const int index));

EXTERN const char* PropType_to_string ARGS((const Prop_Type type));


EXTERN boolean Prop_is_psl_ltl ARGS((const Prop_ptr self));
EXTERN boolean Prop_is_psl_obe ARGS((const Prop_ptr self));

EXTERN boolean Prop_is_game_prop ARGS((const Prop_ptr self));
EXTERN string_ptr Prop_get_game_player ARGS((const Prop_ptr self));

#if HAVE_MATHSAT
struct ConcTrace_TAG; /* concrete trace is used by CEGAR */
EXTERN struct ConcTrace_TAG* Prop_get_ctrace ARGS((const Prop_ptr));
EXTERN void Prop_set_ctrace ARGS((const Prop_ptr, struct ConcTrace_TAG* ctrace));
#endif


/**AutomaticEnd***************************************************************/

#endif /* __PROP_H__ */
