/**CFile***********************************************************************

  FileName    [compileCone.c]

  PackageName [compile]

  Synopsis    [Computes the cone of influence of the model variables.]

  Description [This file contains the functions needed for computing
  the cone of influence (COI) of a given formula. The COI of all the
  variables in the model is pre-computed ancd cached the first time
  a cone of influence is required (function <code>initCoi</code>.
  Functions are also provided that compute the dependency variables
  for a formula, namely those variables that appear in the formula
  or in one of the definitions the formula depends on.]

  SeeAlso     []

  Author      [Marco Roveri and Marco Pistore]

  Copyright   [
  This file is part of the ``compile'' package of NuSMV version 2.
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

******************************************************************************/

#include "compileInt.h"

#include "compile/symb_table/SymbTable.h"

#include "set/set.h"
#include "utils/ustring.h"
#include "utils/assoc.h"
#include "parser/symbols.h"
#include "utils/error.h"
#include "enc/enc.h"

static char rcsid[] UTIL_UNUSED = "$Id: compileCone.c,v 1.11.4.12.4.15.4.8 2007/05/16 15:14:43 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**Macro***********************************************************************

  Synopsis     [Indicates that the dependency computation is ongoing.]

  Description  [The value used during the building of dependencies of
  defined symbols to keep track that compuation is ongoing to discover
  circular definitions.]

  SideEffects  []

  SeeAlso      []

******************************************************************************/
#define BUILDING_DEP_SET (Set_t)-10

/**Macro***********************************************************************

  Synopsis     [Indicates that the COI computation should be verbose.]

  Description  [Indicates that the COI computation should be verbose.]

  SideEffects  []

  SeeAlso      []

******************************************************************************/
#define COI_VERBOSE (opt_verbose_level_gt(options, 2))



/**Macro***********************************************************************

  Synopsis     [Clears and frees (with entries) given hash]

  Description  []

  SeeAlso      []

******************************************************************************/
#define FREE_HASH_ENTRIES(hash, func) \
  if (hash != (hash_ptr) NULL) {      \
    clear_assoc_and_free_entries(hash, func); \
    free_assoc(hash);                         \
    hash = (hash_ptr) NULL;                   \
  }


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

  Synopsis    [The hash of cone for a variable]

  Description [This hash associates to a variable the corresponding
  cone of influence, once it has been computes.]

  SeeAlso     []

******************************************************************************/
static hash_ptr coi_hash = (hash_ptr) NULL;
void init_coi_hash() {
  if (coi_hash == (hash_ptr) NULL) {
    coi_hash = new_assoc();
    nusmv_assert(coi_hash != (hash_ptr) NULL);
  }
}
void insert_coi_hash(node_ptr key, Set_t value) {
  nusmv_assert(coi_hash != (hash_ptr)NULL);
  insert_assoc(coi_hash, key, (node_ptr)value);
}
Set_t lookup_coi_hash(node_ptr key) {
  nusmv_assert(coi_hash != (hash_ptr)NULL);
  return (Set_t) find_assoc(coi_hash, key);
}

static assoc_retval coi_hash_free(char *key, char *data, char * arg)
{
  Set_t element = (node_ptr)data;

  if (element != (Set_t)NULL) {
    Set_ReleaseSet(element);
  }
  return ASSOC_DELETE;
}

void clear_coi_hash() {
  FREE_HASH_ENTRIES(coi_hash, coi_hash_free);
}


/**Variable********************************************************************

  Synopsis    [The hash of dependencies for defined symbols]

  Description [This hash associates to a defined atom the
  corresponding set]

  SeeAlso     []

******************************************************************************/
static hash_ptr define_dep_hash = (hash_ptr)NULL;
void init_define_dep_hash() {
  define_dep_hash = new_assoc();
  nusmv_assert(define_dep_hash != (hash_ptr)NULL);
}
void insert_define_dep_hash(node_ptr key, Set_t value) {
  nusmv_assert(define_dep_hash != (hash_ptr)NULL);
  insert_assoc(define_dep_hash, key, (node_ptr)value);
}
Set_t lookup_define_dep_hash(node_ptr key) {
  nusmv_assert(define_dep_hash != (hash_ptr)NULL);
  return (Set_t) find_assoc(define_dep_hash, key);
}

static assoc_retval define_dep_hash_free(char *key, char *data, char * arg)
{
  Set_t element = (node_ptr)data;

  if ((element != (Set_t) NULL) && (element != BUILDING_DEP_SET)) {
    Set_ReleaseSet(element);
  }
  return ASSOC_DELETE;
}

