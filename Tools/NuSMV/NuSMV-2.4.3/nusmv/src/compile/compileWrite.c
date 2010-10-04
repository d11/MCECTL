/**CFile***********************************************************************

  FileName    [compileWrite.c]

  PackageName [compile]

  Synopsis [Creation of an SMV file containing the flattened or booleanized 
  model.]

  Description [Creation of an SMV file containing the flattened or
  booleanized model, processes will be removed by explicitly
  introducing a process variable and modifying assignments to take
  care of inertia.]

  SeeAlso     []

  Author      [Marco Roveri, Roberto Cavada]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2. 
  Copyright (C) 2004 by ITC-irst. 

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

#include "compileInt.h" 

#include "symb_table/SymbLayer.h"
#include "parser/symbols.h"
#include "parser/mbp_symbols.h"
#include "parser/psl/pslNode.h"
#include "opt/opt.h"

/*---------------------------------------------------------------------------*/
static char rcsid[] UTIL_UNUSED = "$Id: compileWrite.c,v 1.1.2.24.4.1 2007/04/04 12:00:32 nusmv Exp $";
/*---------------------------------------------------------------------------*/

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
static void print_assign ARGS((FILE *, node_ptr, node_ptr));

static int 
write_flatten_define ARGS((const SymbTable_ptr symb_table, FILE* out, 
			   const NodeList_ptr names));

static int 
write_flatten_assign ARGS((const SymbTable_ptr symb_table, FILE* out, 
			   const NodeList_ptr names,
			   FlatHierarchy_ptr hierarchy));

static int 
compile_write_flatten_vars ARGS((const SymbTable_ptr symb_table, 
				 FILE* out, NodeList_ptr vars));

static int 
compile_write_flatten_bool_vars ARGS((const SymbTable_ptr symb_table, 
				      const BoolEnc_ptr bool_enc, 
				      FILE* out, NodeList_ptr vars));


static int 
compile_write_flatten_expr_split ARGS((const SymbTable_ptr symb_table, 
                                       FILE* out, node_ptr l, char* s));

static int 
compile_write_flatten_expr ARGS((const SymbTable_ptr symb_table, 
                                 FILE* out, node_ptr l, char* s));

static int 
compile_write_flatten_expr_pair ARGS((const SymbTable_ptr symb_table, 
				      FILE* out, node_ptr l, char* s));

static int 
compile_write_flatten_bfexpr ARGS((BddEnc_ptr enc, 
				   const SymbTable_ptr symb_table, 
				   SymbLayer_ptr det_layer, 
				   FILE* out, node_ptr n, char* s));

static int compile_write_flatten_psl 
ARGS((const SymbTable_ptr symb_table, FILE* out, node_ptr n));

