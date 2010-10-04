/**CHeaderFile*****************************************************************

  FileName    [enc.h]

  PackageName [enc]

  Synopsis    [Public API for the enc package. Basically methods for
  accessing global encodings are provided here]

  Description []

  SeeAlso     [enc.c]

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

#ifndef __ENC_H__
#define __ENC_H__

#include "utils/utils.h"

#include "enc/bool/BoolEnc.h"
#include "enc/bdd/BddEnc.h"
#include "enc/be/BeEnc.h"


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure definitions                                                     */
/*---------------------------------------------------------------------------*/
typedef enum  {
  VARS_ORD_DEFAULT, 
  VARS_ORD_INPUTS_BEFORE, /* default */
  VARS_ORD_INPUTS_AFTER, 
  VARS_ORD_LEX, 
  VARS_ORD_UNKNOWN

} VarsOrdType;


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Functions declarations                                                    */
/*---------------------------------------------------------------------------*/

EXTERN void Enc_init_encodings ARGS((void));

EXTERN void Enc_init_bool_encoding ARGS((void));
EXTERN void Enc_init_bdd_encoding ARGS((void));
EXTERN void Enc_init_be_encoding ARGS((void));

EXTERN void Enc_quit_encodings ARGS((void));

EXTERN BoolEnc_ptr Enc_get_bool_encoding ARGS((void));
EXTERN BddEnc_ptr Enc_get_bdd_encoding ARGS((void));
EXTERN BeEnc_ptr Enc_get_be_encoding ARGS((void));

EXTERN const char* Enc_vars_ord_to_string ARGS((VarsOrdType));
EXTERN VarsOrdType Enc_string_to_vars_ord ARGS((const char*));
EXTERN const char* Enc_get_valid_vars_ord_types ARGS((void));

#endif /* __ENC_H__ */
