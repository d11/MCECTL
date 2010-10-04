/**CFile*****************************************************************

  FileName    [SymbCache.c]

  PackageName [compile.symb_table]

  Synopsis    [The SymbCache class implementation]

  Description []

  SeeAlso     [SymbCache.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``compile.symb_table'' package of NuSMV
  version 2.  Copyright (C) 2004 by ITC-irst.

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

#include "symb_table_int.h"

#include "SymbCache.h"
#include "SymbCache_private.h"

#include "compile/compile.h"
#include "parser/symbols.h"
#include "utils/assoc.h"
#include "utils/error.h"


static char rcsid[] UTIL_UNUSED = "$Id: SymbCache.c,v 1.1.2.19.6.2 2007/04/20 12:41:29 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/**Struct**********************************************************************

  Synopsis    [The SymbCache class]

  Description []
  
******************************************************************************/
typedef struct SymbCache_TAG
{
  /* Uses the symbol table to flat DEFINEs bodies */
  SymbTable_ptr symb_table;

  /* Cache for symbols names. Contains node of these kinds:
     for variables    VAR, IVAR: available slot, type
     for DEFINEs      CONTEXT: ctx, definition body 
     for constants:   NUMBER: num of instances, Nil
  */
  hash_ptr symbol_hash;

  /* lists */
  NodeList_ptr constants;
  NodeList_ptr defines;

  NodeList_ptr state_vars;
  NodeList_ptr input_vars;
  NodeList_ptr vars;

  /* Symbols (variables and DEFINEs) */
  NodeList_ptr state_symbols;
  NodeList_ptr input_symbols; 
  NodeList_ptr state_input_symbols;

  /* DEFINEs whose body was not fully declared when the DEFINE was
     declared. These will be later processed when the set of symbols is
     required. */
  NodeList_ptr pending_defines; 

  /* Several operation with the same key are performed on the symbol
     hash.  To improve performance, the last operated key and the
     associated value are stored separately here. */
  node_ptr last_symbol_hash_key; 
  node_ptr last_symbol_hash_value; 
   
} SymbCache;


/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void symb_cache_init ARGS((SymbCache_ptr self, 
                                  SymbTable_ptr symb_table));

static void symb_cache_deinit ARGS((SymbCache_ptr self));

static void 
symb_cache_new_symbol ARGS((SymbCache_ptr self, node_ptr name, 
                            node_ptr value));

static node_ptr symb_cache_lookup_symbol ARGS((const SymbCache_ptr self, 
                                               const node_ptr name));

static void
symb_cache_define_to_symbols_lists ARGS((SymbCache_ptr self, node_ptr define));

static void symb_cache_resolve_pending_defines ARGS((SymbCache_ptr self));

