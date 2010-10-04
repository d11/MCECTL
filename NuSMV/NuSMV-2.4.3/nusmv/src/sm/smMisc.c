/**CFile***********************************************************************

  FileName    [smMisc.c]

  PackageName [sm]

  Synopsis    [This file contain the main routine for the batch use of NuSMV.]

  Description [This file contain the main routine for the batch use of
  NuSMV. The batch main executes the various model checking steps in a
  predefined order. After the processing of the input file than it
  return to the calling shell.]

  SeeAlso     []

  Author      [Adapted to NuSMV by Marco Roveri]

  Copyright   [
  This file is part of the ``sm'' package of NuSMV version 2. 
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

******************************************************************************/

#include "smInt.h"
#include "set/set.h"
#include "fsm/bdd/BddFsm.h"

#if HAVE_MBP
#include "mbp/mbp.h"
#endif

#include "utils/error.h"
#include "mc/mc.h"
#include "enc/enc.h"
#include "bmc/bmcUtils.h"
#include "compile/compileInt.h" /* for CompileFlatten_init_flattener */
#include "opt/opt.h"

static char rcsid[] UTIL_UNUSED = "$Id: smMisc.c,v 1.26.2.26.2.3.2.26.4.5 2007/04/06 11:06:35 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [The batch main.]

  Description [The batch main. It first read the input file, than
  flatten the hierarchy. Aftre this preliminar phase it creates the
  boolean variables necessary for the encoding and then start
  compiling the read model into BDD. Now computes the reachable states
  depending if the flag has been set. before starting verifying if the
  properties specified in the model hold or not it computes the
  fairness constraints. You can also activate the reordering and
  also choose to avoid the verification of the properties.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void smBatchMain()
{
/*   SymbTable_ptr st; */

  /* Necessary to have standard behavior in the batch mode */
  util_resetlongjmp();
  CATCH {

  /* ================================================== */
  /*   1: Read the model                                */
  /* ================================================== */
  if (Cmd_CommandExecute("read_model")) nusmv_exit(1);

  /* ================================================== */
  /*  2: Flatten hierarchy                              */
  /* ================================================== */
  if (Cmd_CommandExecute("flatten_hierarchy")) nusmv_exit(1);

  /* If the -lp option is used, list the properties and exit */
  if (opt_list_properties(options) == true) {
    if (Cmd_CommandExecute("show_property")) nusmv_exit(1);
    return;
  }

  /* ================================================== */
  /*  3: Builds the encodings                           */
  /* ================================================== */
  if (Cmd_CommandExecute("encode_variables")) nusmv_exit(1);

  /* ================================================== */
  /*  4: Builds the flat FSMs                           */
  /* ================================================== */
  if (Cmd_CommandExecute("build_flat_model")) nusmv_exit(1);


  /* --------------------------------------------------- */
  /*  Write the flat and bool FSMs (if required)         */
  /* ----------------------------------------------------*/
  if (get_output_flatten_model_file(options) != NIL(char)) { 
    if (Cmd_CommandExecute("write_flat_model")) nusmv_exit(1);
  }

  if (get_output_boolean_model_file(options) != NIL(char)) {
    if (Cmd_CommandExecute("build_boolean_model")) nusmv_exit(1);
    if (Cmd_CommandExecute("write_boolean_model")) nusmv_exit(1);
  }

  /* ================================================== */
  /*  5.1 BMC starts                                    */
  /* ================================================== */
  if (opt_bmc_mode(options) == true) {
    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Entering BMC mode...\n");
    }
    
    /* games cannot be checked with BMC */
    if (opt_mbp_game(options)) nusmv_assert(false); 

    if (Cmd_CommandExecute("build_boolean_model")) nusmv_exit(1);

    /* Initializes the bmc package, and commits both the model and the
       determinization layers: */
    if (Cmd_CommandExecute("bmc_setup")) nusmv_exit(1);

    if (get_prop_no(options) != -1) {
      int prop_no = get_prop_no(options);
      Prop_ptr prop;

      if (opt_verbose_level_gt(options, 0)) {
        fprintf(nusmv_stderr, "Verifying property %d...\n", prop_no);
      }
      
      if ( (prop_no < 0) || (prop_no >= PropDb_get_size()) ) {
        fprintf(nusmv_stderr, 
                "Error: \"%d\" is not a valid property index\n", prop_no);
        nusmv_exit(1);
      }

      prop = PropDb_get_prop_at_index(prop_no);
      
      switch (Prop_get_type(prop)) {
      case Prop_Ltl:
	{
	  int rel_loop; 
	  
	  /* skip if -ils option is given */
	  if (opt_ignore_ltlspec(options)) break;

	  rel_loop = Bmc_Utils_ConvertLoopFromString(get_bmc_pb_loop(options), 
						     NULL);
	  Bmc_GenSolveLtl(prop, get_bmc_pb_length(options), rel_loop, 
			  /*increasing length*/ TRUE , 
			  TRUE, BMC_DUMP_NONE, NULL);
	  break;
	} 
	
      case Prop_Psl:
	{
	  int rel_loop = Bmc_Utils_ConvertLoopFromString(get_bmc_pb_loop(options), 
							 NULL);
          /* skip if -ips option is given */
	  if (opt_ignore_pslspec(options)) break;

	  Bmc_check_psl_property(prop, false, false, false, 
				 get_bmc_pb_length(options), rel_loop);
	  break;
	}
	
      case Prop_Invar: 
	/* skip if -ii option is given */
	if (opt_ignore_invar(options)) break;
	
        Bmc_GenSolveInvar(prop, TRUE, BMC_DUMP_NONE, NULL);
	break;
	
      default:
        fprintf(nusmv_stderr, 
                "Error: only LTL, PSL and INVAR properties can be checked in BMC mode\n");
        nusmv_exit(1);
      } /* switch on type */

    }
    else {
      /* Checks all ltlspecs, invarspecs and pslspecs */
      
      if (! opt_ignore_ltlspec(options)) {
	lsList props; 
	lsGen  iterator; 
	Prop_ptr prop;
	int rel_loop;

	if (opt_verbose_level_gt(options, 0)) {
	  fprintf(nusmv_stderr, "Verifying the LTL properties...\n");
	}

	
	props = PropDb_get_props_of_type(Prop_Ltl);	  
        nusmv_assert(props != LS_NIL);
	
        lsForEachItem(props, iterator, prop) {
          rel_loop = Bmc_Utils_ConvertLoopFromString(get_bmc_pb_loop(options), 
						     NULL);

          Bmc_GenSolveLtl(prop, get_bmc_pb_length(options), rel_loop, 
                          /*increasing length*/ TRUE, TRUE, BMC_DUMP_NONE, NULL);
        }

        lsDestroy(props, NULL); /* the list is no longer needed */
      }

      if (! opt_ignore_pslspec(options)) {
	lsList props; 
	lsGen  iterator; 
	Prop_ptr prop;
	int rel_loop = 
	  Bmc_Utils_ConvertLoopFromString(get_bmc_pb_loop(options), NULL);

	if (opt_verbose_level_gt(options, 0)) {
	  fprintf(nusmv_stderr, "Verifying the PSL properties...\n");
	}
	
	props = PropDb_get_props_of_type(Prop_Psl);	  
        nusmv_assert(props != LS_NIL);
	
        lsForEachItem(props, iterator, prop) {
	  if (Prop_is_psl_ltl(prop)) {
	    Bmc_check_psl_property(prop, false, false, false, 
				   get_bmc_pb_length(options), rel_loop);
	  }
        }

        lsDestroy(props, NULL); /* the list is no longer needed */
      }

      if (! opt_ignore_invar(options)) {
        lsList props;
        lsGen  iterator; 
        Prop_ptr prop;
	
	if (opt_verbose_level_gt(options, 0)) {
	  fprintf(nusmv_stderr, "Verifying the INVAR properties...\n");
	}
      
	props = PropDb_get_props_of_type(Prop_Invar);
        nusmv_assert(props != LS_NIL);
        
        lsForEachItem(props, iterator, prop) {
          Bmc_GenSolveInvar(prop, TRUE, BMC_DUMP_NONE, NULL);
        }

        lsDestroy(props, NULL); /* the list is no longer needed */
      }
    }

    return;
  } /* end of BMC */

  /* ================================================== */
  /*  5.2 BDD-based model checking starts               */
  /* ================================================== */
  
  /* Builds the BDD FSM of the whole read model.
     If COI is enabled there is no reason to create global BDD FSM since
     every property will have its one instance of a BDD FSM.
  */
  if (opt_cone_of_influence(options) == false) {
    if (Cmd_CommandExecute("build_model")) nusmv_exit(1);
  }

  /* checks the fsm if required */
  if (opt_check_fsm(options) == true) {
    if (opt_cone_of_influence(options)) {
      fprintf(nusmv_stderr,
      "WARNING: Check for totality of the transition relation cannot currently\n"
      "performed in batch mode if the cone of influence reduction has been enabled.\n");
      nusmv_exit(1);
    }
    /* The Game FSM cannot be checked */
    if (opt_mbp_game(options)) {
      fprintf(nusmv_stderr,
      "WARNING: Check for totality of the Game transition relations cannot \n"
	      "currently performed.\n");
      nusmv_exit(1);
    }
 
    BddFsm_check_machine(Prop_master_get_bdd_fsm()); 
  }

#if HAVE_MBP
#warning [AT] Here probably a GAME FSM should be converted to a usual FSM for LTL, CTL, etc, specs.
#warning [MR] The parser currently inhibit the specification of temporal properties. They must be enabled again
#warning [MR] We should think on which FSM to check the properties, and then possibly remove this conditional
#endif

  if (get_prop_no(options) != -1) {
    char command[20 + sizeof(long)];
    sprintf(command, "check_property -n %d", get_prop_no(options));
    if (Cmd_CommandExecute(command)) nusmv_exit(1);
  }
  else {
    /* Evaluates the Specifications */
    if (!opt_ignore_spec(options)) {
      PropDb_verify_all_type(Prop_Ctl);
    }

    if (!opt_ignore_compute(options)) {
      PropDb_verify_all_type(Prop_Compute);
    }

    /* Evaluates the LTL specifications */
    if (!opt_ignore_ltlspec(options)) {
      PropDb_verify_all_type(Prop_Ltl);
    }

    /* Evaluates the PSL specifications */
    if (!opt_ignore_pslspec(options)) {
      PropDb_verify_all_type(Prop_Psl);
    }

    /* Evaluates CHECKINVARIANTS */
    if (!opt_ignore_invar(options)) {
      PropDb_verify_all_type(Prop_Invar);
    }

#if HAVE_MBP
    /* GAME SPECIFICATIONS.
       They are always evaluated. 
       This code is actually used only if there is a game definition
       containing game specifications.
    */
    PropDb_verify_all_type(Prop_ReachTarget);
    PropDb_verify_all_type(Prop_AvoidTarget);
    PropDb_verify_all_type(Prop_ReachDeadlock);
    PropDb_verify_all_type(Prop_AvoidDeadlock);
    PropDb_verify_all_type(Prop_BuchiGame);
    PropDb_verify_all_type(Prop_GenReactivity);
#endif
  }

  
  /* Reporting of statistical information. */
  if (opt_verbose_level_gt(options, 0)) {  
    print_usage(nusmv_stdout); 
  }

  /* Computing and Reporting of the Effect of Reordering */
  if (opt_reorder(options)) {
    fprintf(nusmv_stdout, "\n========= starting reordering ============\n");
    dd_reorder(dd_manager, get_reorder_method(options), DEFAULT_MINSIZE);
    fprintf(nusmv_stdout, "\n========= after reordering ============\n");
    if (opt_verbose_level_gt(options, 0)) {  
      print_usage(nusmv_stdout);
    }

    if (Cmd_CommandExecute("write_order")) nusmv_exit(1);
  }

  /* Reporting of Reachable States */
  if (opt_print_reachable(options) == true) {
    if (opt_cone_of_influence(options)) {
      fprintf(nusmv_stderr,
	      "WARNING: Statistics of reachable states is not currently available\n"
	      "in batch mode if cone of influence reduction has been enabled.\n");
      nusmv_exit(1);
    }
    /* The Game FSM cannot be checked */
    if (opt_mbp_game(options)) {
      fprintf(nusmv_stderr,
      "WARNING: Statistics of reachable states is not currently available\n"
	      "for Game transition relations.\n");
      nusmv_exit(1);
    }

    BddFsm_print_reachable_states_info(Prop_master_get_bdd_fsm(), 
				       false, /* do not print states */
				       nusmv_stdout);
  }
  
  } FAIL { 
    fprintf(nusmv_stderr, "\nNuSMV terminated by a signal\n");
    nusmv_exit(1);
  }
}


