/**CFile*****************************************************************

  FileName    [BddEncCache.c]

  PackageName [enc.bdd]

  Synopsis [The BddEncCache class implementation. This class is
  intended to be used exclusively by the class BddEnc.]

  Description []

  SeeAlso     [BddEncCache.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``enc.bdd'' package of NuSMV version 2. 
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

#include "BddEncCache.h"
#include "BddEnc_private.h"
#include "bddInt.h"

#include "utils/error.h"
#include "parser/symbols.h"
#include "compile/compile.h"


static char rcsid[] UTIL_UNUSED = "$Id: BddEncCache.c,v 1.1.2.12.4.6.6.6 2007/04/20 13:05:54 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/**Struct**********************************************************************

  Synopsis    [The BddEncCache class]

  Description []
  
******************************************************************************/
typedef struct BddEncCache_TAG
{
  SymbTable_ptr symb_table;
  DdManager* dd;

  /* This hash associates to an atom the corresponding ADD leaf if
  defined. Suppose to have a declaration of this kind: 
     VAR state : {idle, stopped} 
  then in the constant hash for the atom idle there is the
  corresponding leaf ADD, i.e. the ADD whose value is the symbol
  idle. This hash is used by the evaluator. 
  Also, this keeps track of reference counting of multiple times 
  declared constants. */
  hash_ptr constant_hash;

  /* associates var names with corresponding ADDs */
  hash_ptr vars_hash;

  /* hash table used by the evaluator */
  hash_ptr eval_hash;  

} BddEncCache;


/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void 
bdd_enc_cache_init ARGS((BddEncCache_ptr self, SymbTable_ptr symb_table, 
                         DdManager* dd));
                                     
static void bdd_enc_cache_deinit ARGS((BddEncCache_ptr self));

static assoc_retval 
hash_free_add ARGS((char* key, char* data, char* arg));

static assoc_retval 
hash_free_add_array ARGS((char* key, char* data, char* arg));

static assoc_retval 
hash_free_add_counted ARGS((char* key, char* data, char* arg));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Class constructor]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
BddEncCache_ptr BddEncCache_create(SymbTable_ptr symb_table, DdManager* dd)
{
  BddEncCache_ptr self = ALLOC(BddEncCache, 1);

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  bdd_enc_cache_init(self, symb_table, dd); 
  return self;  
}


/**Function********************************************************************

  Synopsis           [Class destructor]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_destroy(BddEncCache_ptr self)
{
  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  bdd_enc_cache_deinit(self);
  FREE(self);  
}


/**Function********************************************************************

  Synopsis           [Call to associate given constant to the relative add]

  Description        [This methods adds the given constant only if it 
  does not exist already, otherwise it only increments a reference counter, 
  to be used when the constant is removed later.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_new_constant(BddEncCache_ptr self, node_ptr constant, 
                              add_ptr constant_add)
{
  node_ptr data;

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  /* we don't store number constants into the symb table, so we are
     lazy in that case */
  nusmv_assert(SymbTable_is_symbol_constant(self->symb_table, constant)
               || (node_get_type(constant) == NUMBER));

  /* Not already defined. We reuse already defined leaf */
  if (! BddEncCache_is_constant_encoded(self, constant)) {
    data = find_node(CONS, NODE_FROM_INT(1), (node_ptr) constant_add);
    add_ref(constant_add);
    insert_assoc(self->constant_hash, constant, data);
  }
  else {
    /* increments the ref counter: */
    data = find_assoc(self->constant_hash, constant);
    setcar(data, NODE_FROM_INT(NODE_TO_INT(car(data)) + 1));
  }
} 


/**Function********************************************************************

  Synopsis           [Removes the given constant from the internal hash]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_remove_constant(BddEncCache_ptr self, node_ptr constant)
{
  node_ptr data;

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  data = find_assoc(self->constant_hash, constant);
  nusmv_assert(data != (node_ptr) NULL);
  
  if (NODE_TO_INT(car(data)) <= 1) {
    add_free(self->dd, (add_ptr) cdr(data));
    free_node(data);
    insert_assoc(self->constant_hash, constant, (node_ptr) NULL);
  }
  else {
    /* there are still references */
    setcar(data, NODE_FROM_INT(NODE_TO_INT(car(data)) - 1));
  }
}


