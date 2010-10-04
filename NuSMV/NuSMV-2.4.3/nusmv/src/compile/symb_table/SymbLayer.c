/**CFile*****************************************************************

  FileName    [SymbLayer.c]

  PackageName [compile.symb_table]

  Synopsis    [Implementation of the system-wide SymbLayer]

  Description []

  SeeAlso     [SymbLayer.h]

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

#include "SymbLayer.h"
#include "SymbLayer_private.h"
#include "SymbCache.h"
#include "SymbCache_private.h"
#include "symb_table_int.h"

#include "compile/compileInt.h"

#include "parser/symbols.h"

#include "utils/error.h"
#include "utils/NodeList.h"


static char rcsid[] UTIL_UNUSED = "$Id: SymbLayer.c,v 1.1.2.24.4.4 2007/04/20 12:41:29 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Type***********************************************************************

  Synopsis [A SymbLayer is a colleciont of different kind of symbols,
  like variables, DEFINES, constants, etc. After it had been filled
  with symbols, it can must be committed to a encoding in order to
  have the symbols it contains converted to a form that depends on the
  specific encoding type (for example BDDs, Bes, booleanized scalar
  variables). ]

  Description [A layer always belongs to a
  SymbTable instance, that can hold many layers in a stack, and that
  is responsible for their life cycles. 
  A given layer is identified within the symbol table it belongs to by 
  a name as a string of characters.  

  The layer must be used as a container of symbols, to declare new
  symbols within an encoding. First a layer is filled in with symbols,
  then it is committed to one or more encodings like BoolEnc, BddEnc
  or BeEnc. 

  When a layer is no longer useful, it must be removed from all the 
  encodings it had been committed to, and then can be removed from 
  the symbol table as well. ]

******************************************************************************/
typedef struct SymbLayer_TAG {
  char* name;
  LayerInsertPolicy insert_policy;

  /* the current number of encodings self is registered with */
  int committed_to_encs; 
  
  /* all symbols are contained inside the set of these lists. */
  NodeList_ptr input_vars;
  NodeList_ptr state_vars;
  NodeList_ptr all_vars;
  NodeList_ptr define_symbols;
  NodeList_ptr constant_symbols;

  /* These lists contain only boolean vars */
  NodeList_ptr input_bool_vars;
  NodeList_ptr state_bool_vars;
  
  /* all symbols: */
  NodeList_ptr all_symbols;

  SymbCache_ptr cache;

} SymbLayer;



/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void symb_layer_init ARGS((SymbLayer_ptr self, const char* name, 
                                  const LayerInsertPolicy policy, 
                                  SymbCache_ptr cache));

static void symb_layer_deinit ARGS((SymbLayer_ptr self));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Class SymbLayer constructor]

  Description        [name is copied, the caller keeps ownership of cache.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbLayer_ptr SymbLayer_create(const char* name, 
                               const LayerInsertPolicy policy, 
                               SymbCache_ptr cache)
{
  SymbLayer_ptr self = ALLOC(SymbLayer, 1);

  SYMB_LAYER_CHECK_INSTANCE(self);

  symb_layer_init(self, name, policy, cache); 
  return self;
}


/**Function********************************************************************

  Synopsis           [Class SymbLayer destructor]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_destroy(SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);

  symb_layer_deinit(self); 
  FREE(self);
}


/**Function********************************************************************

  Synopsis           [Sets the layer name.]

  Description        [This method is protected (not usable by users, only 
  used by the symbol table when renaming a layer]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_set_name(SymbLayer_ptr self, const char* new_name)
{
  SYMB_LAYER_CHECK_INSTANCE(self);

  /* frees the previous name if needed */
  if (self->name != (char*) NULL) { FREE(self->name); }
  if (new_name != (char*) NULL)  self->name = util_strsav((char*) new_name);
  else self->name = (char*) NULL;
}


