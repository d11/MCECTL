/**CFile***********************************************************************

  FileName    [TraceTable.c]

  PackageName [trace.plugins]

  Synopsis    [Routines related to TraceTable object.]

  Description [ This file contains the definition of \"TraceTable\" 
  class.]
    
  SeeAlso     []

  Author      [Ashutosh Trivedi]

  Copyright   [
  This file is part of the ``trace'' package of NuSMV version 2. 
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


#include "TraceTable_private.h"
#include "TraceTable.h"
#include "TracePlugin.h"
#include "trace/TraceNode.h"
#include "trace/pkg_traceInt.h"
#include "fsm/bdd/BddFsm.h"

static char rcsid[] UTIL_UNUSED = "$Id: TraceTable.c,v 1.1.2.20.4.5.6.2 2007/04/05 17:45:00 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void trace_table_finalize ARGS((Object_ptr object, void* dummy)); 
static int trace_table_print_row_style ARGS((TraceTable_ptr self, 
               Trace_ptr trace, 
               const array_t* layer_names, 
               FILE* output));

static int trace_table_print_column_style ARGS((TraceTable_ptr self, 
            Trace_ptr trace,  
            const array_t* layer_names, 
            FILE* output)); 

static void trace_table_print_vars_rows ARGS((TraceTable_ptr self, 
                Trace_ptr trace, 
                NodeList_ptr symbols, 
                boolean is_only_input, 
                boolean is_only_state, 
                FILE* output));

/*---------------------------------------------------------------------------*/
/* Definition of external functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Creates an Table Plugin and initializes it.]

  Description [Table plugin constructor. As arguments it takes variable style
  which decides the style of printing the trace. The possible values of the
  style variable may be: TRACE_TABLE_TYPE_ROW and TRACE_TABLE_TYPE_COLUMN.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
TraceTable_ptr TraceTable_create(TraceTableStyle style)
{
  TraceTable_ptr self = ALLOC(TraceTable, 1);

  TRACE_TABLE_CHECK_INSTANCE(self);

  trace_table_init(self, style);
  return self;
}

/* ---------------------------------------------------------------------- */
/*     Protected Methods                                                  */
/* ---------------------------------------------------------------------- */

