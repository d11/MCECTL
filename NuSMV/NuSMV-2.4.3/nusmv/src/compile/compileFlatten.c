/**CFile***********************************************************************

  FileName    [compileFlatten.c]

  PackageName [compile]

  Synopsis    [Flattening of the model.]

  Description [Performs the flattening of the model. 

  We start from the module <code>main</code> and we recursively
  instantiate all the modules or processes declared in it.<br>
  Consider the following example: <blockquote> 

  MODULE main<br>
    ...<br> 
    VAR<br> 
     a : boolean;<br> 
     b : foo;<br> 
     ...<br><br>

  MODULE foo<br>
    VAR <br>
     z : boolean;<br>
    
    ASSIGN<br>
     z := 1;<br>
     
    </blockquote>
             
  The flattening instantiate the module foo in the
  <code>main</code> module. You can refer to the variables
  "<code>z</code>" declared in the module <code>foo</code> after
  the flattening by using the dot notation <code>b.z</code>.]

  SeeAlso     []

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2.
  Copyright (C) 1998-2005 by CMU and ITC-irst.

  NuSMV version 2 is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
  MA 02111-1307 USA.

  For more information on NuSMV see <http://nusmv.irst.itc.it> or
  email to <nusmv-users@irst.itc.it>.  
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/

#include "compileInt.h"
#include "FlatHierarchy.h"

#include "symb_table/SymbTable.h"
#include "symb_table/SymbLayer.h"
#include "symb_table/SymbType.h"
#include "symb_table/symb_table.h"
#include "type_checking/type_checking.h"

#include "parser/symbols.h"
#include "parser/psl/pslNode.h"

#include "utils/ustring.h"
#include "utils/assoc.h"
#include "utils/error.h"
#include "utils/range.h"


static char rcsid[] UTIL_UNUSED = "$Id: compileFlatten.c,v 1.25.4.30.2.4.2.56.4.19 2007/05/16 15:14:43 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef enum {
  State_Variables_Instantiation_Mode,
  Input_Variables_Instantiation_Mode
} Instantiation_Variables_Mode_Type;

typedef enum {
  Get_Definition_Mode,
  Expand_Definition_Mode
} Definition_Mode_Type;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**Macro***********************************************************************

Synopsis     [Body of define in evaluation]

Description [Indicates that the body of a define is under the
flattening, it is usde to discover possible recursive definitions.]

SideEffects []

SeeAlso      [Flatten_GetDefinition]

******************************************************************************/
#define BUILDING_FLAT_BODY (node_ptr)-11



/**Macro***********************************************************************

Synopsis     [Cleans and frees the hash]

Description [A utility used by internal clean up code for hash tables]

SideEffects []

SeeAlso      []

******************************************************************************/
#define FREE_HASH(hash)                   \
      {                                   \
        if (hash != (hash_ptr) NULL) {    \
          free_assoc(hash);               \
          hash = (hash_ptr) NULL;         \
	}                                 \
      }


/* Return value in case an error occurs */
#define TYPE_ERROR ((node_ptr) -1)


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

Synopsis    [This variable is locally checked by the (de)initialization code 
of the flattener]

Description []

******************************************************************************/
static boolean flattener_initialized = false;


/**Variable********************************************************************

Synopsis    [The internal name of the process-selector input variable.]

Description [The default name is <tt>PROCESS_SELECTOR_VAR_NAME</tt>.
If there are processes this variable is given the name in the
function Compile_FlattenHierarchy. Otherwise (if there are no processes)
the variable is left Nil.]
  
******************************************************************************/
node_ptr proc_selector_internal_vname = Nil;

/**Variable********************************************************************

Synopsis    [The mode to perform variable instantiation.]

Description [Depending the value of this variable we perform
instantiation of state variables or input variables.]

******************************************************************************/
static Instantiation_Variables_Mode_Type variable_instantiate_mode = 
State_Variables_Instantiation_Mode;

static void set_variable_instantiation_to_input () {
  variable_instantiate_mode = Input_Variables_Instantiation_Mode;
}
static void set_variable_instantiation_to_state (void) {
  variable_instantiate_mode = State_Variables_Instantiation_Mode;
}
static int variable_instantiation_is_input (void) {
  return(variable_instantiate_mode == Input_Variables_Instantiation_Mode);
}

/**Variable********************************************************************

Synopsis    [The expansion mode for definitions is sexp flattening.]

Description [Depending on the value of this variable, a definition
is expanded or not during the flattening of a sexp.]

******************************************************************************/
static Definition_Mode_Type definition_mode = Get_Definition_Mode;

void set_definition_mode_to_get () {
  definition_mode = Get_Definition_Mode;
}
void set_definition_mode_to_expand () {
  definition_mode = Expand_Definition_Mode;
}
int definition_mode_is_expand (void) {
  return(definition_mode == Expand_Definition_Mode);
}


/**Variable********************************************************************

Synopsis    [The hash containing the definition of each module read in.]

Description [This hash uses the name of the module as index, and for
each module it stores the following data structure:<br>
<center><code>&lt;LAMBDA , arguments, module_body&gt;</code></center><br>
I.e. it is a node, whose type is <code>LAMBDA</code> and whose "car" are
the module arguments and the "cdr" is the module body (i.e. normal
assignments, init assignments and next assignments.
]

******************************************************************************/
static hash_ptr module_hash;
void insert_module_hash(node_ptr x, node_ptr y)
{
  insert_assoc(module_hash, x, y);
}

node_ptr lookup_module_hash(node_ptr x)
{
  return(find_assoc(module_hash, x));
}

static void init_module_hash(void)
{
  /* Auxiliary variable used to traverse the parse tree. */
  node_ptr m;
  /* The parse tree representing the input files. */
  extern node_ptr parsed_tree;

  module_hash = new_assoc();
  m = parsed_tree;
  while (m != Nil) {
    node_ptr cur_module = car(m);

    if (Nil != cur_module && node_get_type(cur_module) == MODULE) {
      node_ptr name = find_atom(car(car(cur_module)));
      node_ptr params = cdr(car(cur_module));
      node_ptr def = cdr(cur_module);

      if (find_assoc(module_hash, name)) error_redefining(name);
      insert_module_hash(name, new_node(LAMBDA, params, reverse(def)));
    }
    m = cdr(m);
  }
}

static void clear_module_hash(void) { FREE_HASH(module_hash); }


/**Variable********************************************************************

Synopsis    [The <code>param_hash</code> associates actual to formal
paramaters of a module.]

Description [This hash is used by <code>make_params</code> to detect
multiple substitution for parameters and to perform substitution of
formal parameters with actual parameters.]

SeeAlso     [make_params]

******************************************************************************/
static hash_ptr param_hash;
static void init_param_hash() { param_hash = new_assoc(); }
static void clear_param_hash() { FREE_HASH(param_hash); }

node_ptr lookup_param_hash(node_ptr x) { return(find_assoc(param_hash, x)); }
static void insert_param_hash(node_ptr x, node_ptr y) 
{ insert_assoc(param_hash, x, y); }


/**Variable********************************************************************

Synopsis    [The hash of flatten_def]

Description [This hash associates to an atom corresponding to a
defined symbol the corresponding flattened body.]

******************************************************************************/
static hash_ptr flatten_def_hash = (hash_ptr)NULL;
static void init_flatten_def_hash() 
{
  flatten_def_hash = new_assoc();
  nusmv_assert(flatten_def_hash != (hash_ptr)NULL);
}

static void insert_flatten_def_hash(node_ptr key, node_ptr value) 
{
  nusmv_assert(flatten_def_hash != (hash_ptr)NULL);
  insert_assoc(flatten_def_hash, key, (node_ptr)value);
}

static node_ptr lookup_flatten_def_hash(node_ptr key) 
{
  nusmv_assert(flatten_def_hash != (hash_ptr)NULL);
  return((node_ptr)find_assoc(flatten_def_hash, key));
}

static assoc_retval flatten_def_hash_free(char *key, char *data, char * arg) 
{
  node_ptr element = (node_ptr)data;
  /* Notice that this hash may contain elements set to
     BUILDING_FLAT_BODY in cases of errors inside the flattening
     procedure */
  if (element != (node_ptr)NULL && element != BUILDING_FLAT_BODY) {
    free_node(element);
  }
  return(ASSOC_DELETE);
}

static void clear_flatten_def_hash() 
{
  if (flatten_def_hash != (hash_ptr) NULL) {
    clear_assoc_and_free_entries(flatten_def_hash, flatten_def_hash_free);
    free_assoc(flatten_def_hash);
    flatten_def_hash = (hash_ptr) NULL;
  }
}


/**Variable********************************************************************

Synopsis    [Variable containing the current context in the
instantiation phase.]

Description [Variable containing the current context in the
instantiation phase. It is used in the instantiation of the
arguments of modules or processes.]

******************************************************************************/
static node_ptr param_context = Nil;

/**Variable********************************************************************

Synopsis    [The stack containing the nesting for modules.]

Description [This variable contains the nesting of modules. It is
used in the instantiation phase to check for recursively defined modules.]

******************************************************************************/
static node_ptr module_stack = Nil;



/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void 
compile_instantiate ARGS((SymbLayer_ptr, node_ptr, node_ptr, node_ptr,
                          node_ptr *, FlatHierarchy_ptr));

static void 
compile_instantiate_by_name ARGS((SymbLayer_ptr, node_ptr, node_ptr, node_ptr, 
                                  node_ptr *, FlatHierarchy_ptr));

static void 
compile_instantiate_var ARGS((SymbLayer_ptr layer, node_ptr, node_ptr, 
                              node_ptr, node_ptr*, FlatHierarchy_ptr));
static void 
compile_instantiate_vars ARGS((SymbLayer_ptr layer, node_ptr, node_ptr,
                               node_ptr *, FlatHierarchy_ptr));

static node_ptr put_in_context ARGS((node_ptr));

static node_ptr 
compileFlattenSexpRecur ARGS((const SymbTable_ptr, node_ptr, node_ptr));