static void 
symb_cache_remove_symbol ARGS((SymbCache_ptr self, node_ptr name));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Class constructor]

  Description        [Callable only by the SymbTable instance that owns self.
  The caller keeps the ownership of given SymbTable instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbCache_ptr SymbCache_create(SymbTable_ptr symb_table)
{
  SymbCache_ptr self = ALLOC(SymbCache, 1);

  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_init(self, symb_table); 
  return self;  
}


/**Function********************************************************************

  Synopsis           [Class destructor]

  Description        [Callable only by the SymbTable instance that owns self.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_destroy(SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_deinit(self);
  FREE(self);  
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbCache_get_vars(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  return self->vars;
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared state variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbCache_get_state_vars(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  return self->state_vars;
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared input variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbCache_get_input_vars(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  return self->input_vars;
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared constants]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbCache_get_constants(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  return self->constants;
}


/**Function********************************************************************

  Synopsis           [Returns the list of all DEFINEs]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbCache_get_defines(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  return self->defines;
}


/**Function********************************************************************

  Synopsis [Returns the list of all state variables and those DEFINEs
  whose body refer directly or indirectly to state variables]

  Description [Only state variables and DEFINEs can occur within the
  returned list. Returned list belongs to self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbCache_get_state_symbols(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_resolve_pending_defines(self);
  return self->state_symbols;
}


/**Function********************************************************************

  Synopsis [Returns the list of all input variables and those DEFINEs
  whose body refer directly or indirectly to input variables]

  Description [Only input variables and DEFINEs can occur within the
  returned list. Returned list belongs to self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbCache_get_input_symbols(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_resolve_pending_defines(self);
  return self->input_symbols;
}


/**Function********************************************************************

  Synopsis [Returns the list of those DEFINEs whose body refer
  directly or indirectly to both state and input variables]

  Description [Only DEFINEs whose body contains both state and input
  vars can occur within the returned list. Returned list belongs to
  self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbCache_get_state_input_symbols(const SymbCache_ptr self)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_resolve_pending_defines(self);
  return self->state_input_symbols;
}


/**Function********************************************************************

  Synopsis           [Declares a new input variable.]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_new_input_var(SymbCache_ptr self, node_ptr var, 
                             SymbType_ptr type)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_new_symbol(self, var, new_node(IVAR, Nil, (node_ptr) type));
}


/**Function********************************************************************

  Synopsis           [Declares a new state variable.]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_new_state_var(SymbCache_ptr self, node_ptr var, 
                             SymbType_ptr type)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_new_symbol(self, var, new_node(VAR, Nil, (node_ptr) type));
}


/**Function********************************************************************

  Synopsis           [Removes a variable from the cache of symbols, and from 
  the flattener module]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_remove_var(SymbCache_ptr self, node_ptr var)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_var(self, var));

  symb_cache_remove_symbol(self, var);
}


/**Function********************************************************************

  Synopsis           [Declares a new DEFINE.]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_new_define(SymbCache_ptr self, node_ptr name, 
                          node_ptr ctx, node_ptr definition)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb_cache_new_symbol(self, name, new_node(CONTEXT, ctx, definition));
}


/**Function********************************************************************

  Synopsis           [Removes a DEFINE from the cache of symbols, and from 
  the flattener define hash]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_remove_define(SymbCache_ptr self, node_ptr define)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_define(self, define));

  symb_cache_remove_symbol(self, define);
}


/**Function********************************************************************

  Synopsis           [Declares a new constant.]

  Description [This (private) method can be used only by SymbLayer,
  otherwise the resulting status will be corrupted. Multiple-time 
  declared constant are accepted, and a reference count is kept to deal with 
  them]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_new_constant(SymbCache_ptr self, node_ptr name)
{
  node_ptr c;
  SYMB_CACHE_CHECK_INSTANCE(self);
  
  c = symb_cache_lookup_symbol(self, name); 
  
  if (c == Nil) {
    symb_cache_new_symbol(self, name, find_node(NUMBER, (node_ptr) 1, Nil));
  }
  else {
    int count; 
    nusmv_assert(node_get_type(c) == NUMBER);

    count = NODE_TO_INT(car(c));
    free_node(c);
    symb_cache_new_symbol(self, name, 
                          find_node(NUMBER, NODE_FROM_INT(count+1), Nil));
  }
}


/**Function********************************************************************

  Synopsis [Removes a constant from the cache of symbols, and from 
  the flattener module]

  Description [Removal is performed taking into account of reference
  counting, as constants can be shared among several layers. This
  (private) method can be used only by SymbLayer, otherwise the
  resulting status will be corrupted.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbCache_remove_constant(SymbCache_ptr self, node_ptr constant)
{
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_constant(self, constant));

  symb_cache_remove_symbol(self, constant);
}


/**Function********************************************************************

  Synopsis           [Returns the type of a given variable]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbType_ptr SymbCache_get_var_type(const SymbCache_ptr self, 
                                    const node_ptr name)
{
  node_ptr symb; 
 
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_var(self, name)); 
  
  symb = symb_cache_lookup_symbol(self, name);

  return SYMB_TYPE(cdr(symb));
}


/**Function********************************************************************

  Synopsis           [Returns the body of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr SymbCache_get_define_body(const SymbCache_ptr self, 
                                   const node_ptr name)
{
  node_ptr symb; 
 
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_define(self, name)); 
  
  symb = symb_cache_lookup_symbol(self, name);
  return cdr(symb);  
}


/**Function********************************************************************

  Synopsis           [Returns the flattenized body of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr 
SymbCache_get_define_flatten_body(const SymbCache_ptr self,
                                  const node_ptr name)
{
  node_ptr res;

  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_define(self, name));   

  set_definition_mode_to_expand();
  res = Flatten_GetDefinition(self->symb_table, name); 
  set_definition_mode_to_get();

  return res;
}


/**Function********************************************************************

  Synopsis           [Returns the context of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr SymbCache_get_define_context(const SymbCache_ptr self, 
                                      const node_ptr name)
{
  node_ptr symb; 
 
  SYMB_CACHE_CHECK_INSTANCE(self);
  nusmv_assert(SymbCache_is_symbol_define(self, name)); 
  
  symb = symb_cache_lookup_symbol(self, name);
  return car(symb);  
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is a state variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_state_var(const SymbCache_ptr self, 
                                      const node_ptr name)
{
  node_ptr symb;
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb = symb_cache_lookup_symbol(self, name);
  return (symb != (node_ptr) NULL) && (node_get_type(symb) == VAR); 
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is an input
  variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_input_var(const SymbCache_ptr self, 
                                      const node_ptr name)
{
  node_ptr symb;
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb = symb_cache_lookup_symbol(self, name);
  return (symb != (node_ptr) NULL) && (node_get_type(symb) == IVAR); 
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is either a state or
  an input variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_var(const SymbCache_ptr self, const node_ptr name)
{
  node_ptr symb;
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb = symb_cache_lookup_symbol(self, name);
  return (symb != (node_ptr) NULL) && ((node_get_type(symb) == VAR) 
                                       || (node_get_type(symb) == IVAR));
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is declared]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_declared(const SymbCache_ptr self, 
                                     const node_ptr name)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  return (symb_cache_lookup_symbol(self, name) != (node_ptr) NULL) || 
    SymbCache_is_symbol_constant(self, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is a declared
  constant]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_constant(const SymbCache_ptr self, 
                                     const node_ptr name)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  return NodeList_belongs_to(self->constants, name);
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is a declared
  DEFINE]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbCache_is_symbol_define(const SymbCache_ptr self, 
                                   const node_ptr name)
{
  node_ptr symb;
  SYMB_CACHE_CHECK_INSTANCE(self);

  symb = symb_cache_lookup_symbol(self, name);
  return (symb != (node_ptr) NULL) && (node_get_type(symb) == CONTEXT);
}


/**Function********************************************************************

  Synopsis [Returns true if var_list contains at least one input
  variable]

  Description        [The given list of variables is traversed until an input 
  variable is found]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean SymbCache_list_contains_input_var(const SymbCache_ptr self,
                                          const NodeList_ptr var_list)
{
  ListIter_ptr iter;  

  SYMB_CACHE_CHECK_INSTANCE(self);

  iter = NodeList_get_first_iter(var_list);
  while (! ListIter_is_end(iter)) {
    if (SymbCache_is_symbol_input_var(self, 
                      NodeList_get_elem_at(var_list, iter))) return true;

    iter = ListIter_get_next(iter);
  }

  return false;
}


/**Function********************************************************************

  Synopsis           [Returns true if var_list contains at least one state 
  variable]

  Description        [The given list of variables is traversed until 
  a state variable is found]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean SymbCache_list_contains_state_var(const SymbCache_ptr self,
                                          const NodeList_ptr var_list)
{
  ListIter_ptr iter;  

  SYMB_CACHE_CHECK_INSTANCE(self);

  iter = NodeList_get_first_iter(var_list);
  while (! ListIter_is_end(iter)) {
    if (SymbCache_is_symbol_state_var(self, 
              NodeList_get_elem_at(var_list, iter))) return true;

    iter = ListIter_get_next(iter);
  }

  return false;
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbols list contains 
  one or more undeclared variable names]

  Description        [Iterates through the elements in var_list
  checking each one to see if it is one undeclared variable.]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean SymbCache_list_contains_undef_var(const SymbCache_ptr self,
                                          const NodeList_ptr var_list)
{
  ListIter_ptr iter;  

  SYMB_CACHE_CHECK_INSTANCE(self);

  iter = NodeList_get_first_iter(var_list);
  while (! ListIter_is_end(iter)) {
    if (!SymbCache_is_symbol_var(self, 
                 NodeList_get_elem_at(var_list, iter))) return true;

    iter = ListIter_get_next(iter);
  }

  return false;
}


/*--------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis      [Insert a new value in the symbol hash]

  Description   [This takes into account also the caching of last accessed 
  value]
  
  SideEffects        []

******************************************************************************/
static void 
symb_cache_new_symbol(SymbCache_ptr self, node_ptr name, node_ptr value)
{  
  switch (node_get_type(value)) {
  case IVAR: /* Input var */
    /* not previously declared */
    nusmv_assert(symb_cache_lookup_symbol(self, name) == (node_ptr) NULL);

    NodeList_append(self->input_symbols, name);
    NodeList_append(self->input_vars, name);
    NodeList_append(self->vars, name);
    break;

  case VAR: /* State var */
    /* not previously declared */
    nusmv_assert(symb_cache_lookup_symbol(self, name) == (node_ptr) NULL);

    NodeList_append(self->state_symbols, name);
    NodeList_append(self->state_vars, name);
    NodeList_append(self->vars, name);
    break;

  case CONTEXT: 
    /* This is a DEFINE, add it to the list of pending DEFINEs: */
    /* not previously declared */
    nusmv_assert(symb_cache_lookup_symbol(self, name) == (node_ptr) NULL);
    nusmv_assert(!NodeList_belongs_to(self->pending_defines, name));

    NodeList_append(self->pending_defines, name);
    NodeList_append(self->defines, name);
    break;

  case NUMBER:
    /* This is a constant: add it to the list of constants, but only
       if the ref counter is one and it is not already defined
       (i.e. it is a new constant, and not a ref counting
       decreasing) */
    nusmv_assert(NODE_TO_INT(car(value)) > 0);
    if ( (NODE_TO_INT(car(value)) == 1) && 
         !NodeList_belongs_to(self->constants, name) ) {
      NodeList_append(self->constants, name);
    }
    break;

  default:
    internal_error("symb_cache_new_symbol: unknown symbol type");
  }      

  insert_assoc(self->symbol_hash, name, value);
  self->last_symbol_hash_key = name;
  self->last_symbol_hash_value = value;
}


/**Function********************************************************************

  Synopsis           [Removes any reference of the given symbol from the 
  cache, and from the flattener internal hashes as well]

  Description        [A service of the remove_* private methods]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void symb_cache_remove_symbol(SymbCache_ptr self, node_ptr name)
{
  NodeList_ptr* iter;
  NodeList_ptr lists[] = { 
    self->defines,
    self->state_vars,
    self->input_vars,
    self->vars,
    self->state_symbols,
    self->input_symbols,
    self->state_input_symbols,
    self->pending_defines,

    NULL /* terminator */
  };    
  
  /* fixes the lists: */
  for (iter = lists; *iter != NODE_LIST(NULL); ++iter) {
    if (NodeList_belongs_to(*iter, name)) {
      /* finds the occurence and removes it */
      ListIter_ptr iter2 = NodeList_get_first_iter(*iter);
      while (!ListIter_is_end(iter2)) {
        if (NodeList_get_elem_at(*iter, iter2) == name) {
          /* found it: removes the occurence: */
          NodeList_remove_elem_at(*iter, iter2);
          break;
        }
        iter2 = ListIter_get_next(iter2);
      }
    }
  }      


  {  /* fixes the hash table, and constants list */
    node_ptr val = find_assoc(self->symbol_hash, name);
    if (val != Nil) {
      if (node_get_type(val) == NUMBER) {
        /* it is a constant, removes it only if the reference counting
           goes to zero */
        int count = NODE_TO_INT(car(val));
        nusmv_assert(count > 0);

        if (count > 1) {
          /* decreases the constant reference counting */
          symb_cache_new_symbol(self, name, 
                                find_node(NUMBER, NODE_FROM_INT(count-1), Nil));
        }
        else { /* constant reaches a zero reference counting: removes it */
          ListIter_ptr iter;

          /* cleans up the symbols hash */
          insert_assoc(self->symbol_hash, name, Nil); 

          /* removes the constant from the constants list as well */
          iter = NodeList_get_first_iter(self->constants);
          while (!ListIter_is_end(iter)) {
            if (NodeList_get_elem_at(self->constants, iter) == name) {
              /* found it: removes the constant */
              NodeList_remove_elem_at(self->constants, iter);
              break;
            }
            iter = ListIter_get_next(iter);
          }
          
          /* cleans up the flattener hashes */
          Flatten_remove_symbol_info(name);
        }
      }
      else {
        insert_assoc(self->symbol_hash, name, Nil); /* not a constant */
        Flatten_remove_symbol_info(name); /* cleans up the flattener hashes */
      }

      free_node(val);
    }
    
    /* cleans up structures for lazy evaluation if it is the case */
    if (self->last_symbol_hash_key == name) {
      self->last_symbol_hash_key = Nil;
      self->last_symbol_hash_value = Nil;
    }
  } /* end of block */
}