static int compile_write_constants
ARGS((const SymbTable_ptr symb_table, FILE* out));


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Prints the flatten version of FSM of an SMV model.]

  Description        [Prints on the specified file the flatten
  FSM of an SMV model, i.e. a list of all variable, defines, and all
  constrains (INIT, TRANS, INVAR, ASSIGNS, JUSTICE, COMPASSION).
  Specifications are NOT printed.
  
  layer_names is an array of names of layers terminated by NULL.
  fsm_name is a name of the output structure, usually it is "MODULE main".
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Compile_WriteFlattenFsm(FILE* out,
			    const SymbTable_ptr symb_table, 
			    const char** layer_names, 
			    const char* fsm_name,
			    FlatHierarchy_ptr hierarchy)
{
  const char** iter;

  nusmv_assert(layer_names != (const char**) NULL);

  fprintf(out, "-- Flattened FSM model generated from %s\n"
	  "-- Dumped layers are: ", get_input_file(options)); 
  /* dumps the layer names: */
  iter = layer_names;
  while (*iter != (const char*) NULL) {
    fprintf(out, "%s ", *iter);
    ++iter;
  }
  fprintf(out, "\n\n");

  fprintf(out, "%s\n", fsm_name);

  iter = layer_names;
  while (*iter != (const char*) NULL) {
    SymbLayer_ptr layer = SymbTable_get_layer(symb_table, *iter);
    fprintf(out, "-- Input variables from layer '%s'\n", *iter);
    compile_write_flatten_vars(symb_table, out, SymbLayer_get_input_vars(layer));
    ++iter;
  }

  iter = layer_names;
  while (*iter != (const char*) NULL) {
    SymbLayer_ptr layer = SymbTable_get_layer(symb_table, *iter);
    fprintf(out, "-- State variables from layer '%s'\n", *iter);
    compile_write_flatten_vars(symb_table, out, SymbLayer_get_state_vars(layer));
    ++iter;
  }

  /* DEFINEs */
  iter = layer_names;
  while (*iter != (const char*) NULL) {
    fprintf(out, "-- Defines from layer '%s'\n", *iter);
    write_flatten_define(symb_table, out, 
	 SymbLayer_get_defines(SymbTable_get_layer(symb_table, *iter)));
    ++iter;
  }

  /* CONSTANTS */
  if (!opt_backward_comp(options)) {
    if (compile_write_constants(symb_table, out)) fprintf(out, "\n");
  }

  /* ASSIGNs */
  iter = layer_names;
  while (*iter != (const char*) NULL) {
    fprintf(out, "-- Assignments from layer '%s'\n", *iter);
    write_flatten_assign(symb_table, out, 
			 SymbLayer_get_all_vars(SymbTable_get_layer(symb_table,
								    *iter)),
			 hierarchy);
    ++iter;
  }

  /* CONSTRAINS (except assigns) */
  if (compile_write_flatten_expr_split(symb_table, out,
                                       FlatHierarchy_get_init(hierarchy), 
                                       "INIT\n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out,
                                       FlatHierarchy_get_invar(hierarchy), 
                                       "INVAR\n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out,
                                       FlatHierarchy_get_trans(hierarchy), 
                                       "TRANS\n")) fprintf(out, "\n");

  {
    node_ptr fc = FlatHierarchy_get_justice(hierarchy);
    boolean are_there_compassion = (Nil != FlatHierarchy_get_compassion(hierarchy));

    while(Nil != fc) {
      if (compile_write_flatten_expr(symb_table, out,
                                     car(fc), 
                                     /* For backward compatibility */
                                     are_there_compassion ?
                                     "JUSTICE\n" : "FAIRNESS\n")) fprintf(out, "\n");
      fc = cdr(fc);
    }
  }

  if (compile_write_flatten_expr_pair(symb_table, out, 
				      FlatHierarchy_get_compassion(hierarchy),
				      "COMPASSION\n")) fprintf(out, "\n");
  return ;
}


/**Function********************************************************************

  Synopsis           [Prints the flatten specifications.]

  Description        [Prints into the specified file the flatten
  specifications. 
  layer_names is an array of names of layers terminated by NULL.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Compile_WriteFlattenSpecs(FILE* out,
			       const SymbTable_ptr symb_table, 
			       node_ptr spec,
			       node_ptr compute,
			       node_ptr ltlspec,
			       node_ptr invarspec,
			       node_ptr pslspec,
			       node_ptr reachtarget,
			       node_ptr avoidtarget,
			       node_ptr reachdeadlock,
			       node_ptr avoiddeadlock,
			       node_ptr buchigame,
			       node_ptr genreactivity)
{

  if (compile_write_flatten_expr_split(symb_table, out, spec, 
                                       "SPEC\n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, compute, 
                                       "COMPUTE\n")) fprintf(out, "\n");
  
  if (compile_write_flatten_expr_split(symb_table, out, ltlspec, 
                                       "LTLSPEC\n")) fprintf(out, "\n");
  
  if (compile_write_flatten_expr_split(symb_table, out, invarspec, 
                                       "INVARSPEC\n")) fprintf(out, "\n");

  if (pslspec != Nil) {
    fprintf(nusmv_stderr, "\n********   WARNING   ********\n");
    fprintf(nusmv_stderr,  
	    "This version does not support the flattening of PSL properties.\n"
	    "However, for user's convenience all the PSL properties will be dumped\n"
	    "as comments in the output file.\n");
    fprintf(nusmv_stderr, "******** END WARNING ********\n\n");    

    fprintf(out, 
	    "--- Dumping of PSL properties is not supported by this version of the system.\n"\
	    "--- However, the PSL properties had been dumped here for user's convenience,\n"\
	    "--- as the occurred in the original model. \n");
    compile_write_flatten_psl(symb_table, out, pslspec);
  }

  /* -- Game (realizability) properties -- */
  if (compile_write_flatten_expr_split(symb_table, out, reachtarget, 
                                       "REACHTARGET \n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, avoidtarget, 
                                       "AVOIDTARGET \n")) fprintf(out, "\n");
  
  if (compile_write_flatten_expr_split(symb_table, out, reachdeadlock, 
                                       "REACHDEADLOCK \n")) fprintf(out, "\n");
  
  if (compile_write_flatten_expr_split(symb_table, out, avoiddeadlock, 
                                       "AVOIDDEADLOCK \n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, buchigame, 
                                       "BUCHIGAME \n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, genreactivity, 
                                       "GENREACTIVITY \n")) fprintf(out, "\n");

  return;
}


