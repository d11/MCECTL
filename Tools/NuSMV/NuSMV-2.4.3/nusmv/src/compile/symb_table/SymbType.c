/**CFile*****************************************************************

  FileName    [SymbType.c]

  PackageName [compile.symb_table]

  Synopsis    [Implementation of functions dealing with the type of 
  variables in a symbol table.]

  Description []

  SeeAlso     [SymbType.h]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``compile.symb_table'' package of NuSMV
  version 2.  Copyright (C) 2005 by ITC-irst.

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

#include "SymbType.h"
#include "symb_table.h"
#include "symb_table_int.h"

#include "compile/compileInt.h"
#include "parser/symbols.h"
#include "utils/WordNumber.h"

static char rcsid[] UTIL_UNUSED = "$Id: SymbType.c,v 1.1.2.26.4.3 2007/04/20 12:41:29 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct SymbType_TAG {
  SymbTypeTag tag;
  node_ptr body;
  boolean isMemorySharingInstance;
} SymbType;


/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Constants declarations                                                    */
/*---------------------------------------------------------------------------*/
/* a set of constants to identify different enum-types */
enum Enum_types{ ENUM_TYPE_BOOLEAN, 
                 ENUM_TYPE_PURE_INT,
                 ENUM_TYPE_PURE_SYMBOLIC,
                 ENUM_TYPE_INT_SYMBOLIC,
                };

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Class SymbType constructor]

  Description [The tag must be a correct tag. The 'body' is the
  additional info corresponding to a particular kind of the type: for
  a enum type the body is the list of values; for "INT" or "REAL" the
  body is unused, and set to Nil; for "WORD" it is the NUMBER node
  defining the width of the type; for "WORDARRAY", the body is a pair
  of NUMBER nodes, defining the width of the address, and the width of
  the value. Set-types are used with expressions which represent a set
  values.  "NO-TYPE" is used with expressions which normally do not
  have any type such as assignments. "ERROR" type indicates an error
  (not an actual type).
  No-type, error-type and all set-types (boolean-set, integer-set,
  symbolic-set, symbolic-integer-set) should not be created with this
  constructor, but only with memory-shared function
  SymbTablePkg_..._type.  The reason behind this constrain is that
  only expressions (not variables) can have these types, therefore
  only memory-shared versions of these types are required.

  The constructor does not create a copy of the body, but just remember
  the pointer

  NB: system "reset" command destroys all node_ptr objects, including those
  used in SymbType_ptr. So destroy all symbolic types before the destruction
  of node_ptr objects, i.e. before or during "reset"]

  SideEffects        [allocate memory]

  SeeAlso            [SymbType_destroy]

******************************************************************************/
SymbType_ptr SymbType_create(SymbTypeTag tag, node_ptr body)
{
  SymbType_ptr self = ALLOC(SymbType, 1);
  SYMB_TYPE_CHECK_INSTANCE(self);

  nusmv_assert(SYMB_TYPE_NONE <= tag && tag <= SYMB_TYPE_ERROR);
  self->tag = tag;
  self->isMemorySharingInstance = false;

  if (SYMB_TYPE_ENUM == tag) {
    /* these is an enum type. Find out which kind of enum this is.
       It can be a boolean enum, a pure integer enum, a pure symbolic enum 
       or integer-symbolic enum.
    */
    enum Enum_types enumType = -1; /* for sure incorrect value */

    if (body == boolean_range) { 
      /* boolean enumeration should be very specific, i.e. it should be
         created with the variable 'boolean_range'. So in
         compileFlatten.c in function instantiate_var all
         possible boolean types (such as range 0..1 or
         enum {1,0}) are converted to boolean type.
         N.B. In a enum, TRUE and FALSE constants are
         considered as 1 and 0 (see CompileFlatten_normalise_value_list).
      */
      enumType = ENUM_TYPE_BOOLEAN;
    }
    else {
      node_ptr iter = body;
      int thereIsInt = 0;
      int thereIsSymbolic = 0;
      while (iter != Nil) {
        switch (node_get_type(car(iter))) {
        case ATOM: /* symbolic constant */
        case BIT:
        case DOT:
        case ARRAY:
          thereIsSymbolic = 1;
          break;
        case NUMBER: /* integer numbers */
          thereIsInt = 1; 
          break;

        case TRUEEXP:
        case FALSEEXP:
          /* TRUE and FALSE cannot be in a enum, because 
             funtion CompileFlatten_normalise_value_list normilised them out.
          */
          nusmv_assert(false);

        default: /* only numbers, true, false, and 
                    symbolic constants can be in the constant-list (enum-type).
                 */
          print_node(nusmv_stderr, body); fprintf(nusmv_stderr,"\n\n");
          nusmv_assert(false);
        }
        iter = cdr(iter);
      }/* while */
      /* there must be constant or symbolic constant in the enum */
      nusmv_assert(thereIsInt | thereIsSymbolic);
      
      enumType = (thereIsInt && thereIsSymbolic)
                ?  ENUM_TYPE_INT_SYMBOLIC 
                : thereIsInt
                  ? ENUM_TYPE_PURE_INT
                  : ENUM_TYPE_PURE_SYMBOLIC;
    } /* if-else (body == boolean_range) */
    /* we add additional node to remember the kind of the enum-type */
    self->body = new_node(enumType, body, Nil);
  } 
  else {
    /* except ENUM type only Word and WordArraycan can have body */
    nusmv_assert((Nil != body) == ((SYMB_TYPE_WORD == tag) ||
                                   (SYMB_TYPE_WORDARRAY == tag)));
    /* usual type => just remember the body */
    self->body = body;
  }
  return self;
}