static node_ptr
compileFlattenProcess ARGS((const SymbTable_ptr, node_ptr, FlatHierarchy_ptr));
static void 
compileFlattenProcessRecur ARGS((const SymbTable_ptr, node_ptr, node_ptr, node_ptr, 
                                 FlatHierarchy_ptr));

static void 
compileFlattenSexpModel ARGS((const SymbTable_ptr, const SymbLayer_ptr, 
                              node_ptr, node_ptr, node_ptr,
                              FlatHierarchy_ptr));
static void 
compileFlattenSexpModelAux ARGS((const SymbTable_ptr, const SymbLayer_ptr, 
                                 node_ptr, int,
                                 FlatHierarchy_ptr flatHierarchy));

static void 
compileFlattenSexpModelRecur ARGS((const SymbTable_ptr, const SymbLayer_ptr, 
                                   node_ptr, int,
                                   FlatHierarchy_ptr flatHierarchy));

static Set_t 
compileFlattenConstantSexpCheck ARGS((const SymbLayer_ptr, node_ptr, int));

static node_ptr
compile_flatten_resolve_name_recur ARGS((const SymbTable_ptr symb_table, 
                                         node_ptr n, node_ptr context));

static node_ptr compile_flatten_eval_number ARGS((node_ptr n, boolean pos));

static void 
create_process_symbolic_variables ARGS((SymbTable_ptr symb_table, 
                                        SymbLayer_ptr, node_ptr));

static const char* type_to_string ARGS((int));

static void 
flatten_declare_constants_within_list ARGS((node_ptr range,
                                            SymbLayer_ptr layer)); 

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

Synopsis           [Traverse the module hierarchy, collect all required
the informations and flatten the hierarchy.]

Description        [Traverses the module hierarchy and extracts the
information needed to compile the automaton. The hierarchy of modules
is flattened, the variables are added to the symbol table, all the
necessary parts of the model are collected (i.e. the formulae to be
verified, the initial expressions, etc). Most of the collected
expressions are flattened.

  
The returned value is a structure containing all the collected
parts. See FlatHierarchy_create function for more info about, and
constrains on content of the class FlatHierarchy.

NB: It is the invoker's responsibility to destroy the returned value.

NB: The parameter `create_process_variables` enables the creation of
process variable (i.e. declaration of 'running's ). So, this
parameter can be set up only for users 'main' modules. For auxiliary
modules created during execution (for example, during LTL tablaue
generation) this parameter should be set to false (as is done in ltl.c).]

SideEffects        [None]

SeeAlso            []

******************************************************************************/
FlatHierarchy_ptr Compile_FlattenHierarchy(
  const SymbTable_ptr symb_table, 
  SymbLayer_ptr layer, /* the symbolic layer to flat */
  node_ptr module_name,  /* the <code>ATOM</code> representing the
                            name of the module being instantiated (at
                            the top of the hierarchy. */
  node_ptr inst_name, /* the name of the module instance 
                         at the top of the hierarchy. */
  node_ptr actual, /* the actual module arguments */
  boolean create_process_variables /* enables creation of process variables */)
{
  FlatHierarchy_ptr result = FlatHierarchy_create();
  
  /* collect all the constructs of a hierarchy */
  Compile_ConstructHierarchy(layer, module_name, inst_name, actual, result);

  /* Process the created hierarchy. */
  Compile_ProcessHierarchy(symb_table, layer, result, inst_name,
                           create_process_variables);

  if (FlatHierarchy_get_compassion(result) != Nil) {
    fprintf(nusmv_stdout, 
            "WARNING *** The model contains COMPASSION declarations.        ***\n"
            "WARNING *** Full fairness is not yet fully supported in NuSMV. ***\n"
            "WARNING *** Currently, COMPASSION declarations are only        ***\n"
            "WARNING *** supported for BDD-based LTL Model Checking.        ***\n"
            "WARNING *** Results of CTL Model Checking and of Bounded       ***\n"
            "WARNING *** Model Checking may be wrong.                       ***\n");
  }

  return result;
}


/**Function********************************************************************

Synopsis           [Traverses the module hierarchy and extracts the
information needed to compile the automaton.]

Description        [This function is a subfunction of Compile_FlattenHierarchy.
  
This function traverses the module hierarchy and extracts the
information needed to compile the automaton. The hierarchy of modules
is flattened, the variables are added to the symbol table, all the
necessary parts of the model are collected (i.e. the formulae to be
verified, the initial expressions, etc).
  
The returned value is a structure constraining all the collected parts
which are:
the list of TRANS, INIT, INVAR, ASSIGN, SPEC, COMPUTE, LTLSPEC,
PSLSPEC, INVARSPEC, JUSTICE, COMPASSION,
a full list of variables declared in the hierarchy,
a hash table associating variables to their assignments and constrains.
See FlatHierarchy class for more info.
]

SideEffects        []

******************************************************************************/
void Compile_ConstructHierarchy(
  SymbLayer_ptr layer, /* the layer that must be filled in by the flattening */
  node_ptr module_name /* the <code>ATOM</code> representing the name of the 
                          module being instantiated */,
  node_ptr instance_name /* the name of the module instance to be instantiated */,
  node_ptr actual /* the actual module arguments */,
  FlatHierarchy_ptr result)
{
  node_ptr tmp_assign = Nil;
  compile_instantiate_by_name(layer, module_name, instance_name, actual,
                              &tmp_assign, result);
  
  /* create a list of pairs (process name,assignments in it), it to the result */
  tmp_assign = cons(cons(instance_name, tmp_assign),
                    FlatHierarchy_get_assign(result));
  FlatHierarchy_set_assign(result, tmp_assign);
}


/**Function********************************************************************

Synopsis           [This function processes a hierarchy after 
collecting all its subparts.]

Description        [This processing means:
1. process_selector variable and running defines are declared (only if 
create_process_variables is on)
2. All the required lists of expressions are reversed.
All the constrains (not specifications) are flattened.
3. An association between vars and constrains are created (for ASSIGN,
INIT, INVAR, TRANS).
4. Type checking of the variable and define declarations and of all the 
expressions.
5. Also a correct use of input variables and lack of circular dependenses
are checked.
 
The parameters:
layer is a layer with module variables.
hierachy is a hierarchy to be process.
name is a name of the module instance, i.e. a context of all expressions.
create_process_variables enables creation of process variables.
]

SideEffects        []

SeeAlso            []

******************************************************************************/
void 
Compile_ProcessHierarchy(SymbTable_ptr symb_table,
                         SymbLayer_ptr layer,
                         FlatHierarchy_ptr hierarchy,
                         node_ptr name,
                         boolean create_process_variables)
{
  node_ptr tmp;

  /* --- 1 ---- */
  /* if processes are not allowed then no processes should exist */
  nusmv_assert(create_process_variables ||
               (Nil != FlatHierarchy_get_assign(hierarchy) &&
                Nil == cdr(FlatHierarchy_get_assign(hierarchy))));

  /* Create process_selector variables and running defines (if required at all)
     (this must be done before flattening processes and type checking).
  */
  if (create_process_variables) { /* 'map' is used to get all process names */
    create_process_symbolic_variables(symb_table, layer,
                                      map(car, FlatHierarchy_get_assign(hierarchy)));
  }

  /* --- 2 ---- */

  /* Flatten the expressions INIT, TRANS, INVAR, JUSTICE and COMPASSION */
  tmp = Compile_FlattenSexp(symb_table, FlatHierarchy_get_init(hierarchy), name);
  FlatHierarchy_set_init(hierarchy, tmp);

  tmp = Compile_FlattenSexp(symb_table, FlatHierarchy_get_trans(hierarchy), name);
  FlatHierarchy_set_trans(hierarchy, tmp);

  tmp = Compile_FlattenSexp(symb_table, FlatHierarchy_get_invar(hierarchy), name);
  FlatHierarchy_set_invar(hierarchy, tmp);

  tmp = Compile_FlattenSexp(symb_table,
                            reverse(FlatHierarchy_get_justice(hierarchy)),name);
  FlatHierarchy_set_justice(hierarchy, tmp);

  tmp = Compile_FlattenSexp(symb_table,
                            reverse(FlatHierarchy_get_compassion(hierarchy)),
                            name);
  FlatHierarchy_set_compassion(hierarchy, tmp);


  /* The SPEC, LTLSPEC, PSLSPEC, INVAR_SPEC, COMPUTE properties are
     simply reversed but NOT flattened.
  */
  tmp = reverse(FlatHierarchy_get_spec(hierarchy));
  FlatHierarchy_set_spec(hierarchy, tmp);

  tmp = reverse(FlatHierarchy_get_ltlspec(hierarchy));
  FlatHierarchy_set_ltlspec(hierarchy, tmp);

  tmp = reverse(FlatHierarchy_get_invarspec(hierarchy));
  FlatHierarchy_set_invarspec(hierarchy, tmp);
  
  tmp = reverse(FlatHierarchy_get_pslspec(hierarchy));
  FlatHierarchy_set_pslspec(hierarchy, tmp);

  tmp = reverse(FlatHierarchy_get_compute(hierarchy));
  FlatHierarchy_set_compute(hierarchy, tmp);

  /* --- 3 ---- */
  /* assignments require special management:
     1. they are flattened 
     2. running symbols are added to assignments (if required)
     3. a hash of (var-name -> its assignment, invar, init) is created.
  */
  tmp = compileFlattenProcess(symb_table,
                              FlatHierarchy_get_assign(hierarchy), hierarchy);
  FlatHierarchy_set_assign(hierarchy, tmp);

  /* create association between vars and the constrains they are in */
  compileFlattenSexpModel(symb_table, layer,
                          FlatHierarchy_get_init(hierarchy),
                          FlatHierarchy_get_invar(hierarchy),
                          FlatHierarchy_get_trans(hierarchy),
                          hierarchy);

  /* --- 4 ---- */
  /* type check the obtained module */
  { 
    boolean isOk = true;
    isOk = isOk && TypeChecker_check_layer(
             TypeCheckingPkg_get_global_type_checker(), layer);
    
    isOk = isOk && TypeChecker_check_constrains(
                    TypeCheckingPkg_get_global_type_checker(),
                    FlatHierarchy_get_init(hierarchy),
                    FlatHierarchy_get_trans(hierarchy),
                    FlatHierarchy_get_invar(hierarchy),
                    /* get rid of module names */
                    map(cdr, FlatHierarchy_get_assign(hierarchy)),
                    FlatHierarchy_get_justice(hierarchy),
                    FlatHierarchy_get_compassion(hierarchy));
    if (!isOk) error_type_system_violation(); /* error */
  }

  /* --- 5 ---- */
  /* if process variable should NOT be created then this is not a user module
     but module generated by NuSMV. 
     So the input variables check and check of cycle dependencies of
     assignments may be skipped.
  */
  if (create_process_variables) {
    compileCheckForInputVars(symb_table, 
                             FlatHierarchy_get_trans(hierarchy),
                             FlatHierarchy_get_init(hierarchy),
                             FlatHierarchy_get_invar(hierarchy),
                             FlatHierarchy_get_assign(hierarchy),
                             hierarchy);
    
    Compile_CheckAssigns(symb_table, FlatHierarchy_get_assign(hierarchy));
  }
}


