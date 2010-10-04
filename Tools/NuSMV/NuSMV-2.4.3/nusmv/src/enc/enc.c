/**CFile*****************************************************************

  FileName    [enc.c]

  PackageName [enc]

  Synopsis    [Package level code of package enc]

  Description []

  SeeAlso     [enc.h, encInt.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``enc'' package of NuSMV version 2. 
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

#include "enc.h"
#include "encInt.h"

#include "compile/compile.h"
#include "compile/type_checking/type_checking.h" /*for type of expressions */
#include "utils/ucmd.h"
#include "utils/error.h"


/*---------------------------------------------------------------------------*/
/* Macro definition                                                          */
/*---------------------------------------------------------------------------*/
#define  VARS_ORD_STR_INPUTS_BEFORE "inputs_before"
#define  VARS_ORD_STR_INPUTS_AFTER  "inputs_after"
#define  VARS_ORD_STR_LEX           "lexicographic"
#define  VARS_ORD_STR_DEFAULT       "default"
#define  VARS_ORD_STR_UNKNOWN



/**Variable********************************************************************

  Synopsis    [Global bool encoding]

  Description [This instance must be accessed via dedicated methods]

  SeeAlso     []

******************************************************************************/
static BoolEnc_ptr global_bool_enc = BOOL_ENC(NULL);


/**Variable********************************************************************

  Synopsis    [Global encoding for BDDs]

  Description [This instance must be accessed via dedicated methods]

  SeeAlso     []

******************************************************************************/
static BddEnc_ptr global_bdd_enc = BDD_ENC(NULL);


/**Variable********************************************************************

  Synopsis    [Global encoding for BEs]

  Description [This instance must be accessed via dedicated methods]

  SeeAlso     []

******************************************************************************/
static BeEnc_ptr global_be_enc = BE_ENC(NULL);

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void enc_set_bool_encoding ARGS((BoolEnc_ptr benc));
static void enc_set_bdd_encoding ARGS((BddEnc_ptr enc));
static void enc_set_be_encoding ARGS((BeEnc_ptr enc));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the encoding package]

  Description        [This function initializes only data-structures 
  global to all encoding.
  To initialize particular incoding, you have to invoke corresponding
  init-functions, such as Enc_init_bool_encoding, etc.

  SideEffects        []

  SeeAlso            [Enc_init_bool_encoding, Enc_init_bdd_encoding,
  Enc_reinit_bdd_encoding, Enc_quit_encodings]
******************************************************************************/
void Enc_init_encodings()
{
}


/**Function********************************************************************

  Synopsis           [Initializes the boolean encoding for this session]

  Description        [Call it to initialize for the current session the
  encoding, before flattening. In the current implementation, you must
  call this *before* the flattening phase. After the flattening, 
  you must initialize the bdd encoding as well, and after you created the
  boolean sexp fsm, you must reinitialize the bdd encodings by calling
  Enc_reinit_bdd_encoding. Don't forget to call Enc_quit_encodings when
  the session ends. ]

  SideEffects        []

  SeeAlso            [Enc_init_bdd_encoding, Enc_reinit_bdd_encoding,
  Enc_quit_encodings]
******************************************************************************/
void Enc_init_bool_encoding()
{
  nusmv_assert(global_bool_enc == BOOL_ENC(NULL));

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "\nInitializing global boolean encoding...\n");
  }

  global_bool_enc = BoolEnc_create(Compile_get_global_symb_table());

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "Global boolean encoding initialized.\n");
  }
}


/**Function********************************************************************

  Synopsis           [Initializes the bdd enc for this session]

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void Enc_init_bdd_encoding()
{
  OrdGroups_ptr ord_groups;

  BOOL_ENC_CHECK_INSTANCE(global_bool_enc);
  nusmv_assert(global_bdd_enc == BDD_ENC(NULL));

  if (opt_verbose_level_gt(options, 1)) {
    if (!util_is_string_null(get_input_order_file(options))) {
      fprintf(nusmv_stdout, "\nParsing the ordering file '");
      fprintf(nusmv_stdout, get_input_order_file(options));
      fprintf(nusmv_stdout, "'...\n");
    }
  }

  ord_groups = enc_utils_parse_ordering_file(get_input_order_file(options), 
                                             Enc_get_bool_encoding());

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "\nInitializing global BDD encoding...\n");
  }

  /* ord_groups will belong to global_bdd_enc */
  global_bdd_enc = BddEnc_create(Compile_get_global_symb_table(),
                                 TypeCheckingPkg_get_global_type_checker(),
                                 Enc_get_bool_encoding(), dd_manager, 
                                 ord_groups); 

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "Global BDD encoding initialized.\n");
  }
}


