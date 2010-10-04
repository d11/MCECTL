/**CFile***********************************************************************

  FileName    [compileCmd.c]

  PackageName [compile]

  Synopsis    [Shell interface for the compile package.]

  Description [This file contains the interface of the compile package
  with the interactive shell.]

  SeeAlso     [cmdCmd.c]

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2. 
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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "compileInt.h"
#include "symb_table/SymbTable.h"
#include "symb_table/symb_table.h"
#include "type_checking/type_checking.h"
#include "PredicateNormaliser.h"

#if HAVE_MBP
#include "mbp/mbp.h"
#endif

#include "parser/symbols.h"
#include "parser/parser.h"

#include "cmd/cmd.h"

#include "fsm/FsmBuilder.h"
#include "fsm/sexp/SexpFsm.h"
#include "fsm/bdd/BddFsm.h"

#include "prop/prop.h"
#include "mc/mc.h"
#include "enc/enc.h"

#include "trace/pkg_trace.h"

#include "utils/ucmd.h"
#include "utils/utils_io.h"
#include "utils/error.h" /* for CATCH */



static char rcsid[] UTIL_UNUSED = "$Id: compileCmd.c,v 1.42.2.28.2.2.2.46.4.17 2007/05/16 15:27:35 nusmv Exp $";


/* prototypes of the command functions */
int CommandProcessModel ARGS((int argc, char **argv));
int CommandFlattenHierarchy ARGS((int argc, char **argv));
int CommandShowVars   ARGS((int argc, char **argv));
int CommandEncodeVariables ARGS((int argc, char **argv));
int CommandBuildModel ARGS((int argc, char **argv));
int CommandBuildFlatModel ARGS((int argc, char **argv));
int CommandBuildBooleanModel ARGS((int argc, char **argv));
int CommandDumpModel ARGS((int argc, char **argv));
int CommandAddTrans ARGS((int argc, char **argv));
int CommandAddInit ARGS((int argc, char **argv));
int CommandAddFairness ARGS((int argc, char **argv));
int CommandRestoreModel ARGS((int argc, char **argv));
int CommandWriteOrder ARGS((int argc, char **argv));
int CommandIwls95PrintOption ARGS((int argc, char **argv));
int CommandCPPrintClusterInfo ARGS((int argc, char **argv));
int CommandPrintFsmStats ARGS((int argc, char **argv));
int CommandGo ARGS((int argc, char **argv));
int CommandGoBmc ARGS((int argc, char **argv));
int CommandGetInternalStatus ARGS((int argc, char **argv));
int CommandWriteModelFlat ARGS((int argc, char **argv));
int CommandWriteModelFlatBool ARGS((int argc, char **argv));

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int UsageProcessModel ARGS((void));
static int UsageFlattenHierarchy ARGS((void));
static int UsageShowVars   ARGS((void));
static int UsageBuildModel ARGS((void));
static int UsageBuildFlatModel ARGS((void));
static int UsageBuildBooleanModel ARGS((void));
static int UsageEncodeVariables ARGS((void));
static int UsageWriteOrder ARGS((void));
static int UsageIwls95PrintOption ARGS((void));
static int UsageGo ARGS((void));
static int UsageGoBmc ARGS((void));
static int UsageGetInternalStatus ARGS((void));
static int UsageWriteModelFlat ARGS((void));
static int UsageWriteModelFlatBool ARGS((void));
static int UsagePrintFsmStats ARGS((void));

static void compile_create_flat_model ARGS((void));
static void compile_create_boolean_model ARGS((void));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the compile package.]

  Description        [Initializes the compile package. The set of commands must 
  be explicitly initialized later by calling Compile_InitCmd.]

  SideEffects        []

******************************************************************************/
void Compile_init()
{
  cmps = cmp_struct_init();

  init_check_constant_hash();
  init_expr2bexp_hash();
  init_dependencies_hash();
  init_define_dep_hash();
  init_coi_hash();

  /* the global fsm builder creation */
  nusmv_assert(global_fsm_builder == FSM_BUILDER(NULL));
  global_fsm_builder = FsmBuilder_create(dd_manager);

  /* modules and sub packages initialization */
  SymbTablePkg_init();
  TypeCheckingPkg_init(Compile_get_global_symb_table());

  /* the global predicate-normaliser creation (must be done after
     creation of global type-checker) */
  nusmv_assert(PREDICATE_NORMALISER(NULL) == global_predication_normaliser);
  global_predication_normaliser = 
    PredicateNormaliser_create(TypeCheckingPkg_get_global_type_checker());
}

/**Function********************************************************************

  Synopsis           [Initializes the commands provided by this package]

  Description        []

  SideEffects        []

******************************************************************************/
void Compile_init_cmd()
{
  Cmd_CommandAdd("process_model", CommandProcessModel, 0, false);
  Cmd_CommandAdd("flatten_hierarchy", CommandFlattenHierarchy, 0, false);
  Cmd_CommandAdd("show_vars", CommandShowVars, 0, true);
  Cmd_CommandAdd("encode_variables", CommandEncodeVariables, 0, false);
  Cmd_CommandAdd("build_model", CommandBuildModel, 0, false);
  Cmd_CommandAdd("build_flat_model", CommandBuildFlatModel, 0, false);
  Cmd_CommandAdd("build_boolean_model", CommandBuildBooleanModel, 0, false);
  Cmd_CommandAdd("write_order", CommandWriteOrder, 0, true);
  Cmd_CommandAdd("print_iwls95options", CommandIwls95PrintOption, 0, true);

  /* this is deprecated in 2.4 */
  Cmd_CommandAdd("print_clusterinfo", CommandCPPrintClusterInfo, 0, true); 

  Cmd_CommandAdd("print_fsm_stats", CommandPrintFsmStats, 0, true);
  Cmd_CommandAdd("go", CommandGo, 0, false);
  Cmd_CommandAdd("go_bmc", CommandGoBmc, 0, false);
  Cmd_CommandAdd("get_internal_status", CommandGetInternalStatus, 0, true);
  Cmd_CommandAdd("write_flat_model", CommandWriteModelFlat, 0, true);
  Cmd_CommandAdd("write_boolean_model", CommandWriteModelFlatBool, 0, true);  
}

/**Function********************************************************************

  Synopsis           [Shut down the compile package]

  Description        [Shut down the compile package]

  SideEffects        []

******************************************************************************/
void Compile_quit()
{
  if (opt_verbose_level_gt(options, 2)) {
    fprintf(nusmv_stderr, "Shutting down the compiler...\n");
  }

  /* the global predicate-normaliser destrution */
  PREDICATE_NORMALISER_CHECK_INSTANCE(global_predication_normaliser);
  PredicateNormaliser_destroy(global_predication_normaliser);
  global_predication_normaliser = PREDICATE_NORMALISER(NULL);  

  /* modules and sub packages initialization */  
  TypeCheckingPkg_quit();
  SymbTablePkg_quit();
  CompileFlatten_quit_flattener();

  if (FLAT_HIERARCHY(NULL) != mainFlatHierarchy) {
    FlatHierarchy_destroy(mainFlatHierarchy);
    mainFlatHierarchy = FLAT_HIERARCHY(NULL);
  }

  /* local structures: */
  if (global_fsm_builder != FSM_BUILDER(NULL)) {
    FsmBuilder_destroy(global_fsm_builder);
    global_fsm_builder = FSM_BUILDER(NULL);
  }

  clear_coi_hash();
  clear_define_dep_hash();
  clear_dependencies_hash();
  clear_expr2bexp_hash();
  clear_check_constant_hash();

}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Performs the batch steps and then returns
  control to the interactive shell.]

  CommandName        [process_model]       

  CommandSynopsis    [Performs the batch steps and then returns control
  to the interactive shell.]  

  CommandArguments   [\[-h\] \[-f\] \[-r\] \[-i model-file\] \[-m Method\]]  

  CommandDescription [ Reads the model, compiles it into BDD and
  performs the model checking of all the specification contained in
  it. If the environment variable <tt>forward_search</tt> has been set
  before, then the set of reachable states is computed. If the
  option <tt>-r</tt> is specified, the reordering of variables is
  performed accordingly. This command simulates the batch behavior of
  NuSMV and then returns the control to the interactive shell.<p>

  Command options:<p>
  <dl>
    <dt> <tt>-f</tt>
       <dd> Forces model construction even when COI is enabled.
    <dt> <tt>-r</tt>
       <dd> Performs a variable ordering at the end of the
       computation, and dumps the variable ordering as the command
       line option <tt>-reorder</tt> does.
    <dt> <tt>-i model-file</tt>
       <dd> Sets the environment variable <tt>input_file</tt> to file
           <tt>model-file</tt>, and reads the model from file
           <tt>model-file</tt>.
    <dt> <tt>-m Method</tt>
       <dd> Sets the environment variable <tt>partition_method</tt> to
       <tt>Method</tt> and uses it as partitioning method.
  </dl>
  ]  

  SideEffects        []

