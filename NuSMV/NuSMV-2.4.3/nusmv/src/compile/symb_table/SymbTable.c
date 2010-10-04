/**CFile*****************************************************************

  FileName    [SymbTable.c]

  PackageName [compile.symb_table]

  Synopsis    [Implementation of the system-wide SymbolTable]

  Description []

  SeeAlso     [SymbTable.h]

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

#include "SymbTable.h"

#include "SymbLayer.h"
#include "SymbLayer_private.h"
#include "SymbCache.h"
#include "SymbCache_private.h"

#include "compile/compileInt.h"

#include "parser/symbols.h"
#include "utils/error.h"
#include "utils/NodeList.h"
#include "utils/ustring.h"
#include "utils/assoc.h"

static char rcsid[] UTIL_UNUSED = "$Id: SymbTable.c,v 1.1.2.18.6.6 2007/04/20 12:41:29 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    []

  Description []
  
******************************************************************************/
typedef struct SymbTable_TAG
{
  SymbCache_ptr cache;

  int temp_layer_name_suffix; /* used to create temporary names for layers */
  
  NodeList_ptr layers; /* the list of owned layers */  

  hash_ptr name2layer; /* to associate layers and names */
} SymbTable;


/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/

#define SYMB_TABLE_TEMP_LAYER_NAME "__TempLayer_%d"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void symb_table_init ARGS((SymbTable_ptr self));
static void symb_table_deinit ARGS((SymbTable_ptr self));

static boolean 
symb_table_layer_exists ARGS((const SymbTable_ptr self, 
                              const char* layer_name));

static NodeList_ptr 
symb_table_filter_layer_symbols ARGS((SymbTable_ptr self, 
                                      const char* layer_name, 
                                      NodeList_ptr symbols));

static NodeList_ptr 
symb_table_filter_layers_symbols ARGS((SymbTable_ptr self, 
                                       const array_t* layer_names, 
                                       NodeList_ptr symbols));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Class constructor]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbTable_ptr SymbTable_create()
{
  SymbTable_ptr self = ALLOC(SymbTable, 1);

  SYMB_TABLE_CHECK_INSTANCE(self);

  symb_table_init(self); 
  return self;
}


/**Function********************************************************************

  Synopsis           [Class destructor]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbTable_destroy(SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  
  symb_table_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis           [Creates and adds a new layer]

  Description [The created layer is returned. Do not destroy the
  layer, since it belongs to self. if layer name is NULL, then a
  temporary name will be searched and a new layer will be created. To
  retrieve the layer name, query the returned SymbLayer instance. ]

  SideEffects        []

  SeeAlso            [remove_layer]

******************************************************************************/
SymbLayer_ptr SymbTable_create_layer(SymbTable_ptr self, 
                                     const char* layer_name, 
                                     const LayerInsertPolicy ins_policy)
{
  SymbLayer_ptr layer;
  ListIter_ptr iter;
  char tmp_name[50];

  SYMB_TABLE_CHECK_INSTANCE(self);

  if (layer_name == (char*) NULL) {
    snprintf(tmp_name, sizeof(tmp_name) / sizeof(tmp_name[0]), 
             SYMB_TABLE_TEMP_LAYER_NAME, self->temp_layer_name_suffix);
    ++(self->temp_layer_name_suffix);
    layer_name = tmp_name;
  }

  nusmv_assert(! symb_table_layer_exists(self, layer_name));  
  layer = SymbLayer_create(layer_name, ins_policy, self->cache);
  
  /* searches the insertion point, and inserts the layer */
  iter = NodeList_get_first_iter(self->layers);
  while (!ListIter_is_end(iter)) {
    if (SymbLayer_must_insert_before(layer, 
             SYMB_LAYER(NodeList_get_elem_at(self->layers, iter)))) {
      NodeList_insert_before(self->layers, iter, (node_ptr) layer);
      break;
    }
    
    iter = ListIter_get_next(iter);
  }

  /* if not inserted yet: */
  if (ListIter_is_end(iter)) NodeList_append(self->layers, (node_ptr) layer);
  
  /* we duplicate the key here, to allow the caller to free layer_name
     if dinamically created. Memory will be freed by the deiniter of
     SymbTable */
  insert_assoc(self->name2layer, (node_ptr) find_string((char*) layer_name), 
               (node_ptr) layer); 

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbTable: created new layer '%s'\n",  layer_name);
  }    

  return layer; 
}