/**Function********************************************************************

  Synopsis           [Class SymbType destructor]

  Description        [Deallocate the memory. The destructor 
  does not deallocate memory from the type's body (since the
  constructor did not created the body).

  NOTE: If self is a memory sharing type instance, i.e. a type returned by 
  SymbTablePkg_..._type functions then the destructor will not delete 
  the type.]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
void SymbType_destroy(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  if (!self->isMemorySharingInstance) { /* this is a usual type instance */
    if (SYMB_TYPE_ENUM == self->tag) {
      /* for the enum-type we added one addional node => remove it now */
      node_node_setcar(self->body, Nil);
      free_node(self->body);
    }
    FREE(self);
  }
}


/**Function********************************************************************

  Synopsis           [Class SymbType copy-constructor]

  Description [This function takes one type and returns its copy.

  Note: the body of the type is not copied, i.e. just pointer is remembered.
  See SymbType_create for more info about body.

  Note: the input type should not be a memory-shared type (since there is no
  meaning in coping a memory sharing type).
  ]

  SideEffects        [allocate memory]

  SeeAlso            [SymbType_destroy]

******************************************************************************/
SymbType_ptr SymbType_copy(SymbType_ptr self)
{
  return SymbType_create(self->tag,
                         self->tag == SYMB_TYPE_ENUM 
                         ? car(self->body) : self->body);
}


/**Function********************************************************************

  Synopsis           [Private class SymbType constructor
  for memory sharing type instances]

  Description        [The difference from the public constructor is that this
  constructor marks the created type as a memory sharing type. As
  result the public constructor will not be able to destroy memory
  sharing instance of a type. Use the private constructor
  SymbType_destroy_memory_sharing_type to destroy such instances.
  ]

  SideEffects        [allocate memory]

  SeeAlso            [SymbType_create, SymbType_destroy_memory_sharing_type]

******************************************************************************/
SymbType_ptr SymbType_create_memory_sharing_type(SymbTypeTag tag, node_ptr body)
{
  SymbType_ptr self = SymbType_create(tag, body);
  self->isMemorySharingInstance = true;
  return self;
}


/**Function********************************************************************

  Synopsis           [Private Class SymbType destructor
  for memory sharing instances of types.]

  Description        [The same as the public destructor SymbType_destroy
  but 'self' has to be created by private constructor 
  SymbType_create_memory_sharing_type only.]
  
  SideEffects        []

  SeeAlso            [SymbType_create_memory_sharing_type, SymbType_create]

******************************************************************************/
void SymbType_destroy_memory_sharing_type(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  /* this is a memory sharing instance */
  nusmv_assert(self->isMemorySharingInstance);

  self->isMemorySharingInstance = false;
  SymbType_destroy(self);
}