/**Function********************************************************************

  Synopsis    [Initializes trace table object.]

  Description [As arguments it takes variable /"style /" to print the trace.
  The possible values for the style may be : TRACE_TABLE_TYPE_ROW and
  TRACE_TABLE_TYPE_COLUMN.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void trace_table_init(TraceTable_ptr self, TraceTableStyle style) 
{
  if (style == TRACE_TABLE_TYPE_COLUMN) { 
    trace_plugin_init(TRACE_PLUGIN(self), 
                      "TRACE TABLE PLUGIN - column format");
  }
  else {
    trace_plugin_init(TRACE_PLUGIN(self), 
                      "TRACE TABLE PLUGIN - row format");
  }

  OVERRIDE(Object, finalize) = trace_table_finalize;
  OVERRIDE(TracePlugin, action) = trace_table_action;

  self->style = style;
}


/**Function********************************************************************

  Synopsis    [Deinitializes Explain object.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
void trace_table_deinit(TraceTable_ptr self)
{
  trace_plugin_deinit(TRACE_PLUGIN(self));
}


/**Function********************************************************************

  Synopsis    [Action method associated with TraceTable class.]

  Description [ The action associated with TraceTable is to print the trace
  in the specified file in table format. There are two ways a trace can be
  printed: i). where states are listed row-wise. ii) Where states are listed
  column-wise. This depends on the style variable assoicated with the plugin.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int trace_table_action(const TracePlugin_ptr plugin, Trace_ptr trace,
           const array_t* layer_names, void* opt)
{
  TraceTable_ptr self = TRACE_TABLE(plugin);
  FILE* output = (FILE *) opt;
  int result;

  self->enc = Trace_get_enc(trace);
  self->symb_table = BaseEnc_get_symbol_table(BASE_ENC(self->enc));

  if (self->style == TRACE_TABLE_TYPE_COLUMN) {
    result =  trace_table_print_column_style(self, trace, layer_names, 
               output);
  }
  else {
    result = trace_table_print_row_style(self, trace, layer_names, output);
  }

  return result;
}


/*---------------------------------------------------------------------------*/
/* Static functions                                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Print Trace in Table format with each state on a seperate
  column.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
static int 
trace_table_print_column_style(TraceTable_ptr self, Trace_ptr trace, 
                               const array_t* layer_names, FILE* output) 
{
  DdManager* dd = BddEnc_get_dd_manager(self->enc);
  TraceIterator_ptr iter;

  int num_of_states, num_of_combs, num_of_inputs;
  int i, j;

  NodeList_ptr s_symbs = SymbTable_get_layers_state_symbols(self->symb_table, 
                  layer_names);
  NodeList_ptr si_symbs = 
    SymbTable_get_layers_state_input_symbols(self->symb_table, layer_names);

  NodeList_ptr i_symbs = SymbTable_get_layers_input_symbols(self->symb_table, 
                  layer_names);
  
  { /* ----- prints the header: first states, then comb and inputs ----- */
    NodeList_ptr lists[] = { s_symbs, si_symbs, i_symbs }; 
      
    fprintf(output, "Name\t");

    for (i=0; i < (sizeof(lists)/sizeof(lists[0])); ++i) {
      ListIter_ptr symb_iter = NodeList_get_first_iter(lists[i]);
      while (!ListIter_is_end(symb_iter)) {
        print_node(output, NodeList_get_elem_at(lists[i], symb_iter));
        fprintf(output,"\t");
        symb_iter = ListIter_get_next(symb_iter);
      }
    }
    fprintf(output,"\n");
  } /* -------------------- header here is printed -------------------- */
  
  num_of_states = NodeList_get_length(s_symbs);
  num_of_combs = NodeList_get_length(si_symbs);
  num_of_inputs = NodeList_get_length(i_symbs);

  iter = Trace_begin(trace);
  i = 0;
  while (!TraceIterator_is_end(iter)) {
    BddStates curr_state = TraceNode_get_state(iter);
    BddInputs curr_input = TraceNode_get_input(iter);

    fprintf(output, "S%d", i);

    node_ptr val_list, val_iter;

    /* State */
    val_list = BddEnc_assign_symbols(self->enc, curr_state, 
                                     NodeList_to_node_ptr(s_symbs), 
                                     false, (bdd_ptr*) NULL);
    for (val_iter = val_list; val_iter != Nil; val_iter = cdr(val_iter)) {
      node_ptr symb_value = cdr(car(val_iter));
      fprintf(output, "\t");
      print_node(output, symb_value);
    }
    free_list(val_list);

    /* completes the table with empty values for inputs: */
    for (j=0; j < num_of_combs+num_of_inputs; ++j) { fprintf(output, "\t-"); }
    fprintf(output, "\n");

    /* Combo and inputs, if not the last step */
    if (curr_input != BDD_INPUTS(NULL)) {
      bdd_ptr bdd_combs;

      fprintf(output, "C%d\t", i+1);
      for (j=0; j < num_of_states; ++j) { fprintf(output, "-\t"); }

      bdd_combs = bdd_and(dd, curr_state, curr_input);

      val_list = BddEnc_assign_symbols(self->enc, bdd_combs, 
                                       NodeList_to_node_ptr(si_symbs), 
                                       false, (bdd_ptr*) NULL);
      for (val_iter = val_list; val_iter != Nil; val_iter = cdr(val_iter)) {
        node_ptr symb_value = cdr(car(val_iter));
        print_node(output, symb_value);
        fprintf(output, "\t");
      }
      free_list(val_list);

      for (j=0; j < num_of_inputs; ++j) { fprintf(output, "-\t"); }
      fprintf(output, "\n");
      
      /* completes the table with empty values for states: */     
      fprintf(output, "I%d\t", i+1);
      for (j=0; j < num_of_states+num_of_combs; ++j) {fprintf(output, "-\t");}

      /* inputs */
      val_list = BddEnc_assign_symbols(self->enc, curr_input, 
                                       NodeList_to_node_ptr(i_symbs), 
                                       false, (bdd_ptr*) NULL);
      for (val_iter = val_list; val_iter != Nil; val_iter = cdr(val_iter)) {
        node_ptr symb_value = cdr(car(val_iter));
        print_node(output, symb_value);
        if (cdr(val_iter) != Nil) fprintf(output, "\t");
      }
      free_list(val_list);

      bdd_free(dd, curr_input);
      fprintf(output, "\n");
    }

    bdd_free(dd, curr_state);    

    i += 1;
    iter = TraceIterator_get_next(iter);
  } /* loop on steps */

  NodeList_destroy(i_symbs);
  NodeList_destroy(si_symbs);
  NodeList_destroy(s_symbs);

  return 0;
}