/**Function********************************************************************

  Synopsis           [Prints the boolean FSM of an SMV model.]

  Description        [Prints into the specified file the boolean FSM of an
  SMV model. 
  bool_sexp_fsm should be a boolean Sexp FSM.
  layer_names is an array of layers whose variables will be printed,
  usually this parameter is a list of all layers committed to enc. The array
  should be ended by a NULL element.]
  
  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Compile_WriteBoolFsm(FILE* out,
			  const SymbTable_ptr symb_table,
			  NodeList_ptr layers,
			  const char* fsm_name,
			  SexpFsm_ptr bool_sexp_fsm)
{
  ListIter_ptr iter;

  nusmv_assert(SexpFsm_is_boolean(bool_sexp_fsm)); /* must be booleanized */
  
  fprintf(out, 
	  "-- Flattened Boolean FSM model  generated from %s\n"
	  "-- Dumped layers are: ",
	  get_input_file(options)); 

  /* dumps the layer names: */
  iter = NodeList_get_first_iter(layers);
  while (!ListIter_is_end(iter)) {
    SymbLayer_ptr layer = SYMB_LAYER(NodeList_get_elem_at(layers, iter));
    fprintf(out, SymbLayer_get_name(layer));
    fprintf(out, " ");
    iter = ListIter_get_next(iter);
  }
  fprintf(out, "\n\n");
  
  fprintf(out, "%s\n", fsm_name);

  /* NOTE: Defines are not dumped, therefore should not be booleanized */ 

  /* Input variables */
  iter = NodeList_get_first_iter(layers);
  while (!ListIter_is_end(iter)) {
    SymbLayer_ptr layer = SYMB_LAYER(NodeList_get_elem_at(layers, iter));
    fprintf(out, "-- Input variables from layer '%s'\n", 
	    SymbLayer_get_name(layer));
    compile_write_flatten_bool_vars(symb_table, 
				    SexpFsm_get_bool_enc(bool_sexp_fsm), 
				    out, SymbLayer_get_input_vars(layer));
    iter = ListIter_get_next(iter);
  }

  /* State variables */
  iter = NodeList_get_first_iter(layers);
  while (!ListIter_is_end(iter)) {
    SymbLayer_ptr layer = SYMB_LAYER(NodeList_get_elem_at(layers, iter));
    fprintf(out, "-- State variables from layer '%s'\n", 
	    SymbLayer_get_name(layer));
    compile_write_flatten_bool_vars(symb_table, 
				    SexpFsm_get_bool_enc(bool_sexp_fsm), 
				    out, SymbLayer_get_state_vars(layer));
    iter = ListIter_get_next(iter);
  }

  /* CONSTANTS */
  if (!opt_backward_comp(options)) {
    if (compile_write_constants(symb_table, out)) fprintf(out, "\n");
  }

  /* CONSTRAINS */
  if (compile_write_flatten_expr_split(symb_table, out, 
                                       SexpFsm_get_init(bool_sexp_fsm), 
                                       "INIT\n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, 
                                       SexpFsm_get_invar(bool_sexp_fsm), 
                                       "INVAR\n")) fprintf(out, "\n");

  if (compile_write_flatten_expr_split(symb_table, out, 
                                       SexpFsm_get_trans(bool_sexp_fsm), 
                                       "TRANS\n")) fprintf(out, "\n");
  

  {
    node_ptr fc = SexpFsm_get_justice(bool_sexp_fsm);
    boolean are_there_compassion = (Nil != SexpFsm_get_compassion(bool_sexp_fsm));

    while(Nil != fc) {
      if (compile_write_flatten_expr(symb_table, out,
                                     car(fc), 
                                     /* For backward compatibility */
                                     are_there_compassion ?
                                     "JUSTICE\n" : "FAIRNESS\n")) fprintf(out, "\n");
      fc = cdr(fc);
    }
  }

  if (compile_write_flatten_expr_pair(symb_table, out, 
				      SexpFsm_get_compassion(bool_sexp_fsm), 
				      "COMPASSION\n")) fprintf(out, "\n\n");
  return;
}

