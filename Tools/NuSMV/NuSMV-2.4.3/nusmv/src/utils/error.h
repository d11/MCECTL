/**CHeaderFile*****************************************************************

  FileName    [error.h]

  PackageName [utils]

  Synopsis    [header for the error.c file]

  Description [header for the error.c file.]

  SeeAlso     []

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``utils'' package of NuSMV version 2. 
  Copyright (C) 1998-2001 by CMU and ITC-irst. 

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

  Revision    [$Id: error.h,v 1.6.2.13.4.19.4.4 2007/04/05 15:42:49 nusmv Exp $]

******************************************************************************/

#ifndef _ERROR_H
#define _ERROR_H

#include "utils/utils.h"
#include "node/node.h"
#include "utils/NodeList.h"
#include "prop/prop.h"


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef enum FailureKind_TAG {
  FAILURE_DIV_BY_ZERO, 
  FAILURE_CASE_NOT_EXHAUSTIVE, 
  FAILURE_UNSPECIFIED
} FailureKind;



/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void init_the_node ARGS((void));
EXTERN void set_the_node ARGS((node_ptr));
EXTERN node_ptr get_the_node ARGS((void));
EXTERN int io_atom_isempty ARGS((void));
EXTERN void io_atom_push ARGS((node_ptr));
EXTERN void io_atom_pop ARGS((void));
EXTERN node_ptr io_atom_head ARGS((void));
EXTERN void print_io_atom_stack ARGS((FILE * fd));
EXTERN void start_parsing_err ARGS((void));
EXTERN void nusmv_exit ARGS((int));
EXTERN void rpterr ARGS((const char *, ...));
EXTERN void internal_error ARGS((const char *, ...));
EXTERN void error_multiple_substitution ARGS((node_ptr));
EXTERN void report_failure_node ARGS((node_ptr));
EXTERN void warning_case_not_exhaustive ARGS((node_ptr));
EXTERN void warning_possible_div_by_zero ARGS((node_ptr failure));

EXTERN void range_error ARGS((node_ptr, node_ptr));
EXTERN void type_error ARGS((node_ptr));
EXTERN void error_shadowing ARGS((node_ptr s));
EXTERN void error_redefining ARGS((node_ptr));
EXTERN void error_redefining_operational_symbol ARGS((node_ptr));
EXTERN void error_redefining_input_var ARGS((node_ptr s));
EXTERN void error_assign_input_var ARGS((node_ptr s));
EXTERN void error_ambiguous ARGS((node_ptr s));
EXTERN void error_undefined ARGS((node_ptr));
EXTERN void error_circular ARGS((node_ptr));
EXTERN void error_too_many_vars ARGS((void));
EXTERN void error_not_proper_number ARGS((const char* op, node_ptr));
EXTERN void error_not_proper_numbers ARGS((const char* op, node_ptr, node_ptr));
EXTERN void error_multiple_assignment ARGS((node_ptr));
EXTERN void error_empty_range ARGS((node_ptr, int, int));
EXTERN void error_not_constant_width_of_word_type ARGS((node_ptr));
EXTERN void error_not_constant_width_of_word_array_type ARGS((node_ptr));
EXTERN void error_assign_both ARGS((node_ptr, node_ptr, int, int));
EXTERN void error_multiple_substitution ARGS((node_ptr));
EXTERN void error_unknown_var_in_order_file ARGS((node_ptr));
EXTERN void error_var_appear_twice_in_order_file ARGS((node_ptr));
EXTERN void error_id_appears_twice_in_idlist_file ARGS((node_ptr));
EXTERN void warning_var_appear_twice_in_order_file ARGS((node_ptr));
EXTERN void warning_id_appears_twice_in_idlist_file ARGS((node_ptr n));
EXTERN void warning_variable_not_declared ARGS((node_ptr));
EXTERN void warning_missing_variable ARGS((node_ptr));
EXTERN void warning_missing_variables ARGS((NodeList_ptr vars_list));
EXTERN void warning_non_ag_only_spec ARGS((Prop_ptr));
EXTERN void warning_ag_only_without_reachables ARGS((void));
EXTERN void warning_fsm_init_empty ARGS((void));
EXTERN void warning_fsm_fairness_empty ARGS((void));
EXTERN void warning_fsm_init_and_fairness_empty ARGS((void));
EXTERN void warning_fsm_invar_empty ARGS((void));