/**Function********************************************************************

  Synopsis    [Print Trace in table format with each state in a seperate row.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
static int 
trace_table_print_row_style(TraceTable_ptr self, Trace_ptr trace, 
          const array_t* layer_names, FILE* output)
{
  TraceIterator_ptr iter;
  int i;

  /* Print Header */
  iter = Trace_begin(trace);
  i = 0;
  fprintf(output, "Name\t");
  while (!TraceIterator_is_end(iter)) {
    iter = TraceIterator_get_next(iter);
    fprintf(output, "S%d\t", i++); 
    
    if (!TraceIterator_is_end(iter)) { fprintf(output, "C%d\t", i); }
    if (!TraceIterator_is_end(iter)) { fprintf(output, "I%d\t", i); }
    else fprintf(output, "\n");
  }

  {
    NodeList_ptr ss = SymbTable_get_layers_state_symbols(self->symb_table, 
               layer_names);

    NodeList_ptr sis = 
      SymbTable_get_layers_state_input_symbols(self->symb_table, layer_names);

    NodeList_ptr is = SymbTable_get_layers_input_symbols(self->symb_table, 
               layer_names);
    
    trace_table_print_vars_rows(self, trace, ss, false, true, output);
    trace_table_print_vars_rows(self, trace, sis, false, false, output);
    trace_table_print_vars_rows(self, trace, is, true, false, output);

    NodeList_destroy(is); 
    NodeList_destroy(sis); 
    NodeList_destroy(ss); 
  }
  
  return 0;
}


/**Function********************************************************************

  Synopsis [Prints the rows of name/values associated to given symbols
  list]

  Description [This methods take into account of positions associated
  to state and input symbols. It is provided in order to allow printing of 
  a group of symbols (e.g. only the state symbols, or only the input symbols)]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void trace_table_print_vars_rows(TraceTable_ptr self, Trace_ptr trace, 
                                        NodeList_ptr symbols, 
                                        boolean is_only_input, 
                                        boolean is_only_state, 
                                        FILE* output)
{
  DdManager* dd = BddEnc_get_dd_manager(self->enc);
  boolean is_combinatory = !(is_only_input || is_only_state);
  
  node_ptr* arr_sym = ALLOC(node_ptr, Trace_get_length(trace)+1); 
  node_ptr* arr_iters = ALLOC(node_ptr, Trace_get_length(trace)+1);
  nusmv_assert(arr_sym != (node_ptr*) NULL);
  nusmv_assert(arr_iters != (node_ptr*) NULL);

  /* 1. collects the assignments for all steps */
  {
    int step = 0;
    TraceIterator_ptr iter = Trace_begin(trace);
    while (!TraceIterator_is_end(iter)) {

      bdd_ptr bdd = (bdd_ptr) NULL;
      node_ptr assigns = Nil;
      
      if (is_only_input) {
        bdd = TraceNode_get_input(iter);
      }
      else if (is_combinatory) {
        BddInputs inputs;       
        inputs = TraceNode_get_input(iter);

        /* checks if this is the last step: */
        if (inputs != BDD_INPUTS(NULL)) {
          BddStates states;

          states = TraceNode_get_state(iter);   
          bdd = bdd_and(dd, states, inputs);

          bdd_free(dd, states); 
          bdd_free(dd, inputs); 
        }
      }
      else { /* state variable: */
        nusmv_assert(is_only_state);
        bdd = TraceNode_get_state(iter);
      }
      
      if (bdd != (bdd_ptr) NULL) {
        assigns = BddEnc_assign_symbols(self->enc, bdd, 
                                        NodeList_to_node_ptr(symbols), 
                                        false, NULL);
        bdd_free(dd, bdd);
      }
      arr_sym[step] = assigns; /* stores the resulting assignments */
      arr_iters[step++] = assigns; /* to keep the iterators */

      iter = TraceIterator_get_next(iter);
    } /* loop on time steps */
  }

  /* 2. traverses the collected info and prints the rows */
  {    
    /* it is assumed that all lists in arr_iters have the same length */
    while (arr_iters[0] != Nil) {
      boolean new_row = true;
      int step;
      for (step = 0; step <= Trace_get_length(trace); ++step) {
        if (arr_iters[step] != Nil) {
          node_ptr name = car(car(arr_iters[step]));
          node_ptr val = cdr(car(arr_iters[step]));

          if (new_row) {
            print_node(output, name); /* prints the name */
            if (is_combinatory) fprintf(output, "\t-\t"); 
            else if (is_only_input)  fprintf(output, "\t-\t-\t"); 
            else fprintf(output, "\t"); 
            new_row = false;
          }       
          else {
            fprintf(output, "\t-\t-\t"); 
          }

          /* prints the value of var */
          print_node(output, val);
        
          arr_iters[step] = cdr(arr_iters[step]);
        }
      }

      if (is_combinatory) fprintf(output, "\t-\t-\n"); 
      else if (is_only_input)  fprintf(output, "\t-\n"); 
      else fprintf(output, "\n"); 
    }  /* end of row */
  }

  /* 3. cleans up */
  {
    int step;

    FREE(arr_iters);

    for (step=0; step <= Trace_get_length(trace); ++step) {
      free_list(arr_sym[step]);
    }
    FREE(arr_sym);
  }
    