/**Function********************************************************************

  Synopsis           [Returns the name self had been registered with.]

  Description        [Returned string must not be freed, it belongs to self]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
const char* SymbLayer_get_name(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return (const char*) self->name;
}


/**Function********************************************************************

  Synopsis           [Returns the policy that must be adopted to stack this 
  layer into a layers stack, within a SymbTable instance]

  Description [This method is thought to be used exclusively by class
  SymbTable]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
LayerInsertPolicy SymbLayer_get_insert_policy(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  if (self->insert_policy == SYMB_LAYER_POS_DEFAULT) return SYMB_LAYER_POS_TOP;
  else return self->insert_policy;
}


/**Function********************************************************************

  Synopsis [Compares the insertion policies of self and other, and
 returns true if self must be inserted *before* other]

  Description [Compares the insertion policies of self and other, and
  returns true if self must be inserted *before* other.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbLayer_must_insert_before(const SymbLayer_ptr self, 
                                     const SymbLayer_ptr other)
{
  LayerInsertPolicy p1, p2;

  SYMB_LAYER_CHECK_INSTANCE(self);

  p1 = SymbLayer_get_insert_policy(self);
  p2 = SymbLayer_get_insert_policy(other);

  /* checks that p1 and p2 do not refer the same forced position */
  nusmv_assert((p1 != p2) || ((p1 != SYMB_LAYER_POS_FORCE_TOP) &&
                              (p1 != SYMB_LAYER_POS_FORCE_BOTTOM)));

  switch (p1) {
  case SYMB_LAYER_POS_FORCE_TOP: return true;

  case SYMB_LAYER_POS_DEFAULT:
  case SYMB_LAYER_POS_TOP:
    return p2 != SYMB_LAYER_POS_FORCE_TOP;
    
  case SYMB_LAYER_POS_BOTTOM: return p2 == SYMB_LAYER_POS_FORCE_BOTTOM;

  default:
    internal_error("Unexpected layer insertion policy");
  }

  return false;
}



/**Function********************************************************************

  Synopsis [Called every time an instance is committed within an
  encoding.]

  Description [This method is part of a private registration protocol
  between encodings and layers, and must be considered as a private
  method.  Every time a layer is registered (committed) within an
  enconding, the layer is notified with a call to this method from the
  encoding instance which the layer is committed to. This mechanism
  helps to detect errors when a layer in use by an encoding is removed
  and destroyed from within a symbol table. The destructor will always
  check that self is not in use by any encoding when it is invoked.]

  SideEffects        []

  SeeAlso            [removed_from_enc]

******************************************************************************/
void SymbLayer_committed_to_enc(SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);

  self->committed_to_encs += 1;

  if (opt_verbose_level_gt(options, 4)) {
    fprintf(nusmv_stdout, 
            "SymbLayer '%s': committed to %d encodings\n",  
            SymbLayer_get_name(self), self->committed_to_encs);
  }
}


/**Function********************************************************************

  Synopsis [Called every time an instance is removed from an
  encoding.]

  Description [This method is part of a private registration protocol
  between encodings and layers, and must be considered as a private
  method.  Every time a layer is removed (uncommitted) from an
  enconding, the layer is notified with a call to this method from the
  encoding instance which the layer is being removed from. This mechanism
  helps to detect errors when a layer in use by an encoding is removed
  and destroyed from within a symbol table. The destructor will always
  check that self is not in use by any encoding when it is invoked.]

  SideEffects        []

  SeeAlso            [commit_to_enc]

******************************************************************************/
void SymbLayer_removed_from_enc(SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  nusmv_assert(self->committed_to_encs > 0);

  self->committed_to_encs -= 1;

  if (opt_verbose_level_gt(options, 4)) {
    fprintf(nusmv_stdout, 
            "SymbLayer '%s': removed from encoding (%d remaining)\n",  
            SymbLayer_get_name(self), self->committed_to_encs);
  }
}