/**Function********************************************************************

Synopsis           [Builds the flattened version of an expression.]

Description        [Builds the flattened version of an
expression. It does not expand defined symbols with the
corresponding body.]

SideEffects        []

SeeAlso            [Flatten_GetDefinition, Compile_FlattenSexpExpandDefine]

******************************************************************************/
node_ptr Compile_FlattenSexp(const SymbTable_ptr symb_table, node_ptr sexp, 
                             node_ptr context) 
{
  node_ptr result;
  set_definition_mode_to_get();
  result = compileFlattenSexpRecur(symb_table, sexp, context);
  return result;
}

/**Function********************************************************************

Synopsis           [Flattens an expression and expands defined symbols.]

Description        [Flattens an expression and expands defined symbols.]

SideEffects        []

SeeAlso            [Flatten_GetDefinition, Compile_FlattenSexp]

******************************************************************************/
node_ptr 
Compile_FlattenSexpExpandDefine(const SymbTable_ptr symb_table, 
                                node_ptr sexp, node_ptr context) 
{
  node_ptr result;
  set_definition_mode_to_expand();
  result = compileFlattenSexpRecur(symb_table, sexp, context);
  return result;
}


/**Function********************************************************************

Synopsis           [Gets the flattened version of an atom.]

Description        [Gets the flattened version of an atom. If the
atom is a define then it is expanded. If the definition mode
is set to "expand", then the expanded flattened version is returned,
otherwise, the atom is returned.]

SideEffects        [The <tt>flatten_def_hash</tt> is modified in
order to memoize previously computed definition expansion.]

******************************************************************************/
node_ptr Flatten_GetDefinition(const SymbTable_ptr symb_table, node_ptr atom)
{
  node_ptr result = Nil;

  if (SymbTable_is_symbol_var(symb_table, atom)) result = atom;
  else if (SymbTable_is_symbol_constant(symb_table, atom)) result = atom;
  else if (SymbTable_is_symbol_define(symb_table, atom)) {
    node_ptr exp = lookup_flatten_def_hash(atom);

    /* Check for circular recursive definitions */
    if (exp == BUILDING_FLAT_BODY) { error_circular(atom); }
    if (exp == (node_ptr) NULL) {
      /* The body of a definition is flattened and the flattening is
         saved.  The flattened body is not returned. */
      insert_flatten_def_hash(atom, BUILDING_FLAT_BODY);
      io_atom_push(atom);
      {
        Definition_Mode_Type old_definition_mode = definition_mode;
        /* 
           We need to store the previous definition expansion mode,
           and force it to be expand here since we are attempting to
           expand the body of defined symbols, and we need to to do it
           recursively. If this is not done, then the expansion of
           further defined symbols occurring in the body is not
           performed.
        */
        set_definition_mode_to_expand();

        exp = compileFlattenSexpRecur(symb_table, 
                        SymbTable_get_define_body(symb_table, atom), 
                        SymbTable_get_define_context(symb_table, atom));

        definition_mode = old_definition_mode;
      }
      io_atom_pop();
      insert_flatten_def_hash(atom, exp);
    }
    
    if (definition_mode_is_expand()) result = exp;
    else result = atom;
  }
  else { error_undefined(atom); }
  return result;
}