/**Function********************************************************************

  Synopsis           [Returns the tag (the kind) of the type]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
SymbTypeTag SymbType_get_tag(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return self->tag;
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a enum-type and its values are from 
  boolean range. Otherwise - returns false.]

  Description [The kind of enum-type is analysed in the constructor.]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_boolean_enum(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_ENUM == self->tag && 
    ENUM_TYPE_BOOLEAN == node_get_type(self->body);
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a enum-type and its value 
  are integers only. Otherwise - returns false.]

  Description [The kind of enum-type is analysed in the constructor.]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_pure_int_enum(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_ENUM == self->tag && 
    ENUM_TYPE_PURE_INT == node_get_type(self->body);
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a enum-type and its value 
  are symbolic constants only. Otherwise - returns false.]

  Description [The kind of enum-type is analysed in the constructor.]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_pure_symbolic_enum(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_ENUM == self->tag && 
    ENUM_TYPE_PURE_SYMBOLIC == node_get_type(self->body);
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a enum-type and its value 
  are symbolic AND integer constants. Otherwise - returns false.]

  Description [The kind of enum-type is analysed in the constructor.]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_int_symbolic_enum(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_ENUM == self->tag && 
    ENUM_TYPE_INT_SYMBOLIC == node_get_type(self->body);
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a Word type and the width of 
  the word is 1. Otherwise - returns false.]

  Description []

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_word_1(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  /* expects the body of Word type to be NUMBER */
  return SYMB_TYPE_WORD == self->tag && 1 == node_get_int(self->body);
}


/**Function********************************************************************

  Synopsis [Returns true, if the type is a Word type]

  Description []

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_word(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);

  /* expects the body of Word type to be NUMBER */
  return SYMB_TYPE_WORD == self->tag;
}


/**Function********************************************************************

  Synopsis    [Returns true, if the type is one of the set-types, i.e.
  boolean-set, integer-set, symbolic-set, integer-symbolic-set, and
  false otherwise.]

  Description []

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_set(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_SET_BOOL == self->tag ||
    SYMB_TYPE_SET_INT == self->tag ||
    SYMB_TYPE_SET_SYMB == self->tag ||
    SYMB_TYPE_SET_INT_SYMB == self->tag;
}


/**Function********************************************************************

  Synopsis    [Returns true, if the type is a error-type, and false otherwise.]

  Description [Error type is used to indicate an error]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_error(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_ERROR == self->tag;
}


/**Function********************************************************************

  Synopsis    [Returns true, if the type is a statement-type, 
  and false otherwise.]

  Description []

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
boolean SymbType_is_statement(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  return SYMB_TYPE_STATEMENT == self->tag;
}



/**Function********************************************************************

  Synopsis           [Genereate and returns a list of all possible values 
  of a particular Word type]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr SymbType_generate_all_word_values(SymbType_ptr self)
{
  /* create all the constants for the Word type */
  int width;
  unsigned long long value;
  node_ptr list;
  
  /* All assertions are checked in the function SymbType_get_word_width */

  width = SymbType_get_word_width(self);
  nusmv_assert(width <= WordNumber_max_width());
  
  
  list = Nil; 
  /* create the max value. Two left shifts are used because in C 
     shifting by a full width is not allowed
  */
  value = ~((~0ull) << (width - 1) << 1);
  ++value;
  do {
    --value;
    list = cons(find_node(NUMBER_WORD, 
                          (node_ptr)WordNumber_from_integer(value, width), 
                          Nil),
                 list);
  }
  while ( value > 0); 
  return list;
}


/**Function********************************************************************

  Synopsis           [Returns the width of a Word type]

  Description        [The given type should be Word and the 
  body of the type (given to the constructor) should be NUMBER node.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbType_get_word_width(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  nusmv_assert(SYMB_TYPE_WORD == self->tag &&
               node_get_type(self->body) == NUMBER);
  return node_get_int(self->body); /* the width */
}