******************************************************************************/
int CommandProcessModel(int argc, char **argv)
{
  int c;
  char * partition_method = NIL(char);
  boolean force_reordering = false;
  boolean force_build = false;

  util_getopt_reset();
  while((c = util_getopt(argc,argv,"hfri:m:")) != EOF){
    switch (c) {
    case 'f': force_build = true; break;
    case 'r': force_reordering = true; break;
    case 'i': {
      set_input_file(options, util_optarg);
      break;
    }
    case 'm': {
      partition_method = ALLOC(char, strlen(util_optarg)+1);
      strcpy(partition_method, util_optarg);
      break;
    }
    case 'h': return(UsageProcessModel());
    default:  return(UsageProcessModel());
    }
  }

  if (argc != util_optind) return(UsageProcessModel());

  if (get_input_file(options) == (char *)NULL) {
    fprintf(nusmv_stderr, "Input file is (null). You must set the input file before.\n");
    return 1;
  }

  if (partition_method != NIL(char)) {
    if (TransType_from_string(partition_method) != TRANS_TYPE_INVALID) {
      set_partition_method(options, TransType_from_string(partition_method));
    } else {
      fprintf(nusmv_stderr, "The only possible values for \"-m\" option are:\n\t");
      print_partition_method(nusmv_stderr);
      fprintf(nusmv_stderr, "\n");
      return 1;
    }
  }
  
  if (cmp_struct_get_read_model(cmps) == 0)
    if (Cmd_CommandExecute("read_model")) return 1;
  if (cmp_struct_get_flatten_hrc(cmps) == 0)
    if (Cmd_CommandExecute("flatten_hierarchy")) return 1;
  if (cmp_struct_get_encode_variables(cmps) == 0)
    if (Cmd_CommandExecute("encode_variables")) return 1;
  if (cmp_struct_get_build_model(cmps) == 0) {
    if (!force_build) { if(Cmd_CommandExecute("build_model")) return 1; }
    else if(Cmd_CommandExecute("build_model -f")) return 1;
  }
  if (opt_forward_search(options)) 
    if (Cmd_CommandExecute("compute_reachable")) return 1;

  if (opt_check_fsm(options)) 
    if (Cmd_CommandExecute("check_fsm")) return 1;

  if (! opt_ignore_spec(options))
    if (Cmd_CommandExecute("check_spec")) return 1;

  if (! opt_ignore_compute(options))
    if (Cmd_CommandExecute("compute")) return 1;

  if (! opt_ignore_ltlspec(options))
    if (Cmd_CommandExecute("check_ltlspec")) return 1;

  if (! opt_ignore_pslspec(options))
    if (Cmd_CommandExecute("check_pslspec")) return 1;

  if (! opt_ignore_invar(options))
    if (Cmd_CommandExecute("check_invar")) return 1;

  if (opt_verbose_level_gt(options, 0))
    if (Cmd_CommandExecute("print_usage")) return 1;

  if (force_reordering) { /* If the case activate reordering */
    fprintf(nusmv_stdout, "\n========= starting reordering ============\n");
    dd_reorder(dd_manager, get_reorder_method(options), DEFAULT_MINSIZE);

    if (Cmd_CommandExecute("write_order")) return 1;

    fprintf(nusmv_stdout, "\n========= after reordering ============\n");

    if (opt_verbose_level_gt(options, 0))
      if (Cmd_CommandExecute("print_usage")) return 1;
  }

  return 0;
}

static int UsageProcessModel()
{
  fprintf(nusmv_stderr, "usage: process_model [-r] [-h] [-i model-file] [-m method]\n");
  fprintf(nusmv_stderr, "   -h \t\t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, "   -f \t\t\tForces model construction.\n");
  fprintf(nusmv_stderr, "   -r \t\t\tForces a reordering and dumps the new vars order.\n");
  fprintf(nusmv_stderr, "   -i model-file \tReads the model from file \"model-file\".\n");
  fprintf(nusmv_stderr, "   -m method\t\tUses \"method\" as partition method in model construction.\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Flattens the hierarchy of modules]

  CommandName        [flatten_hierarchy]           

  CommandSynopsis    [Flattens the hierarchy of modules]  

  CommandArguments   [\[-h\]]  

  CommandDescription [
  This command is responsible of the instantiation of modules and
  processes. The instantiation is performed by substituting the actual
  parameters for the formal parameters, and then by prefixing the result via 
  the instance name.]  

  SideEffects        []

******************************************************************************/
int CommandFlattenHierarchy(int argc, char ** argv)
{
  int c;

  util_getopt_reset();
  while ((c = util_getopt(argc,argv,"h")) != EOF) {
    switch (c) {
    case 'h': return UsageFlattenHierarchy();
    default:  return UsageFlattenHierarchy();
    }
  }
  if (argc != util_optind) return UsageFlattenHierarchy();
  if (cmp_struct_get_read_model(cmps) == 0) {
    fprintf(nusmv_stderr, "A model must be read before. Use the \"read_model\" command.\n");
    return 1;
  }


  if (cmp_struct_get_flatten_hrc(cmps)) {
    fprintf(nusmv_stderr, "The hierarchy has already been flattened.\n");
    return 1;
  }
  if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, "Flattening hierarchy...\n");
  }

  /* Initializes the flattener, that must be initialized *after* the
     parsing phase */
  CompileFlatten_init_flattener(); 

  /* Processing of the parse tree and constructions of all the
    expressions for the state machine(s). Here the expansions are
    performed so that modules and processes are created. The expansion
    of modules is such that the formal parameters (if any) are
    replaced by the actual ones and the machine is replicated. 
  */

  /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
  if (opt_mbp_game(options)) {
#warning [AT] Check deinitialization within this function (see below)
    if (Mbp_CommandFlattenHierarchy()) goto flattening_failed;
  }
  else 
#endif
  {
    SymbTable_ptr st; 
    SymbLayer_ptr layer;
    int propErr;

    st = Compile_get_global_symb_table();
    layer = SymbTable_create_layer(st, MODEL_LAYER_NAME, 
				   SYMB_LAYER_POS_BOTTOM);
    
    nusmv_assert(FLAT_HIERARCHY(NULL) == mainFlatHierarchy);

    mainFlatHierarchy = Compile_FlattenHierarchy(st, layer, sym_intern("main"),
						 Nil, Nil, true);

    /* We store properties in the DB of properties */
    propErr = PropDb_fill(st, FlatHierarchy_get_spec(mainFlatHierarchy),
			  FlatHierarchy_get_compute(mainFlatHierarchy),
			  FlatHierarchy_get_ltlspec(mainFlatHierarchy),
			  FlatHierarchy_get_pslspec(mainFlatHierarchy),
			  FlatHierarchy_get_invarspec(mainFlatHierarchy));
    if (0 != propErr) {
      /* cleans up the initialized internal structures */
      FlatHierarchy_destroy(mainFlatHierarchy);
      mainFlatHierarchy = FLAT_HIERARCHY(NULL);
      SymbTable_remove_layer(st, layer);
      goto flattening_failed; /* error */
    }
  }
  
  TraceManager_register_layer(TracePkg_get_global_trace_manager(), 
			      MODEL_LAYER_NAME);

  cmp_struct_set_flatten_hrc(cmps);
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "...done\n");
  }
  
  return 0;

  /* Exception handling */
 flattening_failed:
  PropDb_clean();
  CompileFlatten_quit_flattener();
  cmp_struct_unset_read_model(cmps); /* resets also the command read_model */
  return 1; /* error */
}