/**Function********************************************************************

  Synopsis           [Returns the definition of the given symbol]

  Description        [Returned node can be either VAR, IVAR or CONTEXT. Value
  is searched first in the cache, then in the symbol hash]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr symb_cache_lookup_symbol(const SymbCache_ptr self, 
                                         const node_ptr name)
{
  SYMB_CACHE_CHECK_INSTANCE(self);

  if (self->last_symbol_hash_key != name) {
    self->last_symbol_hash_value = find_assoc(self->symbol_hash, name);
    self->last_symbol_hash_key = name;
  }
  
  return self->last_symbol_hash_value;
}


/**Function********************************************************************

  Synopsis           [Private initializer]

  Description        [Private initializer, called by the constructor]

  SideEffects        []

  SeeAlso            [symb_cache_deinit]

******************************************************************************/
static void symb_cache_init(SymbCache_ptr self, SymbTable_ptr symb_table)
{
  self->symb_table = symb_table;
  self->symbol_hash = new_assoc();
  nusmv_assert(self->symbol_hash != (hash_ptr) NULL);

  self->constants = NodeList_create();
  self->defines = NodeList_create();
  self->input_vars = NodeList_create();
  self->state_vars = NodeList_create();
  self->vars = NodeList_create();

  self->pending_defines = NodeList_create();
  self->state_symbols = NodeList_create();
  self->input_symbols = NodeList_create();
  self->state_input_symbols = NodeList_create();

  self->last_symbol_hash_key = (node_ptr) NULL;
  self->last_symbol_hash_value = (node_ptr) NULL;
}