void clear_define_dep_hash() 
{
  FREE_HASH_ENTRIES(define_dep_hash, define_dep_hash_free);
}


/**Variable********************************************************************

  Synopsis    [The hash of dependencies for a given formula.]

  Description [This hash associates to each formula the corresponding
  set of dependencies.]

  SeeAlso     []

******************************************************************************/
static  hash_ptr dependencies_hash = (hash_ptr)NULL;
void init_dependencies_hash() {
  dependencies_hash = new_assoc();
  nusmv_assert(dependencies_hash != (hash_ptr)NULL);
}
void insert_dependencies_hash(node_ptr key, Set_t value) {
  nusmv_assert(dependencies_hash != (hash_ptr)NULL);
  insert_assoc(dependencies_hash, key, (node_ptr)value);
}
Set_t lookup_dependencies_hash(node_ptr key) {
  nusmv_assert(dependencies_hash != (hash_ptr)NULL);
  return (Set_t) find_assoc(dependencies_hash, key);
}

static assoc_retval dependencies_hash_free(char *key, char *data, char * arg)
{
  Set_t element = (node_ptr)data;

  if (element != (Set_t)NULL) {
    Set_ReleaseSet(element);
  }
  return ASSOC_DELETE;
}

void clear_dependencies_hash() {
  FREE_HASH_ENTRIES(dependencies_hash, dependencies_hash_free);
}

static node_ptr mk_hash_key(node_ptr e, node_ptr c, VarFilterType filter) {
  return find_node(CONS,find_node(CONTEXT, c, e),(node_ptr) filter);
}

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static Set_t 
formulaGetDependenciesRecur ARGS((const SymbTable_ptr, 
				  node_ptr, node_ptr, VarFilterType));


static Set_t 
formulaGetDefinitionDependencies ARGS((const SymbTable_ptr, node_ptr, 
                                       VarFilterType));

static void 
coiInit ARGS((const SymbTable_ptr symb_table, FlatHierarchy_ptr hierarchy));

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Computes dependencies of a given SMV expression]

  Description        [The set of dependencies of a given formula are
  computed. A traversal of the formula is performed. Each time a
  variable is encountered, it is added to the so far computed
  set. When a formula depends on a next variable, then the
  corresponding current variable is added to the set. When an atom is
  found a call to <tt>formulaGetDefinitionDependencies</tt> is
  performed to compute the dependencies.]

  SideEffects        []

  SeeAlso            [formulaGetDefinitionDependencies]

******************************************************************************/
Set_t Formula_GetDependencies(const SymbTable_ptr symb_table, 
			      node_ptr formula, node_ptr context)
{
  return Formula_GetDependenciesByType(symb_table, formula, context, VFT_ALL);
}

/**Function********************************************************************

  Synopsis           [Computes the dependencies of an SMV expression by type]

  Description        [The set of dependencies of a given formula are
  computed, as in Formula_GetDependencies, but the variable type filters the
  dependency collection:
    <dl>
    <dt> <tt>VFT_CURRENT</tt>
       <dd> filters the current variables</dd>
    </dt>
    <dt> <tt>VFT_NEXT</tt>
       <dd> filters out the next variables</dd>
    </dt>
    <dt> <tt>VFT_INPUT</tt>
       <dd> filters out the input variables</dd>
    </dt>
    <dt> <tt>VFT_STATE</tt>
       <dd> filters out the state variables</dd>
    </dt>
    <dt> <tt>VFT_ALL</tt>
       <dd>filters out all the variables</dd>
    </dt>
  </dl>
  Combined modes can be obtained by bit-or: for example VFT_NEXT | VFT_INPUT 
  will enable the search for the variables which are next or input variables.]

  SideEffects        []

  SeeAlso            [formulaGetDependenciesByTypeAux 
formulaGetDefinitionDependencies]

******************************************************************************/
Set_t 
Formula_GetDependenciesByType(const SymbTable_ptr symb_table, 
                              node_ptr formula, node_ptr context, 
                              VarFilterType filter)
{
  Set_t result;
  int temp = yylineno;
  node_ptr hash_key;
  
  if (formula == Nil) return Set_MakeEmpty();
  
  hash_key = mk_hash_key(formula, context, filter);
  result = lookup_dependencies_hash(hash_key);
  if (result == (Set_t)NULL) {
    yylineno = node_get_lineno(formula);
    result = formulaGetDependenciesRecur(symb_table, formula, context, filter);
    yylineno = temp;
    insert_dependencies_hash(hash_key, result);
  }
  return result;  
}