/**Function********************************************************************

  Synopsis           [Prints the boolean specifications of an SMV model.]

  Description        [Prints into the specified file the flattened boolead
  specifications of an SMV model. 

  NOTE: a temporary layer will be created during the dumping for
  determinization variables that derived from the booleanization of
  the specifications. These variable declarations will be printed
  after the specs.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Compile_WriteBoolSpecs(FILE* out,
			    const SymbTable_ptr symb_table,
			    BddEnc_ptr enc,
			    node_ptr spec,
			    node_ptr compute,
			    node_ptr ltlspec,
			    node_ptr invarspec,
			    node_ptr pslspec,
			    node_ptr reachtarget,
			    node_ptr avoidtarget,
			    node_ptr reachdeadlock,
			    node_ptr avoiddeadlock,
			    node_ptr buchigame,
			    node_ptr genreactivity)
{
  /* just code consistency check */
  nusmv_assert(symb_table == BaseEnc_get_symbol_table(BASE_ENC(enc)));
  
  SymbLayer_ptr det_layer;

  /* here we create a temporary layer, in order to hold all
     determinization variables that will be created by the
     booleanization of the properties. This layer will be destroyed
     after the printing of the determinization variables that it will
     possibly contain */
  det_layer = SymbTable_create_layer(symb_table, (char*) NULL, /*temp name*/
				     SYMB_LAYER_POS_DEFAULT);

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, spec, 
				   "SPEC\n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, compute, 
				   "COMPUTE\n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, ltlspec, 
				   "LTLSPEC\n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, invarspec, 
				   "INVARSPEC\n")) fprintf(out, "\n");

  if (pslspec != Nil) {
    fprintf(nusmv_stderr, "\n********   WARNING   ********\n");
    fprintf(nusmv_stderr,  
	    "This version does not support the booleanization of PSL properties.\n"\
	    "However, for user's convenience all the PSL properties will be dumped\n"\
	    "as comments in the output file.\n");
    fprintf(nusmv_stderr, "******** END WARNING ********\n\n");

    fprintf(out, 
	    "--- Dumping of PSL properties is not supported by this version of the system.\n"\
	    "--- However, the PSL properties had been dumped here for user's convenience,\n"\
	    "--- as the occurred in the original model. \n");
    compile_write_flatten_psl(symb_table, out, pslspec);
  }

  /* -- Game (realizability) properties -- */
  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, reachtarget,
				 "REACHTARGET \n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, avoidtarget,
				 "AVOIDTARGET \n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out,
				   reachdeadlock,
				   "REACHDEADLOCK \n")) fprintf(out, "\n");
  
  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out,
				   avoiddeadlock,
				   "AVOIDDEADLOCK \n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out,
				   buchigame,
				   "BUCHIGAME \n")) fprintf(out, "\n");

  if (compile_write_flatten_bfexpr(enc, symb_table, det_layer, out,
				   genreactivity,
				   "GENREACTIVITY \n")) fprintf(out, "\n");

  /* Dumping of the determinization vars, and destruction of the
     temporary layer */
  {
    NodeList_ptr det_vars = SymbLayer_get_input_vars(det_layer);
    if (NodeList_get_length(det_vars) > 0) {
      fprintf(out, "-- Determinization variables of specifications:\n");
      compile_write_flatten_bool_vars(symb_table, 
			      BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(enc)),
			      out, det_vars);
    }    

    /* if game is on => no determinization variables should appear */ 
    nusmv_assert(!opt_mbp_game(options) ||
		 NodeList_get_length(det_vars) == 0);
  }

  SymbTable_remove_layer(symb_table, det_layer);

  return;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
      

