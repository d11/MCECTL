/**CFile***********************************************************************

  FileName    [bmcPkg.c]

  PackageName [bmc]

  Synopsis    [Bmc.Pkg module]

  Description [This module contains all the bmc package handling functions]

  SeeAlso     []

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``bmc'' package of NuSMV version 2.
  Copyright (C) 2000-2001 by ITC-irst and University of Trento.

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
# include "config.h"
#endif

#include "bmcPkg.h"
#include "bmcInt.h" /* for 'options' */
#include "bmcCmd.h"

#include "bmc/sbmc/sbmcPkg.h"

#include "be/be.h"
#include "sat/sat.h"

#include "prop/prop.h"
#include "cmd/cmd.h"
#include "enc/enc.h" 
#include "fsm/be/BeFsm.h"

#ifdef BENCHMARKING
  #include <time.h>
  clock_t start_time;
#endif


static char rcsid[] UTIL_UNUSED = "$Id: bmcPkg.c,v 1.3.2.9.2.4.2.10.4.4 2006/10/24 15:00:49 nusmv Exp $";

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

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/**AutomaticEnd***************************************************************/



/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the BMC structure]

  Description [It builds the vars manager, initializes the package and
  all sub packages, but only if not previously called.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void Bmc_Init()
{
  BeEnc_ptr be_enc;

  /* does not initialize the package if previously initialized */
  if (cmp_struct_get_bmc_init(cmps)) return;

  #ifdef BENCHMARKING
    fprintf(nusmv_stdout,":START:benchmarking Bmc_Init\n");
    start_time = clock();
  #endif

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Initializing the BMC package... \n");
  }

  /* Initializes the be generic interface layer: */
  Be_Init();

  /* sets up Conv module: */
  Bmc_Conv_init_cache();

  /* sets up an hash in the wff module: */
  bmc_init_wff2nnf_hash();

  /* builds the variables manager: */
  Enc_init_be_encoding();
  be_enc = Enc_get_be_encoding();
  BaseEnc_commit_layer(BASE_ENC(be_enc), MODEL_LAYER_NAME);

  /* the determinization layer will be committed by the command
     bmc_setup (see bmc_build_master_be_fsm) */

#ifdef BENCHMARKING
  fprintf(nusmv_stdout,":UTIME = %.4f secs.\n",((double)(clock()-start_time))/CLOCKS_PER_SEC);
  fprintf(nusmv_stdout,":STOP:benchmarking Bmc_Init\n");
#endif
  
  /* Initialize the SBMC sub-package */
  SBmc_Init();

  cmp_struct_set_bmc_init(cmps);
}


/**Function********************************************************************

  Synopsis           [Frees all resources allocated for the BMC model manager]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Bmc_Quit()
{
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Quitting the BMC package... \n");
  }

  /* Shuts down the Be layer: */
  Be_Quit();

  /* Resets the _bmc_test_tableau command private data: */
  Bmc_TestReset();

  /* quits an hash table in the wff module: */
  bmc_quit_wff2nnf_hash();

  /* quits Conv module */
  Bmc_Conv_quit_cache();

  /* Quits the SBMC sub-package */
  SBmc_Quit();

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Done \n");
  }

  cmp_struct_unset_bmc_init(cmps);
}




/**Function********************************************************************

  Synopsis           [Adds all bmc-related commands to the interactive shell]

  Description        []

  SideEffects        []

  SeeAlso            [Sm_Init]

******************************************************************************/
void Bmc_AddCmd()
{
  Cmd_CommandAdd("bmc_setup", Bmc_CommandBmcSetup, 0, false);
  Cmd_CommandAdd("bmc_simulate", Bmc_CommandBmcSimulate, 0, true);
  Cmd_CommandAdd("gen_ltlspec_bmc",
                 Bmc_CommandGenLtlSpecBmc, 0, true);
  Cmd_CommandAdd("gen_ltlspec_bmc_onepb",
                 Bmc_CommandGenLtlSpecBmcOnePb, 0, true);
  Cmd_CommandAdd("check_ltlspec_bmc", Bmc_CommandCheckLtlSpecBmc, 0, true);
  Cmd_CommandAdd("check_ltlspec_bmc_onepb",
		 Bmc_CommandCheckLtlSpecBmcOnePb, 0, true);
#if HAVE_INCREMENTAL_SAT
  Cmd_CommandAdd("check_ltlspec_bmc_inc", Bmc_CommandCheckLtlSpecBmcInc, 
		 0, true);
#endif

  Cmd_CommandAdd("gen_invar_bmc",     Bmc_CommandGenInvarBmc, 0, true);
  Cmd_CommandAdd("check_invar_bmc",   Bmc_CommandCheckInvarBmc, 0, true);
#if HAVE_INCREMENTAL_SAT
  Cmd_CommandAdd("check_invar_bmc_inc",   Bmc_CommandCheckInvarBmcInc, 
		 0, true);
#endif
  Cmd_CommandAdd("_bmc_test_tableau", Bmc_TestTableau, 0, true);

  /* Add the SBMC sub-package commands */
  SBmc_AddCmd();
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