static int UsageFlattenHierarchy()
{
  fprintf(nusmv_stderr, "usage: flatten_hierarchy [-h]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Shows model's symbolic variables and their values]

  CommandName        [show_vars]

  CommandSynopsis    [Shows model's symbolic variables and their values]

  CommandArguments   [\[-h\] \[-s\] \[-i\] \[-m | -o output-file\]]

  CommandDescription [
  Prints symbolic input and state variables of the model with their range of
  values (as defined in the input file).
  <p>
  Command Options:<p>
  <dl>
    <dt> <tt>-s</tt>
       <dd> Prints only state variables.
    <dt> <tt>-i</tt>
       <dd> Prints only input variables.
    <dt> <tt>-m</tt>
       <dd> Pipes the output to the program specified by the
           <tt>PAGER</tt> shell variable if defined, else through the
           <tt>UNIX</tt> command "more".
    <dt> <tt>-o output-file</tt>
       <dd> Writes the output generated by the command to <tt>output-file</tt>
  </dl> ]

  SideEffects        []

******************************************************************************/
int CommandShowVars(int argc, char ** argv)
{
  int c = 0;
  boolean statevars = false;
  boolean inputvars = false;
  short int useMore = 0;
  char* dbgFileName = NIL(char);
  FILE* old_nusmv_stdout = NIL(FILE);

  util_getopt_reset();
  while ((c = util_getopt(argc,argv,"hsimo:")) != EOF){

    switch (c) {
    case 'h': return UsageShowVars();
    case 's':
      statevars = true;
      break;

    case 'i':
      inputvars = true;
      break;

    case 'o':
      if (useMore == 1) return UsageShowVars();
      dbgFileName = util_strsav(util_optarg);
      fprintf(nusmv_stdout, "Output to file: %s\n", dbgFileName);
      break;

    case 'm':
      if (dbgFileName != NIL(char)) return UsageShowVars();
      useMore = 1;
      break;

    default: return UsageShowVars();
    }
  }

  if (argc != util_optind) return UsageShowVars();
  if (!statevars && !inputvars) return UsageShowVars();

  /* we need only a flattened hierarchy to be able to see the variables */
  if (Compile_check_if_flattening_was_built(nusmv_stderr)) return 1;

  if (useMore) {
    old_nusmv_stdout = nusmv_stdout; 
    nusmv_stdout = CmdOpenPipe(useMore);
    if (nusmv_stdout==(FILE*) NULL) {nusmv_stdout=old_nusmv_stdout; return 1;}
  }

  if (dbgFileName != NIL(char)) {
    old_nusmv_stdout = nusmv_stdout;
    nusmv_stdout = CmdOpenFile(dbgFileName);
    if (nusmv_stdout==(FILE*) NULL) {nusmv_stdout = old_nusmv_stdout; return 1;}
  }
    
  {
    SymbTable_ptr st = Compile_get_global_symb_table();
    SymbLayer_ptr model_layer = SymbTable_get_layer(st, MODEL_LAYER_NAME);
    NodeList_ptr list = NODE_LIST(NULL);
    ListIter_ptr iter;
    
    SYMB_LAYER_CHECK_INSTANCE(model_layer);

    if (statevars && inputvars) list = SymbLayer_get_all_vars(model_layer);
    else if (inputvars) list = SymbLayer_get_input_vars(model_layer);
    else list = SymbLayer_get_state_vars(model_layer); 

    set_indent_size(1);
    iter = NodeList_get_first_iter(list);
    while (! ListIter_is_end(iter)) {
      node_ptr name = NodeList_get_elem_at(list, iter);

      if (SymbTable_is_symbol_var(st, name)) {
        print_node(nusmv_stdout, name);
        fprintf(nusmv_stdout, " : ");
        SymbType_print(nusmv_stdout, SymbTable_get_var_type(st, name));
        fprintf(nusmv_stdout, "\n");
      }

      iter = ListIter_get_next(iter);
    } /* loop */
    reset_indent_size();
  }

  if (useMore) {
    CmdClosePipe(nusmv_stdout);
    nusmv_stdout = old_nusmv_stdout;
  }

  if (dbgFileName != NIL(char)) {
    CmdCloseFile(nusmv_stdout);
    nusmv_stdout = old_nusmv_stdout;
  }
  
  return 0;
}


static int UsageShowVars () {
  fprintf(nusmv_stderr,"usage: show_vars [-h] [-s] [-i] [-m | -o file]\n");
  fprintf(nusmv_stderr,"  -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr,"  -s \t\tPrints only the state variables.\n");
  fprintf(nusmv_stderr,"  -i \t\tPrints only the input variables.\n");
  fprintf(nusmv_stderr,"  -m \t\tPipes output through the program specified by the \"PAGER\"\n");
  fprintf(nusmv_stderr,"     \t\tenvironment variable if defined, else through the UNIX command \"more\".\n");
  fprintf(nusmv_stderr,"  -o file\tWrites the generated output to \"file\".\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Builds the BDD variables necessary to compile the
  model into BDD.]

  CommandName        [encode_variables]

  CommandSynopsis    [Builds the BDD variables necessary to compile the
  model into BDD.]  

  CommandArguments   [\[-h\] \[-i order-file\]]  

  CommandDescription [
  Generates the boolean BDD variables and the ADD needed to encode
  propositionally the (symbolic) variables declared in the model.<br>

  The variables are created as default in the order in which they
  appear in a depth first traversal of the hierarchy.<p>

  The input order file can be partial and can contain variables not
  declared in the model. Variables not declared in the model are
  simply discarded. Variables declared in the model which are not
  listed in the ordering input file will be created and appended at the 
  end of the given ordering list, according to the default ordering.<p>

  Command options:<p>
  <dl>
    <dt> <tt>-i order-file</tt>
       <dd> Sets the environment variable <tt>input_order_file</tt> to
       <tt>order-file</tt>, and reads the variable ordering to be used from
       file <tt>order-file</tt>. This can be combined with the
       <tt>write_order</tt> command. The variable ordering is written to a 
       file, which can be inspected and reordered by the user, and then 
       read back in.
  </dl>]  

  SideEffects        []

******************************************************************************/
int CommandEncodeVariables(int argc, char ** argv)
{
  int c;
  char* input_order_file_name = NIL(char);

  util_getopt_reset();
  while ((c = util_getopt(argc,argv,"i:h")) != EOF) {
    switch (c) {
    case 'i': 
      input_order_file_name = ALLOC(char, strlen(util_optarg)+1);
      strcpy(input_order_file_name, util_optarg);
      break;
      
    case 'h': return UsageEncodeVariables();
    default:  return UsageEncodeVariables();
    }
  }

  if (argc != util_optind) return UsageEncodeVariables();

  /* pre-conditions: */
  if (Compile_check_if_flattening_was_built(nusmv_stderr)) return 1;

  if (cmp_struct_get_encode_variables(cmps)) {
    fprintf(nusmv_stderr, "The variables appear to be already built.\n");
    return 1;
  }

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Building variables...");
  }

  if (input_order_file_name != NIL(char)) {
    set_input_order_file(options, input_order_file_name);
  }

  {

  /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
  if (opt_mbp_game(options)) {
    if (Mbp_CommandEncodeVariables()) return 1;
  }
  else 
#endif
    {
      BoolEnc_ptr bool_enc;
      BddEnc_ptr bdd_enc;
      
      /* Creates the bool encoding, and commit the model layer, that was
	 created during the flattening phase */
      Enc_init_bool_encoding();
      bool_enc = Enc_get_bool_encoding();
      BaseEnc_commit_layer(BASE_ENC(bool_enc), MODEL_LAYER_NAME);
      
      /* Creates the bdd encoding, and again commit the model layer */
      Enc_init_bdd_encoding();
      bdd_enc = Enc_get_bdd_encoding();
      BaseEnc_commit_layer(BASE_ENC(bdd_enc), MODEL_LAYER_NAME);
    }
  }
    
  cmp_struct_set_encode_variables(cmps);
    
  if (!opt_reorder(options) 
      && !is_default_order_file(options)
      && !util_is_string_null(get_output_order_file(options))) {
    VarOrderingType dump_type;
    if (opt_write_order_dumps_bits(options)) dump_type = DUMP_BITS;
    else dump_type = DUMP_DEFAULT;

    BddEnc_write_var_ordering(Enc_get_bdd_encoding(), 
			      get_output_order_file(options), 
			      dump_type);
    
    /* batch mode: */
    if (opt_batch(options)) { nusmv_exit(0); }
  }

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "...done\n");
  }

  return 0;
}


static int UsageEncodeVariables()
{
  fprintf(nusmv_stderr, "usage: encode_variables [-h] [-i <file>]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, "   -i <file> \tReads variable ordering from file <file>.\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Compiles the flattened hierarchy into BDD]

  CommandName        [build_model]         

  CommandSynopsis    [Compiles the flattened hierarchy into BDD]  

  CommandArguments   [\[-h\] \[-f\] \[-m Method\]]  

  CommandDescription [
  Compiles the flattened hierarchy into BDD (initial states, invariants,
  and transition relation) using the method specified in the environment
  variable <tt>partition_method</tt> for building the transition relation.<p>

  Command options:<p>
  <dl>
    <dt> <tt>-m Method</tt>
       <dd> Sets the environment variable <tt>partition_method</tt> to
           the value <tt>Method</tt>, and then builds the transition
           relation. Available methods are <code>Monolithic</code>,
           <code>Threshold</code> and <code>Iwls95CP</code>.
    <dt> <tt>-f</tt>
       <dd> Forces model construction. By default, only one partition
            method is allowed. This option allows to overcome this
            default, and to build the transition relation with different
            partitioning methods.
  </dl>]  

  SideEffects        []

******************************************************************************/
int CommandBuildModel(int argc, char ** argv)
{
  int c;
  boolean force_build = false;
  char * partition_method = NIL(char);

  util_getopt_reset();
  while((c = util_getopt(argc,argv,"m:fh")) != EOF){
    switch(c){
    case 'm': {
      partition_method = ALLOC(char, strlen(util_optarg)+1);
      strcpy(partition_method, util_optarg);
      break;
    }
    case 'f': {
      force_build = true;
      break;
    }
    case 'h': return(UsageBuildModel());
    default:  return(UsageBuildModel());
    }
  }
  if (argc != util_optind) return(UsageBuildModel());

  /* pre-conditions: */
  if (Compile_check_if_encoding_was_built(nusmv_stderr)) return 1;

  if (!force_build && cmp_struct_get_build_model(cmps)) {
    fprintf(nusmv_stderr, "A model appears to be already built from file: %s.\n",
            get_input_file(options));
    return 1;
  }

  if (partition_method != NIL(char)) {
    if (TransType_from_string(partition_method) != TRANS_TYPE_INVALID) {
      if ((force_build) &&
          (TransType_from_string(partition_method) == get_partition_method(options))) {
        if (cmp_struct_get_build_model(cmps)) {
          fprintf(nusmv_stderr, "A model for the chosen method has already been constructed.\n");
          return 1;
        }
      }
      set_partition_method(options, TransType_from_string(partition_method));
    } else {
      fprintf(nusmv_stderr, "The only possible values for \"-m\" option are:\n\t");
      print_partition_method(nusmv_stderr);
      fprintf(nusmv_stderr, "\n");
      return 1;
    }
  }
  

  /* constructs the model only if coi is not enabled */
  if (opt_cone_of_influence(options) && !force_build) {
    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, 
              "Construction of BDD model is delayed due to use of COI\n");
    }
    return 0;
  }
  
  {
    /* creates the model only if required (i.e. build_flat_model not called) */
    compile_create_flat_model();

    /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
    if (opt_mbp_game(options)) {
      Mbp_CommandBuildBddModel();
    }
    else 
#endif
      {
        SexpFsm_ptr sexp_fsm;
        BddFsm_ptr  bdd_fsm;

        if (opt_verbose_level_gt(options, 1)) {
          fprintf(nusmv_stderr, "\nCreating the BDD FSM...\n");
        }

        sexp_fsm = Prop_master_get_scalar_sexp_fsm();
      
        bdd_fsm = FsmBuilder_create_bdd_fsm(global_fsm_builder, 
                                            Enc_get_bdd_encoding(), 
                                            sexp_fsm,
                                            get_partition_method(options));

        /* Finally stores the built FSMs: */
        if (Prop_master_get_bdd_fsm() != BDD_FSM(NULL)) {
          nusmv_assert(force_build);

          if (opt_verbose_level_gt(options, 1)) {
            fprintf(nusmv_stderr, "Forcing destruction of previoulsy created BDD FSM...\n");
          }

          /* destroys the previously created fsm, but first reuse cache */
          BddFsm_copy_cache(bdd_fsm, Prop_master_get_bdd_fsm(), 
                            true /*keeps family*/);

          /* this will destroy the current fsm */
          Prop_master_set_bdd_fsm(BDD_FSM(NULL));
        }

        /* Finally stores the built FSMs: */
        Prop_master_set_bdd_fsm(bdd_fsm);

        if (opt_verbose_level_gt(options, 1)) {
          fprintf(nusmv_stderr, "Successfully created the BDD FSM\n");
        }
      }
  }

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr,
            "\nThe model has been built from file %s.\n", get_input_file(options));
  }
  /* We keep track that the master FSM has been built. */
  cmp_struct_set_build_model(cmps);
  return 0;
}