/**Function********************************************************************

Synopsis           [Resets the hashed information about the given symbol]

Description [This method is used when removing symbols (for example,
when removing a layer) as some information about that symbol may be
chached internally to this module. For example this is the case of
defines, whose flatten body are privately cached within this module. 
  
If the symbol is not cached or have no associated information, no
action is taken.  ]

SideEffects        []

******************************************************************************/
void Flatten_remove_symbol_info(node_ptr name)
{
  /* module hash */
  if (lookup_module_hash(name) != (node_ptr) NULL) {
    insert_module_hash(name, (node_ptr) NULL);
  }

  /* param hash */
  if (lookup_param_hash(name) != (node_ptr) NULL) {
    insert_param_hash(name, (node_ptr) NULL);
  }

  /* flatten def */
  if (lookup_flatten_def_hash(name) != (node_ptr) NULL) {
    insert_flatten_def_hash(name, (node_ptr) NULL);
  }
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

Synopsis           [Error message for unsupported feature]

Description        []

SideEffects        []

******************************************************************************/
void error_bit_selection_assignment_not_supported(node_ptr name)
{
  extern int yylineno;
  extern FILE* nusmv_stderr;

  yylineno = node_get_lineno(name);

  start_parsing_err();
  fprintf(nusmv_stderr, "Bit selection '");
  print_node(nusmv_stderr, name);
  fprintf(nusmv_stderr, "':\n");
  fprintf(nusmv_stderr, 
          "Error: Current version does not support assignment "\
          "of bit selections.\n");
  finish_parsing_err();
}


/**Function********************************************************************

Synopsis           [Flatten a hierarchy of SMV processes.]

Description         [This functions takes in input the list of process names
and thier assignments resulting from the instantiation step and
fills in the hash table (parameter assign_hash) with the
associations the following form:
<ul>
<li><tt>init(var) -> (init_assign)</tt><br>
where <tt>init_assign</tt> is the right side of the initialization
assignement of the variable <tt>var</tt>.
<li><tt>next(var) -> (case P1.running : next_assign_1;
case P2.running : next_assign_2;
...
var)</tt><br>
where  <tt>next_assign_i</tt> is the right side of the next
assignement for the variable <tt>var</tt> in process <tt>i</tt>.
When other processes not affecting the variable are running,
the variable stutter.
If there are no processes the data structure will degenerate
into <tt>next(var) -> next_assign</tt>.
<li><tt>var -> (normal_assign)</tt><br>
where  <tt>normal_assign</tt> is the right side of the
normal (invariant) assignement for the variable
<tt>var</tt>.  </ul> 

The parameter proc_assignment_list is a list of pairs 
(process_name, a list of assignment in the process).
]

SideEffects        []

SeeAlso            []

******************************************************************************/
static node_ptr compileFlattenProcess(const SymbTable_ptr symb_table, 
                                      node_ptr proc_assign_list,
                                      FlatHierarchy_ptr flattenHierarchy)
{
  node_ptr l;
  node_ptr result = Nil;
  node_ptr running = sym_intern(RUNNING_SYMBOL);
  /* to make the order of processes declarations the same as in an input file,
     reverse the list except the first element (which is "main" module and 
     must be at the beginning)
  */
  proc_assign_list = cons(car(proc_assign_list),
                          reverse_ns(cdr(proc_assign_list)));

  for (l = proc_assign_list; l != Nil; l = cdr(l)) { /* Loops over processes */
    node_ptr process_assignments = Compile_FlattenSexp(symb_table,
                                                       cdr(car(l)), Nil);
    node_ptr process_name = car(car(l));
    node_ptr running_name = 
      CompileFlatten_resolve_name(symb_table, running, process_name);
    
    result = cons(cons(process_name, process_assignments), result);

    compileFlattenProcessRecur(symb_table, process_assignments, Nil, 
                               running_name, flattenHierarchy);
  }

  return result;
}


/**Function********************************************************************

Synopsis    [Creates association between variables and all the expressions
the variables occur in.]

Description [For every variable var-name in the given expressions the
function creates association between:
1. var-name and and the INVAR expression list the variable occur.
2. init(var-name) and INIT expression list 
3. next(var-name) and TRANS expression list.
The result is remembered by flatHierarchy.

The function compileFlattenProcess works similarly but with assignments.]

SideEffects        []

SeeAlso            [compileFlattenProcess]

******************************************************************************/
static void compileFlattenSexpModel(const SymbTable_ptr symb_table, 
                                    const SymbLayer_ptr layer, 
                                    node_ptr init_expr,
                                    node_ptr invar_expr,
                                    node_ptr trans_expr,
                                    FlatHierarchy_ptr flatHierarchy)
{
  compileFlattenSexpModelAux(symb_table, layer, init_expr, INIT, flatHierarchy);
  compileFlattenSexpModelAux(symb_table, layer, invar_expr, INVAR, flatHierarchy);
  compileFlattenSexpModelAux(symb_table, layer, trans_expr, TRANS, flatHierarchy);
}


/**Function********************************************************************

Synopsis           [Returns a range going from a to b]

Description        [Returns a range going from a to b. An empty range (Nil) 
is returned whether given 'a' is greater than 'b']

SideEffects        []

SeeAlso            []

******************************************************************************/
node_ptr CompileFlatten_expand_range(int a, int b)
{
  node_ptr res = Nil;

  if ((a == 0) && (b == 1)) res = boolean_range;
  else {
    int i;
    for (i=b ; i>=a ; i--) {
      res = find_node(CONS, find_node(NUMBER, NODE_FROM_INT(i), Nil), res);
    }
  }

  return res;
}


/**Function********************************************************************

Synopsis           [Takes a list of values and returns the same 
list being normalised]

Description         [The normalisation includes: all TRUE and FALSE
constants are substituted by 1 and 0 numbers. If the range includes
just 0 and 1, it became the boolean_range.]

SideEffects        []

SeeAlso            []

******************************************************************************/
node_ptr CompileFlatten_normalise_value_list(node_ptr old_value_list)
{
  node_ptr new_tail;
  node_ptr new_head;
  
  /* the list is empty */
  if (old_value_list == Nil) return Nil;
  
  /* normalise the tail */
  new_tail = CompileFlatten_normalise_value_list(cdr(old_value_list));

  /* normalise the head */
  /* 1. Convert true and false to numbers */
  if (node_get_type(car(old_value_list)) == TRUEEXP ||
      node_get_type(car(old_value_list)) == FALSEEXP) {
    /* convert "TRUE" to "1" and "FALSE" to "0". 
       NB: restore the line number for error messages 
    */
    int tmp = yylineno;
    yylineno = node_get_lineno(car(old_value_list));
    new_head = node_get_type(car(old_value_list)) == TRUEEXP 
      ? one_number : zero_number;
    yylineno = tmp;
  }
  else {
    /* do nothing */
    new_head = car(old_value_list);
  }

  /* 2. Check whether this is the boolean values */
  if (Nil != new_tail && Nil == cdr(new_tail) &&
      ( (new_head == one_number  && car(new_tail) == zero_number) || 
        (new_head == zero_number && car(new_tail) == one_number) 
        ) ) {
    return boolean_range;
  }
  /* create a new list with the line info kept */
  return new_lined_node(CONS, new_head, new_tail, node_get_lineno(old_value_list));
}


/**Function********************************************************************

Synopsis           [Takes an expression representing an identifier
and recursively evaluates it.]

Description        [Takes an expression representing an identifier
and recursively evaluates it. This function is responsible of
building the symbolic name of the given symbol <code>n</code> in
module instance <code>context</code>. If the given symbol is a formal
parameter of the module instance, than the actual parameter is
built. If the given symbol is an array, than the expression
referring to the element of the array must evaluate to an unique
integer (not to a set of integers) in the range of array bound.

NULL is returned if the given expression is not syntactically an
identifier.

NOTE: param symb_table is used only when the name is an array whose
index is not a constant number. If the caller is sure that this is
not the case, it is possible to pass NULL instead.]

SideEffects        []

SeeAlso            [compile_flatten_resolve_name_recur]

******************************************************************************/
node_ptr CompileFlatten_resolve_name(const SymbTable_ptr symb_table, 
                                     node_ptr name, node_ptr context)
{
  node_ptr res;
  int temp;

  if (name == Nil) return name;

  temp = yylineno;
  yylineno = node_get_lineno(name);
  res = compile_flatten_resolve_name_recur(symb_table, name, context);
  yylineno = temp;

  if (res == TYPE_ERROR) return (node_ptr) NULL;
  return res;
}


/**Function********************************************************************

Synopsis           [Resolves the given symbol to be a number]

Description        [If given symbol is a number, the node is simply
returned.  If it is a define, the body is
returned if it is a number, otherwise NULL is returned]

SideEffects        []

SeeAlso            []

******************************************************************************/
node_ptr 
CompileFlatten_resolve_number(SymbTable_ptr symb_table, node_ptr n, 
                              node_ptr context)
{
  node_ptr idx;

  idx = compile_flatten_eval_number(n, true);
  if (idx != Nil) return idx;
  
  /* it might be a symbol that evaluates to a number */
  idx = CompileFlatten_resolve_name(symb_table, n, context);
  if (idx == NULL) return (node_ptr) NULL;

  if ((idx != TYPE_ERROR) && SymbTable_is_symbol_define(symb_table, idx)) {
    /* retrieves the define value, and checkes if it is a numeric constant */
    node_ptr body = SymbTable_get_define_flatten_body(symb_table, idx);

    idx = compile_flatten_eval_number(body, true);
    if (idx != Nil) return idx;
  }

  /* not a number! */
  return (node_ptr) NULL;
}


/**Function********************************************************************

Synopsis           [Inits the flattener module]

Description        [Inits all the internal structures, in order to correctly 
bootstrap the flattener ]

SideEffects [This module will be initialized, all previously
iniitalized data will be lost]

SeeAlso            []

******************************************************************************/
void CompileFlatten_init_flattener()
{
  nusmv_assert(!flattener_initialized); /* not already initialized */
  
  if (opt_verbose_level_gt(options, 2)) {
    fprintf(nusmv_stderr, "Initializing the flattener...\n");
  } 

  init_param_hash();
  init_module_hash();
  init_flatten_def_hash();
  flattener_initialized = true;
}


/**Function********************************************************************

Synopsis           [Quits the flattener module]

Description        [Resets all internal structures, in order to correctly 
shut down the flattener. Calls clear_* local functions, and resets all 
private variables. ]

SideEffects [This module will be deinitialized, all previously
iniitalized data will be lost]

SeeAlso            []

******************************************************************************/
void CompileFlatten_quit_flattener()
{
  /* deinits the flattener only if previously initialized */
  if (!flattener_initialized) return;

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stderr, "Clearing the flattener...\n");
  } 

  clear_param_hash();
  clear_module_hash();
  clear_flatten_def_hash();
  
  /* ---------------------------------------------------------------------- */
  /*                        Reseting of variables                           */
  /* ---------------------------------------------------------------------- */

  /* lists: */
  free_list(module_stack);
  module_stack = Nil;

  /* simple nodes: */
  if (proc_selector_internal_vname != Nil) {
    free_node(proc_selector_internal_vname);
    proc_selector_internal_vname = Nil;
  }
  
  if (param_context != Nil) {
    free_node(param_context);
    param_context = Nil;  
  }

  /* other vars: */
  variable_instantiate_mode = State_Variables_Instantiation_Mode;
  definition_mode = Get_Definition_Mode;

  flattener_initialized = false;
}


