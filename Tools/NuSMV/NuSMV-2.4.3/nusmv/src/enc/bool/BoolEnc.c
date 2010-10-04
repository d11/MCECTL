/**CFile***********************************************************************

  FileName    [BoolEnc.c]

  PackageName [enc.bool]

  Synopsis    [Implementaion of class 'BoolEnc']

  Description []

  SeeAlso     [BoolEnc.h]

  Author      [Roberto Cavada]

  Copyright   [
  This file is part of the ``enc.bool'' package of NuSMV version 2. 
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

  Revision    [$Id: BoolEnc.c,v 1.1.2.22.4.12 2007/05/11 08:37:48 nusmv Exp $]

******************************************************************************/

#include "BoolEnc.h" 
#include "BoolEnc_private.h" 

#include "enc/encInt.h"
#include "compile/compile.h"
#include "parser/symbols.h"

#include "utils/WordNumber.h"
#include "utils/utils.h" 
#include "utils/error.h" 
#include "utils/ustring.h" 


static char rcsid[] UTIL_UNUSED = "$Id: BoolEnc.c,v 1.1.2.22.4.12 2007/05/11 08:37:48 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define BOOL_ENC_DEFAULT_LAYER_SUFFIX \
  "_bool"

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/* See 'BoolEnc_private.h' for class 'BoolEnc' definition. */ 

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

  Synopsis     [A global variable shared between all the BoolEnc class 
  instances]

  Description  [Initialized the first time an instance of class BoolEnc is 
  created (see SymbTable constructor), and deinitialized when the last one 
  is destroyed]

******************************************************************************/
static node_ptr boolean_type = (node_ptr) NULL;
static int bdd_enc_instances = 0;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void bool_enc_finalize ARGS((Object_ptr object, void* dummy));

static void 
bool_enc_encode_var ARGS((BoolEnc_ptr self, node_ptr var, 
                          SymbLayer_ptr src_layer, SymbLayer_ptr dest_layer));

static node_ptr 
bool_enc_encode_scalar_var ARGS((BoolEnc_ptr self, node_ptr name, int suffix,
                                 node_ptr values, 
                                 SymbLayer_ptr src_layer, 
                                 SymbLayer_ptr dest_layer));

static void 
bool_enc_set_var_encoding ARGS((BoolEnc_ptr self, node_ptr name, 
                                node_ptr enc));

static void 
bool_enc_traverse_encoding ARGS((const BoolEnc_ptr self,
                                 node_ptr enc, NodeList_ptr list));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The BoolEnc class constructor]

  Description        [The BoolEnc class constructor]

  SideEffects        []

  SeeAlso            [BoolEnc_destroy]   
  
******************************************************************************/
BoolEnc_ptr BoolEnc_create(SymbTable_ptr symb_table)
{
  BoolEnc_ptr self = ALLOC(BoolEnc, 1);
  BOOL_ENC_CHECK_INSTANCE(self);

  bool_enc_init(self, symb_table);
  return self;
}


/**Function********************************************************************

  Synopsis           [The BoolEnc class destructor]

  Description        [The BoolEnc class destructor]

  SideEffects        []

  SeeAlso            [BoolEnc_create]   
  
******************************************************************************/
VIRTUAL void BoolEnc_destroy(BoolEnc_ptr self)
{
  BOOL_ENC_CHECK_INSTANCE(self);

  Object_destroy(OBJECT(self), NULL);
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is the name of 
  a bit variable that is part of a scalar var]

  Description        []

  SideEffects        []

  SeeAlso            [BoolEnc_get_scalar_var_of_bit]

******************************************************************************/
boolean BoolEnc_is_var_bit(const BoolEnc_ptr self, node_ptr name)
{
  BOOL_ENC_CHECK_INSTANCE(self);
  return (node_get_type(name) == BIT);
}


/**Function********************************************************************

  Synopsis           [Returns true if the given symbol is the name of 
  a scalar (non-boolean) variable]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
boolean BoolEnc_is_var_scalar(const BoolEnc_ptr self, node_ptr name)
{
  node_ptr enc;

  BOOL_ENC_CHECK_INSTANCE(self);
  
  enc = BoolEnc_get_var_encoding(self, name);
  return enc != boolean_type;
}



/**Function********************************************************************

  Synopsis           [Returns the name of the scalar variable whose 
  the given bit belongs]

  Description        [Returns the name of the scalar variable whose 
  the given bit belongs. The given var MUST be a bit]

  SideEffects        []

  SeeAlso            [BoolEnc_is_var_bit]

******************************************************************************/
node_ptr BoolEnc_get_scalar_var_from_bit(const BoolEnc_ptr self, node_ptr name)
{
  BOOL_ENC_CHECK_INSTANCE(self);
  nusmv_assert(BoolEnc_is_var_bit(self, name));

  return car(name);
}