/**Function********************************************************************

  Synopsis           [Compute the dependencies of two set of formulae]

  Description [Given a formula and a list of fairness constraints, the
  set of variables occurring in them is computed.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
Set_t Formulae_GetDependencies(const SymbTable_ptr symb_table, 
			       node_ptr formula, 
			       node_ptr justice, node_ptr compassion)
{
  Set_t result;
  Set_t result1, result2, result3;

  result1 = Formula_GetDependencies(symb_table, formula, Nil);
  result2 = Formula_GetDependencies(symb_table, justice, Nil);
  result3 = Formula_GetDependencies(symb_table, compassion, Nil);

  result = Set_Union(Set_Union(result1, result2),result3);
  return result;
}


/**Function********************************************************************

  Synopsis           [Computes the COI of a given set of variables, defined
  within the given symb_table]

  Description        [Computes the COI of a given set of variables, defined
  within the given symb_table]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
Set_t ComputeCOI(const SymbTable_ptr symb_table, Set_t base)
{
  Set_t coi = base;

  if (! cmp_struct_get_coi(cmps)) {
    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "Initializing Cone Of Influence...\n");
    }

    coiInit(symb_table, mainFlatHierarchy);
    cmp_struct_set_coi(cmps);

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stderr, "COI initialized.\n");
    }
  }

  while (Set_IsEmpty(base) == false) {
    node_ptr name = Set_GetFirst(base);
    coi = Set_Union(coi, lookup_coi_hash(name));
    base = Set_GetRest(base);
  }
  return coi;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Compute the dependencies of an atom]

  Description [This function computes the dependencies of an atom. If
  the atom corresponds to a variable then the singleton with the
  variable is returned. If the atom corresponds to a "running"
  condition the singleton with variable PROCESS_SELECTOR_VAR_NAME is
  returned. Otherwise if the atom corresponds to a defined symbol the
  dependency set corresponding to the body of the definition is
  computed and returned. filter specifies what variables we are
  interested to, as in Formula_GetDependenciesByType, and
  is_inside_next is supposed to be true if the atom is inside a Next,
  false otherwise.]

  SideEffects        [The <tt>define_dep_hash</tt> is modified in
  order to memoize previously computed dependencies of defined symbols.]

  SeeAlso            [Formula_GetDependencies]

******************************************************************************/

static Set_t 
formulaGetDefinitionDependencies(const SymbTable_ptr symb_table, 
                                 node_ptr formula, VarFilterType filter)
{
  Set_t result = Set_MakeEmpty();

  if (SymbTable_is_symbol_var(symb_table, formula)) {
    if ( ( (filter & VFT_INPUT) && SymbTable_is_symbol_input_var(symb_table,
                                                                 formula)) ||
         ( (filter & VFT_CURRENT) && SymbTable_is_symbol_state_var(symb_table,
                                                                 formula))) {
      return Set_MakeSingleton((Set_Element_t) formula);
    }
    else
      return Set_MakeEmpty();	  
  } /* end of outer if branch */  
  else if (SymbTable_is_symbol_define(symb_table, formula)) {
    Set_t res = lookup_define_dep_hash(formula);
    if (res == BUILDING_DEP_SET) { error_circular(formula); }
    
    if (res == (Set_t) NULL) {
      /* We mark the formula as open and we start looking for the body
	 dependencies. */
      insert_define_dep_hash(formula, BUILDING_DEP_SET);
      io_atom_push(formula);
      res = Formula_GetDependenciesByType(symb_table, 
                            SymbTable_get_define_body(symb_table, formula), 
                            SymbTable_get_define_context(symb_table, formula),
                            filter);
      io_atom_pop();
      
      /* We mark the formula as closed, storing the computed
	 dependencies for further use. */
      insert_define_dep_hash(formula, res);
    }

    result = res;
  }
  else {
    fprintf(nusmv_stderr, "Undefined symbol \"");
    print_node(nusmv_stderr, formula);
    fprintf(nusmv_stderr, "\"\n");
    nusmv_exit(1);
  }    
  
  return result;
}