/**Function********************************************************************

Synopsis           [Instantiates the given variable.]

Description        [It takes as input a variable and a context, and
depending on the type of the variable some operation are performed in order to
instantiate it in the given context:
<br><br>
<ul>
<li><b>BOOLEAN</b><br>
if the variable is of type boolean, then we add an entry in
<code>symbol_hash</code> saying that the variable values are <code>{0,1}</code>.</li>
<li><b>RANGE</b><br>
if the variable is a range of the form <code>M..N</code>, then
we add an entry in the <code>symbol_hash</code> saying that the
variable values are <code>{M, M+1, ..., N-1, N}</code>. If
<code>M</code> is less or equal to <code>N</code>, than an error occurs.</li>
<li><b>ENUMERATION</b><br>
if the variable is a scalar variable whose possible values are
<code>{a1, a2, ..., aN}</code>, then we add an entry in the
<code>symbol_hash</code> saying that the variable values are
<code>{a1, ..., aN}</code>. </li>
<li><b>ARRAY</b><br>
for each element of the array it is created the corresponding
symbol. Suppose you have the following definition "<code>VAR
x : array 1..4 of boolean;</code>". We call this function
for 4 times, asking at each call <code>i</code> (<code>i</code> from 1
to 4) to declare the boolean variable <code>x\[i\]</code>.</li>
<li><b>MODULE</b><br>
If the variable is an instantiation of a module, than their
arguments (if any) are contextualized, and passed as actual
parameter to <code>instantiate_by_name<code> with the name of the
instantiated module as root name (to extract its definition)
and as variable name as the name of the module (to perform
flattening).</li>
<li><b>PROCESS</b><br>
If the variable is of type process, than we extract the
module name and args, we perform the contextualization of the
process arguments and we perform a call to
<tt>Compile_ConstructHierarchy</tt> using the variable name as process
name (to perform flattening), the module name as root name (to
extract its definition) and the computed actual parameters.</li>
</ul><br>
The variables of type boolean, scalar, and array depending the kind
of variable instantiation mode are appended to
<tt>input_variables</tt> or to <tt>state_variables</tt>.
]

SideEffects        []

SeeAlso            [compile_instantiate_vars]

******************************************************************************/
static void compile_instantiate_var(SymbLayer_ptr layer, node_ptr name,
                                    node_ptr type, node_ptr context, 
                                    node_ptr *assign, FlatHierarchy_ptr result)
{  
  node_ptr name_ctx = find_node(DOT, find_atom(context), find_atom(name));
  yylineno = node_get_lineno(type);

  /* Checks that it is not a clashing module parameter name */
  if (lookup_param_hash(name_ctx)) {
    error_shadowing(name); 
  }

  name = CompileFlatten_resolve_name(NULL, name, context);

  if (!SymbLayer_can_declare_var(layer, name)) error_redefining(name); 

  switch (node_get_type(type)) {

  case BOOLEAN: {
    SymbType_ptr symbolicType;

    flatten_declare_constants_within_list(boolean_range, layer);
    symbolicType = SymbType_create(SYMB_TYPE_ENUM, boolean_range);

    if (variable_instantiation_is_input()) {
      SymbLayer_declare_input_var(layer, name, symbolicType);
    } 
    else SymbLayer_declare_state_var(layer, name, symbolicType);

    FlatHierarchy_add_to_var_list(result, name);
    break;
  }
    
  case TWODOTS: {
    node_ptr expanded_range = Nil;
    int dim1, dim2;
    
    dim1 = NODE_TO_INT(car(car(type)));
    dim2 = NODE_TO_INT(car(cdr(type)));

    /* Checks if the range is a "range", i.e. a range is from "a" to "b"
       with the constraint that "b >= a" */
    expanded_range = CompileFlatten_expand_range(dim1, dim2);
    if (expanded_range == Nil) { error_empty_range(name, dim1, dim2); }
    flatten_declare_constants_within_list(expanded_range, layer);

    if (dim1 == dim2) {
      /* Single-value range */
      node_ptr val = car(expanded_range);
      if (SymbLayer_can_declare_constant(layer, val)) {
        SymbLayer_declare_constant(layer, val);
      }
      SymbLayer_declare_define(layer, name, context, val);

      fprintf(nusmv_stderr, "WARNING: single-value variable '");
      print_node(nusmv_stderr, name);
      fprintf(nusmv_stderr, "' has been stored as a constant\n");
    }
    else {
      SymbType_ptr symbolicType = SymbType_create(SYMB_TYPE_ENUM, expanded_range);
      if (variable_instantiation_is_input()) {
        SymbLayer_declare_input_var(layer, name, symbolicType);
      } 
      else SymbLayer_declare_state_var(layer, name, symbolicType);

      FlatHierarchy_add_to_var_list(result, name);
    }
    break;
  }

  case SCALAR: {
    node_ptr value_list = CompileFlatten_normalise_value_list(car(type));
    node_ptr iter;

    /* check that all symbolic constants are not DOTs since only 
       process_selector may have complex symbolic constants.
    */
    for (iter = value_list; Nil != iter; iter = cdr(iter)) {
      /* complex symbolic constant and the var is NOT _process_selector_
         NOTE: the name of process_selector is the same as in
         create_process_symbolic_variables
      */ 
      if (DOT == node_get_type(car(iter)) && 
          name != find_node(DOT, Nil, sym_intern(PROCESS_SELECTOR_VAR_NAME))) {
        yylineno = node_get_lineno(car(iter));
        rpterr("unexpected \".\" in a costant name \n");
      }
    }

    flatten_declare_constants_within_list(value_list, layer);

    /* Checks whether the variable has a single-value list. In that
       case it creates a new define, instead of a new variable.  The
       user is warned about this action. */
    if (cdr(value_list) == Nil) {
      /* Single-value list */
      node_ptr val = car(value_list);
      if (SymbLayer_can_declare_constant(layer, val)) {
        SymbLayer_declare_constant(layer, val);
      }
      SymbLayer_declare_define(layer, name, context, val);

      fprintf(nusmv_stderr, "WARNING: single-value variable '");
      print_node(nusmv_stderr, name);
      fprintf(nusmv_stderr, "' has been stored as a constant\n");
    }
    else {
      SymbType_ptr symbolicType = SymbType_create(SYMB_TYPE_ENUM, value_list);
      if (variable_instantiation_is_input()) {
        SymbLayer_declare_input_var(layer, name, symbolicType);
      } 
      else SymbLayer_declare_state_var(layer, name, symbolicType); 

      FlatHierarchy_add_to_var_list(result, name);
    }

    break;
  }

  case MODTYPE: {
    node_ptr actual;

    param_context = context;
    actual = map(put_in_context, cdr(type));
    compile_instantiate_by_name(layer, car(type), name, actual, assign, result);
    break;
  }

  case PROCESS: {
    node_ptr actual;
    node_ptr pmod_name = car(car(type));
    node_ptr pmod_args = cdr(car(type));

    param_context = context;
    actual = map(put_in_context, pmod_args);

    Compile_ConstructHierarchy(layer, pmod_name, name, actual, result);
    break;
  }

  case ARRAY: {
    int dim1, dim2;
    int i;
   
    dim1 = NODE_TO_INT(car(car(car(type))));
    dim2 = NODE_TO_INT(car(cdr(car(type))));
   
    for (i=dim1; i<=dim2; ++i) {
      node_ptr index = find_node(NUMBER, NODE_FROM_INT(i), Nil);
      if (SymbLayer_can_declare_constant(layer, index)) {
        SymbLayer_declare_constant(layer, index);
      }
      /* Creates the name[i] variable */
      compile_instantiate_var(layer, find_node(ARRAY, name, index),
                              cdr(type), context, assign, result);
    }
    break;
  }

  case INTEGER:{
    SymbType_ptr symbolicType = SymbType_create(SYMB_TYPE_INTEGER,
                                                Nil /*body*/);
    /* add to the symbol table */
    if (variable_instantiation_is_input()) {
      SymbLayer_declare_input_var(layer, name, symbolicType);
    } 
    else SymbLayer_declare_state_var(layer, name, symbolicType); 

    FlatHierarchy_add_to_var_list(result, name);
    break;
  }

  case REAL: {
    SymbType_ptr symbolicType = SymbType_create(SYMB_TYPE_REAL, Nil /*body*/);
    /* add to the symbol table */
    if (variable_instantiation_is_input()) {
      SymbLayer_declare_input_var(layer, name, symbolicType);
    } 
    else SymbLayer_declare_state_var(layer, name, symbolicType); 

    FlatHierarchy_add_to_var_list(result, name);
    break;
  }
    
  case WORD: {
    SymbType_ptr symbolicType;
    /* the correctness of the width-expression is checked 
       in the type-checking phase.
       In future, the width can be a constant expression 
       and here we will calculate the value.
    */
    /* here we check only that the width is a NUMBER node. 
       Later we may try to allow constant expressions
    */
    if (NUMBER != node_get_type(car(type))) {
      error_not_constant_width_of_word_type(name);
    }

    symbolicType = SymbType_create(SYMB_TYPE_WORD, car(type));

    /* add to the symble table */
    if (variable_instantiation_is_input()) {
      SymbLayer_declare_input_var(layer, name, symbolicType);
    } 
    else SymbLayer_declare_state_var(layer, name, symbolicType); 

    FlatHierarchy_add_to_var_list(result, name);
    break;
  }

  case WORDARRAY: {
    SymbType_ptr symbolicType;
    /* the correctness of the width-expressions are checked 
       in the type-checking phase.
       In future, the widths can be constant expressions
       and here we will calculate the values.
    */
    /* here we check only that the address and value widths are NUMBER node. 
       Later we may try to allow constant expressions
    */
    if (NUMBER != node_get_type(car(type)) || 
        NUMBER != node_get_type(cdr(type))) {
      error_not_constant_width_of_word_array_type(name);
    }

    symbolicType = SymbType_create(SYMB_TYPE_WORDARRAY,
                                   new_lined_node(CONS,
                                                  car(type), cdr(type),
                                                  node_get_lineno(type)));

    /* add to the symble table */
    if (variable_instantiation_is_input()) {
      SymbLayer_declare_input_var(layer, name, symbolicType);
    } 
    else SymbLayer_declare_state_var(layer, name, symbolicType); 

    FlatHierarchy_add_to_var_list(result, name);
    break;
  }
    
  default:
    internal_error("compile_instantiate_var: type = %d", node_get_type(type));
  }

}