/**Function********************************************************************

  Synopsis           [Writes DEFINE declarations in SMV format on a
  file.]

  Description        [Writes DEFINE declarations in SMV format on a
  file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int write_flatten_define(const SymbTable_ptr symb_table, FILE* out, 
				const NodeList_ptr names)
{
  ListIter_ptr iter;

  if (NodeList_get_length(names) > 0) { fprintf(out, "DEFINE\n"); }

  iter = NodeList_get_first_iter(names);
  while (! ListIter_is_end(iter)) {
    node_ptr name = NodeList_get_elem_at(names, iter);
    node_ptr fdef = SymbTable_get_define_flatten_body(symb_table, name);
    
    if (fdef != Nil) {
      print_node(out, name);
      fprintf(out, " := ");
      print_node(out, fdef);
      fprintf(out, ";\n");
    }
    else { 
      fprintf(nusmv_stderr, "write_flatten_define: Flattening failed\n"); 
    }
    
    iter = ListIter_get_next(iter);
  }

  fprintf(out, "\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes flattened ASSIGN declarations in SMV format on a
  file.]

  Description        [Writes flattened ASSIGN declarations in SMV format on a
  file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int write_flatten_assign(const SymbTable_ptr symb_table, 
				FILE* out, const NodeList_ptr vars,
				FlatHierarchy_ptr hierarchy)
{
  ListIter_ptr iter; 

  iter = NodeList_get_first_iter(vars);
  while (! ListIter_is_end(iter)) {
    node_ptr name = NodeList_get_elem_at(vars, iter);
    node_ptr init_name = find_node(SMALLINIT, name, Nil);
    node_ptr next_name = find_node(NEXT, name, Nil);
    node_ptr invar_expr = FlatHierarchy_lookup_assign(hierarchy, name);
    node_ptr init_expr = FlatHierarchy_lookup_assign(hierarchy, init_name);
    node_ptr next_expr = FlatHierarchy_lookup_assign(hierarchy, next_name);

    if ((init_expr != (node_ptr) NULL) ||
        (next_expr != (node_ptr) NULL) ||
        (invar_expr != (node_ptr) NULL)) {
      fprintf(out, "ASSIGN\n");
    }

    if (init_expr != (node_ptr) NULL) print_assign(out, init_name, init_expr);
    if (invar_expr != (node_ptr) NULL) print_assign(out, name, invar_expr);    
    if (next_expr != (node_ptr) NULL) print_assign(out, next_name, next_expr);

    if ((init_expr != (node_ptr)NULL) ||
        (next_expr != (node_ptr)NULL) ||
        (invar_expr != (node_ptr)NULL)) {
      fprintf(out, "\n");
    }
    
    iter = ListIter_get_next(iter);
  }

  fprintf(out, "\n"); 
  return 1;
}


/**Function********************************************************************

  Synopsis           [Prints an assignement statement]

  Description        [[Prints an assignement statement]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void print_assign(FILE * out, node_ptr lhs, node_ptr rhs)
{
  print_node(out, lhs);
  fprintf(out, " := ");
  print_node(out, rhs);
  fprintf(out, ";\n");
}



/**Function********************************************************************

  Synopsis           [Writes VAR and IVAR declarations in SMV format on a 
  file.]

  Description        [Writes VAR and IVARS declarations in SMV format on a 
  file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_vars(const SymbTable_ptr symb_table, 
				      FILE* out, NodeList_ptr vars)
{
  ListIter_ptr iter;
  enum { WFV_UNKNOWN, WFV_INPUT, WFV_STATE } last_insert = WFV_UNKNOWN;

  if (NodeList_get_length(vars) == 0) return 0;
 
  iter = NodeList_get_first_iter(vars);
  while (!ListIter_is_end(iter)) {
    node_ptr name = NodeList_get_elem_at(vars, iter);
    
    if (SymbTable_is_symbol_var(symb_table, name)) {
      
      if (SymbTable_is_symbol_state_var(symb_table, name) 
	  && last_insert != WFV_STATE) {
	fprintf(out, "VAR\n");
	last_insert = WFV_STATE;
      }
      else if (SymbTable_is_symbol_input_var(symb_table, name) 
	       && 
	       last_insert != WFV_INPUT) {
	fprintf(out, "IVAR\n");
	last_insert = WFV_INPUT;
      }

      print_node(out, name);
      fprintf(out, " : ");
      SymbType_print(out, SymbTable_get_var_type(symb_table, name));
      fprintf(out, ";\n"); /* end of the variable output */
    }

    iter = ListIter_get_next(iter);
  } /* loop */

  fprintf(out, "\n");
  return 1;
}