static int UsageBuildModel()
{
  fprintf(nusmv_stderr, "usage: build_model [-h] [-f] [-m Method]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage\n");
  fprintf(nusmv_stderr, "   -m Method \tUses \"Method\" as partitioning method, and set it as default method\n");
  fprintf(nusmv_stderr, "\t\tto be used in the following image computations.\n");
  fprintf(nusmv_stderr, "\t\tThe currently available methods are:\n\t\t");
  print_partition_method(nusmv_stderr);
  fprintf(nusmv_stderr, "\n   -f \t\tForces the model re-construction, even if a model has already been built\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Compiles the flattened hierarchy into SEXP]

  CommandName        [build_flat_model]            

  CommandSynopsis    [Compiles the flattened hierarchy into SEXP]  

  CommandArguments   [\[-h\]]  

  CommandDescription [
  Compiles the flattened hierarchy into SEXP (initial states, invariants,
  and transition relation).<p>]  

  SideEffects        []

******************************************************************************/
int CommandBuildFlatModel(int argc, char ** argv)
{
  int c;

  util_getopt_reset();
  while((c = util_getopt(argc,argv,"h")) != EOF){
    switch(c){
    case 'h': return(UsageBuildFlatModel());
    default:  return(UsageBuildFlatModel());
    }
  }
  if (argc != util_optind) return(UsageBuildFlatModel());

  /* pre-conditions: */
  if (Compile_check_if_encoding_was_built(nusmv_stderr)) return 1;

  if (cmp_struct_get_build_flat_model(cmps)) {
    fprintf(nusmv_stderr, "A model appears to be already built from file: %s.\n",
            get_input_file(options));
    return 1;
  }
  
  /* does the work: */
  compile_create_flat_model();

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr,
            "\nThe sexp model has been built from file %s.\n", 
	    get_input_file(options));
  }

  return 0;
}