/**Function********************************************************************

  Synopsis           [Recursive call to Formula_GetDependenciesByType]

  Description        [Recursive call to Formula_GetDependenciesByType]

  SideEffects        []

  SeeAlso            [formulaGetDefinitionDependencies 
Formula_GetDependenciesByType]

******************************************************************************/
static Set_t 
formulaGetDependenciesRecur(const SymbTable_ptr symb_table, 
			    node_ptr formula, node_ptr context,
			    VarFilterType filter)
{
  if (formula == Nil) return Set_MakeEmpty();

  switch (node_get_type(formula)) {
  case CONTEXT:
    return Formula_GetDependenciesByType(symb_table, cdr(formula), 
                                         car(formula), filter);

  case BIT:
    /* ignore bits, consider only scalar vars */
    return Formula_GetDependenciesByType(symb_table, car(formula), 
                                         context, filter);    

  case ATOM:
    {
      node_ptr name = find_node(DOT, context, find_atom(formula));
      node_ptr param = lookup_param_hash(name);
      boolean is_constant = 
	SymbTable_is_symbol_constant(symb_table, find_atom(formula));

      if (!is_constant) {
	/* try with a flatten version of atom: */
	is_constant = SymbTable_is_symbol_constant(symb_table, name);
      }

      if ((param  != (node_ptr) NULL) && is_constant) {
	error_ambiguous(formula);
      }
      if (param != (node_ptr) NULL) {
        return Formula_GetDependenciesByType(symb_table, param, context, 
					     filter);
      }
      if (is_constant) return Set_MakeEmpty();
     
      /* it should be a defined symbol, running, or a variable */
      return formulaGetDefinitionDependencies(symb_table, 
               CompileFlatten_resolve_name(symb_table, formula, context),
                                              filter);
    }      

    /* a variable of word type */
  case WORD:
    /* ignore width (a constant), consider only bits */
    return Formula_GetDependenciesByType(symb_table, car(formula), 
                                         context, filter);

    /* no dependencies */
  case FAILURE:
  case TRUEEXP:
  case FALSEEXP:
  case NUMBER:
  case NUMBER_FRAC:
  case NUMBER_REAL:
  case NUMBER_EXP:
  case NUMBER_WORD:
  case TWODOTS: /* Sets */
    return Set_MakeEmpty();

    /* unary operation */
  case NEXT:   /* next(alpha), with alpha possibly complex, thus ... */
    /* it is guaranteed that no nested next exist  */
    if(filter & VFT_NEXT) {
      /* the next variables become the current varibales from this frame on */
      filter=(filter & (~VFT_NEXT))|VFT_CURRENT;
      return Formula_GetDependenciesByType(symb_table, car(formula), context,
                                           filter); 
    }
    else
      return Set_MakeEmpty();
    
  case NOT:    /* Unary boolean connectives */
  case UMINUS:
  case EX:    /* CTL unary Temporal Operators */
  case SMALLINIT:  /* used for init(expr) */
  case AX:
  case EF:
  case AF:
  case EG:
  case AG:
  case EBF:    /* CTL unary bounded Temporal Operators */
  case ABF:
  case EBG:
  case ABG:
  case EBU:    /* CTL binary bounded Temporal Operators */
  case ABU:
  case OP_NEXT:    /* LTL unary Temporal Operators */
  case OP_PREC:
  case OP_NOTPRECNOT:
  case OP_FUTURE:
  case OP_ONCE:
  case OP_GLOBAL:
  case OP_HISTORICAL:
  case CAST_BOOL: /* Casts */
  case CAST_WORD1:
    return Formula_GetDependenciesByType(symb_table, car(formula), context,
                                         filter);
 
    /* binary operation */
  case EQDEF: /* assignment */
  case CONS:
  case UNION:
  case SETIN:
  case COLON:
  case PLUS:    /* Numerical Operations */
  case MINUS:
  case TIMES:
  case DIVIDE:
  case MOD:
  case LSHIFT:  /* Binary shifts and rotates */
  case RSHIFT:
  case LROTATE:
  case RROTATE:
  case CONCATENATION: /* concatenation */
  case EQUAL:   /* Comparison Operations */
  case NOTEQUAL:
  case LT:
  case GT:
  case LE:
  case GE:
  case AND:    /* Binary boolean connectives */
  case OR:
  case XOR:
  case XNOR:
  case IMPLIES:
  case IFF:
  case EU:     /* CTL binary  Temporal Operators */
  case AU:
  case UNTIL:    /* LTL binary Temporal Operators */
  case RELEASES:
  case SINCE:
  case TRIGGERED:
  case MAXU:    /* MIN MAX operators */
  case MINU:
   {
      Set_t left = Formula_GetDependenciesByType(symb_table, car(formula), 
                                               context, filter);
      Set_t right = Formula_GetDependenciesByType(symb_table, cdr(formula), 
                                               context, filter);
      return Set_Union(left, right);
    }

   /* 3-arity operations */
  case CASE:
  case IFTHENELSE:
    {
      Set_t partial;
      Set_t condition = Formula_GetDependenciesByType(symb_table, 
                                                car(car(formula)),context, 
                                                filter);
      Set_t then_arg  = Formula_GetDependenciesByType(symb_table, 
                                                cdr(car(formula)), context, 
                                                filter);
      Set_t else_arg  = Formula_GetDependenciesByType(symb_table, 
                                                cdr(formula), context, filter);

      partial = Set_Union(condition, then_arg);
      return Set_Union(partial, else_arg);
    }

  case BIT_SELECTION:
    {
      Set_t object   = Formula_GetDependenciesByType(symb_table, 
                                                     car(formula), context, 
                                                     filter);
      Set_t high_bit = Formula_GetDependenciesByType(symb_table, 
                                                     car(cdr(formula)), context, 
                                                     filter);
      Set_t low_bit  = Formula_GetDependenciesByType(symb_table, 
                                                     cdr(cdr(formula)), context, 
                                                     filter);
      return Set_Union(Set_Union(object, high_bit), low_bit);
    }

    /* Operations on WORDARRAYs */
  case WAWRITE:
    {
      Set_t memory   = Formula_GetDependenciesByType(symb_table, 
                                                     car(formula), context, 
                                                     filter);
      Set_t location = Formula_GetDependenciesByType(symb_table, 
                                                     car(cdr(formula)), context, 
                                                     filter);
      Set_t address  = Formula_GetDependenciesByType(symb_table, 
                                                     cdr(cdr(formula)), context, 
                                                     filter);
      return Set_Union(Set_Union(memory, location), address);
    }

  case WAREAD:
    {
      Set_t memory   = Formula_GetDependenciesByType(symb_table, 
                                                     car(formula), context, 
                                                     filter);
      Set_t location = Formula_GetDependenciesByType(symb_table, 
                                                     cdr(formula), context, 
                                                     filter);
      return Set_Union(memory, location);
    }


    /* name cases */
  case DOT:
  case ARRAY:
    {
      node_ptr name = CompileFlatten_resolve_name(symb_table, formula, context);
              
      if (SymbTable_is_symbol_constant(symb_table, name)) {
        return Set_MakeEmpty();
      }
      else return formulaGetDefinitionDependencies(symb_table, name, filter);
    }

  default:
    {
      fprintf(nusmv_stderr,
        "Formula_GetDependencies: Reached undefined connective (%d)\n",
        node_get_type(formula));
      nusmv_exit(1);
      break;
    }
  }
  return Set_MakeEmpty();
}