/**Function********************************************************************

  Synopsis [Writes boolean VAR and IVAR declarations in SMV format on
  a file. Non boolean vars are dumped as defines for the sake of
  readability of conterexamples. ]

  Description [Writes boolean VAR and IVARS declarations in SMV format
  on a file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_bool_vars(const SymbTable_ptr symb_table, 
					   const BoolEnc_ptr bool_enc, 
					   FILE* out, NodeList_ptr vars)
{
  ListIter_ptr iter;
  enum { WFV_UNKNOWN, WFV_DEFINE, 
	 WFV_INPUT, WFV_STATE } last_insert = WFV_UNKNOWN;

  if (NodeList_get_length(vars) == 0) return 0;
 
  iter = NodeList_get_first_iter(vars);
  while (!ListIter_is_end(iter)) {
    node_ptr name = NodeList_get_elem_at(vars, iter);
    
    if (SymbTable_is_symbol_var(symb_table, name)) {
      if (!SymbTable_is_symbol_bool_var(symb_table, name)) { 
	if (!opt_backward_comp(options)) {
	  /* dumps the scalar variable as a define */
	  node_ptr body;
	  if (last_insert != WFV_DEFINE) {
	    fprintf(out, "DEFINE\n");
	    last_insert = WFV_DEFINE;
	  }
	  print_node(out, name);
	  fprintf(out, " := ");
	  body = BoolEnc_get_var_encoding(bool_enc, name);
	  print_node(out, body);
	  fprintf(out, ";\n");
	}
      }

      else {
	/* dumps it as boolean var */
	if (SymbTable_is_symbol_state_var(symb_table, name) 
	    && last_insert != WFV_STATE) {
	  fprintf(out, "VAR\n");
	  last_insert = WFV_STATE;
	}
	else if (SymbTable_is_symbol_input_var(symb_table, name) 
		 && 
		 last_insert != WFV_INPUT) {
	  fprintf(out, "IVAR\n");
	  last_insert = WFV_INPUT;
	}
	
	print_node(out, name);
	fprintf(out, " : ");
	SymbType_print(out, SymbTable_get_var_type(symb_table, name));
	fprintf(out, ";\n"); /* end of the variable output */
      }
    }

    iter = ListIter_get_next(iter);
  } /* loop */

  fprintf(out, "\n");
  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes flattened expression in SMV format on a file.]

  Description        [Writes a generic expression prefixed by a given
  string in SMV format on a file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_expr_split(const SymbTable_ptr symb_table, 
				      FILE* out, node_ptr n, char* s)
{
  if (n == Nil) return 0;

  switch (node_get_type(n)) {
  case CONS:
  case AND:
    compile_write_flatten_expr_split(symb_table, out, car(n), s);
    compile_write_flatten_expr_split(symb_table, out, cdr(n), s);
    break;

  default: 
    compile_write_flatten_expr(symb_table, out, n, s);
    break;
  } /* switch */

  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes flattened expression in SMV format on a file.]

  Description        [Writes a generic expression prefixed by a given
  string in SMV format on a file.
  Returns true if at least one character was printed, and false otherwise.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_expr(const SymbTable_ptr symb_table, 
				      FILE* out, node_ptr n, char* s)
{
  if (n == Nil || (node_get_type(n) == TRUEEXP /*this chech is optimization*/&&
		   n == find_node(TRUEEXP, Nil, Nil))) return 0;

  /* flatten only if required, i.e. there is explicit context */
  if (CONTEXT == node_get_type(n)) {
    n = Compile_FlattenSexp(symb_table, cdr(n), car(n));
  }
  
  fprintf(out, "%s ", s);
  print_node(out, n);
  fprintf(out, "\n\n");
  return 1;
}

/**Function********************************************************************

  Synopsis           [Writes PSL properties as they are.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_psl(const SymbTable_ptr symb_table, 
				     FILE* out, node_ptr n)
{
  if (n == Nil) return 0;

  switch (node_get_type(n)) {
  case CONS:
  case AND:
    compile_write_flatten_psl(symb_table, out, car(n));
    compile_write_flatten_psl(symb_table, out, cdr(n));
    break;
    
  default:
    fprintf(out, "-- PSLSPEC\n--   ");
    print_node(out, n);
    fprintf(out, "\n\n");
  } /* switch */

  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes flattened expression in SMV format on a file.]

  Description        [Writes a generic expression prefixed by a given
  string in SMV format on a file. The given layer is intended to hold the 
  determization variables that are created by the booleanization process of 
  the properties, that are kept not booleanized within the system.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_bfexpr(BddEnc_ptr enc, 
					const SymbTable_ptr symb_table, 
					SymbLayer_ptr det_layer, 
					FILE* out, node_ptr n, char* s)
{
  /* Nil and TRUEEXP indicate the end of a list */
  if (n == Nil || (TRUEEXP == node_get_type(n)/*this check is optimization*/ &&
		   n == find_node(TRUEEXP, Nil, Nil))) return 0;

  switch (node_get_type(n)) {
  case CONS:
  case AND:
    compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, car(n), s);
    compile_write_flatten_bfexpr(enc, symb_table, det_layer, out, cdr(n), s);
    break;

  default:
    /* if this assertion is violated ask [AT].
       [AT] I think all not-game specification are always wrapped into CONTEXT
       and not booleanized.
       In previous version if exp is not CONTEXT it is not booleanized.
       In any case booleanization caches results => no problem to invoke 
       already booleanized exp.
       24-07-2006. Remove this assertion and mdp_symbols include if
       the violation is not violated for a long time.
    */
    nusmv_assert(CONTEXT == node_get_type(n) /* usual spec */ ||
		 MBP_SPEC_WRAPPER == node_get_type(n) /* game spec */);

    /* booleanized property before printing */
    fprintf(out, "\n%s", s);
    print_node(out, Compile_expr2bexpr(enc, det_layer, n));
    fprintf(out, "\n");
    break;
  } /* switch */
  
  return 1;
}


/**Function********************************************************************

  Synopsis [Writes flattened expression pairs in SMV format on a
  file.]

  Description [Writes a list of flattened expression pairs prefixed by
  a given string in SMV format on a file.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_flatten_expr_pair(const SymbTable_ptr symb_table, 
					   FILE* out, node_ptr l, char* s)
{
  if (l == Nil) return 0;

  while (l) {
    node_ptr n = car(l);
    l = cdr(l);
    nusmv_assert(node_get_type(n) == CONS);

    fprintf(out, "%s (", s);

    if (node_get_type(n) == CONTEXT) {
      /* flats the context */
      node_ptr fn = Compile_FlattenSexp(symb_table, car(n), Nil);
      print_node(out, fn);
    } 
    else print_node(out, car(n));

    fprintf(out, ", ");
    if (node_get_type(n) == CONTEXT) {
      /* flats the definition */
      node_ptr fn = Compile_FlattenSexp(symb_table, cdr(n), Nil);
      print_node(out, fn);
    } 
    else print_node(out, cdr(n));
    

    fprintf(out, ")\n\n");
  }
  return 1;
}


/**Function********************************************************************

  Synopsis           [Writes the set of non-numeric constants as CONSTANTS 
  statement]

  Description        [Returns 1 if at least one char have been written, 0 
  otherwise]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int compile_write_constants(const SymbTable_ptr symb_table, FILE* out) 
{
  boolean written_once = false;

  NodeList_ptr consts = SymbTable_get_constants(symb_table);
  ListIter_ptr iter = NodeList_get_first_iter(consts);
  while (! ListIter_is_end(iter)) {
    node_ptr name = NodeList_get_elem_at(consts, iter);
    if (node_get_type(name) == ATOM || node_get_type(name) == DOT) {
      /* a name to be written */
      if (!written_once) {
	fprintf(out, "CONSTANTS\n ");
	written_once = true;
      }
      else fprintf(out, ", ");
      
      print_node(out, name);
    }

    iter = ListIter_get_next(iter);
  }

  if (written_once) {
    fprintf(out, " ;\n"); /* to close the statement */
    return 1;
  }

  return 0;
}