/**Function********************************************************************

  Synopsis           [Private deinitializer]

  Description        [Private deinitializer, called by the destructor]

  SideEffects        []

  SeeAlso            [symb_cache_init]

******************************************************************************/
static void symb_cache_deinit(SymbCache_ptr self)
{
  free_assoc(self->symbol_hash);  

  NodeList_destroy(self->vars);
  NodeList_destroy(self->state_vars);
  NodeList_destroy(self->input_vars);
  NodeList_destroy(self->defines);
  NodeList_destroy(self->constants);

  NodeList_destroy(self->state_symbols);
  NodeList_destroy(self->input_symbols);
  NodeList_destroy(self->state_input_symbols);
  NodeList_destroy(self->pending_defines);
}


/**Function********************************************************************

  Synopsis           [Used to update symbols lists when a DEFINE is declared. 
  The given DEFINE will be added to one of state, input or state-input symbols 
  lists.]

  Description [This method must be called after all symbols occuring
  directly or indirectly within its body are all declared. If one or
  more are undeclared, an internal error occurs.] 

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void  
symb_cache_define_to_symbols_lists(SymbCache_ptr self, node_ptr define)
{
  NodeList_ptr deps;
  node_ptr body;
  boolean state, input, constant; 

  /* check that it does not occur to the list of pending DEFINEs: */
  nusmv_assert(!NodeList_belongs_to(self->pending_defines, define));

  /* checks if already added to symbols lists: */
  if (NodeList_belongs_to(self->state_symbols, define) || 
      NodeList_belongs_to(self->input_symbols, define) ||
      NodeList_belongs_to(self->state_input_symbols, define)) return;

  body = SymbCache_get_define_flatten_body(self, define);
  deps = Compile_get_expression_dependencies(self->symb_table, body);
  
  state = SymbCache_list_contains_state_var(self, deps);
  input = SymbCache_list_contains_input_var(self, deps);
  constant = (!(state || input)) && (body != (node_ptr) NULL);
  nusmv_assert(state || input || constant);

  /* If state and input vars occur, state_input list is updated */
  if (state && input)  NodeList_append(self->state_input_symbols, define);
  else if (state) NodeList_append(self->state_symbols, define);
  else if (input) NodeList_append(self->input_symbols, define);
  /* constants are managed within state symbols for convenience */
  else if (constant) NodeList_append(self->state_symbols, define);

  NodeList_destroy(deps);
}


/**Function********************************************************************

  Synopsis           [If there are pending DEFINEs that wait for an assignment 
  within the lists of state, input and state-input symbols list, they are 
  resolved and assigned.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void symb_cache_resolve_pending_defines(SymbCache_ptr self)
{
  ListIter_ptr iter;
  
  iter = NodeList_get_first_iter(self->pending_defines);
  while (! ListIter_is_end(iter)) {
    node_ptr define;

    define = NodeList_remove_elem_at(self->pending_defines, iter);
    symb_cache_define_to_symbols_lists(self, define);
    
    iter = NodeList_get_first_iter(self->pending_defines);
  }
}