/**Function********************************************************************

  Synopsis           [Returns the width of the address in a WordArray type]

  Description        [.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbType_get_wordarray_awidth(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);

  nusmv_assert(SYMB_TYPE_WORDARRAY == self->tag &&
               node_get_type(self->body) == CONS && 
               node_get_type(car(self->body)) == NUMBER &&
               node_get_type(cdr(self->body)) == NUMBER);
  
  return node_get_int(car(self->body)); /* the address width */
}


/**Function********************************************************************

  Synopsis           [Returns the width of the value in a WordArray type]

  Description        [.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int SymbType_get_wordarray_vwidth(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);

  nusmv_assert(SYMB_TYPE_WORDARRAY == self->tag &&
               node_get_type(self->body) == CONS &&
               node_get_type(car(self->body)) == NUMBER &&
               node_get_type(car(self->body)) == NUMBER);
  
  return node_get_int(cdr(self->body)); /* the value width */
}



/**Function********************************************************************

  Synopsis    [Returns the line number where the type was declared.]

  Description [The body of the type, provided during construction, is
  a node NUMBER specifying the width of the Word or a node CONS
  specifying the address-value widths or WordArray.  This node was
  create during parsing and contains the line number of the type
  declaration.
  NB: The type should not be memory-sharing.
  NB: Virtually this function is used only in TypeChecker_is_type_wellformed
  ]

  SideEffects        []

  SeeAlso            [SymbType_create]

******************************************************************************/
int SymbType_get_word_line_number(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  nusmv_assert(( SYMB_TYPE_WORD == self->tag &&
                 node_get_type(self->body) == NUMBER
               ) ||
               ( SYMB_TYPE_WORDARRAY == self->tag &&
                 node_get_type(self->body) == CONS));
  nusmv_assert(!self->isMemorySharingInstance); /* type is not memory-shared */
  return node_get_lineno(self->body);
}


/**Function********************************************************************

  Synopsis           [Returns the list of values of an enum type]

  Description        [The given type has to be a ENUM type.
  The return list is a list of all possible values of a enum type. This list
  was provided during construction.
  
  NB: Memory sharing types do not have particular values, since they
  are "simplified".]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr SymbType_get_enum_type_values(SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  nusmv_assert(SYMB_TYPE_ENUM == self->tag);
  nusmv_assert(!self->isMemorySharingInstance); /* type is not memory-shared */
  /* for the enum-type we added one addional node => skip it now */
  return car(self->body);
}


/**Function********************************************************************

  Synopsis            [Prints the type structure to the output stream.]

  Description         [This function is made very similar to print_node.
  If a Enum type was created with SymbType_create then all its values will be 
  printed, otherwise the type was created with SymbTablePkg_..._type
  and simplified type name (instead of actual type values) is printed.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void SymbType_print (FILE* output_stream, SymbType_ptr self)
{
  SYMB_TYPE_CHECK_INSTANCE(self);
  switch (self->tag) {

  case SYMB_TYPE_NONE: 
    fprintf(output_stream, "no-type");
    break;

  case SYMB_TYPE_STATEMENT: 
    fprintf(output_stream, "statement");
    break;

  case SYMB_TYPE_ENUM: 
    if (!self->isMemorySharingInstance) {
      /* this is a usual type instance => print all the values */
      if (ENUM_TYPE_BOOLEAN == node_get_type(self->body)) {
        /* we distinguish boolean type from all other Enum types */
        fprintf(output_stream, "boolean");
      }
      else { /* print all the values */
        node_ptr l = SymbType_get_enum_type_values(self);
        fprintf(output_stream, "{");
        while (l != Nil) {
          print_node(output_stream, car(l));
          l = cdr(l);
          if (l != Nil) { fprintf(output_stream, ", "); }
        }
        fprintf(output_stream, "}");
      }
    } 
    else { /* this is memory sharing types (therefore, are "simplified") */
      switch(node_get_type(self->body)) {
      case ENUM_TYPE_BOOLEAN:
        fprintf(output_stream, "boolean");
        break;
      case ENUM_TYPE_PURE_INT:
        fprintf(output_stream, "integer-enum");
        break;
      case ENUM_TYPE_PURE_SYMBOLIC:
        fprintf(output_stream, "symbolic-enum");
        break;
      case ENUM_TYPE_INT_SYMBOLIC:
        fprintf(output_stream, "integer-and-symbolic-enum");
        break;
      default: nusmv_assert(false);
      }
    } /* else */
    break;

  case SYMB_TYPE_INTEGER:   
    fprintf(output_stream, "integer");
    break;;

  case SYMB_TYPE_REAL:  
    fprintf(output_stream, "real");
    break;

  case SYMB_TYPE_WORD:  
    fprintf(output_stream, "word[");
    fprintf(output_stream, "%d", SymbType_get_word_width(self));
    fprintf(output_stream, "]");
    break;

  case SYMB_TYPE_WORDARRAY:  
    fprintf(output_stream, "array ");

    fprintf(output_stream, "word[");
    fprintf(output_stream, "%d", SymbType_get_wordarray_awidth(self));
    fprintf(output_stream, "]");

    fprintf(output_stream, " of ");

    fprintf(output_stream, "word[");
    fprintf(output_stream, "%d", SymbType_get_wordarray_vwidth(self));
    fprintf(output_stream, "]");
    break;

  case SYMB_TYPE_SET_BOOL:  fprintf(output_stream, "boolean-set"); break;
  case SYMB_TYPE_SET_INT:  fprintf(output_stream, "integer-set"); break;
  case SYMB_TYPE_SET_SYMB: fprintf(output_stream, "symbolic-set"); break;
  case SYMB_TYPE_SET_INT_SYMB: fprintf(output_stream, "integer-symbolic-set");
    break;

  case SYMB_TYPE_ERROR:
    fprintf(output_stream, "error-type");
    break;

  default: nusmv_assert(false);
  }
  return;
}