static int UsageBuildFlatModel()
{
  fprintf(nusmv_stderr, "usage: build_flat_model [-h]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Compiles the flattened hierarchy into boolean SEXP]

  CommandName        [build_boolean_model]         

  CommandSynopsis    [Compiles the flattened hierarchy into boolean SEXP]  

  CommandArguments   [\[-h\] \[-f\]]  

  CommandDescription [
  Compiles the flattened hierarchy into boolean SEXP 
  (initial states, invariants, and transition relation).<p>]  

  SideEffects        []

******************************************************************************/
int CommandBuildBooleanModel(int argc, char ** argv)
{
  int c;
  boolean forced = false;

  util_getopt_reset();
  while((c = util_getopt(argc,argv,"hf")) != EOF){
    switch(c){
    case 'h': return(UsageBuildBooleanModel());
    case 'f': forced = true; break;
    default:  return(UsageBuildBooleanModel());
    }
  }
  if (argc != util_optind) return(UsageBuildBooleanModel());

  /* pre-conditions: */
  if (Compile_check_if_encoding_was_built(nusmv_stderr)) return 1;

  if (cmp_struct_get_build_bool_model(cmps) && !forced) {
    fprintf(nusmv_stderr, "A model appears to be already built from file: %s.\n",
            get_input_file(options));
    return 1;
  }
  
  /* constructs the model only if coi is not enabled */
  if (opt_cone_of_influence(options) && !forced) {
    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, 
	      "Construction of boolean model is delayed due to use of COI\n");
    }
    return 0;
  }

  /* creates the flat fsm */
  compile_create_flat_model();
  
  /* creates the boolean fsm */
  compile_create_boolean_model();

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr,
            "\nThe boolean sexp model has been built from file %s.\n", 
	    get_input_file(options));
  }

  /* We keep track that the master FSM has been built. */
  cmp_struct_set_build_bool_model(cmps);
  return 0;
}

static int UsageBuildBooleanModel()
{
  fprintf(nusmv_stderr, "usage: build_boolean_model [-h][-f]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, "   -f \t\tForces the boolean model construction.\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Writes variable order to file.]

  CommandName        [write_order]         

  CommandSynopsis    [Writes variable order to file.]  

  CommandArguments   [\[-h\] \[-b\] \[(-o | -f) order-file\]]  

  CommandDescription [Writes the current order of BDD variables in the
  file specified via the -o option. If no option is specified the environment 
  variable <tt>output_order_file</tt> will be considered. If the variable 
  <tt>output_order_file</tt> is unset (or set to an empty value) then standard 
  output will be used. The option <tt>-b</tt> forces the dumped 
  variable ordering to contain only boolean variables. 
  All the scalar variables will be substituted by those variables bits
  that encode them.  The variables bits will occur within the dumped
  variable ordering depending on the position they have within the
  system when the command is executed. 
  <p>
  Command options:<p>
  <dl>
    <dt> <tt>-b</tt> 
       <dd> Dumps bits of scalar variables instead of the single
       scalar variables. When specified, this option temporary
       overloads the current value of the system variable
       <tt>write_order_dumps_bits</tt>.

    <dt> <tt>-o order-file</tt>
       <dd> Sets the environment variable <tt>output_order_file</tt>
       to <tt>order-file</tt> and then dumps the ordering list into that file.
    <dt> <tt>-f order-file</tt>
       <dd> Alias for -o option. Supplied for backward compatibility. 
  </dl>]  

  SideEffects        []

******************************************************************************/
int CommandWriteOrder(int argc, char **argv)
{
  int c;
  char* order_output_fname = NIL(char);
  VarOrderingType dump_type;

  if (opt_write_order_dumps_bits(options)) dump_type = DUMP_BITS;
  else dump_type = DUMP_DEFAULT;

  /*
   * Parse the command line.
  */
  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "o:f:hb")) != EOF) {
    switch (c) {
    case 'h':
      if (order_output_fname != NIL(char)) FREE(order_output_fname); 
      return(UsageWriteOrder());

    case 'b':
      dump_type = DUMP_BITS;
      break;

    case 'o':
    case 'f':
      if (order_output_fname != NIL(char)) {
        /* already called (via the alias): exit */
        FREE(order_output_fname);
        return UsageWriteOrder();
      } 
      order_output_fname = ALLOC(char, strlen(util_optarg)+1);
      nusmv_assert(order_output_fname);
      strcpy(order_output_fname, util_optarg);
      break;

    default:
      if (order_output_fname != NIL(char)) FREE(order_output_fname);
      return(UsageWriteOrder());
    }
  }
  
  /* side effect on variable output_order_file: */
  if (order_output_fname != NIL(char)) { 
    set_output_order_file(options, order_output_fname);
    FREE(order_output_fname);
  }

  if (dd_manager == NIL(DdManager)) {
    fprintf(nusmv_stderr, "The DD Manager has not been created yet.\n");
    return 1;
  }

  /* pre-conditions: */
  if (Compile_check_if_encoding_was_built(nusmv_stderr)) return 1;

  BddEnc_write_var_ordering(Enc_get_bdd_encoding(), 
			    get_output_order_file(options), 
			    dump_type);
  
  /* batch mode: */
  if (opt_batch(options) && !opt_reorder(options))  { nusmv_exit(0); }

  return 0;
}