/**Function********************************************************************

Synopsis           [Recursively applies <tt>compile_instantiate_var</tt>.]

Description        [Recursively applies <tt>compile_instantiate_var</tt> to
a given list of variables declaration, and performs some check for
multiple variable definitions.]

SideEffects        []

SeeAlso            [compile_instantiate_var]

******************************************************************************/
static void compile_instantiate_vars(SymbLayer_ptr layer, node_ptr var_list, 
                                     node_ptr mod_name,
                                     node_ptr *assign, 
                                     FlatHierarchy_ptr result)
{
  node_ptr rev_vars_list;
  node_ptr iter;

  rev_vars_list = reverse_ns(var_list);
  iter = rev_vars_list;
  while (iter != Nil) {
    node_ptr cur_var = car(iter);
    node_ptr name = car(cur_var);
    node_ptr type = cdr(cur_var);

    compile_instantiate_var(layer, name, type, mod_name, assign, result);

    iter = cdr(iter);
  }

  free_list(rev_vars_list);
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

Synopsis           [Put a variable in the current "context"]

Description        [Put a variable in the current "context", which
is stored in <code>param_context</code>.]

SideEffects        [None]

SeeAlso            [param_context]

******************************************************************************/
static node_ptr put_in_context(node_ptr v)
{
  return(find_node(CONTEXT, param_context, v));
}


/**Function********************************************************************

Synopsis [Builds the parameters of a module from the list of formal
parameters of the module itself.]

Description        [Builds the parameters of a module from the list
of formal parameters of the module itself and a <tt>basename</tt>.<br>
There must be a one to one correspondence between the elements of
<tt>actual</tt> and <tt>formal</tt> parameters. If the number of elements of
the lists are different then, an error occurs.]

SideEffects        [In the <tt>param_hash</tt>, the new parameter is
associated to the old one.]

******************************************************************************/
static void make_params(node_ptr basename, node_ptr actual, node_ptr formal)
{
  while (formal) {
    node_ptr old, new;
    
    if (!actual) {
      extern int yylineno;
      if (basename != (node_ptr) NULL) {
        yylineno = node_get_lineno(basename);
        rpterr("too few actual parameters");
      }
      else {
        yylineno = node_get_lineno(formal);
        rpterr("module 'main' cannot have formal parameters");
      }
    }
      
    new = find_node(DOT, basename, find_atom(car(formal)));
    old = car(actual);
    formal = cdr(formal);
    actual = cdr(actual);
    if (lookup_param_hash(new)) { error_multiple_substitution(new); }
    insert_param_hash(new, old);
  }
  if (actual) rpterr("too many actual parameters");
}


/**Function********************************************************************

Synopsis           [Instantiates all in the body of a module.]

Description        [This function is responsible of the
instantiation of the body of a module. The module definition
(parameter and body) is <tt>mod_def</tt> and the module instance name
<tt>mod_name</tt> are passed as arguments. First we instantiate the
arguments of the given module. Then it loops over the module
definition searching for defined symbols (i.e. those introduced by
the keyword <tt>DEFINE</tt>) and inserts their definition in the
<tt>symbol_hash</tt>. After this preliminary phase it loops again
over module body in order to performs the other instantiation, and
to extract all the information needed to compile the automaton,
i.e. the list of processes, the TRANS statements, the INIT
statements, ... and so on.

NB: After parsing and creating the module hash table, the order of
declarations is correct (not reversed). This function reverse the order
of SPEC, LTLSPEC, PSLSPEC, INVARSPEC, COMPUTE, JUSTICE AND COMPATION
]
  

SideEffects        []

SeeAlso            [compile_instantiate_var compile_instantiate_vars]

******************************************************************************/
static void compile_instantiate(SymbLayer_ptr layer, node_ptr mod_def,
                                node_ptr mod_name, 
                                node_ptr actual,
                                node_ptr *assign, FlatHierarchy_ptr result)
{
  node_ptr mod_body_decls;
  node_ptr mod_formal_args  = car(mod_def); /* Module formal parameters */
  node_ptr mod_body         = cdr(mod_def); /* Module body */

  make_params(mod_name, actual, mod_formal_args); /* creates local parameters */

  /* We first instantiate all the definitions, in case they are
     constants used in the array declarations.
     loop over module declaration
  */
  for (mod_body_decls = mod_body; mod_body_decls != Nil;
       mod_body_decls = cdr(mod_body_decls)) {
    
    node_ptr cur_decl = car(mod_body_decls);
 
    switch (node_get_type(cur_decl)) {
    case DEFINE: 
      {
        node_ptr define_iter;
        /* loop over DEFINE declaration */
        for (define_iter = car(cur_decl); define_iter != Nil;
             define_iter = cdr(define_iter)) {
          node_ptr cur_define = car(define_iter);
          node_ptr name = CompileFlatten_resolve_name(NULL, car(cur_define), mod_name);
          node_ptr definition = cdr(cur_define);

          yylineno = node_get_lineno(define_iter);
          if (SymbLayer_can_declare_define(layer, name)) {
            SymbLayer_declare_define(layer, name, mod_name, definition);
          }
          else { error_redefining(name); }
        }/* loop on defines */
      }
      break;
    default: break;
    }
  } /* loop over module declarations */


  /* Now, we instantiate all the other elements of a module.
     loop again over module declaration
  */
  for (mod_body_decls = mod_body; mod_body_decls != Nil;
       mod_body_decls = cdr(mod_body_decls)) {
    node_ptr cur_decl = car(mod_body_decls);
    node_ptr tmp;
    yylineno = node_get_lineno(cur_decl);
   
    switch (node_get_type(cur_decl)) {
    case ISA:
      compile_instantiate_by_name(layer, car(cur_decl), mod_name,
                                  Nil, assign, result);
      break;

    case VAR:
      compile_instantiate_vars(layer, car(cur_decl), mod_name, assign, result);
      break;

    case IVAR:
      set_variable_instantiation_to_input();
      compile_instantiate_vars(layer, car(cur_decl), mod_name, assign, result);
      
      set_variable_instantiation_to_state();
      break;
      
    case TRANS:
      tmp = find_node(AND, FlatHierarchy_get_trans(result),
                      find_node(CONTEXT, mod_name, car(cur_decl)));
      FlatHierarchy_set_trans(result, tmp);
      break;

    case INIT:
      tmp = find_node(AND, FlatHierarchy_get_init(result),
                      find_node(CONTEXT, mod_name, car(cur_decl)));
      FlatHierarchy_set_init(result, tmp);
      break;

    case INVAR:
      tmp = find_node(AND, FlatHierarchy_get_invar(result),
                      find_node(CONTEXT, mod_name, car(cur_decl)));
      FlatHierarchy_set_invar(result, tmp);
      break;

    case SPEC:
      tmp = cons(find_node(CONTEXT, mod_name, car(cur_decl)),
                 FlatHierarchy_get_spec(result));
      FlatHierarchy_set_spec(result, tmp);
      break;

    case LTLSPEC:
      tmp = cons(find_node(CONTEXT, mod_name, car(cur_decl)),
                 FlatHierarchy_get_ltlspec(result));
      FlatHierarchy_set_ltlspec(result, tmp);
      break;

    case PSLSPEC:
      tmp = cons(PslNode_new_context(mod_name, car(cur_decl)),
                 FlatHierarchy_get_pslspec(result));
      FlatHierarchy_set_pslspec(result, tmp);
      break;      

    case INVARSPEC:
      tmp = cons(find_node(CONTEXT, mod_name, car(cur_decl)),
                 FlatHierarchy_get_invarspec(result));
      FlatHierarchy_set_invarspec(result, tmp);
      break;

    case COMPUTE:
      tmp = cons(find_node(CONTEXT, mod_name, car(cur_decl)),
                 FlatHierarchy_get_compute(result));
      FlatHierarchy_set_compute(result, tmp);
      break;

    case JUSTICE:
      tmp = cons(find_node(CONTEXT, mod_name, car(cur_decl)),
                 FlatHierarchy_get_justice(result));
      FlatHierarchy_set_justice(result, tmp);
      break;

    case COMPASSION:
      tmp = cons(cons(find_node(CONTEXT, mod_name, car(car(cur_decl))), 
                      find_node(CONTEXT, mod_name, cdr(car(cur_decl)))),
                 FlatHierarchy_get_compassion(result));
      FlatHierarchy_set_compassion(result, tmp);
      break;

    case ASSIGN:
      /* an assign may be void */
      if (car(cur_decl) != Nil) {
        *assign = find_node(AND, *assign,
                            find_node(CONTEXT, mod_name, car(cur_decl)));
      }
      break;
      
    case DEFINE: break; /* already dealt with */

    case CONSTANTS:
      /* declares the contained constants: */
      flatten_declare_constants_within_list(reverse_ns(car(cur_decl)), layer);
      break;

    case PRED:
      FlatHierarchy_add_pred(result, cur_decl);
      break;

    case MIRROR:
      FlatHierarchy_add_mirror(result, cur_decl);
      break;

    default: nusmv_assert(false); /* unknown kind of declaration */
    }
  } /* loop over module declartions */
  
}


/**Function********************************************************************

Synopsis           [Starts the flattening from a given point in the
module hierarchy.]

Description        [<tt>module_name</tt> is the name of the module being
instantiated. The name of the module instance 
is <tt>instance_name</tt>. First checks if the module exists. Then it checks
if the module is recursively defined, and if the case an error is
printed out. If these checks are passed, then it proceeds in the
instantiation of the body of the module.]

SideEffects        []

SeeAlso            []

******************************************************************************/
static void compile_instantiate_by_name(SymbLayer_ptr layer,
                                        node_ptr module_name, 
                                        node_ptr instance_name,
                                        node_ptr actual,
                                        node_ptr *assign, 
                                        FlatHierarchy_ptr result)
{
  node_ptr s;
  node_ptr mod_name = find_atom(module_name);         /* find module name */
  node_ptr mod_def  = lookup_module_hash(mod_name); /* find module definition  */

  yylineno = node_get_lineno(module_name);
  if (mod_def == (node_ptr) NULL) { 
    /* The module is undefined */
    error_undefined(module_name); 
  }
   
  /* scans module_stack in order to find if there are recursively
     defined modules */
  s = module_stack;
  while (s != Nil) {
    if (car(s) == mod_name) {
      rpterr("module \"%s\" is recursively defined",
             get_text((string_ptr)car(module_name)));
    }
    s = cdr(s);
  }

  /* append current module to module_stack */
  module_stack = cons(mod_name, module_stack);

  compile_instantiate(layer, mod_def, instance_name, actual, assign, result);

  /* elimination of current module form module_stack */
  s = cdr(module_stack);
  free_node(module_stack);
  module_stack = s;
}


/**Function********************************************************************

Synopsis           [Recursive function for flattenig a sexp.]

Description        [Recursive function for flattenig a sexp.]

SideEffects        []

SeeAlso            [Compile_FlattenSexp Compile_FlattenSexpExpandDefine]

******************************************************************************/
static node_ptr 
compileFlattenSexpRecur(const SymbTable_ptr symb_table, 
                        node_ptr sexp, node_ptr context)
{
  node_ptr result = Nil;
  int temp = yylineno;

  if (sexp == Nil) return sexp;

  yylineno = node_get_lineno(sexp);

  switch (node_get_type(sexp)) {
    /* base cases for which no flattening necessary */
  case FAILURE:
  case TRUEEXP:
  case FALSEEXP:
  case NUMBER:
  case NUMBER_WORD:
  case NUMBER_FRAC:
  case NUMBER_REAL:
  case NUMBER_EXP:
    result = find_atom(sexp);
    break;

  case ATOM:
    {
      node_ptr param;
      node_ptr atom = find_atom(sexp);
      node_ptr name = find_node(DOT, context, atom);

      /* It is a constant. No flattening necessary */
      if (SymbTable_is_symbol_constant(symb_table, atom)) {
        result = atom;
        break;
      }
      
      if (SymbTable_is_symbol_constant(symb_table, name)) {
        result = name;
        break;
      }      

      param = lookup_param_hash(name);
      if (param != (node_ptr)NULL) {
        /* The result of the flattening is then flattening of parameters */
        result = compileFlattenSexpRecur(symb_table, param, context);
        break;
      }
      /* It can be a defined symbol, a running condition or a variable */
      result = Flatten_GetDefinition(symb_table, 
                                     CompileFlatten_resolve_name(symb_table, sexp, context));
      break;
    }

  case SELF:
    fprintf(nusmv_stderr,
            "compileFlattenSexpRecur: invalid usage of identifier \"self\"\n");

    error_reset_and_exit(1);
    break;

  case DOT:
  case ARRAY:
    {
      node_ptr name = CompileFlatten_resolve_name(symb_table, sexp, context);
      result = Flatten_GetDefinition(symb_table, name);
      break;
    }

  case CONTEXT:
    /* (CONTEXT (cxt . expr)) */
    result = compileFlattenSexpRecur(symb_table, cdr(sexp), car(sexp));
    break;

    /* Unary operators */
  case NOT:
  case NEXT:
  case CAST_BOOL:
  case CAST_WORD1:
  case UMINUS:
    {
      node_ptr body = compileFlattenSexpRecur(symb_table, car(sexp), context);
      result = new_node(node_get_type(sexp), body, Nil);
      break;
    }

    /* binary operators */
  case CONS:
  case AND:
  case OR:
  case XOR:
  case XNOR:
  case IMPLIES:
  case IFF:
  case PLUS:
  case MINUS:
  case TIMES:
  case DIVIDE:
  case MOD:
  case LSHIFT:
  case RSHIFT:
  case LROTATE:
  case RROTATE:
  case LT:
  case GT:
  case LE:
  case GE:
  case UNION:
  case SETIN:
  case EQUAL:
  case NOTEQUAL:
  case CONCATENATION:
  case COLON: /* can be part of CASE or BIT_SELECTION only */
  case CASE:
  case WAREAD:
  case WAWRITE:
  case SLT: /* SMT expressions */
  case SGT:
  case SLE:
  case SGE:
  case SIGN_EXTEND:
    {
      node_ptr left  = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr right = compileFlattenSexpRecur(symb_table, cdr(sexp), context);
      result = new_node(node_get_type(sexp), left, right);
      break;
    }

  case BIT_SELECTION:
    {
      node_ptr base = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr bits = compileFlattenSexpRecur(symb_table, cdr(sexp), context);
      /* Later we may implement constant-exression (then here it will
         be computed) 
      */
      if (NUMBER != node_get_type(car(bits)) ||
          NUMBER != node_get_type(cdr(bits))) {
        fprintf(nusmv_stderr,
                "compileFlattenSexpRecur: bit-selection expression with"
                " not constant bit specifiers\n");
        error_reset_and_exit(1);
      }
      result = new_node(BIT_SELECTION, base, bits);
      break;
    }

  case TWODOTS:
    {
      /* We don't need to expand it, eval did it */
      result = sexp;
      break;
    }

    /* CTL Unary operators */
  case EX:
  case AX:
  case EF:
  case AF:
  case EG:
  case AG:
    {
      node_ptr body = compileFlattenSexpRecur(symb_table, car(sexp), context);

      result = new_node(node_get_type(sexp), body, Nil);
      break;
    }

    /* CTL bynary operators */
  case EU:
  case AU:
    {
      node_ptr left  = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr right = compileFlattenSexpRecur(symb_table, cdr(sexp), context);

      result = new_node(node_get_type(sexp), left, right);
      break;
    }

    /* CTL bounded Temporal Operators */
  case EBF:
  case ABF:
  case EBG:
  case ABG:
  case EBU:
  case ABU:
    {
      node_ptr body  = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr range = cdr(sexp);

      /* checks the range: */
      if (! Utils_check_subrange_not_negative(range) ) {
        error_invalid_subrange(range);
      }

      result = new_node(node_get_type(sexp), body, range);
      break;
    }

    /* LTL unary temporal operators */
  case OP_NEXT:
  case OP_PREC:
  case OP_NOTPRECNOT:
  case OP_FUTURE:
  case OP_ONCE:
  case OP_GLOBAL:
  case OP_HISTORICAL:
    {
      node_ptr body  = compileFlattenSexpRecur(symb_table, car(sexp), context);
      result = new_node(node_get_type(sexp), body, Nil);
      break;
    }

    /* LTL binary temporal operators */
  case UNTIL:
  case RELEASES:
  case SINCE:
  case TRIGGERED:
    {
      node_ptr left   = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr right  = compileFlattenSexpRecur(symb_table, cdr(sexp), context);

      result = new_node(node_get_type(sexp), left, right);
      break;
    }

    /* MIN and MAX operators */
  case MINU:
  case MAXU:
    {
      node_ptr left   = compileFlattenSexpRecur(symb_table, car(sexp), context);
      node_ptr right  = compileFlattenSexpRecur(symb_table, cdr(sexp), context);

      result = new_node(node_get_type(sexp), left, right);
      break;
    }
  case EQDEF:
    {
      node_ptr left  = car(sexp);
      node_ptr right = cdr(sexp) ;
      node_ptr res_left, res_right;

      switch (node_get_type(left)) {
      case SMALLINIT:
      case NEXT:
        {
          /* we are dealing with init(x) := init_expr or next(x) := next_expr */
          node_ptr name = compileFlattenSexpRecur(symb_table, car(left), context);
          if (node_get_type(name) == BIT_SELECTION) {
            error_bit_selection_assignment_not_supported(left);
          }

          if (! SymbTable_is_symbol_declared(symb_table, name)) {
            error_undefined(name);
          }
          res_left = new_node(node_get_type(left), name, Nil);
          res_right = compileFlattenSexpRecur(symb_table, right, context);
          break;
        }

      default:
        {
          /* we are dealing with x := simple_expr */
          res_left  = compileFlattenSexpRecur(symb_table, left, context);
          res_right = compileFlattenSexpRecur(symb_table, right, context);
          break;
        }
      }

      result = new_node(EQDEF, res_left, res_right);
      break;
    }

  default:
    fprintf(nusmv_stderr, 
            "compileFlattenSexpRecur: undefined node type (%d)\n",
            node_get_type(sexp));
    error_reset_and_exit(1);
  }
  
  nusmv_assert(result != Nil);
  yylineno = temp;
  return result;
}


/**Function********************************************************************

Synopsis           [Recursive definition of compileFlattenProcess]

Description        [Recursive definition of compileFlattenProcess.
If running is Nil there are no processes => no need to create
data structure with CASEs (for next-assignments).]

SideEffects        []

SeeAlso            []

******************************************************************************/
static void 
compileFlattenProcessRecur(const SymbTable_ptr symb_table, 
                           node_ptr assign, node_ptr context, node_ptr running,
                           FlatHierarchy_ptr flatHierarchy)
{
  if (assign == Nil) return;
  yylineno = node_get_lineno(assign);
  switch (node_get_type(assign)) {
  case CONS:
  case AND:
    compileFlattenProcessRecur(symb_table, car(assign), context, 
                               running, flatHierarchy);
    compileFlattenProcessRecur(symb_table, cdr(assign), context, 
                               running, flatHierarchy);
    break;

  case CONTEXT:
    compileFlattenProcessRecur(symb_table, cdr(assign), car(assign), 
                               running, flatHierarchy);
    break;

  case EQDEF: 
    {
      node_ptr vname, lhsa, stored;
      node_ptr left  = car(assign);
      node_ptr right = cdr(assign);

      switch (node_get_type(left)) {
      case SMALLINIT: /* init assignement */ {
        vname = CompileFlatten_resolve_name(symb_table, car(left), context);
        lhsa = find_node(node_get_type(left), vname, Nil);
        stored = FlatHierarchy_lookup_assign(flatHierarchy, lhsa);

        if (Nil != stored) error_redefining(vname);
      }
        break;
  
      case NEXT: /* next assignement */
        {
          vname = CompileFlatten_resolve_name(symb_table, car(left), context);
          lhsa = find_node(node_get_type(left), vname, Nil);
          stored = FlatHierarchy_lookup_assign(flatHierarchy, lhsa);
    
          /* there are processes => create CASE with "running" */
          if (Nil != proc_selector_internal_vname) {
            /* create default value for assignment, i.e. var name  */
            if (Nil == stored) stored = vname;
            /* create a CASE with running guard */
            right = new_node(CASE, new_node(COLON, running, right), stored);
          }
          else { /* no processes => no CASE things */
            if (Nil != stored) error_redefining(vname);
          }
        }
        break;
      
      default: 
        /* Invariant assignment */
        {
          vname = lhsa = CompileFlatten_resolve_name(symb_table, left, context);
          stored = FlatHierarchy_lookup_assign(flatHierarchy, lhsa);
          if (Nil != stored)  error_redefining(vname);
        }
      }
      FlatHierarchy_insert_assign(flatHierarchy, lhsa, right);
      
      
      break;
    } /* outer switch case EQDEF */
    
  default:
    internal_error("compileFlattenProcessRecur: type = %d", 
                   node_get_type(assign));
  } /* outer switch case */

}


/**Function********************************************************************

Synopsis           []

Description        [see compileFlattenSexpModel]

SideEffects        []

SeeAlso            []

******************************************************************************/
static void 
compileFlattenSexpModelAux(const SymbTable_ptr symb_table, 
                           const SymbLayer_ptr layer, 
                           node_ptr expr, int type,
                           FlatHierarchy_ptr flatHierarchy)
{
  int saved_yylineno = yylineno;

  if (expr == Nil) return;
  yylineno = node_get_lineno(expr);
  compileFlattenSexpModelRecur(symb_table, layer, expr, type, flatHierarchy);
  yylineno = saved_yylineno;
  return;
}

/**Function********************************************************************

Synopsis           []

Description        [see compileFlattenSexpModel]

SideEffects        []

SeeAlso            []

******************************************************************************/
static void compileFlattenSexpModelRecur(const SymbTable_ptr symb_table, 
                                         const SymbLayer_ptr layer, 
                                         node_ptr expr, int type,
                                         FlatHierarchy_ptr flatHierarchy)
{
  if (expr == Nil) return;

  if (node_get_type(expr) == AND) {
    compileFlattenSexpModelAux(symb_table, layer, car(expr), type, flatHierarchy);
    compileFlattenSexpModelAux(symb_table, layer, cdr(expr), type, flatHierarchy);
  }
  else {
    Set_t dep;
    dep = Formula_GetDependencies(symb_table, expr, Nil);

    if (Set_IsEmpty(dep)) {
      dep = compileFlattenConstantSexpCheck(layer, expr, type);
    }

    while (! Set_IsEmpty(dep)) {
      node_ptr var = (node_ptr) Set_GetFirst(dep);
      node_ptr index;

      switch (type) {
      case INIT: index = find_node(SMALLINIT, var, Nil); break;
      case INVAR: index = var; break;
      case TRANS: index = find_node(NEXT, var, Nil); break;
      default:
        fprintf(nusmv_stderr, "compileFlattenSexpModelRecur: Unknown expr type\n");
        error_reset_and_exit(1);
        break;
      }
      FlatHierarchy_add_constrains(flatHierarchy, index, expr);

      dep = Set_GetRest(dep);
    } /* while loop*/

  }
}


/**Function********************************************************************

Synopsis           [Called when a constant has been found in INVAR, INIT or 
TRANS]

Description        [If the constant is trivially true, it is skipped;
otherwise it will be associated to each fsm of all state variables occurring 
within the given layer]

SideEffects        [required]

SeeAlso            [optional]

******************************************************************************/
static Set_t compileFlattenConstantSexpCheck(const SymbLayer_ptr layer, 
                                             node_ptr expr, int type)
{
  Set_t res; 

  fprintf(nusmv_stderr, "Constant expression found in a %s statement in",
          type_to_string(type));
  start_parsing_err();
  fprintf(nusmv_stderr, " The expression is \"");
  print_node(nusmv_stderr, expr);
  fprintf(nusmv_stderr, "\"");

  if ( ((node_get_type(expr) == NUMBER) && (node_get_int(expr) == 1)) || 
       (node_get_type(expr) == TRUEEXP) ) {
    fprintf(nusmv_stderr, " (Skipped)\n");
    res = Set_MakeEmpty();
  }
  else {
    node_ptr vars; 

    fprintf(nusmv_stderr, "\n");
    vars = NodeList_to_node_ptr(SymbLayer_get_state_vars(layer));
    res = Set_Make(vars);
  }
  
  return res;
}


/**Function********************************************************************

Synopsis           [Creates the internal process selector variable, within 
the given layer]

Description        [Creates an input variable to denote 
the internal process selector, and the defines to denote
the corresponding 'running' symbols.

'process_name_list' is a list of existing processes names. If the list
contains just one element ("main") no variables and defines are
declared (no need). This happens if there is no "process" modules or
the modules were flattened (which also removes "process" things).

NB for developers: the internal process selector variable is by
default positioned at the top of the ordering. It is attached to
<tt>input_variables</tt> and <tt>all_variables</tt> too. ]

SideEffects        [<tt>input_variables</tt> and
<tt>all_variables</tt> are affected.]

SeeAlso            []

******************************************************************************/
static void 
create_process_symbolic_variables(SymbTable_ptr symb_table, 
                                  SymbLayer_ptr layer, 
                                  node_ptr process_name_list)
{
  /* the list of process always contain one element */
  nusmv_assert(CONS == node_get_type(process_name_list));

  /* there is just one module (main). Therefore, nothing should be done */
  if(Nil == cdr(process_name_list)) {
    /* during flattening "main" is denoted by Nil */
    nusmv_assert(Nil == car(process_name_list));
    
    return;
    /* Note that the symbols "_process_selector_" or "running" may be
       already defined. This happens, for example, if flattened module
       is read. But there is no need to care about it. If there are
       no processes then user can define its one _process_selector_ or
       running as usual symbols.
    */
  }
  
  /* -- There are several process -- */

  /* initialise the global variable with "process_selector" name */
  proc_selector_internal_vname = 
    find_node(DOT, Nil, sym_intern(PROCESS_SELECTOR_VAR_NAME));

  {
    /* internally "main" is denoted by Nil. change now Nil to "main". */
    node_ptr l = process_name_list;
    while (Nil != l && Nil != car(l)) l = cdr(l);
    
    /* there should always be a Nil element ("main" module)*/
    nusmv_assert(Nil != l);
    setcar(l, sym_intern("main"));
  }

  /* check that a user did not create its own  _process_selector_ */
  if (SymbTable_is_symbol_declared(symb_table, proc_selector_internal_vname)) {
    error_redefining_operational_symbol(proc_selector_internal_vname);
  }

  /* declare the "process-selector" symbol with a proper values */
  {
    SymbType_ptr symbolicType;

    flatten_declare_constants_within_list(process_name_list, layer);
    symbolicType = SymbType_create(SYMB_TYPE_ENUM, process_name_list);
    SymbLayer_declare_input_var(layer,
                                proc_selector_internal_vname,
                                symbolicType);
  }
  

  /* Declare DEFINES representing "running"s symbols */
  {
    node_ptr main_atom = sym_intern("main");
    node_ptr running_atom = sym_intern(RUNNING_SYMBOL);
    node_ptr iter;
    
    for (iter = process_name_list; iter != Nil; iter = cdr(iter)) {
      node_ptr module_name, def_name, def_body;

      module_name = car(iter);
      
      if (module_name == main_atom) {
        /* internally main is represented as Nil */
        def_name = CompileFlatten_resolve_name(symb_table, running_atom, Nil);
      }
      else def_name = CompileFlatten_resolve_name(symb_table, running_atom, 
                                                  module_name);

      /* check that the symbol has not been already defined */
      if (SymbTable_is_symbol_declared(symb_table, def_name)) {
        error_redefining_operational_symbol(def_name);
      }
      
      /* creating the body of DEFINE: _process_selector = Pi */
      def_body = find_node(EQUAL, proc_selector_internal_vname, module_name);

      /* The flatten hash has to be filled with the flattened
         body of the newly defined symbol. 
      */
      insert_flatten_def_hash(def_name, 
                              Compile_FlattenSexp(symb_table, def_body, Nil));

      /* declare the define: */
      SymbLayer_declare_define(layer, def_name, Nil /*context*/, def_body); 
    } /* for */
  }
}


/**Function********************************************************************

Synopsis           [required]

Description        [optional]

SideEffects        []

SeeAlso            [optional]

******************************************************************************/
static const char* type_to_string(int type) 
{
  switch (type) {
  case INIT:
    return "INIT";
  case INVAR:
    return "INVAR";
  case TRANS:
    return "TRANS";
  default:
    nusmv_assert(false); 
  }
}


/**Function********************************************************************

Synopsis [Performs the recursive step of
<code>CompileFlatten_resolve_name</code>. Returns TYPE_ERROR if not
resolvable name]

Description        [Performs the recursive step of
<code>CompileFlatten_resolve_name</code>.]

SideEffects        []

SeeAlso            [CompileFlatten_resolve_name]

******************************************************************************/
static node_ptr 
compile_flatten_resolve_name_recur(const SymbTable_ptr symb_table, 
                                   node_ptr n, node_ptr context)
{
  node_ptr temp, name;

  yylineno = node_get_lineno(n);
  switch (node_get_type(n)) {

  case CONTEXT: return compile_flatten_resolve_name_recur(symb_table, 
                                                          cdr(n), car(n));

  case ATOM:
    name = find_node(DOT, context, find_atom(n));
    temp = lookup_param_hash(name);

    if (temp != (node_ptr) NULL) {
      temp = compile_flatten_resolve_name_recur(symb_table, temp, context);
      if (temp == TYPE_ERROR) {
        nusmv_assert(0);
        rpterr("type error, module parameter = %s",
               get_text((string_ptr) car(n)));
      }
      return temp;
    }
    else return name;
    
  case NUMBER: return find_atom(n);

  case BIT: 
    temp = compile_flatten_resolve_name_recur(symb_table, car(n), context);
    if (temp == TYPE_ERROR) { rpterr("type error, operator bit"); }
    return find_node(BIT, temp, cdr(n)); /* cdr(n) is a int */ 

  case DOT:
    temp = (node_ptr) NULL;
    if (car(n) != (node_ptr) NULL) {
      temp = compile_flatten_resolve_name_recur(symb_table, car(n), context);
      if (temp == TYPE_ERROR) { rpterr("type error, operator = ."); }
    }
    return find_node(DOT, temp, find_atom(cdr(n)));

  case ARRAY:
    {
      node_ptr idx;

      temp = compile_flatten_resolve_name_recur(symb_table, car(n), context);
      if (temp == TYPE_ERROR) { rpterr("type error, operator = []"); }

      idx = CompileFlatten_resolve_number(symb_table, cdr(n), context);
      if (idx == (node_ptr) NULL) {
        fprintf(nusmv_stderr, "For array '");
        print_node(nusmv_stderr, car(n));
        fprintf(nusmv_stderr, "':\n");
        rpterr("A constant number was expected as array index"); 
      }
      return find_node(ARRAY, temp, idx);
    }

  case BIT_SELECTION:
    {
      node_ptr name;
      node_ptr t1, t2;
      name = compile_flatten_resolve_name_recur(symb_table, car(n), context);
      if (name == TYPE_ERROR) return TYPE_ERROR;

      nusmv_assert(node_get_type(cdr(n)) == COLON);
      t1 = compile_flatten_resolve_name_recur(symb_table, car(cdr(n)), context);
      if (t1 == TYPE_ERROR) return TYPE_ERROR;

      t2 = compile_flatten_resolve_name_recur(symb_table, cdr(cdr(n)), context);
      if (t2 == TYPE_ERROR) return TYPE_ERROR;

      return find_node(BIT_SELECTION, name, find_node(COLON, t1, t2));
    }

  case SELF: return context;

  default: 
    return TYPE_ERROR;
  }
}


/**Function********************************************************************

Synopsis [Simple evaluator for positive and negative constant
numbers]

Description        [Returns a NUMBER node, or Nil n is not a number. 
This is a private service of function CompileFlatten_resolve_number]

SideEffects        []

SeeAlso            []

******************************************************************************/
static node_ptr compile_flatten_eval_number(node_ptr n, boolean pos)
{
  if (node_get_type(n) == NUMBER) {
    if (pos) return find_atom(n);
    else return find_node(NUMBER, NODE_FROM_INT(-node_get_int(n)), Nil);
  }

  if (node_get_type(n) == UMINUS) {
    return compile_flatten_eval_number(car(n), !pos);
  }

  return Nil;
}


/**Function********************************************************************

Synopsis [Traverses the list of values, and declare all constants (leaves) it
finds]

Description        [Constnats will occur within the given layer]

SideEffects        []

SeeAlso            []

******************************************************************************/
static void flatten_declare_constants_within_list(node_ptr value_list, 
                                                  SymbLayer_ptr layer) 
{
  while (value_list != Nil) {
    node_ptr name = car(value_list);
    
    if (node_get_type(name) == DOT) {
      name = CompileFlatten_resolve_name(NULL, name, Nil);
    }
    else name = find_atom(name);
    
    if (SymbLayer_can_declare_constant(layer, name)) {
      SymbLayer_declare_constant(layer, name);
    }
    value_list = cdr(value_list);
  }
}