/**Function********************************************************************

  Synopsis [Returns the list of boolean vars used in the encoding of
  given scalar var]

  Description        [Returned list must be destroyed by the caller]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
NodeList_ptr BoolEnc_get_var_bits(const BoolEnc_ptr self, node_ptr name)
{
  NodeList_ptr res; 
  node_ptr enc;

  BOOL_ENC_CHECK_INSTANCE(self);
  
  enc = BoolEnc_get_var_encoding(self, name);
  res = NodeList_create();
  bool_enc_traverse_encoding(self, enc, res);
  return res;
}


/**Function********************************************************************

  Synopsis           [Given a variable, returns its boolean encoding]

  Description        [Given variable must have been encoded by self]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr BoolEnc_get_var_encoding(const BoolEnc_ptr self, node_ptr name)
{
  node_ptr enc; 

  BOOL_ENC_CHECK_INSTANCE(self);

  enc = find_assoc(self->var2enc, name); 
  nusmv_assert(enc != Nil); /* must be previoulsy encoded */
  return enc;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The BoolEnc class private initializer]

  Description        [The BoolEnc class private initializer]

  SideEffects        []

  SeeAlso            [BoolEnc_create]   
  
******************************************************************************/
void bool_enc_init(BoolEnc_ptr self, SymbTable_ptr symb_table)
{
  /* base class initialization */
  base_enc_init(BASE_ENC(self), symb_table);
  
  /* static members */
  /* init a shared variable (done by the first constructor call only) */
  if (boolean_type == (node_ptr) NULL) { 
    nusmv_assert(bdd_enc_instances == 0);
    boolean_type = find_node(BOOLEAN, Nil, Nil); 
  }

  bdd_enc_instances += 1;

  /* members initialization */
  self->var2enc = new_assoc();

  /* virtual methods settings */  
  OVERRIDE(Object, finalize) = bool_enc_finalize;

  /* inherited by BaseEnc: */
  OVERRIDE(BaseEnc, commit_layer) = bool_enc_commit_layer; 
  OVERRIDE(BaseEnc, remove_layer) = bool_enc_remove_layer; 

}


/**Function********************************************************************

  Synopsis           [The BoolEnc class private deinitializer]

  Description        [The BoolEnc class private deinitializer]

  SideEffects        []

  SeeAlso            [BoolEnc_destroy]   
  
******************************************************************************/
void bool_enc_deinit(BoolEnc_ptr self)
{
  /* members deinitialization */
  free_assoc(self->var2enc);  

  /* static members deinitialization: */
  bdd_enc_instances -= 1;
  if (bdd_enc_instances == 0) { 
    /* this is the last instance */
    nusmv_assert(boolean_type != (node_ptr) NULL);
    free_node(boolean_type);
    boolean_type = (node_ptr) NULL;
  }

  /* base class deinitialization */
  base_enc_deinit(BASE_ENC(self));
}