/**Function********************************************************************

  Synopsis           [Returns true whether the given constant has been encoded]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean BddEncCache_is_constant_encoded(const BddEncCache_ptr self, 
                                        node_ptr constant)
{
  node_ptr data; 

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  data = find_assoc(self->constant_hash, constant);
  
  return ((data != (node_ptr) NULL) && (NODE_TO_INT(car(data)) > 0));
}


/**Function********************************************************************

  Synopsis      [Returns the ADD corresponding to the given constant, or 
  NULL if not defined]

  Description   [Returned ADD is referenced, NULL is returned if the given 
  constant is not currently encoded]
  
  Notes         []

  SideEffects   []

******************************************************************************/
add_ptr BddEncCache_lookup_constant(const BddEncCache_ptr self, 
                                    node_ptr constant)
{
  node_ptr data;
  add_ptr res = (add_ptr) NULL;

  BDD_ENC_CACHE_CHECK_INSTANCE(self);
  
  data = find_assoc(self->constant_hash, constant);
  if ((data != (node_ptr) NULL) &&  (NODE_TO_INT(car(data)) > 0)) {
    res = (add_ptr) cdr(data);
    if (res != (add_ptr) NULL) { add_ref(res); }    
  }

  return res; 
}


/**Function********************************************************************

  Synopsis           [Call this to insert the encoding for a given boolean
  variable]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_new_boolean_var(BddEncCache_ptr self, node_ptr var_name,
                         add_ptr var_add)
{
  BDD_ENC_CACHE_CHECK_INSTANCE(self);
  
  /* must be a variable declared inside the symbolic table */
  if (! SymbTable_is_symbol_var(self->symb_table, var_name)) {
    /* is it a non registered NEXT variable? */
    if ((node_get_type(var_name) == NEXT) && 
        !SymbTable_is_symbol_var(self->symb_table, car(var_name))) {
      internal_error("BddEncCache: trying to encode a new var not " \
                     "previously declared\n");
    }
  }

  /* not already encoded */
  nusmv_assert(! BddEncCache_is_boolean_var_encoded(self, var_name)); 
  
  add_ref(var_add);
  insert_assoc(self->vars_hash, var_name, (node_ptr) var_add);
}


/**Function********************************************************************

  Synopsis           [Removes the given variable from the internal hash]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_remove_boolean_var(BddEncCache_ptr self, node_ptr var_name)
{
  add_ptr add;

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  add = (add_ptr) find_assoc(self->vars_hash, var_name);
  nusmv_assert(add != (add_ptr) NULL);
  
  add_free(self->dd, add);
  insert_assoc(self->vars_hash, var_name, (node_ptr) NULL);
}


/**Function********************************************************************

  Synopsis           [Returns true whether the given boolean variable has 
  been encoded]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean BddEncCache_is_boolean_var_encoded(const BddEncCache_ptr self, 
                                           node_ptr var_name)
{
  BDD_ENC_CACHE_CHECK_INSTANCE(self);
  return (find_assoc(self->vars_hash, var_name) != (node_ptr) NULL);
}


/**Function********************************************************************

  Synopsis           [Retrieves the add associated with the given boolean 
  variable, if previously encoded. ]

  Description        [Returned add is referenced. NULL is returned if the 
  variable is not encoded.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
add_ptr BddEncCache_lookup_boolean_var(const BddEncCache_ptr self,
                                       node_ptr var_name)
{
  add_ptr res;

  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  res = (add_ptr) find_assoc(self->vars_hash, var_name);
  if (res != (add_ptr) NULL) { add_ref(res); }
  
  return res;  
}


/**Function********************************************************************

  Synopsis           [This method is used during the evaluation, 
  to set the internal cache of already evaluated expressions.]

  Description        [The provided array of ADD will belong to "self"
  and will be freed during destuctions of the class or setting a new 
  value for the same node_ptr.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_set_evaluation(BddEncCache_ptr self, node_ptr name,
                                AddArray_ptr add_array)
{
  AddArray_ptr old_array;
  BDD_ENC_CACHE_CHECK_INSTANCE(self);

  old_array = ADD_ARRAY(find_assoc(self->eval_hash, name));
  if ((old_array != BDD_ENC_EVALUATING) && (old_array != ADD_ARRAY(NULL))){
    nusmv_assert(old_array != add_array);
    AddArray_destroy(self->dd, old_array);
  }
  insert_assoc(self->eval_hash, name, (node_ptr) add_array); 
}


/**Function********************************************************************

  Synopsis           [Retrieve the evaluation of a given symbol, 
  as an array of ADD]

  Description        [ If not evaluated, given symbol returns NULL. If the 
  evaluation is in the process, BDD_ENC_EVALUATING is returned.
  Otherwise an array of ADD is returned.
  The returned array must be destroyed by the invoker!]

  NB: For all expressions except of the Word type the returned 
  array can contain only one element.
  NB: The evaluations of expression "0" and "1" (corresponding to
  add_zero() and add_one(), respectively) are inserted to the cached
  before the first invocation of this function.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr BddEncCache_get_evaluation(BddEncCache_ptr self, node_ptr name)
{
  AddArray_ptr res;
  BDD_ENC_CACHE_CHECK_INSTANCE(self);
  res = ADD_ARRAY(find_assoc(self->eval_hash, name));
  /* create a duplicate, if it is possible */
  if (ADD_ARRAY(NULL) == res || BDD_ENC_EVALUATING == res) return res;
  return AddArray_duplicate(res);
}