/**Function********************************************************************

  Synopsis           [returns true if the given type is "backward compatible",
  i.e. a enum or integer type.]

  Description        [We distinguish "old" types because we may want to turn
  off the type checking on these types for backward
  compatibility. Integer is also considered as "old", because an enum
  of integer values is always casted to Integer.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
boolean SymbType_is_back_comp(SymbType_ptr type)
{
  return SYMB_TYPE_ENUM == SymbType_get_tag(type) ||
    SYMB_TYPE_INTEGER == SymbType_get_tag(type);
}



/**Function********************************************************************

  Synopsis           [Returns one of the given types, if the other one
  can be implicitly converted to the former one. Otherwise - Nil.]

  Description        [The implicit conversion is performed
  in accordance to the type order.
  NOTE: only memory-shared types can be given to this function.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
SymbType_ptr SymbType_get_greater(SymbType_ptr type1, SymbType_ptr type2)
{
  SymbType_ptr returnType;

  returnType = SymbType_convert_right_to_left(type1, type2);
  if (returnType) return returnType; /* conversion was successful */
  /* return with swapped types and return whatever is obtained */
  return SymbType_convert_right_to_left(type2, type1);
}


/**Function********************************************************************

  Synopsis           [Returns the left type, if the right
  one can be implicitly converted to the left one. NULL - otherwise]

  Description        [The implicit conversion is performed
  in accordance to the type order.
  NOTE: only memory-shared types can be given to this function.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
SymbType_ptr 
SymbType_convert_right_to_left(SymbType_ptr leftType, SymbType_ptr rightType)
{
  /* I expect the given types are shared. So I will not worry about memory */
  
  /* the same pointers, so, the same types, so, just return */
  if (leftType == rightType) return leftType;

  /* pointer are different => the types are different.
     Try to perform implicit convertion of the right operand to the left one.
     No need to care about word, word-array and no-type since
     they cannot be implicitly converted to any other type.
     NB: No-type is created for assignment, and symbolic (predicate-normalised)
     FSM may push assignments into case-expressions, for example.
  */


  /* integer * bool => integer */
   if (SymbType_get_tag(leftType) == SYMB_TYPE_INTEGER) {
     if (SymbType_is_boolean_enum(rightType)) return leftType;
     else return nullType;
   }
   
   /* convert right from bool to Int to decrease number of cases to check */
   if (SymbType_is_boolean_enum(rightType)) {
     rightType = SymbTablePkg_integer_type();
   }
   
   /* real * int => real */
   if (SymbType_get_tag(leftType) == SYMB_TYPE_REAL) {
     return (SymbType_get_tag(rightType) == SYMB_TYPE_INTEGER)
       ? leftType : nullType;
   }

   /* int_symb_enum * (int or symb_enum) => int_symb_enum */
   if (SymbType_is_int_symbolic_enum(leftType)) {
     return (SymbType_get_tag(rightType) == SYMB_TYPE_INTEGER ||
             SymbType_is_pure_symbolic_enum(rightType))
       ? leftType : nullType;
   }

   /* only set-types remain */

   /* int_set * bool_set=> int_set */
   if (SymbType_get_tag(leftType) == SYMB_TYPE_SET_INT) {
     return SymbType_get_tag(rightType) == SYMB_TYPE_SET_BOOL
       ? leftType : SYMB_TYPE(NULL);
   }

   /* int_symb_set * any set-type => int_symb_set  */
   if (SymbType_get_tag(leftType) == SYMB_TYPE_SET_INT_SYMB) {
     return SymbType_is_set(rightType) ? leftType : SYMB_TYPE(NULL);
   }

   /* all other combinations are illegal */
   return nullType;
}