/**Function********************************************************************

  Synopsis           [Initializes the be enc for this session]

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void Enc_init_be_encoding()
{
  BOOL_ENC_CHECK_INSTANCE(global_bool_enc);
  nusmv_assert(global_be_enc == BE_ENC(NULL));

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "\nInitializing global BE encoding...\n");
  }

  global_be_enc = BeEnc_create(Compile_get_global_symb_table(), 
                               Enc_get_bool_encoding());

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "Global BE encoding initialized.\n");
  }
}


/**Function********************************************************************

  Synopsis           [Call to destroy all encodings, when session ends]

  Description        [Call to destroy encodings, when session ends. 
  Enc_init_encodings had to be called before calling this function.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void Enc_quit_encodings()
{
  enc_set_bdd_encoding(BDD_ENC(NULL));
  enc_set_be_encoding(BE_ENC(NULL));
  enc_set_bool_encoding(BOOL_ENC(NULL));
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
BoolEnc_ptr Enc_get_bool_encoding(void)
{
  BOOL_ENC_CHECK_INSTANCE(global_bool_enc);
  return global_bool_enc;
}
 

/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
BddEnc_ptr Enc_get_bdd_encoding(void)
{
  BDD_ENC_CHECK_INSTANCE(global_bdd_enc);
  return global_bdd_enc;
}


/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
BeEnc_ptr Enc_get_be_encoding(void)
{
  BE_ENC_CHECK_INSTANCE(global_be_enc);
  return global_be_enc;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Set the global boolean encoding]

  Description        [Set the global boolean encoding. If it was already 
  set, it is detroyed before the new assignment]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
static void enc_set_bool_encoding(BoolEnc_ptr benc)
{
  if (global_bool_enc != BOOL_ENC(NULL)) {
    BoolEnc_destroy(global_bool_enc);
  }
      
  global_bool_enc = benc; 
}


/**Function********************************************************************

  Synopsis           [Set the global bdd encoding]

  Description        [Set the global bdd encoding. If it was already 
  set, it is detroyed before the new assignment]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
static void enc_set_bdd_encoding(BddEnc_ptr enc)
{
  if (global_bdd_enc != BDD_ENC(NULL)) {
    BddEnc_destroy(global_bdd_enc);
  }
      
  global_bdd_enc = enc;      
}


/**Function********************************************************************

  Synopsis           [Set the global be encoding]

  Description        [Set the global be encoding. If it was already 
  set, it is detroyed before the new assignment]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
static void enc_set_be_encoding(BeEnc_ptr enc)
{
  if (global_be_enc != BE_ENC(NULL)) {
    BeEnc_destroy(global_be_enc);
  }
      
  global_be_enc = enc;      
}


/**Function********************************************************************

  Synopsis           [Returns the string corresponding to give parameter]

  Description        [Returned string does not have to be freed]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
const char* Enc_vars_ord_to_string(VarsOrdType vot)
{
  switch (vot) {

  case VARS_ORD_INPUTS_BEFORE: return VARS_ORD_STR_INPUTS_BEFORE;
  case VARS_ORD_INPUTS_AFTER: return VARS_ORD_STR_INPUTS_AFTER ;
  case VARS_ORD_LEX: return VARS_ORD_STR_LEX;
  case VARS_ORD_DEFAULT: return VARS_ORD_STR_DEFAULT;
  case VARS_ORD_UNKNOWN: internal_error("Wrong var ordering type");
  default:
    nusmv_assert(false); /* no other possible cases */
  }
  return NULL;
}


/**Function********************************************************************

  Synopsis           [Converts a string to the corresponding var order type.]

  Description        [VARS_ORD_UNKNOWN is returned when the string does not 
  match the given string]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
VarsOrdType Enc_string_to_vars_ord(const char* str)
{
  if (strcmp(str, VARS_ORD_STR_DEFAULT) == 0) return VARS_ORD_DEFAULT;
  if (strcmp(str, VARS_ORD_STR_INPUTS_BEFORE) == 0) {
    return VARS_ORD_INPUTS_BEFORE;
  }
  if (strcmp(str, VARS_ORD_STR_INPUTS_AFTER) == 0) {
    return VARS_ORD_INPUTS_AFTER;
  }
  if (strcmp(str, VARS_ORD_STR_LEX) == 0) return VARS_ORD_LEX;
  return VARS_ORD_UNKNOWN;
}


/**Function********************************************************************

  Synopsis           [Returns a string of all possible values for 
  vars_ord_type]

  Description        [Returned string does not have to be freed]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
const char* Enc_get_valid_vars_ord_types()
{
  return VARS_ORD_STR_DEFAULT  ", " \
    VARS_ORD_STR_INPUTS_BEFORE ", " \
    VARS_ORD_STR_INPUTS_AFTER  ", " \
    VARS_ORD_STR_LEX;
}