#if 0 //RC: remove!   
  symb_iter = NodeList_get_first_iter(symbols);
  while (!ListIter_is_end(symb_iter)) {
    node_ptr name; 
    add_ptr var_value_add;

    /* prints the var name */
    name = NodeList_get_elem_at(symbols, symb_iter);

    print_node(output, name);

    var_value_add = BddEnc_expr_to_add(self->enc, name, Nil);

    iter = Trace_begin(trace);
    while (!TraceIterator_is_end(iter)) {
      node_ptr var_value;
      add_ptr vars_add;
      add_ptr it_add;

      if (is_only_input) {
        BddInputs inputs;

        vars_add = BDD_INPUTS(NULL);
        inputs = TraceNode_get_input(iter);

        /* checks if this is the last step: */
        if (inputs != BDD_INPUTS(NULL)) {
          vars_add = bdd_to_add(dd, inputs);
          bdd_free(dd, inputs); 
        }
      }
      else if (is_combinatory) {
        BddInputs inputs;

        vars_add = BDD_INPUTS(NULL);
        inputs = TraceNode_get_input(iter);

        /* checks if this is the last step: */
        if (inputs != BDD_INPUTS(NULL)) {
          BddInputs states;
          BddStatesInputs states_inputs;

          states = TraceNode_get_state(iter);   
          states_inputs = bdd_and(dd, states, inputs);
          vars_add = bdd_to_add(dd, states_inputs);
    
          bdd_free(dd, states_inputs); 
          bdd_free(dd, states); 
          bdd_free(dd, inputs); 
        }
      }
      else {
        /* state variable: */
        BddStates states;

        nusmv_assert(is_only_state);

        states = TraceNode_get_state(iter);
        vars_add = bdd_to_add(dd, states);
        bdd_free(dd, states); 
      }

      if (vars_add != (add_ptr) NULL) {
        it_add = add_if_then(dd, vars_add, var_value_add);
        add_free(dd, vars_add);

        var_value = add_value(dd, it_add);
        add_free(dd, it_add);

        if (iter == Trace_begin(trace)) {
          if (is_combinatory) fprintf(output, "\t-\t"); 
          else if (is_only_input)  fprintf(output, "\t-\t-\t"); 
          else fprintf(output, "\t");    
        }
        else {
          fprintf(output, "\t-\t-\t"); 
        }

        /* prints the value of var */
        print_node(output, var_value);  
      }

      iter = TraceIterator_get_next(iter);
    } /* loop on time steps */

    add_free(dd, var_value_add);

    if (is_combinatory) fprintf(output, "\t-\t-\n"); 
    else if (is_only_input)  fprintf(output, "\t-\n"); 
    else fprintf(output, "\n");      
   
    symb_iter = ListIter_get_next(symb_iter);
  } /* loop on variables */

#endif

}



/**Function********************************************************************

  Synopsis    [Trace Table finalize method.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void trace_table_finalize(Object_ptr object, void* dummy) 
{
  TraceTable_ptr self = TRACE_TABLE(object);

  trace_table_deinit(self);
  FREE(self);
}