/**Function********************************************************************

  Synopsis           [Encodes all variables within the given layer]

  Description [A new layer will be constructed if there is not yet
  any.  The new layer will be called ${layer_name}_bool and will be
  added to the symbol table that self uses. The new layer will be
  locked by self either until the layer is was originally created from
  is released or until self is destroyed. Given a committed layer, it
  is always possible to obtain the corresponding created boolean layer
  by calling BoolEnc_get_bool_layer. ]

  SideEffects        [A new layer will be created if not already existing]

  SeeAlso            [bool_enc_remove_layer]   
  
******************************************************************************/
VIRTUAL void bool_enc_commit_layer(BaseEnc_ptr base_enc, 
                                   const char* layer_name)
{
  BoolEnc_ptr self;
  SymbLayer_ptr src_layer, dest_layer;
  const char* dest_layer_name;
  NodeList_ptr vars;
  ListIter_ptr iter;

  self = BOOL_ENC(base_enc);

  /* Calls the base method to add this layer */
  base_enc_commit_layer(base_enc, layer_name);

  src_layer = SymbTable_get_layer(BASE_ENC(self)->symb_table, layer_name);

  /* queries for the corresponding boolean layer, to see if it is
     already up and running: */
  dest_layer_name = BoolEnc_scalar_layer_to_bool_layer(layer_name);
  dest_layer = SymbTable_get_layer(BASE_ENC(self)->symb_table, dest_layer_name);
  if (dest_layer == SYMB_LAYER(NULL)) {
    /* does not exist yet, creates one. It will be created with the same policy 
       the layer it derives from has */
    dest_layer = SymbTable_create_layer(BASE_ENC(self)->symb_table, 
                                        dest_layer_name, 
                                        SymbLayer_get_insert_policy(src_layer));
  }
  
  /* becomes an user of the bool layer: */
  base_enc_commit_layer(base_enc, dest_layer_name);

  /* now encodes all variables within the given layer, and puts them into the 
     boolean layer */
  vars = SymbLayer_get_all_vars(src_layer);
  iter = NodeList_get_first_iter(vars);
  while (!ListIter_is_end(iter)) {
    node_ptr var;
    var = NodeList_get_elem_at(vars, iter);

    if (opt_verbose_level_gt(options, 2)) {
      fprintf(nusmv_stderr, "BoolEnc: encoding variable '");
      print_node(nusmv_stderr, var);
      fprintf(nusmv_stderr, "'...\n");
    }
    
    bool_enc_encode_var(self, var, src_layer, dest_layer);

    iter = ListIter_get_next(iter);
  }
}
      

/**Function********************************************************************

  Synopsis           [Removes the encoding of all variables occurring within 
  the given layer, and those that had been created within the corresponding 
  boolean layer during the boolean encoding. Then releases both the layers, 
  and removes the boolean layer from the symbol table.]

  Description        [  WARNING: If the layer has been
  renamed after having been committed, it is the *new* name (the name
  the layer has when it is being removed) that must be used, and *not*
  the name that had been used when commiting it.]

  SideEffects        []

  SeeAlso            [bool_enc_commit_layer]   
  
******************************************************************************/
VIRTUAL void bool_enc_remove_layer(BaseEnc_ptr base_enc, 
                                   const char* layer_name)
{
  BoolEnc_ptr self;
  SymbLayer_ptr layer, bool_layer;
  NodeList_ptr vars;
  ListIter_ptr iter;
  const char* bool_layer_name;

  self = BOOL_ENC(base_enc);

  bool_layer_name = BoolEnc_scalar_layer_to_bool_layer(layer_name);
  layer = SymbTable_get_layer(BASE_ENC(self)->symb_table, layer_name);
  bool_layer = SymbTable_get_layer(BASE_ENC(self)->symb_table, bool_layer_name);  

  /* removes all encodings from vars within layer */
  vars = SymbLayer_get_all_vars(layer);
  iter = NodeList_get_first_iter(vars);
  while (!ListIter_is_end(iter)) {
    node_ptr var;
    var = NodeList_get_elem_at(vars, iter);

    if (opt_verbose_level_gt(options, 2)) {
      fprintf(nusmv_stderr, "BoolEnc: removing encoding of variable '");
      print_node(nusmv_stderr, var);
      fprintf(nusmv_stderr, "'...\n");
    }
  
    if (BoolEnc_get_var_encoding(self, var) != Nil) {
      NodeList_ptr bits;
      ListIter_ptr iter;

      /* Gets rid of all bits that were created by the encoding process
         of this variable */
      bits = BoolEnc_get_var_bits(self, var);
      iter = NodeList_get_first_iter(bits);
      while (!ListIter_is_end(iter)) {
        node_ptr bit;
        bit = NodeList_get_elem_at(bits, iter);
        if (BoolEnc_get_var_encoding(self, bit) != Nil) {
          bool_enc_set_var_encoding(self, bit, Nil);
        }
        iter = ListIter_get_next(iter);
      }
      NodeList_destroy(bits);

      /* gets rid of the var's encoding as well */
      bool_enc_set_var_encoding(self, var, Nil);
    }
  
    iter = ListIter_get_next(iter);
  }

  /* Calls the base method to get rid of both the source and boolean layer */
  base_enc_remove_layer(base_enc, layer_name);
  base_enc_remove_layer(base_enc, bool_layer_name);

  /* Removes the boolean layer from the symb table */
  SymbTable_remove_layer(BASE_ENC(self)->symb_table, bool_layer);
}