/**Function********************************************************************

  Synopsis           [Returns the minimal type to which the both given types
  can be converted, or Nil if there is none.]

  Description        [The implicit conversion is performed
  in accordance to the type order.
  NOTE: only memory-shared types can be given to this function.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
SymbType_ptr 
SymbType_get_minimal_common(SymbType_ptr type1, SymbType_ptr type2)
{
  /* I expect the given types are shared. So I will not worry about memory */

  /* the same pointers, so, the same types, so, just return the type */
  if (type1 == type2) return type1;

  /* pointer are different => the types are different.
     Try to perform implicit convertion of both operand to
     a minimal type.
     No need to care about word, word-array and no-type since
     they cannot be implicitly converted to any other type.
     NB: No-type is created for assignment, and (predicate-normalised)
     symbolic FSM may push assignments into case-expressions, for example.
  */

  /* cast boolean to int (to decrease the number of cases)*/
  if (SymbType_is_boolean_enum(type1)) type1 = SymbTablePkg_integer_type();
  if (SymbType_is_boolean_enum(type2)) type2 = SymbTablePkg_integer_type();

  /* (integer * bool) or (bool * integer) => integer */
  if (type1 == type2) return type1;
  
   
  /* both are [Int or Real] -> Real */
  if ( ( SymbType_get_tag(type1) == SYMB_TYPE_INTEGER ||
         SymbType_get_tag(type1) == SYMB_TYPE_REAL ) && 
       ( SymbType_get_tag(type2) == SYMB_TYPE_INTEGER ||
         SymbType_get_tag(type2) == SYMB_TYPE_REAL ) ) {
    return SymbTablePkg_real_type();
  }
  
  /* both are [Int or Symbolic or SymbolicInt] -> SymbolicInt */
  if ( ( SymbType_get_tag(type1) == SYMB_TYPE_INTEGER ||
         SymbType_is_pure_symbolic_enum(type1) || 
         SymbType_is_int_symbolic_enum(type1) ) &&
       ( SymbType_get_tag(type2) == SYMB_TYPE_INTEGER ||
         SymbType_is_pure_symbolic_enum(type2) || 
         SymbType_is_int_symbolic_enum(type2) ) ) {
    return SymbTablePkg_int_symbolic_enum_type();
  }

  /* only set types remain */

  /* both are [int_set or bool_set] => int_set */
  if ( (SymbType_get_tag(type1) == SYMB_TYPE_SET_BOOL ||
        SymbType_get_tag(type1) == SYMB_TYPE_SET_INT) &&
       (SymbType_get_tag(type2) == SYMB_TYPE_SET_BOOL ||
        SymbType_get_tag(type2) == SYMB_TYPE_SET_INT)) {
    return SymbTablePkg_integer_set_type();
  }
  /* both are any set type => int_symb_set */
  if (SymbType_is_set(type1) || SymbType_is_set(type2)) {
    return SymbTablePkg_integer_symbolic_set_type();
  }
  
  /* the given types are incompatible => error */
  return SYMB_TYPE(NULL);
}