/**Function********************************************************************

  Synopsis           [Call this method to know if a new constant can be 
  declared within this layer.]

  Description [Since more than one layer can declare the same constants, 
  this method might return true even if another layer already contain the 
  given constant. If the constant had already been declared within self, 
  false is returned. ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbLayer_can_declare_constant(const SymbLayer_ptr self, 
                                       const node_ptr name)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return (!NodeList_belongs_to(self->constant_symbols, name));
}


/**Function********************************************************************

  Synopsis           [Call this method to know if a new variable can be 
  declared within this layer.]

  Description [Returns true if the given symbol does not exist
  within the symbol table which self belongs to. Returns
  false if the symbol was already declared. ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbLayer_can_declare_var(const SymbLayer_ptr self,
                                  const node_ptr name)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return !SymbCache_is_symbol_declared(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Call this method to know if a new DEFINE can be 
  declared within this layer.]

  Description [Returns true if the given symbol does not exist within
  the symbol table which self belongs to. Returns false if the symbol
  was already declared. ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean SymbLayer_can_declare_define(const SymbLayer_ptr self,
                                     const node_ptr name)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return !SymbCache_is_symbol_declared(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Insert a new constant]

  Description        [A new constant is created]

  SideEffects        []

  SeeAlso            [SymbLayer_can_declare_constant]

******************************************************************************/
void SymbLayer_declare_constant(SymbLayer_ptr self, node_ptr name)
{
  SYMB_LAYER_CHECK_INSTANCE(self);  
  nusmv_assert(SymbLayer_can_declare_constant(self, name));

  SymbCache_new_constant(self->cache, name);
  NodeList_append(self->constant_symbols, name);  

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbLayer '%s': declared new constant '",  
            self->name);
    print_node(nusmv_stdout, name);
    fprintf(nusmv_stdout, "'\n");    
  }    
}


/**Function********************************************************************

  Synopsis           [Insert a new input variable]

  Description        [A new input variable is created of a given type.
  The variable type can be created with SymbType_create or returned by
  funtions SymbTablePkg_..._type. 
  The layer is responsible for destroying the variable's type.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_declare_input_var(SymbLayer_ptr self, node_ptr var_name, 
                                 SymbType_ptr type)
{
  SYMB_LAYER_CHECK_INSTANCE(self); 

  /* not already declared */
  if (!SymbLayer_can_declare_var(self, var_name)) {
    fprintf(stderr, "Error: Cannot declare input variable ");
    print_node(stderr, var_name);
    internal_error("Symbol already declared");
  }

  if (SymbType_is_boolean_enum(type)) {
    NodeList_append(self->input_bool_vars, var_name);
  }
   
  NodeList_append(self->input_vars, var_name);
  NodeList_append(self->all_vars, var_name);
  NodeList_append(self->all_symbols, var_name);

  SymbCache_new_input_var(self->cache, var_name, type);

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbLayer '%s': declared new input variable '",  
            self->name);
    print_node(nusmv_stdout, var_name);
    fprintf(nusmv_stdout, "'\n");    
  }    

}