/**Function********************************************************************

  Synopsis           [Given the name of a usual scalar layer, a name of the 
  corresponding boolean layer is returned.]

  Description        [
  Returned string belongs to the function and should NOT be modified or freed.
  ]

  SideEffects        []

  SeeAlso            []   
  
******************************************************************************/
const char* BoolEnc_scalar_layer_to_bool_layer(const char* layer_name)
{
  char* bool_layer_name;
  string_ptr str;
  
  bool_layer_name = ALLOC(char, strlen(layer_name) + 
                          strlen(BOOL_ENC_DEFAULT_LAYER_SUFFIX) + 1);
  nusmv_assert(bool_layer_name != (char*) NULL);
  strcpy(bool_layer_name, layer_name);
  strcat(bool_layer_name, BOOL_ENC_DEFAULT_LAYER_SUFFIX);
  
  /* the strings are used to avoid caring about memory */
  str = find_string(bool_layer_name);
  FREE(bool_layer_name);

  return str_get_text(str);
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [The BoolEnc class virtual finalizer]

  Description [Called by the class destructor]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static VIRTUAL void bool_enc_finalize(Object_ptr object, void* dummy) 
{
  BoolEnc_ptr self = BOOL_ENC(object);

  bool_enc_deinit(self);
  FREE(self);
}


/**Function********************************************************************

  Synopsis    [Encodes a single variable ]

  Description [If it is a scalar variable, its values are expanded and a
  set of bits (new boolean variables) will be created within the
  dest_layer. All leaves (constant values of the values) will be
  created within the src_layer, it they are not defined yet.]
  
  SideEffects []

  SeeAlso     []

******************************************************************************/
static void 
bool_enc_encode_var(BoolEnc_ptr self, node_ptr var, 
                    SymbLayer_ptr src_layer, SymbLayer_ptr dest_layer)
{
  node_ptr scalar_enc;
  SymbType_ptr type;

  type = SymbTable_get_var_type(BASE_ENC(self)->symb_table, var);

  /*  Only ENUM and WORD types are implemented at the moment */
  switch (SymbType_get_tag(type)) {
  case SYMB_TYPE_ENUM: { /* ENUM type */
    node_ptr values = SymbType_get_enum_type_values(type);
    nusmv_assert(Nil != values);
    scalar_enc = (values == boolean_range)
      ? boolean_type /* it is a boolean var */
      : bool_enc_encode_scalar_var(self, var, 0, values, src_layer, dest_layer); 
    break;
  }

  case SYMB_TYPE_WORD: /* Word type */
    if (opt_encode_word_monolithic(options)) { /* encode Word as one ADD tree */
      node_ptr values = SymbType_generate_all_word_values(type);
      scalar_enc = bool_enc_encode_scalar_var(self, var, 0, values, src_layer,
                                              dest_layer); 
    } 
    else { /* encode Word as an array of bits' (ADD trees) */
      int width = SymbType_get_word_width(type);
      int suffix;
      scalar_enc = Nil;
      
      for (suffix = width-1; suffix >= 0; --suffix) {
        node_ptr bitVar = find_node(BIT, var, NODE_FROM_INT(suffix));
        /* declare a new boolean var -- bit of the Word*/
        if (! SymbTable_is_symbol_var(BASE_ENC(self)->symb_table, bitVar)) {
          /* the type is created every time, because the "reset" frees them */
          SymbType_ptr type = SymbType_create(SYMB_TYPE_ENUM, boolean_range);
          
          if (SymbTable_is_symbol_input_var(BASE_ENC(self)->symb_table, var)) {
            SymbLayer_declare_input_var(dest_layer, bitVar, type);
          }
          else SymbLayer_declare_state_var(dest_layer, bitVar, type);
          
          bool_enc_set_var_encoding(self, bitVar, boolean_type);
        }
        /* create a list of bits (high bits at the beginning).
           NB: representation of a variable is given by node_ptr and 
           with help of find_node (not array_t, for example), because
           the destructor does not free any memory
        */
        scalar_enc = find_node(CONS, bitVar, scalar_enc);
      } /* for */
      /* wrap the list into a WORD node */
      scalar_enc = find_node(WORD, reverse_ns(scalar_enc), 
                             find_node(NUMBER, NODE_FROM_INT(width), Nil));
    }
    break;

  case SYMB_TYPE_WORDARRAY: /* WordArray type */ {
    fprintf(nusmv_stderr, "Unable to booleanize WordArrays.\n");
    nusmv_assert((false));
  }

  case SYMB_TYPE_INTEGER:
  case SYMB_TYPE_REAL:
    return;
  default: nusmv_assert(false); /* no other kinds of types are implemented */
  } /* switch */
  
  bool_enc_set_var_encoding(self, var, scalar_enc);
}


/**Function********************************************************************

  Synopsis [Encodes a scalar variable, by creating all boolean vars
  (bits) needed to encode the var itself. Created bool vars are pushed
  within the given destination layer. ]

  Description [The returned structure is a tree, whose internal nodes
  are ITE nodes or BITS variables, and leaves are constants values. 
  
  <expr> :: ITE ( COLON (bit, left), right) | 
            constant

  where bit is a variable name (a bit), and left and right are <expr>.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr 
bool_enc_encode_scalar_var(BoolEnc_ptr self, node_ptr name, int suffix,
                           node_ptr values, 
                           SymbLayer_ptr src_layer, SymbLayer_ptr dest_layer)
{
  node_ptr var, left, right;

  /* Final case: we reached a leaf */
  if (cdr(values) == Nil) {
    var = find_atom(car(values));
    if (SymbLayer_can_declare_constant(src_layer, var)) {
      SymbLayer_declare_constant(src_layer, var);
    }
    return var;
  }

  /* Intermediate case, declare the scalar variable */
  var = find_node(BIT, name, NODE_FROM_INT(suffix));
  if (! SymbTable_is_symbol_var(BASE_ENC(self)->symb_table, var)) {
    SymbType_ptr type = SymbType_create(SYMB_TYPE_ENUM, boolean_range);
    if (SymbTable_is_symbol_input_var(BASE_ENC(self)->symb_table, name)) { 
      SymbLayer_declare_input_var(dest_layer, var, type);
    }
    else SymbLayer_declare_state_var(dest_layer, var, type);
    
    bool_enc_set_var_encoding(self, var, boolean_type);
  }
  

  /* Finally construct the sub binary tree, by decomposing left and
     right sides: */
  left  = bool_enc_encode_scalar_var(self, name, suffix + 1, 
                                     even_elements(values), 
                                     src_layer, dest_layer);

  right = bool_enc_encode_scalar_var(self, name, suffix + 1, 
                                     odd_elements(values), 
                                     src_layer, dest_layer);

  return find_node(IFTHENELSE, find_node(COLON, var, left), right);
}