/**Function********************************************************************

  Synopsis           [Pre-compute the COI of the variables]

  Description        [Computes the COI of all the variables occurring within 
  the symbol table]

  SideEffects        []

  SeeAlso            [ComputeCOI]

******************************************************************************/
static void coiInit(const SymbTable_ptr symb_table, FlatHierarchy_ptr hierarchy)
{
  Set_t nonassign_vars = Set_MakeEmpty();
  NodeList_ptr vars;
  ListIter_ptr iter;

  init_coi_hash(); /* no action is taken here if already initialized */

  if (COI_VERBOSE) { fprintf(nusmv_stdout,"*** INIT COI ***\n"); }

  vars = SymbTable_get_vars(symb_table); 
  for (iter = NodeList_get_first_iter(vars); 
       ! ListIter_is_end(iter); iter = ListIter_get_next(iter)) {
    node_ptr var = NodeList_get_elem_at(vars, iter);

    node_ptr invar_var = var;
    node_ptr init_var = find_node(SMALLINIT, var, Nil);
    node_ptr next_var = find_node(NEXT, var, Nil);

    node_ptr invar_a= FlatHierarchy_lookup_assign(hierarchy, invar_var);
    node_ptr invar_c= FlatHierarchy_lookup_constrains(hierarchy, invar_var);
    node_ptr init_a = FlatHierarchy_lookup_assign(hierarchy, init_var);
    node_ptr init_c = FlatHierarchy_lookup_constrains(hierarchy, init_var);
    node_ptr next_a = FlatHierarchy_lookup_assign(hierarchy, next_var);
    node_ptr next_c = FlatHierarchy_lookup_constrains(hierarchy, next_var);

 
    Set_t base = Set_MakeSingleton(var);
    int nonassign = 0;

    if (Nil != invar_a) {
      base = Set_Union(Formula_GetDependencies(symb_table, invar_a, Nil), base);
    }
    if (Nil != invar_c) {
      nonassign = 1;
      base = Set_Union(Formula_GetDependencies(symb_table, invar_c, Nil), base);
    }
    
    if (Nil != init_a) {
      base = Set_Union(Formula_GetDependencies(symb_table, init_a, Nil), base);
    }
    if (Nil != init_c) {
      nonassign = 1;
      base = Set_Union(Formula_GetDependencies(symb_table, init_c, Nil), base);
    }

    if (Nil != next_a) {
      base = Set_Union(Formula_GetDependencies(symb_table, next_a, Nil), base);
    }
    if (Nil != next_c) {
      nonassign = 1;
      base = Set_Union(Formula_GetDependencies(symb_table, next_c, Nil), base);
    }

    insert_coi_hash(var, base);

    if (nonassign) {
      nonassign_vars = Set_AddMember(nonassign_vars, var);
    }

    if (COI_VERBOSE) {
      fprintf(nusmv_stdout,"Variable  ");
      print_node(nusmv_stdout, var);
      fprintf(nusmv_stdout,"\n");

      if (nonassign) {
	fprintf(nusmv_stdout,"  Has non-assign constraints\n");
      }

      fprintf(nusmv_stdout,"  Initial coi: ");
      print_node(nusmv_stdout,base);
      fprintf(nusmv_stdout,"\n");
    }
  } /* vars iteration */

  {
    boolean changed = true;
    while (changed) {
      
      if (COI_VERBOSE) {
	fprintf(nusmv_stdout,"*** ITERATE COI ***\n");
      }

      changed = false;

      for (iter = NodeList_get_first_iter(vars); 
	   ! ListIter_is_end(iter); iter = ListIter_get_next(iter)) {

	node_ptr var = NodeList_get_elem_at(vars, iter);

	Set_t coi = lookup_coi_hash(var);
	Set_t old_coi = coi;

	Set_t tmp;

	int nonassign = Set_IsMember(nonassign_vars, var);

	if (COI_VERBOSE) {
	  fprintf(nusmv_stdout,"Variable ");
	  print_node(nusmv_stdout, var);
	  fprintf(nusmv_stdout,"\n");

	  if (nonassign) {
	    fprintf(nusmv_stdout,"  Has non-assign constraints\n");
	  }

	  fprintf(nusmv_stdout,"  Old coi: ");
	  print_node(nusmv_stdout, old_coi);
	  fprintf(nusmv_stdout,"\n");
	}

	for (tmp = old_coi; ! Set_IsEmpty(tmp); tmp = Set_GetRest(tmp)) {
	  node_ptr name = (node_ptr) Set_GetFirst(tmp);
	  coi = Set_Union(coi, lookup_coi_hash(name));;
	}

	if (coi != old_coi) {
	  changed = true;
	  insert_coi_hash(var, coi);
	  
	  if (COI_VERBOSE) {
	    fprintf(nusmv_stdout,"  New coi: ");
	    print_node(nusmv_stdout, coi);
	    fprintf(nusmv_stdout,"\n");
	  }
	}
	else {
	  if (COI_VERBOSE) {
	    fprintf(nusmv_stdout,"  No changes\n");
	  }
	}

	if (nonassign) {
	  for (tmp = coi; ! Set_IsEmpty(tmp); tmp = Set_GetRest(tmp)) {
	    node_ptr name = (node_ptr) Set_GetFirst(tmp);

	    if (! Set_IsMember(nonassign_vars, name)) {
	      if (COI_VERBOSE) {
		fprintf(nusmv_stdout,
			"  Found non-assignment constraints for ");
		print_node(nusmv_stdout,name);
		fprintf(nusmv_stdout,"\n");
	      }
	      nonassign_vars = Set_AddMember(nonassign_vars, name);
	      {
		Set_t other_coi = lookup_coi_hash(name);

		if (! Set_IsMember(other_coi, var)) {
		  changed = true;
		  other_coi = Set_AddMember(other_coi, var);
		  insert_coi_hash(name, other_coi);
		}
	      }

	    }
	  } /* for loop */
	}

      } /* for loop */
    } /* while loop */

    if (COI_VERBOSE) {
      fprintf(nusmv_stdout,"*** END COI ***\n");
    }
  }
}