/**Function********************************************************************

  Synopsis           [Insert a new state variable]

  Description        [A new state variable is created of a given type.
  The variable type can be created with SymbType_create or returned by
  funtions SymbTablePkg_..._type. 
  The layer is responsible for destroying the variable's type.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_declare_state_var(SymbLayer_ptr self, node_ptr var_name, 
                                 SymbType_ptr type)
{
  SYMB_LAYER_CHECK_INSTANCE(self);

  if (! SymbLayer_can_declare_var(self, var_name)) {
    fprintf(stderr, "Error: cannot declare state variable ");
    print_node(stderr, var_name);
    internal_error("Symbol already declared");
  }

  
  
  if (SymbType_is_boolean_enum(type)) {
    NodeList_append(self->state_bool_vars, var_name);
  }
  
  NodeList_append(self->state_vars, var_name);
  NodeList_append(self->all_vars, var_name);
  NodeList_append(self->all_symbols, var_name);

  SymbCache_new_state_var(self->cache, var_name, type);

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbLayer '%s': declared new state variable '",  
            self->name);
    print_node(nusmv_stdout, var_name);
    fprintf(nusmv_stdout, "'\n");    
  }    
}


/**Function********************************************************************

  Synopsis           [Insert a new DEFINE]

  Description        [A new DEFINE of a given value is created. name must be  
  contestualized, context is provided as a separated information]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_declare_define(SymbLayer_ptr self, node_ptr name, 
                              node_ptr context, node_ptr definition)
{
  SYMB_LAYER_CHECK_INSTANCE(self);

  if (!SymbLayer_can_declare_define(self, name)) {
    fprintf(stderr, "Error: cannot declare DEFINE ");
    print_node(stderr, name);    
    internal_error("SymbLayer_declare_define: name already declared\n");
  }
  
  NodeList_append(self->define_symbols, name);
  NodeList_append(self->all_symbols, name);

  SymbCache_new_define(self->cache, name, context, definition);

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbLayer '%s': declared new DEFINE '",  
            self->name);
    print_node(nusmv_stdout, name);
    fprintf(nusmv_stdout, "'\n");    
  }    
}                                    


/**Function********************************************************************

  Synopsis           [Removes a variable previously delcared]

  Description [This method can be called only if self is not currently
  commited to any encoding]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbLayer_remove_var(SymbLayer_ptr self, node_ptr name)
{
  nusmv_assert(self->committed_to_encs == 0); /* not in use by encs */
  nusmv_assert(SymbCache_is_symbol_var(self->cache, name));

  /* destroys the variable' type */
  SymbType_destroy(SymbCache_get_var_type(self->cache, name));

  /* removes the variable  */
  SymbCache_remove_var(self->cache, name);

  {
    /* Removes the symbol from the internal lists */
    NodeList_ptr lists[] = {
      self->input_vars, 
      self->state_vars,
      self->all_vars, 
      self->input_bool_vars,
      self->state_bool_vars, 
      self->all_symbols };

    int i;
    for (i=0; i<sizeof(lists)/sizeof(*lists); ++i) {
      if (NodeList_belongs_to(lists[i], name)) {
        ListIter_ptr iter = NodeList_get_first_iter(lists[i]);
        while (!ListIter_is_end(iter)) {
          if (NodeList_get_elem_at(lists[i], iter) == name) {
            NodeList_remove_elem_at(lists[i], iter);
            break;
          }
          iter = ListIter_get_next(iter);
        }
      }
    }
  }


  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbLayer '%s': removed variable '", self->name);
    print_node(nusmv_stdout, name);
    fprintf(nusmv_stdout, "'\n");    
  }
}