EXTERN void error_var_not_in_order_file ARGS((node_ptr));
EXTERN void error_out_of_memory ARGS((long));
EXTERN void error_invalid_subrange ARGS((node_ptr range));
EXTERN void error_game_definition_contains_input_vars ARGS((node_ptr var_name));
EXTERN void error_property_contains_input_vars ARGS((Prop_ptr));
EXTERN void error_assign_exp_contains_input_vars ARGS((node_ptr));
EXTERN void error_trans_exp_contains_input_vars ARGS((node_ptr));
EXTERN void error_invar_exp_contains_input_vars ARGS((node_ptr));
EXTERN void error_init_exp_contains_input_vars ARGS((node_ptr));
EXTERN void error_second_player_var ARGS((node_ptr));
EXTERN void error_second_player_next_var ARGS((node_ptr));

EXTERN void error_unknown_preprocessor ARGS((const char* prep_name));
EXTERN void error_type_system_violation ARGS((void));
EXTERN void error_psl_not_supported_feature ARGS((void));
EXTERN void error_psl_not_supported_feature_next_number ARGS((void));
EXTERN void error_not_supported_feature ARGS((const char* msg));
EXTERN void error_expected_number ARGS((void));

EXTERN void warning_psl_not_supported_feature 
ARGS((node_ptr psl_spec, int index));

EXTERN void error_psl_repeated_replicator_id ARGS((void));

EXTERN void error_invalid_number ARGS((const char* szNumber));
EXTERN void error_bmc_invalid_k_l ARGS((const int k, const int l));
EXTERN void error_property_already_specified ARGS((void));

EXTERN void 
error_invalid_numeric_value ARGS((int value, const char* reason));

EXTERN void error_file_not_found ARGS((const char* filename));

EXTERN void finish_parsing_err ARGS((void));
EXTERN void error_reset_and_exit ARGS((int val));

EXTERN void init_memory ARGS((void));

EXTERN node_ptr 
failure_make ARGS((const char* msg, FailureKind kind, int lineno));

EXTERN const char* failure_get_msg ARGS((node_ptr failure));
EXTERN FailureKind failure_get_kind ARGS((node_ptr failure));
EXTERN int failure_get_lineno ARGS((node_ptr failure));



#include <setjmp.h>
#ifdef __CYGWIN__ 
#define JMPBUF jmp_buf
#define SETJMP(buf,val) setjmp(buf)
#define LONGJMP(buf,val) longjmp(buf, val)
#else
# ifdef __MINGW32__
#define JMPBUF jmp_buf
#define SETJMP(buf,val) setjmp(buf)
#define LONGJMP(buf,val) longjmp(buf, val)
# else
#  define JMPBUF sigjmp_buf
#  define SETJMP(buf,val) sigsetjmp(buf, val)
#  define LONGJMP(buf,val) siglongjmp(buf, val)
# endif
#endif

EXTERN JMPBUF * util_newlongjmp ARGS((void));
EXTERN void util_longjmp ARGS((void));
EXTERN void util_cancellongjmp ARGS((void));
EXTERN void util_resetlongjmp ARGS((void));
#define util_setlongjmp() SETJMP(*(util_newlongjmp()), 1)
/* warning take care not to do something like this:

  CATCH {
     cmd1....
     return 1;
  } FAIL {
     cmd2...
    return 0;
  }

  The right way to use it is:

  { 
    type result;

    CATCH {
     cmd1....
     result = value;
    } FAIL {
     cmd2...
     result = 1;
    }
    return(result);
  }
  I.e. return inside CATCH/FAIL may cause damage of the stack 
*/
#define CATCH if (util_setlongjmp() == 0) { 
#define FAIL  util_cancellongjmp(); } else

#endif /* _ERROR_H */