/**Function********************************************************************

  Synopsis           [Associates the given variable with the specified 
  boolean encoding]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void 
bool_enc_set_var_encoding(BoolEnc_ptr self, node_ptr name, node_ptr enc)
{
  insert_assoc(self->var2enc, name, enc);
}



/**Function********************************************************************

  Synopsis           [Fills the given list with the BIT vars which 
  occurs into the given var encoding]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void bool_enc_traverse_encoding(const BoolEnc_ptr self, 
                                       node_ptr enc, NodeList_ptr list)
{
  node_ptr bit;
  
  /* constant or number terminate (numbers are not stored as constants): */
  if ( SymbTable_is_symbol_constant(BASE_ENC(self)->symb_table, enc)
       || (node_get_type(enc) == NUMBER) || (enc == boolean_type)) return;

  if (node_get_type(enc) == IFTHENELSE) { /* usual IFTHENELSE encoding */
    bit = car(car(enc));
    if (! NodeList_belongs_to(list, bit)) NodeList_append(list, bit);
    
    bool_enc_traverse_encoding(self, cdr(car(enc)), list); /* 'then' */
    bool_enc_traverse_encoding(self, cdr(enc), list);      /* 'else' */
  }
  else if (node_get_type(enc) == WORD) { /* Word, i.e. array of bit-vars */
    node_ptr iter;
    nusmv_assert(!opt_encode_word_monolithic(options)); /*Word encoded as bit-list*/
    for (iter = car(enc); iter != Nil; iter = cdr(iter)) {
      if(!NodeList_belongs_to(list, car(iter))) NodeList_append(list, car(iter));
    }
  }
  /* no other kind of node can appear at this level: */
  else nusmv_assert(false);
}

/**AutomaticEnd***************************************************************/