/**Function********************************************************************

  Synopsis           [Prints usage statistic.]

  Description        [Prints on <code>nusmv_stdout</code> usage
  statistics, i.e. the amount of memory used, the amount of time
  spent, the number of BDD nodes allocated and the size of the model
  in BDD.]

  SideEffects        []

  SeeAlso            [compilePrintBddModelStatistic]
******************************************************************************/
void print_usage(FILE * file)
{
  fprintf(nusmv_stdout, "######################################################################\n");
  util_print_cpu_stats(file);
  fprintf(file, "######################################################################\n");
  fprintf(file, "BDD statistics\n");
  fprintf(file, "--------------------\n");
  fprintf(file, "BDD nodes allocated: %d\n", get_dd_nodes_allocated(dd_manager));
  fprintf(file, "--------------------\n");
  if (opt_cone_of_influence(options) == false) {
#if HAVE_MBP
    if (Prop_master_get_game_bdd_fsm() != GAME_BDD_FSM(NULL)) {
      GameBddFsm_print_info(Prop_master_get_game_bdd_fsm(), file);    
    }
    else 
#endif
   if (Prop_master_get_bdd_fsm() != BDD_FSM(NULL)) {
      BddFsm_print_info(Prop_master_get_bdd_fsm(), file);    
    }
  }
  else {
    fprintf(nusmv_stderr, "WARNING: Model Statistics is not currently available\n");
    fprintf(nusmv_stderr, "if cone of influence reduction has been enabled.\n");
  }
}



/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void restore_nusmv_stdout()
{
  if (nusmv_stdout != def_nusmv_stdout) {
    fclose(nusmv_stdout);
    nusmv_stdout = def_nusmv_stdout;
  }
}



/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

  SeeAlso            []
******************************************************************************/
void restore_nusmv_stderr()
{
  if (nusmv_stderr != def_nusmv_stderr) {
    fclose(nusmv_stderr);
    nusmv_stderr = def_nusmv_stderr;
  }  
}