static int UsageWriteOrder()
{
  fprintf(nusmv_stderr, "usage: write_order [-h] | [-b] [(-o | -f) <file>]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, 
	  "   -b \t\tDumps bits of scalar variables instead of the single \n"\
	  "      \t\tscalar variables. \n"\
	  "      \t\tSee also the system variable write_order_dumps_bits.\n");
  fprintf(nusmv_stderr, "   -o <file>\tWrites ordering to file <file>.\n");
  fprintf(nusmv_stderr, "   -f <file>\tThe same of option -o. Supplied for backward compatibility.\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Prints out information about the clustering. 
  ** DEPRECATED in 2.4 **]

  CommandName        [print_clusterinfo]

  CommandSynopsis [Prints out  information about the
  clustering. This command is *deprecated* in 2.4]

  CommandArguments   [\[-h\] \| \[-m\] \| \[-o output-file\]]

  CommandDescription [Deprecated in 2.4: use print_fsm_stats instead. 

  This command prints out information
  regarding each cluster. In particular for each cluster it prints
  out the cluster number, the size of the cluster (in BDD nodes), the
  variables occurring in it, the size of the cube that has to be
  quantified out relative to the cluster and the variables to be
  quantified out.<p>

  Command options:<p>
  <dl>
    <dt> <tt>-m</tt>
       <dd> Pipes the output generated by the command through the
            program specified by the <tt>PAGER</tt> shell variable if
            defined, or through the UNIX utility "more".
    <dt> <tt>-o output-file</tt>
       <dd> Redirects the generated output to the file
            <tt>output-file</tt>.
  </dl>          
  ]

  SideEffects        []

******************************************************************************/
int CommandCPPrintClusterInfo(int argc, char ** argv)
{
  int res; 

  fprintf(nusmv_stderr, "----------------------------------------------------------------------\n");
  fprintf(nusmv_stderr, "--             Deprecated in 2.4: use 'print_fsm_stats'             --\n");
  fprintf(nusmv_stderr, "----------------------------------------------------------------------\n");

  res = CommandPrintFsmStats(argc, argv);

  fprintf(nusmv_stderr, "----------------------------------------------------------------------\n");
  fprintf(nusmv_stderr, "--             Deprecated in 2.4: use 'print_fsm_stats'             --\n");
  fprintf(nusmv_stderr, "----------------------------------------------------------------------\n");

  return res;
}


/**Function********************************************************************

  Synopsis           [Prints out information about the fsm and clustering.]

  CommandName        [print_fsm_stats]

  CommandSynopsis    [Prints out information about the fsm and clustering.]  

  CommandArguments   [\[-h\] \| \[-m\] \| \[-o output-file\]]

  CommandDescription [This command prints out information
  regarding the fsm and each cluster. In particular for each cluster
  it prints out the cluster number, the size of the cluster (in BDD
  nodes), the variables occurring in it, the size of the cube that has
  to be quantified out relative to the cluster and the variables to be
  quantified out.<p>

  Command options:<p>
  <dl>
    <dt> <tt>-m</tt>
       <dd> Pipes the output generated by the command through the
            program specified by the <tt>PAGER</tt> shell variable if
            defined, or through the UNIX utility "more".
    <dt> <tt>-o output-file</tt>
       <dd> Redirects the generated output to the file
            <tt>output-file</tt>.
  </dl>          
  ]

  SideEffects        []

******************************************************************************/
int CommandPrintFsmStats(int argc, char ** argv)
{
  int c;
  int useMore = 0;
  char* dbgFileName = NIL(char);
  FILE* old_nusmv_stdout = NIL(FILE);

  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "hmo")) != EOF) {
    switch (c) {
    case 'h':
      return UsagePrintFsmStats();
    case 'o':
      if (useMore == 1) return UsagePrintFsmStats();
      dbgFileName = util_strsav(util_optarg);
      fprintf(nusmv_stdout, "Output to file: %s\n", dbgFileName);
      break;
    case 'm':
      if (dbgFileName != NIL(char)) return UsagePrintFsmStats();
      useMore = 1;
      break;
    default:
      return UsagePrintFsmStats();
    }
  } /* while */

  if (useMore) {
    old_nusmv_stdout = nusmv_stdout; 
    nusmv_stdout = CmdOpenPipe(useMore);
    if (nusmv_stdout==(FILE*) NULL) {nusmv_stdout=old_nusmv_stdout; return 1;}
  }
  if (dbgFileName != NIL(char)) {
    old_nusmv_stdout = nusmv_stdout;
    nusmv_stdout = CmdOpenFile(dbgFileName);
    if (nusmv_stdout==(FILE*) NULL) {nusmv_stdout = old_nusmv_stdout; return 1;}
  }

  {
    BddFsm_ptr fsm = Prop_master_get_bdd_fsm();
    
    if (fsm != BDD_FSM(NULL)) {
      BddFsm_print_info(fsm, nusmv_stdout);
    }
  }

  if (useMore) {
    CmdClosePipe(nusmv_stdout);
    nusmv_stdout = old_nusmv_stdout;
  }
  if (dbgFileName != NIL(char)) {
    CmdCloseFile(nusmv_stdout);
    nusmv_stdout = old_nusmv_stdout;
  }

  return 0;
}

static int UsagePrintFsmStats()
{
  fprintf(nusmv_stderr, "usage: print_fsm_stats [-h] [-m] [-o file]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage\n");
  fprintf(nusmv_stderr, "   -m \t\tPipes output through the program specified by\n");
  fprintf(nusmv_stderr, "      \t\tthe \"PAGER\" shell variable if defined,\n");
  fprintf(nusmv_stderr, "      \t\t else through the UNIX command \"more\"\n");
  fprintf(nusmv_stderr, "   -o file\tWrites the output to \"file\".\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Prints the Iwls95 Options.]

  CommandName        [print_iwls95options]         

  CommandSynopsis    [Prints the Iwls95 Options.]  

  CommandArguments   [\[-h\]]  

  CommandDescription [This command prints out the configuration
  parameters of the IWLS95 clustering algorithm, i.e.
  <tt>image_verbosity</tt>, <tt>image_cluster_size</tt> and
  <tt>image_W{1,2,3,4}</tt>.]

  SideEffects        []

******************************************************************************/
int CommandIwls95PrintOption(int argc, char ** argv)
{
  int c;
  ClusterOptions_ptr opts;

  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "h")) != EOF) {
    switch (c) {
    case 'h':
      return(UsageIwls95PrintOption());
    default:
      return(UsageIwls95PrintOption());
    }
  } /* while */
  
  opts = ClusterOptions_create(options);
  ClusterOptions_print(opts, nusmv_stdout);
  ClusterOptions_destroy(opts);
  
  return 0;
}

static int UsageIwls95PrintOption()
{
  fprintf(nusmv_stderr, "usage: print_iwls95options [-h]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Implements the go command]

  CommandName        [go]

  CommandSynopsis    [Initializes the system for the verification.]

  CommandArguments   [\[-h\]\[-f\]]

  CommandDescription [This command initializes the system for
  verification. It is equivalent to the command sequence
  <tt>read_model</tt>, <tt>flatten_hierarchy</tt>,
  <tt>encode_variables</tt>, <tt>build_model</tt>,
  <tt>build_flat_model</tt>, <tt>build_boolean_model</tt>. 
  If some commands have already been
  executed, then only the remaining ones will be invoked.<p>
  Command options:<p>  
  <dl><dt> -f
  <dd> Forces the model contruction.<p> 
  <dt> -h
  <dd> Prints the command usage.<p> 
  </dl>
  ]

  SideEffects        []

******************************************************************************/
int CommandGo(int argc, char ** argv)
{
  int c;
  boolean forced = false;

  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "hf")) != EOF) {
    switch (c) {
    case 'h': return UsageGo();
    case 'f': forced = true; break;
    default: return UsageGo();
    }
  } /* while */
  
  if (cmp_struct_get_read_model(cmps) == 0)
    if (Cmd_CommandExecute("read_model")) return 1;
  if (cmp_struct_get_flatten_hrc(cmps) == 0)
    if (Cmd_CommandExecute("flatten_hierarchy")) return 1;
  if (cmp_struct_get_encode_variables(cmps) == 0)
    if (Cmd_CommandExecute("encode_variables")) return 1;
  if (cmp_struct_get_build_flat_model(cmps) == 0)
    if(Cmd_CommandExecute("build_flat_model")) return 1;
  if (cmp_struct_get_build_model(cmps) == 0) {
    if (!forced) { if (Cmd_CommandExecute("build_model")) return 1; }
    else if (Cmd_CommandExecute("build_model -f")) return 1;
  }
  return 0;
}

static int UsageGo()
{
  fprintf(nusmv_stderr, "usage: go [-h]\n");
  fprintf(nusmv_stderr, "   -f \t\tForces the model construction\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Implements the go_bmc command]

  CommandName        [go_bmc]

  CommandSynopsis    [Initializes the system for the BMC verification.]

  CommandArguments   [\[-h\]]

  CommandDescription [This command initializes the system for
  verification. It is equivalent to the command sequence
  <tt>read_model</tt>, <tt>flatten_hierarchy</tt>,
  <tt>encode_variables</tt>, <tt>build_boolean_model</tt>, <tt>bmc_setup</tt>. 
  If some commands have already been
  executed, then only the remaining ones will be invoked.<p>
  Command options:<p>  
  <dl>
  <dt> -f
  <dd> Forces the model construction.<p> 
  <dt> -h
  <dd> Prints the command usage.<p> 
  </dl>
  ]

  SideEffects        []

******************************************************************************/
int CommandGoBmc(int argc, char ** argv)
{
  int c;
  boolean forced = false;

  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "hf")) != EOF) {
    switch (c) {
    case 'h': return(UsageGoBmc());
    case 'f': forced = true; break;
    default: return(UsageGoBmc());
    }
  } /* while */
  
  if (cmp_struct_get_read_model(cmps) == 0)
    if (Cmd_CommandExecute("read_model")) return 1;
  if (cmp_struct_get_flatten_hrc(cmps) == 0)
    if (Cmd_CommandExecute("flatten_hierarchy")) return 1;
  if (cmp_struct_get_encode_variables(cmps) == 0)
    if (Cmd_CommandExecute("encode_variables")) return 1;
  if (cmp_struct_get_build_bool_model(cmps) == 0) {
    if (!forced) { if (Cmd_CommandExecute("build_boolean_model")) return 1; }
    else if (Cmd_CommandExecute("build_boolean_model -f")) return 1;
  }
  if (cmp_struct_get_bmc_setup(cmps) == 0) {
    if (!forced) { if (Cmd_CommandExecute("bmc_setup")) return 1; }
    else if (Cmd_CommandExecute("bmc_setup -f")) return 1;
  }

  return 0;
}