/**Function********************************************************************

  Synopsis [Cleans those hashed entries that are about a symbol that
  is being removed]

  Description [This is called by the BddEnc class when a layer is
  begin removed and the cache has to be cleaned up]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void BddEncCache_clean_evaluation_about(BddEncCache_ptr self, 
                                        NodeList_ptr symbs)
{
  node_ptr all_exprs = assoc_get_keys(self->eval_hash, true);
  node_ptr iter;
  
  for (iter=all_exprs; iter != Nil; iter = cdr(iter)) {
    node_ptr expr = car(iter);
    NodeList_ptr list = 
      Compile_get_expression_dependencies(self->symb_table, expr);
    ListIter_ptr var_iter;

    if (NodeList_get_length(list) == 0) {
      if (NodeList_belongs_to(symbs, expr)) {
        BddEncCache_set_evaluation(self, expr, ADD_ARRAY(NULL));  
      }
    }
    else {
      for (var_iter=NodeList_get_first_iter(symbs); !ListIter_is_end(var_iter);
           var_iter=ListIter_get_next(var_iter)) {
        node_ptr name = NodeList_get_elem_at(symbs, var_iter);    
        if (NodeList_belongs_to(list, name)) {
          /* removes the corresponding entry */
          BddEncCache_set_evaluation(self, expr, ADD_ARRAY(NULL));
        }
      }
    }

    NodeList_destroy(list);
  }
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Private initializer]

  Description        [Private initializer, called by the constructor]

  SideEffects        []

  SeeAlso            [bdd_enc_cache_deinit]

******************************************************************************/
static void bdd_enc_cache_init(BddEncCache_ptr self, 
                               SymbTable_ptr symb_table, DdManager* dd) 
{
  self->symb_table = symb_table;
  self->dd = dd;

  self->constant_hash = new_assoc();
  nusmv_assert(self->constant_hash != (hash_ptr) NULL);

  self->vars_hash = new_assoc();
  nusmv_assert(self->vars_hash != (hash_ptr) NULL);

  self->eval_hash = new_assoc();
  nusmv_assert(self->eval_hash != (hash_ptr) NULL);
  {
    /* insert the association between expression "1" and add_one(), and
       "0" and add_zero().
    */
    AddArray_ptr array;
 
    array = AddArray_from_add(add_zero(self->dd));
    BddEncCache_set_evaluation(self, zero_number, array);

    array = AddArray_from_add(add_one(self->dd));
    BddEncCache_set_evaluation(self, one_number, array);
  }
}



/**Function********************************************************************

  Synopsis           [Private deinitializer]

  Description        [Private deinitializer, called by the destructor]

  SideEffects        []

  SeeAlso            [bdd_enc_cache_init]

******************************************************************************/
static void bdd_enc_cache_deinit(BddEncCache_ptr self)
{
  st_foreach(self->constant_hash, &hash_free_add_counted, 
             (char*) self->dd);
  free_assoc(self->constant_hash);  

  st_foreach(self->vars_hash, &hash_free_add, (char*) self->dd);
  free_assoc(self->vars_hash);  

  st_foreach(self->eval_hash, &hash_free_add_array, (char*) self->dd);
  free_assoc(self->eval_hash);  
}


/**Function********************************************************************

  Synopsis           [Private micro function used when destroying caches of
  adds]

  Description        [Called when pushing the status, and during
  deinitialization]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static assoc_retval 
hash_free_add(char* key, char* data, char* arg) 
{
  if ((data != (char*) NULL)){
    add_free((DdManager*) arg, (add_ptr) data);
  }
  return ASSOC_DELETE;
}

/**Function********************************************************************

  Synopsis           [Private micro function used when destroying caches of
  adds]

  Description        [Called when pushing the status, and during
  deinitialization]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static assoc_retval 
hash_free_add_array(char* key, char* data, char* arg) 
{
  if ((data != (char*) NULL) && (ADD_ARRAY(data) != BDD_ENC_EVALUATING)) {
    AddArray_destroy((DdManager*) arg, ADD_ARRAY(data));
  }
  return ASSOC_DELETE;
}


/**Function********************************************************************

  Synopsis           [Private micro function used when destroying caches of
  adds]

  Description        [Called when pushing the status, and during
  deinitialization. The kind of nodes that must be removed here is 
  CONS(integer, add). Of course it is the add that must be freed.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static assoc_retval 
hash_free_add_counted(char* key, char* data, char* arg) 
{
  node_ptr cons = (node_ptr) data;
  if ((cons != (node_ptr)NULL)) {
    add_free((DdManager*) arg, (add_ptr) cdr(cons));
    free_node(cons);
  }

  return ASSOC_DELETE;
}