/**Function********************************************************************

  Synopsis           [Returns a minimal set-type which the given type
  can be implicitly converted to, or NULL if this is impossible.]

  Description        [
  The implicit conversion is performed in accordance to the type order.
  NOTE: only memory-shared types can be given to this function.]

  SideEffects        []

  SeeAlso            [SymbType_make_type_from_set_type]
******************************************************************************/
SymbType_ptr SymbType_make_set_type(SymbType_ptr type)
{
  /* converts to set types if it is required:
     bool -> bool_set
     int -> int_set
     symb -> symb_set
     int_symb_enum -> int_symb_set
     any set type -> the same set type
     other type -> NULL
  */
  if (SymbType_is_boolean_enum(type)) {
    return SymbTablePkg_boolean_set_type();
  }
  if (SymbType_get_tag(type) == SYMB_TYPE_INTEGER) {
    return SymbTablePkg_integer_set_type();
  }
  else if (SymbType_is_pure_symbolic_enum(type)) {
    return SymbTablePkg_symbolic_set_type();
  }
  else if (SymbType_is_int_symbolic_enum(type)) {
    return SymbTablePkg_integer_symbolic_set_type();
  } 
  else if (SymbType_is_set(type)) {
    return type;
  } 
  else return nullType;
}


/**Function********************************************************************

  Synopsis           [This function is opposite to
  SymbType_make_set_type, i.e. if the given type is one of
  the set-types, then the type without "set" suffix is
  returned. Otherwise the time is returned without change.]

  Description        [
  More precisely the following conversion takes place:
  boolean-set -> boolean
  integer-set ->integer
  symbolic-set -> symbolic-enum
  integer-symbolic-set -> integer-symbolic-set
  another type -> the same time

  The implicit conversion is performed in accordance to the type order.
  NOTE: only memory-shared types can be given to this function.]

  SideEffects        []

  SeeAlso            [SymbType_make_set_type]
******************************************************************************/
SymbType_ptr SymbType_make_from_set_type(SymbType_ptr type)
{
  if (SymbType_get_tag(type) == SYMB_TYPE_SET_BOOL) {
    return SymbTablePkg_boolean_enum_type();
  }
  if (SymbType_get_tag(type) == SYMB_TYPE_SET_INT) {
    return SymbTablePkg_integer_type();
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_SET_SYMB) {
    return SymbTablePkg_pure_symbolic_enum_type();
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_SET_INT_SYMB) {
    return SymbTablePkg_int_symbolic_enum_type();
  }
  else return type;
}


/**Function********************************************************************

  Synopsis           [This function takes a NOT memory shared type 
  and returns a memory shared one. ]

  Description        [The input type should have 
  a corresponding memory shared type. For example, function type 
  and error type do not have memory shared instances.]

  SideEffects        []

  SeeAlso            []
******************************************************************************/
SymbType_ptr SymbType_make_memory_shared(SymbType_ptr type)
{
  if (SymbType_get_tag(type) == SYMB_TYPE_INTEGER) {
    type = SymbTablePkg_integer_type();
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_REAL) {
    type = SymbTablePkg_real_type();
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_WORD) {
    type = SymbTablePkg_word_type(SymbType_get_word_width(type));
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_WORDARRAY) {
    int awidth = SymbType_get_wordarray_awidth(type);
    int vwidth = SymbType_get_wordarray_vwidth(type); 
    type = SymbTablePkg_wordarray_type(awidth, vwidth); 
  }
  else if (SymbType_get_tag(type) == SYMB_TYPE_ENUM) {
    if (SymbType_is_boolean_enum(type)) {
      type = SymbTablePkg_boolean_enum_type();
    }
    else if (SymbType_is_pure_int_enum(type)) { 
      type = SymbTablePkg_integer_type(); /* convert to Integer */
    }
    else if (SymbType_is_pure_symbolic_enum(type)) {
      type = SymbTablePkg_pure_symbolic_enum_type();
    }
    else {
      nusmv_assert(SymbType_is_int_symbolic_enum(type));
      type = SymbTablePkg_int_symbolic_enum_type();
    }
  }
  else {
    nusmv_assert(false); /* unknown type */
  }
  return type;

}