static int UsageGoBmc()
{
  fprintf(nusmv_stderr, "usage: go_bmc [-h][-f]\n");
  fprintf(nusmv_stderr, "   -f \t\tForces the model contruction\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Implements the get_internal_status command]

  CommandName        [get_internal_status]

  CommandSynopsis    [Returns the internal status of the system.]

  CommandArguments   [\[-h\]]

  CommandDescription [Prints out the internal status of the system. i.e.
  <ul>
  <li> -1 : read_model has not yet been executed or an error occurred
            during its execution. </li>
  <li>  0 : flatten_hierarchy has not yet been executed or an error
            occurred during its execution. </li>
  <li>  1 : encode_variables has not yet been executed or an error
            occurred during its execution. </li>
  <li>  2 : build_model has not yet been executed or an error occurred
            during its execution. </li>
  </ul>
  Command options:<p>  
  <dl><dt> -h
  <dd> Prints the command usage.<p> 
  </dl>
  ]

  SideEffects        []

******************************************************************************/
int CommandGetInternalStatus(int argc, char ** argv)
{
  int c;

  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "h")) != EOF) {
    switch (c) {
    case 'h':
      return(UsageGetInternalStatus());
    default:
      return(UsageGetInternalStatus());
    }
  } /* while */
  
  if (cmp_struct_get_read_model(cmps) == 0){
    fprintf(nusmv_stderr, "The internal status is: -1\n");
    return 0;
  }
  if (cmp_struct_get_flatten_hrc(cmps) == 0){
    fprintf(nusmv_stderr, "The internal status is: 0\n");
    return 0;
  }
  if (cmp_struct_get_encode_variables(cmps) == 0){
    fprintf(nusmv_stderr, "The internal status is: 1\n");
    return 0;
  }
  if (cmp_struct_get_build_model(cmps) == 0){
    fprintf(nusmv_stderr, "The internal status is: 2\n");
    return 0;
  }
  return 0;
}

static int UsageGetInternalStatus()
{
  fprintf(nusmv_stderr, "usage: get_internal_status [-h]\n");
  fprintf(nusmv_stderr, "   -h \t\tPrints the command usage.\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes the currently loaded SMV model in the
  specified file, after having flattened it]

  CommandName        [write_flat_model]            

  CommandSynopsis    [Writes a flat model of a given SMV file]  

  CommandArguments   [\[-h\] \[-o filename\]]  

  CommandDescription [Processes are eliminated
  and a corresponding equivalent model is printed out.
  If no file is specified, the file specified with the environment variable
  <tt>output_flatten_model_file</tt> is used if any, otherwise standard output
  is used as output.
  <p>
  Command options:<p>
  <dl>
     <dt><tt>-o filename</tt>
       <dd> Attempts to write the flat SMV model in <tt>filename</tt>.
  </dl>
  ]  

  SideEffects        []

******************************************************************************/
int CommandWriteModelFlat(int argc, char **argv)
{
  int c = 0;
  int rv = 0;
  char* output_file = NIL(char);
  FILE* ofileid = NIL(FILE);
  int bSpecifiedFilename = FALSE; 
  
  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "ho:")) != EOF) {
    switch (c) {
    case 'h': return UsageWriteModelFlat();
    case 'o':
      output_file = ALLOC(char, strlen(util_optarg)+1);
      nusmv_assert(output_file);
      strcpy(output_file, util_optarg);
      bSpecifiedFilename = TRUE; 
      break;

    default:
      break;
    }
  }

  if (argc != util_optind) return UsageWriteModelFlat();

  if (output_file == NIL(char)) {
    output_file = get_output_flatten_model_file(options);
  }
  if (output_file == NIL(char)) {
    ofileid = nusmv_stdout;
  } 
  else {
    ofileid = fopen(output_file, "w");
    if (ofileid == NULL) {
      fprintf(nusmv_stderr, "Unable to open file \"%s\".\n", output_file);
      if (bSpecifiedFilename == TRUE)  FREE(output_file);
      return 1;
    }
  }

  /* pre-conditions: */
  if (Compile_check_if_flattening_was_built(nusmv_stderr)) return 1;

  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Writing flat model into file \"%s\"..", 
	    output_file == (char *)NULL ? "stdout" : output_file);
  }

  CATCH {

    /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
    if (opt_mbp_game(options)) {
      Mbp_CommandWriteFlatModel(ofileid);
    }
    else 
#endif
      {
	SymbTable_ptr st = Compile_get_global_symb_table();

	const char* layers[] = {MODEL_LAYER_NAME, NULL}; 

	Compile_WriteFlattenFsm(ofileid, st,
				layers, "MODULE main", mainFlatHierarchy);
	Compile_WriteFlattenSpecs(ofileid, st,
				  FlatHierarchy_get_spec(mainFlatHierarchy),
				  FlatHierarchy_get_compute(mainFlatHierarchy),
				  FlatHierarchy_get_ltlspec(mainFlatHierarchy),
				  FlatHierarchy_get_invarspec(mainFlatHierarchy),
				  FlatHierarchy_get_pslspec(mainFlatHierarchy),
				  Nil, Nil, Nil, Nil, Nil, Nil);
      }

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, ".. done.\n");
    }
  } FAIL {
    rv = 1;
  }
  fflush(ofileid);

  if (ofileid != nusmv_stdout) {
    fclose(ofileid);
    if (bSpecifiedFilename) FREE(output_file);
  }
  return rv;
}

static int UsageWriteModelFlat(void)
{
  fprintf(nusmv_stderr, "usage: write_flat_model [-h] [-o filename]\n");
  fprintf(nusmv_stderr, "  -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, "  -o filename\tWrites output to \"filename\"\n");
  return 1;
}