/**function********************************************************************

  Synopsis           [Returns the number of declared contants]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_constants_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->constant_symbols);
}


/**Function********************************************************************

  Synopsis           [Returns the list of declared contants]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_constants(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->constant_symbols;
}


/**Function********************************************************************

  Synopsis           [Returns the number of declared state variables.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_state_vars_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->state_vars);
}


/**Function********************************************************************

  Synopsis           [Returns the number of declared boolean state variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_bool_state_vars_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->state_bool_vars);
}


/**Function********************************************************************

  Synopsis           [Returns the number of declared input variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_input_vars_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->input_vars);
}


/**Function********************************************************************

  Synopsis           [Returns the number of declared boolean input variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_bool_input_vars_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->input_bool_vars);
}


/**Function********************************************************************

  Synopsis           [Returns the number of DEFINEs.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbLayer_get_defines_num(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return NodeList_get_length(self->define_symbols);
}


/**Function********************************************************************

  Synopsis           [Returns the list  of DEFINEs.]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_defines(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->define_symbols;
}


/**Function********************************************************************

  Synopsis [Returns the entire list of variables declared in this
  layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_all_vars(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->all_vars;
}


/**Function********************************************************************

  Synopsis [Returns the entire list of symbols (constants, variables,
  etc.) declared in this layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_all_symbols(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->all_symbols;
}


/**Function********************************************************************

  Synopsis [Returns the list of state variables declared in this
  layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_state_vars(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->state_vars;
}


/**Function********************************************************************

  Synopsis [Returns the list of input variables declared in this
  layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_input_vars(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->input_vars;
}


/**Function********************************************************************

  Synopsis [Returns the list of declared boolean input variables
  declared in this layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_bool_input_vars(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->input_bool_vars;
}


/**Function********************************************************************

  Synopsis [Returns the list of declared boolean state variables
  declared in this layer]

  Description        [Self keeps the ownership of the returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_bool_state_vars(const SymbLayer_ptr self)
{
  SYMB_LAYER_CHECK_INSTANCE(self);
  return self->state_bool_vars;
}


/**Function********************************************************************

  Synopsis [Returns the list of boolean variables declared in this
  layer]

  Description        [WARNING: The *caller* is responsible for destroying the
  returned instance]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr SymbLayer_get_bool_vars(const SymbLayer_ptr self)
{
  node_ptr list; 
  SYMB_LAYER_CHECK_INSTANCE(self);
  
  list = append_ns(NodeList_to_node_ptr(self->input_bool_vars), 
                   NodeList_to_node_ptr(self->state_bool_vars));

  return NodeList_create_from_list(list);
}


/*--------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Private method called by the constructor]

  Description        [Called by the constructor]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void 
symb_layer_init(SymbLayer_ptr self, const char* name, 
                const LayerInsertPolicy policy, SymbCache_ptr cache)
{   
  self->name = (char*) NULL;
  SymbLayer_set_name(self, name);
  
  self->insert_policy = policy;
  self->committed_to_encs = 0;
  self->cache = cache;
 
  /* creation of lists */
  self->input_vars = NodeList_create(); 
  self->state_vars = NodeList_create();
  self->all_vars = NodeList_create(); 
  self->define_symbols = NodeList_create();
  self->constant_symbols = NodeList_create();
  self->input_bool_vars = NodeList_create();
  self->state_bool_vars = NodeList_create(); 
  self->all_symbols = NodeList_create();
}


/**Function********************************************************************

  Synopsis           [Private method called by the destructor]

  Description        [Called by the destructor]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void symb_layer_deinit(SymbLayer_ptr self)
{
  ListIter_ptr list_iter;

  nusmv_assert(self->committed_to_encs == 0); /* not in use by encs */
  
  /* variables: */
  list_iter = NodeList_get_first_iter(self->all_vars);
  while (!ListIter_is_end(list_iter)) {
    node_ptr name = NodeList_get_elem_at(self->all_vars, list_iter);
    /* destory the variable' type */
    SymbType_destroy(SymbCache_get_var_type(self->cache, name));
    /* remove the variable  */
    SymbCache_remove_var(self->cache, name);
    list_iter = ListIter_get_next(list_iter);
  }

  /* constants: */
  list_iter = NodeList_get_first_iter(self->constant_symbols);
  while (!ListIter_is_end(list_iter)) {
    SymbCache_remove_constant(self->cache, 
                              NodeList_get_elem_at(self->constant_symbols, 
                                                   list_iter));
    list_iter = ListIter_get_next(list_iter);
  }

  /* DEFINEs */
  list_iter = NodeList_get_first_iter(self->define_symbols);
  while (!ListIter_is_end(list_iter)) {
    SymbCache_remove_define(self->cache, 
                            NodeList_get_elem_at(self->define_symbols, 
                                                 list_iter));
    list_iter = ListIter_get_next(list_iter);
  }

  /* frees the name */
  if (self->name != (char*) NULL) { FREE(self->name); }

  /* Frees the lists */
  NodeList_destroy(self->input_vars); 
  NodeList_destroy(self->state_vars);
  NodeList_destroy(self->all_vars); 
  NodeList_destroy(self->define_symbols);
  NodeList_destroy(self->constant_symbols);
  NodeList_destroy(self->input_bool_vars);
  NodeList_destroy(self->state_bool_vars); 
  NodeList_destroy(self->all_symbols);
}