/**Function********************************************************************

  Synopsis           [Removes and destroys a layer]

  Description [The layer must be not in use by any encoding, so remove
  it from all encodings before calling this method. The removed layer
  will be no longer available after the invocation of this method. ]

  SideEffects        []

  SeeAlso            [create_layer]

******************************************************************************/
void SymbTable_remove_layer(SymbTable_ptr self, SymbLayer_ptr layer)
{
  ListIter_ptr iter;

  SYMB_TABLE_CHECK_INSTANCE(self);
  nusmv_assert(symb_table_layer_exists(self, SymbLayer_get_name(layer)));

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbTable: destroying layer '%s'\n",  
            SymbLayer_get_name(layer));
  }

  /* searches the layer */
  iter = NodeList_get_first_iter(self->layers);
  while (!ListIter_is_end(iter)) {
    SymbLayer_ptr lay;
    lay = SYMB_LAYER(NodeList_get_elem_at(self->layers, iter));
    if (layer == lay) {
      /* found the layer */
      NodeList_remove_elem_at(self->layers, iter);

      insert_assoc(self->name2layer, 
                   (node_ptr) find_string((char*) SymbLayer_get_name(layer)), 
                   (node_ptr) NULL);
      SymbLayer_destroy(layer);
      return;
    }

    iter = ListIter_get_next(iter);
  }
  
  nusmv_assert(!ListIter_is_end(iter)); /* This layer had not been found */
}


 /**Function********************************************************************

  Synopsis          [Given its name, returns a layer]

  Description [NULL is returned when the layer does not exist within
  self.  Returned SymbLayer instance belongs to self.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbLayer_ptr 
SymbTable_get_layer(const SymbTable_ptr self, const char* layer_name) 
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SYMB_LAYER( find_assoc(self->name2layer, 
                                (node_ptr) find_string((char*) layer_name)) );
}



/**Function********************************************************************

  Synopsis           [Renames an existing layer]

  Description [Use to rename an existing layer. Useful for example to
  substitute an existing layer with another.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbTable_rename_layer(const SymbTable_ptr self, 
                            const char* layer_name, const char* new_name)
{
  SymbLayer_ptr layer;

  SYMB_TABLE_CHECK_INSTANCE(self);

  layer = SymbTable_get_layer(self, layer_name);
  SYMB_LAYER_CHECK_INSTANCE(layer);

  /* sets the new name */
  SymbLayer_set_name(layer, new_name);

  /* removes previous name association */
  insert_assoc(self->name2layer, (node_ptr) find_string((char*) layer_name), 
               (node_ptr) NULL); 

  /* adds the new name association */
  insert_assoc(self->name2layer, (node_ptr) find_string((char*) new_name), 
               (node_ptr) layer); 

  if (opt_verbose_level_gt(options, 3)) {
    fprintf(nusmv_stdout, "SymbTable: renamed layer '%s' to '%s'\n",  
            layer_name, new_name);
  }
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr 
SymbTable_get_vars(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_vars(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared state variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr 
SymbTable_get_state_vars(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_state_vars(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared input variables]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr 
SymbTable_get_input_vars(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_input_vars(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of all declared constants]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr 
SymbTable_get_constants(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_constants(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of all DEFINEs]

  Description        [Returned instance belongs to self, do not destroy or 
  change it]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr 
SymbTable_get_defines(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_defines(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the number of all declared variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbTable_get_vars_num(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return NodeList_get_length(SymbTable_get_vars(self));
}


/**Function********************************************************************

  Synopsis           [Returns the number of all declared state variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbTable_get_state_vars_num(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return NodeList_get_length(SymbTable_get_state_vars(self));
}


/**Function********************************************************************

  Synopsis           [Returns the number of all declared input variables]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbTable_get_input_vars_num(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return NodeList_get_length(SymbTable_get_input_vars(self));
}


/**Function********************************************************************

  Synopsis           [Returns the number of all declared constants]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbTable_get_constants_num(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return NodeList_get_length(SymbTable_get_constants(self));
}


/**Function********************************************************************

  Synopsis           [Returns the number of all DEFINEs]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbTable_get_defines_num(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return NodeList_get_length(SymbTable_get_defines(self)); 
}


/**Function********************************************************************

  Synopsis           [Returns the list of all state variables and those DEFINEs
  whose body refer directly or indirectly to state variables]

  Description [Only state variables and DEFINEs can occur within the
  returned list. Returned list belongs to self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_state_symbols(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_state_symbols(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of all input variables and those DEFINEs
  whose body refer directly or indirectly to input variables]

  Description [Only input variables and DEFINEs can occur within the
  returned list. Returned list belongs to self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_input_symbols(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_input_symbols(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of those DEFINEs whose body refer
  directly or indirectly to both state and input variables]

  Description [Only DEFINEs whose body contains both state and input
  vars can occur within the returned list. Returned list belongs to
  self, do not destroy it]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_state_input_symbols(const SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_state_input_symbols(self->cache);
}


/**Function********************************************************************

  Synopsis           [Returns the list of state symbols that belong to the 
  layer of the model symbols]

  Description        [Everytime this method is called, it will create and
  calculate a new list.  The existance of a layer called
  MODEL_LAYER_NAME will be checked, and if not found, and empty list
  will be returned.

  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_model_state_symbols(SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layer_symbols(self, MODEL_LAYER_NAME, 
                         SymbTable_get_state_symbols(self));
}


/**Function********************************************************************

  Synopsis           [Returns the list of input symbols that belong to the 
  layer of the model symbols]

  Description [The returned instance belongs to self. Everytime this
  method is called, it will create and calculate a new list.  The
  existance of a layer called MODEL_LAYER_NAME will be checked, and if
  not found, and empty list will be returned.

  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_model_input_symbols(SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layer_symbols(self, MODEL_LAYER_NAME, 
                                 SymbTable_get_input_symbols(self));
}


/**Function********************************************************************

  Synopsis [Returns the list of both state and input symbols that
  belong to the layer of the model symbols. It returns those DEFINES
  whose body contain both state and input variables. This methods does
  _NOT_ return the state symbols plus the input symbols.]

  Description [The returned instance belongs to self. Everytime this
  method is called, it will create and calculate a new list.  The
  existance of a layer called MODEL_LAYER_NAME will be checked, and if
  not found, and empty list will be returned.

  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_model_state_input_symbols(SymbTable_ptr self)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layer_symbols(self, MODEL_LAYER_NAME, 
                                 SymbTable_get_state_input_symbols(self));
}


/**Function********************************************************************

  Synopsis           [Returns the list of state symbols that belong to the 
  given layers]

  Description        [Everytime this method is called, it will create and
  calculate a new list. layers is an array of strings terminated by NULL.
  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_layers_state_symbols(SymbTable_ptr self, 
                                                const array_t* layer_names)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layers_symbols(self, layer_names, 
                          SymbTable_get_state_symbols(self));
}


/**Function********************************************************************

  Synopsis           [Returns the list of input symbols that belong to the 
  given layers]

  Description        [Everytime this method is called, it will create and
  calculate a new list. layers is an array of strings terminated by NULL.
  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr SymbTable_get_layers_input_symbols(SymbTable_ptr self, 
                                                const array_t* layer_names)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layers_symbols(self, layer_names, 
                                 SymbTable_get_input_symbols(self));
}


/**Function********************************************************************

  Synopsis [Returns the list of state and input symbols that belong to
  the given layers, meaning those DEFINES whose body contain both
  state and input variables. This methods does _NOT_ return the state
  symbols plus the input symbols.]

  Description        [Everytime this method is called, it will create and
  calculate a new list. layers is an array of strings terminated by NULL.
  WARNING: The returned instance must be destroyed by the caller]

  SideEffects        []

  See Also           []

******************************************************************************/
NodeList_ptr 
SymbTable_get_layers_state_input_symbols(SymbTable_ptr self, 
                                         const array_t* layer_names)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  return symb_table_filter_layers_symbols(self, layer_names, 
                                 SymbTable_get_state_input_symbols(self));
}


/**Function********************************************************************

  Synopsis           [Returns the type of a given variable]

  Description        [The type belongs to the layer, do not destroy it.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbType_ptr
SymbTable_get_var_type(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SYMB_TYPE(SymbCache_get_var_type(self->cache, name));
}


/**Function********************************************************************

  Synopsis           [Returns the body of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr 
SymbTable_get_define_body(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_define_body(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Returns the flattenized body of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr 
SymbTable_get_define_flatten_body(const SymbTable_ptr self,
                                  const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_define_flatten_body(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Returns the context of the given DEFINE name]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr 
SymbTable_get_define_context(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_get_define_context(self->cache, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is a state variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_state_var(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_state_var(self->cache, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is an input variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_input_var(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_input_var(self->cache, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is either a state or
  an input variable.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_var(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_var(self->cache, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is a variable of enum type 
  with the values 0 and 1 (boolean)]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean
SymbTable_is_symbol_bool_var(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);

  if (SymbCache_is_symbol_var(self->cache, name)) {
    SymbType_ptr type = SymbTable_get_var_type(self, name);
  return (SYMB_TYPE(NULL) != type) && SymbType_is_boolean_enum(type);
  }

  return false;
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is declared]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_declared(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_declared(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is a declared
  DEFINE]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_define(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_define(self->cache, name);
}


/**Function********************************************************************

  Synopsis [Returns true if the given symbol is a declared
  constant]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean 
SymbTable_is_symbol_constant(const SymbTable_ptr self, const node_ptr name)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_is_symbol_constant(self->cache, name);
}


/**Function********************************************************************

  Synopsis           [Returns true if var_list contains at least one input
  variable, false otherwise]

  Description        [The given list of variables is traversed until an input 
  variable is found]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean 
SymbTable_list_contains_input_var(const SymbTable_ptr self, 
                                  const NodeList_ptr var_list)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_list_contains_input_var(self->cache, var_list);
}


/**Function********************************************************************

  Synopsis           [Returns true if var_list contains at least one state 
  variable, false otherwise]

  Description        [The given list of variables is traversed until 
  a state variable is found]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean 
SymbTable_list_contains_state_var(const SymbTable_ptr self, 
                                  const NodeList_ptr var_list)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_list_contains_state_var(self->cache, var_list);
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbols list contains 
  one or more undeclared variable names, false otherwise]

  Description        [Iterates through the elements in var_list
  checking each one to see if it is one undeclared variable.]

  SideEffects        []

  See Also           []

******************************************************************************/
boolean 
SymbTable_list_contains_undef_var(const SymbTable_ptr self, 
                                  const NodeList_ptr var_list)
{
  SYMB_TABLE_CHECK_INSTANCE(self);
  return SymbCache_list_contains_undef_var(self->cache, var_list);
}


/*--------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Private initializer]

  Description        [Private initializer, called by the constructor only]

  SideEffects        []

  SeeAlso            [symb_table_deinit]

******************************************************************************/
static void symb_table_init(SymbTable_ptr self)
{
  self->cache = SymbCache_create(self);

  self->temp_layer_name_suffix = 0;
  self->layers = NodeList_create();

  self->name2layer = new_assoc();
  nusmv_assert(self->name2layer != (hash_ptr) NULL);  
}


/**Function********************************************************************

  Synopsis           [Private deinitializer]

  Description        [Private deinitializer, called by the destructor only]

  SideEffects        []

  SeeAlso            [symb_table_init]

******************************************************************************/
static void symb_table_deinit(SymbTable_ptr self)
{
  ListIter_ptr iter;

  /* get rid of all contained layers */
  iter = NodeList_get_first_iter(self->layers);
  while (! ListIter_is_end(iter)) {
    SymbLayer_destroy(SYMB_LAYER(NodeList_get_elem_at(self->layers, iter)));
    iter = ListIter_get_next(iter);
  }
  NodeList_destroy(self->layers);
  
  SymbCache_destroy(self->cache);

  free_assoc(self->name2layer);  
}


/**Function********************************************************************

  Synopsis           [Returns true if the given name corresponds to an already
  registered layer.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static boolean 
symb_table_layer_exists(const SymbTable_ptr self, const char* layer_name)
{
  return (SymbTable_get_layer(self, layer_name) != SYMB_LAYER(NULL));
}


/**Function********************************************************************

  Synopsis           [Given a list of symbols, returns a new list that 
  contains only those symbols that have been declared within the model 
  layer]
  

  Description        [An empty list is returned if the model layer has not been 
  created yet. The caller is responsible for destroying the returned list]

  SideEffects        []

  See Also           []

******************************************************************************/
static NodeList_ptr 
symb_table_filter_layer_symbols(SymbTable_ptr self, const char* layer_name, 
                                NodeList_ptr symbols)
{
  NodeList_ptr res;
  array_t* layers = array_alloc(const char*, 1);
  array_insert(const char*, layers, 0, layer_name);

  res = symb_table_filter_layers_symbols(self, layers, symbols);

  array_free(layers);
  return res;
}


/**Function********************************************************************

  Synopsis [Given a list of symbols and a list of layers names,
  returns a new list that contains only those symbols that have been
  declared within the given layers]
  
  Description [The caller is responsible for destroying the
  returned list]

  SideEffects        []

  See Also           []

******************************************************************************/
static NodeList_ptr 
symb_table_filter_layers_symbols(SymbTable_ptr self, 
                                 const array_t* layer_names, 
                                 NodeList_ptr symbols)
{  
  NodeList_ptr res;
  const char* layer_name;
  int idx;

  res = NodeList_create();

  arrayForEachItem(const char*, (array_t*) layer_names, idx, layer_name) {
    SymbLayer_ptr layer = SymbTable_get_layer(self, layer_name);

    if (layer != SYMB_LAYER(NULL)) {
      NodeList_ptr symbs_in_layer;  
      ListIter_ptr iter;

      symbs_in_layer = SymbLayer_get_all_symbols(layer);

      iter = NodeList_get_first_iter(symbols);
      while (!ListIter_is_end(iter)) {
        node_ptr name = NodeList_get_elem_at(symbols, iter);
        if (NodeList_belongs_to(symbs_in_layer, name) && 
            !NodeList_belongs_to(res, name)) {
          NodeList_append(res, name);
        }
        iter = ListIter_get_next(iter);
      }
    }
  } /* outer loop */
  
  return res;  
}