/**Function********************************************************************

  Synopsis [Writes a flat and boolean model of a given SMV file]

  CommandName        [write_boolean_model]         

  CommandSynopsis    [Writes a flattened and booleanized model of a 
  given SMV file]  

  CommandArguments   [\[-h\] \[-o filename\]]  

  CommandDescription [Writes the currently loaded SMV model in the
  specified file, after having flattened and booleanized it. Processes
  are eliminated and a corresponding equivalent model is printed
  out. 

  If no file is specified, the file specified via the environment
  variable <tt>output_boolean_model_file</tt> is used if any,
  otherwise standard output is used.
  <p>
  Command options:<p>
  <dl>
     <dt><tt>-o filename</tt>
       <dd> Attempts to write the flat and boolean SMV model in 
       <tt>filename</tt>.
  </dl>

  ** New in 2.4.0 and later **
  Scalar variables are dumped as DEFINEs whose body is their boolean
  encoding.
  
  This allows the user to still express and see parts of the generated
  boolean model in terms of the original model's scalar variables
  names and values, and still keeping the generated model purely boolean. 

  Also, symbolic constants are dumped within a CONSTANTS statement to
  declare the values of the original scalar variables' for future
  reading of the generated file.]

  SideEffects        []

******************************************************************************/
int CommandWriteModelFlatBool(int argc, char **argv)
{
  int c = 0;
  int rv = 0;
  char* output_file = NIL(char);
  FILE* ofileid = NIL(FILE);
  int bSpecifiedFilename = FALSE; 
  
  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "ho:")) != EOF) {
    switch (c) {
    case 'h': return UsageWriteModelFlatBool();
    case 'o':
      output_file = ALLOC(char, strlen(util_optarg)+1);
      nusmv_assert(output_file);
      strcpy(output_file, util_optarg);
      bSpecifiedFilename = TRUE; 
      break;

    default:
      break;
    }
  }

  if (argc != util_optind) return UsageWriteModelFlatBool();

  if (output_file == NIL(char)) { 
    output_file = get_output_boolean_model_file(options);
  }

  if (output_file == NIL(char)) {
    ofileid = nusmv_stdout;
  } 
  else {
    ofileid = fopen(output_file, "w");
    if (ofileid == NULL) {
      fprintf(nusmv_stderr, "Unable to open file \"%s\".\n", output_file);
      if (bSpecifiedFilename == TRUE)  FREE(output_file);
      return 1;
    }
  }

  if (Compile_check_if_bool_model_was_built(nusmv_stderr, true)) return 1;


  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "Writing boolean model into file \"%s\"..", output_file == (char *)NULL ? "stdout" : output_file);
  }

  CATCH {
   /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
    if (opt_mbp_game(options)) {
      Mbp_CommandWriteBooleanModel(ofileid);
    }
    else 
#endif
      {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	SymbTable_ptr st = BaseEnc_get_symbol_table(BASE_ENC(enc));
	NodeList_ptr layers = BaseEnc_get_committed_layers(BASE_ENC(enc));
	
	Compile_WriteBoolFsm(ofileid, st, layers, "MODULE main",
			     Prop_master_get_bool_sexp_fsm());
	
	Compile_WriteBoolSpecs(ofileid, st, enc,
			       FlatHierarchy_get_spec(mainFlatHierarchy),
			       FlatHierarchy_get_compute(mainFlatHierarchy),
			       FlatHierarchy_get_ltlspec(mainFlatHierarchy),
			       FlatHierarchy_get_invarspec(mainFlatHierarchy),
			       FlatHierarchy_get_pslspec(mainFlatHierarchy),
			       Nil, Nil, Nil, Nil, Nil, Nil);

	if ((BddEnc_get_reordering_count(enc) > 0) && 
	    (get_output_order_file(options) != (char*) NULL) && 
	    (ofileid != nusmv_stdout)) { 
	  /* there was a reordering, so a variable ordering file is dumped */	 
	  BddEnc_write_var_ordering(enc, get_output_order_file(options), 
				    opt_write_order_dumps_bits(options) ? 
				    DUMP_BITS : DUMP_DEFAULT);

	  fprintf(nusmv_stderr, 
		  "%d reordering(s) occurred. Dumped variable ordering to '%s'\n", 
		  BddEnc_get_reordering_count(enc), 
		  get_output_order_file(options));
	  BddEnc_reset_reordering_count(enc);
	}

      }

    if (opt_verbose_level_gt(options, 0)) {
      fprintf(nusmv_stderr, ".. done.\n");
    }
  } FAIL {
    rv = 1;
  }
  fflush(ofileid);

  if (ofileid != nusmv_stdout) {
    fclose(ofileid);
    if (bSpecifiedFilename == TRUE)  FREE(output_file);
  }
  return rv;
}

static int UsageWriteModelFlatBool(void)
{
  fprintf(nusmv_stderr, "usage: write_boolean_model [-h] [-o filename]\n");
  fprintf(nusmv_stderr, "  -h \t\tPrints the command usage.\n");
  fprintf(nusmv_stderr, "  -o filename\tWrites output to \"filename\".\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [creates the  master scalar fsm if needed]

  Description        []

  SideEffects        []

******************************************************************************/
static void compile_create_flat_model()
{ 

  /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
  if (opt_mbp_game(options)) {
    Mbp_CommandBuildFlatModel();
  }
  else {
#endif
    
    SymbTable_ptr st;
    NodeList_ptr vars;
    SexpFsm_ptr sexp_fsm;

    if (Prop_master_get_scalar_sexp_fsm() != SEXP_FSM(NULL)) return;

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "\nCreating the scalar FSM...\n");
    }

    st = Compile_get_global_symb_table();

    vars = SymbLayer_get_all_vars(SymbTable_get_layer(st, MODEL_LAYER_NAME));

    sexp_fsm = FsmBuilder_create_sexp_fsm(global_fsm_builder,
					  Enc_get_bdd_encoding(), 
					  SYMB_LAYER(NULL), /* scalar fsm */
					  SYMB_LAYER(NULL), /* no inlining */
					  NodeList_to_node_ptr(vars));
    
    /* Builds the sexp FSM of the whole read model */
    Prop_master_set_scalar_sexp_fsm(sexp_fsm);

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "Successfully created the scalar FSM\n");
    }

#if HAVE_MBP
  }
#endif

  /* We keep track that the master FSM has been built. */
  cmp_struct_set_build_flat_model(cmps);
}


/**Function********************************************************************

  Synopsis  [Creates the  master boolean fsm if needed.
  A new layer called DETERM_LAYER_NAME
  will be added if the bool fsm is created.]

  Description [The newly created layer will be committed to both the
  boolean and bdd encodings. Warning: it is assumed here that the flat model 
  has been already created]

  SideEffects        []

******************************************************************************/
static void compile_create_boolean_model()
{
  /* If this is a game(realizability problem), invoke a specialized function */
#if HAVE_MBP
  if (opt_mbp_game(options)) {
    Mbp_CommandBuildBooleanModel();
  }
  else 
#endif

  if (Prop_master_get_bool_sexp_fsm() == SEXP_FSM(NULL)) {
    SexpFsm_ptr scalar_fsm;
    SexpFsm_ptr bool_fsm;
    SymbTable_ptr st;
    SymbLayer_ptr det_layer, inl_layer;

    int reord_status;
    dd_reorderingtype rt;
    DdManager* dd;

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "\nCreating the boolean FSM...\n");
    }

    /* temporary disables reordering */
    dd = BddEnc_get_dd_manager(Enc_get_bdd_encoding());
    reord_status = dd_reordering_status(dd, &rt);
    if (reord_status == 1) { dd_autodyn_disable(dd); }

    /* Creates a new layer for determinization vars */
    st = Compile_get_global_symb_table();

    det_layer = SymbTable_create_layer(st, DETERM_LAYER_NAME, 
				       SYMB_LAYER_POS_BOTTOM);

    inl_layer = SymbTable_create_layer(st, INLINING_LAYER_NAME, 
				       SYMB_LAYER_POS_BOTTOM);
    
    scalar_fsm = Prop_master_get_scalar_sexp_fsm();

    bool_fsm = SexpFsm_scalar_to_boolean(scalar_fsm, 
					 det_layer, inl_layer);

    Prop_master_set_bool_sexp_fsm(bool_fsm);

    /* Possibly added determinization variables introduced while
       building the boolean sexp model must be committed to the
       encodings */
    BaseEnc_commit_layer(BASE_ENC(Enc_get_bool_encoding()), 
			 DETERM_LAYER_NAME);
    BaseEnc_commit_layer(BASE_ENC(Enc_get_bdd_encoding()), 
			 DETERM_LAYER_NAME);

    /* Possibly added inlining variables introduced while
       building the boolean sexp model must be committed to the
       encodings */
    BaseEnc_commit_layer(BASE_ENC(Enc_get_bool_encoding()), 
			 INLINING_LAYER_NAME);
    BaseEnc_commit_layer(BASE_ENC(Enc_get_bdd_encoding()), 
			 INLINING_LAYER_NAME);
    
    /* If dynamic reordering was enabled, then it is re-enabled */
    if (reord_status == 1) { dd_autodyn_enable(dd, rt); }

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "Successfully created the boolean FSM\n");
    }

  }
}
