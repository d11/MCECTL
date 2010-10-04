/**CSourceFile*****************************************************************

  FileName    [AddArray.c]

  PackageName [utils]

  Synopsis    []

  Description []

  SeeAlso     [AddArray.h]

  Author      [Andrei Tchaltsev]

  Copyright   [
  This file is part of the ``enc.utils'' package of NuSMV version 2. 
  Copyright (C) 2005 by ITC-irst.

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

#include "AddArray.h"
#include "dd/dd.h" /* required by functions of word arithmetic operations */
#include "enc/operators.h" /* for word arithmetic operation */
#include "parser/symbols.h"
#include "opt/opt.h"
#include "utils/ustring.h"
#include "utils/error.h"

static char rcsid[] UTIL_UNUSED = "$Id: AddArray.c,v 1.1.2.5.4.8 2007/04/20 13:05:54 nusmv Exp $";


extern options_ptr options; 


/*---------------------------------------------------------------------------*/
/* Types definition                                                          */
/*---------------------------------------------------------------------------*/

/**Type***********************************************************************

  Synopsis           [A local pointer definition used by signed operators]

  Description        [A pointer to a function in the form: AddArray_ptr
   foo(DdManager*, AddArray_ptr, AddArray_ptr) Used to minimize code
   of signed operators]

******************************************************************************/
typedef AddArray_ptr (*APFDAA)(DdManager*, AddArray_ptr, AddArray_ptr);


/*---------------------------------------------------------------------------*/
/* Macro definition                                                          */
/*---------------------------------------------------------------------------*/

/**Macro  ********************************************************************

  Synopsis           [converts a type from (actually used) array_t* to our
  artificial AddArray_ptr]

  Description        [The type AddArray_ptr is used just to hide
  array_t (to enable type-checking by compilers).
  But the actuall data-structure used is array_t.]

******************************************************************************/
#define array2AddArray(array) ADD_ARRAY(array)


/**Macro  ********************************************************************

  Synopsis           [converts a type from our
  artificial AddArray_ptr to (actually used) array_t*]

  Description        [See array2AddArray]

******************************************************************************/
#define AddArray2array(addArray) ((array_t*)(addArray))


/*---------------------------------------------------------------------------*/
/* Variable definition                                                       */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static void add_array_full_adder ARGS((DdManager* dd,
                                       add_ptr arg1,
                                       add_ptr arg2,
                                       add_ptr carry_in,
                                       add_ptr* sum,
                                       add_ptr* carry_out));

static void add_array_adder ARGS((DdManager* dd,
                                  AddArray_ptr arg1,
                                  AddArray_ptr arg2,
                                  add_ptr carry_in,
                                  AddArray_ptr* res,
                                  add_ptr* carry_out));

static AddArray_ptr add_array_apply_on_bits ARGS((DdManager* dd,
                                                  NPFNN/*NPFCVT*/ op,
                                                  AddArray_ptr arg));

static void add_array_division_remainder ARGS((DdManager* dd,
                                               AddArray_ptr arg1,
                                               AddArray_ptr arg2,
                                               AddArray_ptr* quotient,
                                               AddArray_ptr* remainder));

static add_ptr
add_array_create_default_value_of_shift_operation 
ARGS((DdManager* dd, AddArray_ptr number,
      int width, const char* errMessage));


static AddArray_ptr 
add_array_word_signed_operator ARGS((DdManager* dd, APFDAA op, 
                                     AddArray_ptr arg1, AddArray_ptr arg2));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [constructor. Create an array of "number" ADDs]

  Description        [number must be positive. The index of the 
  array goes from 0 to (number - 1).]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_create(int number)
{
  nusmv_assert(number > 0);
  return array2AddArray(array_alloc(add_ptr, number));
}


/**Function********************************************************************

  Synopsis           [Creates a new AddArray from given WordNumber]

  Description [If monolithic word encoding is disabled, returned add
  array has the same width of the given word number; otherwise size
  will be 1]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_from_word_number(DdManager* dd, WordNumber_ptr wn)
{
  if (opt_encode_word_monolithic(options)) {

    /* normalise the word constant, i.e. create it with the help of
       WordNumber_from_integer (to share memory and make pointers unique)
    */
    node_ptr new_expr;
    wn = WordNumber_from_integer(WordNumber_get_value(wn),
                                 WordNumber_get_width(wn));
    
    new_expr = find_node(NUMBER_WORD, (node_ptr) wn, Nil);
    return AddArray_from_add(add_leaf(dd, new_expr));
  }

  /* list of ADD encoding of Word */
  else {
    AddArray_ptr res;
    int i;
    const int width = WordNumber_get_width(wn);
    
    res = AddArray_create(width);
    for (i = 0; i < width; ++i) { 
      node_ptr value = WordNumber_get_bit(wn, i) ? one_number : zero_number;
      AddArray_set_n(res, i, add_leaf(dd, value));
    }
    return res;
  }
}


/**Function********************************************************************

  Synopsis           [given an ADD create an AddArray consisting of 
  one element]

  Description        [Given ADD must already be referenced.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_from_add(add_ptr add)
{ 
  array_t* array = array_alloc(add_ptr, 1);
  array_insert(add_ptr, array, 0, add);
  return array2AddArray(array);
}


/**Function********************************************************************

  Synopsis           [create a new AddArray, a copy of a given one]

  Description        [During duplication all ADD will be referenced.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_duplicate(AddArray_ptr self)
{
  array_t* new;
  array_t* old;
  int i;
  
  old = AddArray2array(self);
  new = array_alloc(add_ptr, array_n(old));
  for (i = 0; i < array_n(old); ++i) {
    add_ptr add = array_fetch(add_ptr, old, i);
    add_ref(add);
    array_insert(add_ptr, new, i, add);
  }
  return array2AddArray(new);
}


/**Function********************************************************************

  Synopsis           [destructor of the class]

  Description        [The memory will be freed and all ADD will be
  de-referenced]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void AddArray_destroy(DdManager* dd, AddArray_ptr self)
{
  int i;
  array_t* array = AddArray2array(self);
  for (i = 0; i < array_n(array); ++i) {
    add_free(dd, array_fetch(add_ptr, array, i));
  }
  array_free(array);
}


/**Function********************************************************************

  Synopsis           [returns the size (number of elements) of the array]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int AddArray_get_size(AddArray_ptr self)
{
  return array_n(AddArray2array(self));
}


/**Function********************************************************************

  Synopsis           [This function returns the first element of 
  the array]

  Description        [The array should contain exactly one element]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
add_ptr AddArray_get_add(AddArray_ptr self)
{
  array_t* array = AddArray2array(self);
  nusmv_assert(array_n(array) == 1);
  return array_fetch(add_ptr, array, 0);
}


/**Function********************************************************************

  Synopsis           [Returns the element number "n" from
  the array.]

  Description        ["n" can be from 0 to (size-1).
  The returned ADD is NOT referenced.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
add_ptr AddArray_get_n(AddArray_ptr self, int number)
{
  array_t* array = AddArray2array(self);
  return array_fetch(add_ptr, array, number);
}


/**Function********************************************************************

  Synopsis           [Returns the sum of the sizes of the ADDs within self]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
size_t AddArray_get_add_size(const AddArray_ptr self, DdManager* dd)
{
  size_t size = 0;
  int i;
    
  for (i=AddArray_get_size(self)-1; i>=0; --i) {
    size += add_size(dd, AddArray_get_n(self, i));
  }
  return size;
}


/**Function********************************************************************

  Synopsis           [Sets the element number "number" to "add".]

  Description        [The given ADD "add" must already be referenced.
  The previous value should already be de-referenced if it is necessary.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void AddArray_set_n(AddArray_ptr self, int number, add_ptr add)
{
  array_t* array = AddArray2array(self);
  array_insert(add_ptr, array, number, add);
}


/**Function********************************************************************

  Synopsis           [Perform the addition operations
  on two Word expressions represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The size of both arguments should be the same.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_plus(DdManager* dd, AddArray_ptr arg1,
                                               AddArray_ptr arg2)
{
  AddArray_ptr res;
  add_ptr carry;
  add_ptr zero = add_zero(dd);

  add_array_adder(dd, arg1, arg2, zero, &res, &carry);

  add_free(dd, zero);
  add_free(dd, carry);
  return res;
}


/**Function********************************************************************

  Synopsis           [Perform the subtraction operations
  on two Word expressions represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The size of both arguments should be the same.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_minus(DdManager* dd, AddArray_ptr arg1,
                                                AddArray_ptr arg2)
{
  AddArray_ptr res;
  add_ptr carry;

  AddArray_ptr not_arg2;
  add_ptr one = add_one(dd);

  /* arg1 - arg2 ==  arg1 + (not arg2) + 1 */
  not_arg2 = add_array_apply_on_bits(dd, node_not, arg2);
  
  add_array_adder(dd, arg1, not_arg2, one, &res, &carry);

  add_free(dd, one);
  add_free(dd, carry);
  AddArray_destroy(dd, not_arg2);
  return res;
}


/**Function********************************************************************

  Synopsis [Applies unary operator to each bit of given argument, and
  returns resulting add array]

  Description        [Returned AddArray must be destroyed by the caller]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_apply_unary(DdManager* dd,
                                       AddArray_ptr arg1,
                                       FP_A_DA op)
{
  const int width = AddArray_get_size(arg1);
  AddArray_ptr res;
  int i;

  nusmv_assert(width > 0);

  res = AddArray_create(width);
  /* at every interation, process one bit and collect the result in res */
  for (i = 0; i < width; ++i) {
    AddArray_set_n(res, i, op(dd, AddArray_get_n(arg1, i)));
  }
  
  return res;
}


/**Function********************************************************************

  Synopsis [Applies binary operator to each bits pair of given
  arguments, and returns resulting add array]

  Description        [Returned AddArray must be destroyed by the caller]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_apply_binary(DdManager* dd,
                                        AddArray_ptr arg1,
                                        AddArray_ptr arg2,
                                        FP_A_DAA op)
{
  const int width = AddArray_get_size(arg1);
  AddArray_ptr res;
  int i;

  /* the size of array should be the same and positive */
  nusmv_assert(width == AddArray_get_size(arg2) && width > 0);

  res = AddArray_create(width);
  /* at every interation, process one bit and collect the result in res */
  for (i = 0; i < width; ++i) {
    AddArray_set_n(res, i, 
                   op(dd, 
                      AddArray_get_n(arg1, i), 
                      AddArray_get_n(arg2, i)));
  }
  
  return res;
}



/**Function********************************************************************

  Synopsis [Returns an ADD that is the disjunction of all bits of arg]

  Description        [Returned ADD is referenced]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
add_ptr AddArray_make_disjunction(DdManager* dd, AddArray_ptr arg)
{
  add_ptr res;
  int i;

  nusmv_assert(AddArray_get_size(arg) > 0);

  res = add_zero(dd);
  for (i=AddArray_get_size(arg)-1; i>=0; --i) {
    add_ptr tmp = add_or(dd, AddArray_get_n(arg, i), res);
    add_free(dd, res);
    res = tmp;
  }

  return res;
}


/**Function********************************************************************

  Synopsis [Returns an ADD that is the conjunction of all bits of arg]

  Description        [Returned ADD is referenced]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
add_ptr AddArray_make_conjunction(DdManager* dd, AddArray_ptr arg)
{
  add_ptr res;
  int i;

  res = add_one(dd);
  for (i=AddArray_get_size(arg)-1; i>=0; --i) {
    add_ptr tmp = add_and(dd, AddArray_get_n(arg, i), res);
    add_free(dd, res);
    res = tmp;
  }

  return res;
}


/**Function********************************************************************

  Synopsis           [Changes the sign of the given word.]

  Description        []

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_uminus(DdManager* dd, AddArray_ptr arg)
{
  const int width = AddArray_get_size(arg);
  int i;
  
  AddArray_ptr res;
  AddArray_ptr arg_zero;
  AddArray_ptr not_arg;
  add_ptr carry;

  add_ptr zero = add_zero(dd);
  add_ptr one = add_one(dd);

  arg_zero = AddArray_create(width);
  for (i=width-1; i>=0; --i) AddArray_set_n(arg_zero, i, add_dup(zero));
  
  /* -arg ==  (not arg) + 1 */
  not_arg = add_array_apply_on_bits(dd, node_not, arg);  
  add_array_adder(dd, not_arg, arg_zero, one, &res, &carry);

  AddArray_destroy(dd, not_arg); 
  AddArray_destroy(dd, arg_zero); 
  add_free(dd, one);
  add_free(dd, zero);  
  add_free(dd, carry);

  return res;
}


/**Function********************************************************************

  Synopsis           [Perform the multiplication operations
  on two Word expressions represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The size of both arguments should be the same.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_times(DdManager* dd,
                                 AddArray_ptr arg1, AddArray_ptr arg2)
{
  
  int index;
  const int N = AddArray_get_size(arg1);
  AddArray_ptr res = AddArray_create(N);

  nusmv_assert( N > 0 && N == AddArray_get_size(arg2));

  /* set the result array to (arg2 & arg1[0]), i.e. process first bit of arg1 */
  for (index = 0 ; index < N; ++index) {
    AddArray_set_n(res, index, add_and(dd, AddArray_get_n(arg1, 0),
                                       AddArray_get_n(arg2, index)));
  }

  /* A*B = ((B & A[0])<<0) +...+ ((B & A[i])<<i) +...+ ((B & A[N-1])<<N-1) */

  /* sum up all (arg2 & arg1[i]).
   Index begins with 1 because first bit has been already processed */
  for (index = 1; index < N; ++index) {
    int k;
    add_ptr carry_in;
    
    /* create an adder, i.e. (N - index) lower bits of (B & A[index])*/
    AddArray_ptr adder = AddArray_create(N - index);
    for (k = 0; k < N - index; ++ k){
      add_ptr add = add_and(dd, AddArray_get_n(arg1, index),
                            AddArray_get_n(arg2, k));
      AddArray_set_n(adder, k, add);
    } /* for k */

    /* add the adder to the (N - index) higher bits of the result */
    carry_in = add_zero(dd); /* it will be de-referenced in the loop */

    for (k = 0; k < N - index; ++k) {
      add_ptr sum;
      add_ptr tmp_carry;
      
      add_array_full_adder(dd, AddArray_get_n(res, k + index),
                           AddArray_get_n(adder, k),
                           carry_in, &sum, &tmp_carry);
      add_free(dd, AddArray_get_n(res, k + index));
      AddArray_set_n(res, k + index, sum);
      add_free(dd, carry_in);
      carry_in = tmp_carry;
    } /* for k */

    AddArray_destroy(dd, adder);
  } /* for index */
  return res;
}


/**Function********************************************************************

  Synopsis           [Perform the division operations
  on two Word expressions represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The size of both arguments should be the same.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_divide(DdManager* dd,
                                  AddArray_ptr arg1, AddArray_ptr arg2)
{
  AddArray_ptr quotient;
  AddArray_ptr remainder;

  add_array_division_remainder(dd, arg1, arg2, &quotient, &remainder);
  
  AddArray_destroy(dd, remainder);
  return quotient;
}


/**Function********************************************************************

  Synopsis           [Perform the remainder operations
  on two Word expressions represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The size of both arguments should be the same.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_mod(DdManager* dd,
                               AddArray_ptr arg1, AddArray_ptr arg2)
{
  AddArray_ptr quotient;
  AddArray_ptr remainder;

  add_array_division_remainder(dd, arg1, arg2, &quotient, &remainder);
  
  AddArray_destroy(dd, quotient);
  return remainder;
}


/**Function********************************************************************

  Synopsis           [Performs left shift operations
  on a Word expression represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The "number" argument represent 
  the number of bits to shift. "number" should have only one ADD.
  NB: The invoker should destroy the returned array.

  NB for developers:
  Every i-th bit of returned array will be:
       ITE(number=0 , arg[i],
           ITE(number=1, arg[i-1],
            ...
             ITE(number=i, arg[0],
              ITE(number >=0 && number <= width, zero, FAILURE)
   Does anyone know a better encoding?
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_left_shift(DdManager* dd, AddArray_ptr arg,
                                      AddArray_ptr number)
{
  const int width = AddArray_get_size(arg);
  const int nw = AddArray_get_size(number);
  AddArray_ptr res;
  add_ptr def_case;
  int maxn, i;
  
  nusmv_assert(width>0 && nw>0);
  
  res = AddArray_create(width);
  
  /* creates the default case, i.e. the last ITE (see description) */
  def_case = add_array_create_default_value_of_shift_operation(dd,
                               number, width,
                              "Right operand of left-shift is out of range");

  if (nw > 1) maxn = (2 << (nw-1)) - 1;
  else maxn = width;

  /* proceed one bit at every iteration */
  for (i = 0; i < width; ++i) {
    add_ptr bit = add_dup(def_case);
    int n;

    /* create all other ITEs */
    for (n = MIN(i, maxn); n >= 0; --n) {
      add_ptr numeqn_add, tmp;

      /* create  ITE(number=n, arg[i-n], ...) */
      if (nw > 1) {
        AddArray_ptr an = AddArray_from_word_number(dd, 
                              WordNumber_from_integer(n, nw));
        AddArray_ptr numeqn = AddArray_word_equal(dd, number, an);
        numeqn_add = add_dup(AddArray_get_add(numeqn));
        AddArray_destroy(dd, numeqn);
        AddArray_destroy(dd, an);
      }
      else { /* number is not a word */
        add_ptr n_add = add_leaf(dd, find_node(NUMBER, NODE_FROM_INT(n), Nil));
        numeqn_add = add_apply(dd, node_equal, 
                               AddArray_get_add(number), n_add);
        add_free(dd, n_add);
      }
      
      tmp = add_ifthenelse(dd, numeqn_add, AddArray_get_n(arg, i - n),
                           bit);

      add_free(dd, numeqn_add);
      add_free(dd, bit);
      bit = tmp;
    } /* for (n) */

    /* set the obtained bit into the returned array */
    AddArray_set_n(res, i, bit);
  } /* for (i) */

  add_free(dd, def_case); 

  return res;
}


/**Function********************************************************************

  Synopsis           [Performs right shift operations
  on a Word expression represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The "number" argument represent 
  the number of bits to shift. "number" should have only one ADD.
  NB: The invoker should destroy the returned array.

  NB for developers:
  Every i-th bit of returned array will be:
       ITE(number=0 , arg[i],
           ITE(number=1, arg[i+1],
            ...
             ITE(number=width-1-i, arg[width-1],
              ITE(number >=0 && number <= width, zero, FAILURE)
   Does anyone have a better encoding?
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_right_shift(DdManager* dd, AddArray_ptr arg,
                                       AddArray_ptr number)
{

  const int width = AddArray_get_size(arg);
  const int nw = AddArray_get_size(number);
  AddArray_ptr res;
  add_ptr def_case;
  int maxw, i;
  
  nusmv_assert(width>0 && nw>0);
  
  res = AddArray_create(width);
  
  /* creates the default case, i.e. the last ITE (see description) */
  def_case = add_array_create_default_value_of_shift_operation(dd,
                               number, width,
                              "Right operand of right-shift is out of range");

  if (nw > 1) maxw = MIN((2 << (nw-1)), width);
  else maxw = width;

  /* proceed one bit at every iteration */
  for (i = 0; i < width; ++i) {
    add_ptr bit = add_dup(def_case);
    int n;

    /* create all other ITEs */
    for (n=maxw-1; n >= 0; --n) {
      add_ptr numeqn_add, tmp;
      
      if (n+i < width) { 
        /* a valid index */
        if (nw > 1) {
          AddArray_ptr an = AddArray_from_word_number(dd, 
                              WordNumber_from_integer(n, nw));
          AddArray_ptr numeqn = AddArray_word_equal(dd, number, an);
          numeqn_add = add_dup(AddArray_get_add(numeqn));
          AddArray_destroy(dd, numeqn);
          AddArray_destroy(dd, an);
        }
        else { /* number is not a word */
          add_ptr n_add = add_leaf(dd, 
                                   find_node(NUMBER, NODE_FROM_INT(n), Nil));
          numeqn_add = add_apply(dd, node_equal, 
                                 AddArray_get_add(number), n_add);
          add_free(dd, n_add);
        }
      
        tmp = add_ifthenelse(dd, numeqn_add, AddArray_get_n(arg, i+n),
                             bit);

        add_free(dd, numeqn_add);
        add_free(dd, bit);
        bit = tmp;
      }
    } /* for (n) */

    /* set the obtained bit into the returned array */
    AddArray_set_n(res, i, bit);
  } /* for (i) */

  add_free(dd, def_case);
  return res;
}


/**Function********************************************************************

  Synopsis           [Performs left rotate operations
  on a Word expression represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The "number" argument represent 
  the number of bits to rotate. "number" should have only one ADD.
  NB: The invoker should destroy the returned array.

  NB for developers:
  Every i-th bit  of returned array will be:
       ITE(number=0 , arg[i],
           ITE(number=1, arg[i-1],
            ...
             ITE(number=i, arg[0],
              ITE(number=i+1, arg[width-1],
               ...
               ITE(number=width-1, arg[i+1],
                ITE(number=width, arg[i], FAILURE]
   Does anyone have a better encoding?
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_left_rotate(DdManager* dd, AddArray_ptr arg,
                                       AddArray_ptr number)
{
  const int width = AddArray_get_size(arg);
  const int wn = AddArray_get_size(number);
  AddArray_ptr res;
  add_ptr err_case;
  int maxn, i;

  nusmv_assert(width > 0);

  res = AddArray_create(width);

  /* create the default case, i.e. the last ITE (see description) */
  err_case = add_leaf(dd, 
               failure_make("Right operand of rotate operation is out of range",
                            FAILURE_UNSPECIFIED, 
                            (int) node_get_lineno(get_the_node())));

  if (wn > 1) maxn = MIN(((2 << (wn-1)) - 1), width);
  else maxn = width;
  
  /* proceed one bit at every iteration */
  for (i = 0; i < width; ++i) {
    int k;
    add_ptr bit = add_dup(err_case); /* it is de-ref in the loop */

    /* create all other ITEs */
    for (k=maxn ; k >= 0; --k) {
      add_ptr neqk, tmp;

      if (wn > 1) {
        AddArray_ptr ak = AddArray_from_word_number(dd, 
                              WordNumber_from_integer(k, wn));
        AddArray_ptr aneqk = AddArray_word_equal(dd, number, ak);
        neqk = add_dup(AddArray_get_add(aneqk));
        AddArray_destroy(dd, aneqk);
        AddArray_destroy(dd, ak);        
      }
      else {
        add_ptr k_add = add_leaf(dd, find_node(NUMBER, NODE_FROM_INT(k), Nil));
        neqk = add_apply(dd, node_equal, AddArray_get_add(number), k_add);
        add_free(dd, k_add);
      }

      tmp = add_ifthenelse(dd, neqk,
                           AddArray_get_n(arg, 
                                          (i >= k) ? i-k : i-k+width),
                           bit);
      
      add_free(dd, neqk);
      add_free(dd, bit);
      bit = tmp;
    } /* for (k) */

    /* set the obtained bit into the returned array */
    AddArray_set_n(res, i, bit);
  } /* for (i) */

  add_free(dd, err_case);

  return res;
}


/**Function********************************************************************

  Synopsis           [Performs right rotate operations
  on a Word expression represented as an array of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The "number" argument represent 
  the number of bits to rotate. "number" should have only one ADD.
  NB: The invoker should destroy the returned array.

  NB for developers:
  Every i-th bit of returned array will be:
       ITE(number=0 , arg[i],
           ITE(number=1, arg[i+1],
            ...
             ITE(number=width-1-i, arg[width-1],
              ITE(number=width-2-i, arg[0],
               ...
               ITE(number=width-1, arg[i-1],
                ITE(number=width, arg[i], FAILURE]
   Does anyone have a better encoding?
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_right_rotate(DdManager* dd, AddArray_ptr arg,
                                        AddArray_ptr number)
{
  const int width = AddArray_get_size(arg);
  const int wn = AddArray_get_size(number);
  AddArray_ptr res;
  add_ptr err_case;
  int maxn, i;

  nusmv_assert(width > 0);

  res = AddArray_create(width);

  /* create the default case, i.e. the last ITE (see description) */
  err_case = add_leaf(dd, 
               failure_make("Right operand of rotate operation is out of range",
                            FAILURE_UNSPECIFIED, 
                            (int) node_get_lineno(get_the_node())));

  if (wn > 1) maxn = MIN(((2 << (wn-1)) - 1), width);
  else maxn = width;
  
  /* proceed one bit at every iteration */
  for (i = 0; i < width; ++i) {
    int k;
    add_ptr bit = add_dup(err_case); /* it is de-ref in the loop */

    /* create all other ITEs */
    for (k=maxn ; k >= 0; --k) {
      add_ptr neqk, tmp;

      if (wn > 1) {
        AddArray_ptr ak = AddArray_from_word_number(dd, 
                              WordNumber_from_integer(k, wn));
        AddArray_ptr aneqk = AddArray_word_equal(dd, number, ak);
        neqk = add_dup(AddArray_get_add(aneqk));
        AddArray_destroy(dd, aneqk);
        AddArray_destroy(dd, ak);        
      }
      else {
        add_ptr k_add = add_leaf(dd, find_node(NUMBER, NODE_FROM_INT(k), Nil));
        neqk = add_apply(dd, node_equal, AddArray_get_add(number), k_add);
        add_free(dd, k_add);
      }

      tmp = add_ifthenelse(dd, neqk,
                           AddArray_get_n(arg, ((i+k) % width)),
                           bit);
      
      add_free(dd, neqk);
      add_free(dd, bit);
      bit = tmp;
    } /* for (k) */

    /* set the obtained bit into the returned array */
    AddArray_set_n(res, i, bit);
  } /* for (i) */

  add_free(dd, err_case);

  return res;
}


/**Function********************************************************************

  Synopsis           [Performs less-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_less(DdManager* dd,
                                AddArray_ptr arg1, AddArray_ptr arg2)
{
  /* A < B 
     === NOT (A >= B) 
     === NOT ("carry bit of" (B - A - 1))
     === NOT NOT ("carry bit of" (B + 11...111 + 1 - A - 1))
     === "carry bit of" ((not A) + B)
  */

  AddArray_ptr not_arg1 = add_array_apply_on_bits(dd, node_not, arg1);
  add_ptr zero = add_zero(dd);
 
  AddArray_ptr plus;
  add_ptr carry;
 
  add_array_adder(dd, not_arg1, arg2, zero, &plus, &carry);

  AddArray_destroy(dd, plus);
  add_free(dd, zero);
  AddArray_destroy(dd, not_arg1);
  
  return AddArray_from_add(carry);
}


/**Function********************************************************************

  Synopsis           [Performs less-or-equal operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_less_equal(DdManager* dd,
                                      AddArray_ptr arg1, AddArray_ptr arg2)
{
  /* A <= B ===
     A < (B + 1) ==
     === "carry bit of" ((not A) + (B + 1))    (see  AddArray_word_less)
  */

  AddArray_ptr not_arg1 = add_array_apply_on_bits(dd, node_not, arg1);
  add_ptr one = add_one(dd);
 
  AddArray_ptr plus;
  add_ptr carry;
 
  add_array_adder(dd, not_arg1, arg2, one, &plus, &carry);
  
  AddArray_destroy(dd, plus);
  add_free(dd, one);
  AddArray_destroy(dd, not_arg1);
  
  return AddArray_from_add(carry);
}


/**Function********************************************************************

  Synopsis           [Performs greater-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_greater(DdManager* dd,
                                   AddArray_ptr arg1, AddArray_ptr arg2)
{
  return AddArray_word_less(dd, arg2, arg1);
}


/**Function********************************************************************

  Synopsis           [Performs greater-or-equal operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_greater_equal(DdManager* dd,
                                         AddArray_ptr arg1, AddArray_ptr arg2)
{
    return AddArray_word_less_equal(dd, arg2, arg1);
}


/**Function********************************************************************

  Synopsis           [Performs _signed_ less-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_signed_less(DdManager* dd,
                                       AddArray_ptr arg1, AddArray_ptr arg2)
{
  /* A <s B 
     === (A[msb] & !B[msb]) | ((A[msb] = B[msb]) & (A <u B))   */
  return add_array_word_signed_operator(dd, AddArray_word_less, arg1, arg2);  
}


/**Function********************************************************************

  Synopsis           [Performs _signed_ less-equal-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_signed_less_equal(DdManager* dd,
                                             AddArray_ptr arg1, 
                                             AddArray_ptr arg2)
{
  /* A <=s B 
     === (A[msb] & !B[msb]) | ((A[msb] = B[msb]) & (A <=u B))   */
  return add_array_word_signed_operator(dd, AddArray_word_less_equal, 
                                        arg1, arg2); 
}


/**Function********************************************************************

  Synopsis           [Performs _signed_ greater-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_signed_greater(DdManager* dd,
                                          AddArray_ptr arg1, AddArray_ptr arg2)
{
  /* A >s B 
     === B <s A */
  return add_array_word_signed_operator(dd, AddArray_word_less, arg2, arg1);
}


/**Function********************************************************************

  Synopsis           [Performs _signed_ greater-equal-then operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_signed_greater_equal(DdManager* dd,
                                                AddArray_ptr arg1, 
                                                AddArray_ptr arg2)
{
  /* A >=s B 
     === B <=s A */
  return add_array_word_signed_operator(dd, AddArray_word_less_equal, 
                                        arg2, arg1); 
}


/**Function********************************************************************

  Synopsis           [Extends the sign of arg]

  Description        [arg_repeat must be a constant number]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_sign_extend(DdManager* dd,
                                       AddArray_ptr arg, 
                                       AddArray_ptr arg_repeat)
{
  const int width = AddArray_get_size(arg);

  AddArray_ptr res;
  add_ptr add; 
  int repeat;
  int tot_w;
  int i;

  nusmv_assert(width > 0);

  {
    add_ptr add_repeat;
    node_ptr node_repeat;

    add_repeat = AddArray_get_add(arg_repeat);
    nusmv_assert(add_isleaf(add_repeat));

    node_repeat = add_get_leaf(dd, add_repeat);
    nusmv_assert(node_get_type(node_repeat) == NUMBER);
    repeat = NODE_TO_INT(car(node_repeat));
    nusmv_assert(repeat >= 0);
  }

  tot_w = width + repeat;
  res = AddArray_create(tot_w);  

  /* copies the argument */
  for (i = 0; i < width; ++i) {
    add = AddArray_get_n(arg, i);
    add_ref(add);
    AddArray_set_n(res, i, add);
  }

  /* extends the sign */
  for (;i < tot_w; ++i) {
    add_ref(add);
    AddArray_set_n(res, i, add);
  }

  return res;
}


/**Function********************************************************************

  Synopsis           [Performs equal-operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_equal(DdManager* dd,
                                 AddArray_ptr arg1, AddArray_ptr arg2)
{
  add_ptr result = add_one(dd);
  int i;
  const int width = AddArray_get_size(arg1);

  nusmv_assert(width == AddArray_get_size(arg2) && width > 0);
  
  for (i = 0; i < width; ++i) {
    add_ptr bit_equal = add_iff(dd, AddArray_get_n(arg1, i), 
                                AddArray_get_n(arg2, i));
    add_ptr tmp = add_and(dd, bit_equal, result);
    add_free(dd, bit_equal);
    add_free(dd, result);
    result = tmp;

    if (add_is_zero(dd, result)) break; /* lazy eval */
  }
  
  return AddArray_from_add(result);
}


/**Function********************************************************************

  Synopsis           [Performs not-equal-operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [the size of arguments should be the same
  The returned array will constain only one (boolean) ADD.
  NB: The invoker should destroy the returned array.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_not_equal(DdManager* dd,
                                     AddArray_ptr arg1, AddArray_ptr arg2)
{
  add_ptr result = add_zero(dd);
  int i;
  const int width = AddArray_get_size(arg1);
  
  nusmv_assert(width == AddArray_get_size(arg2) && width > 0);
  
  for (i = 0; i < width; ++i) {
    add_ptr bit_not_equal = add_xor(dd, AddArray_get_n(arg1, i), 
                                    AddArray_get_n(arg2, i));
    add_ptr tmp = add_or(dd, bit_not_equal, result);
    add_free(dd, bit_not_equal);
    add_free(dd, result);
    result = tmp;
  }
  
  return AddArray_from_add(result);
}


/**Function********************************************************************

  Synopsis [Creates a ITE word array. then and else array must have
  the same width, or else is allowed to have size 1 when encoding a
  failure node. Returned array width is as large as _then. _if can have 
  any size]

  Description        [The invoker should destroy the returned array.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_ite(DdManager* dd,
                               AddArray_ptr _if,
                               AddArray_ptr _then, AddArray_ptr _else)
{
  int width, i;
  AddArray_ptr res;
  boolean is_else_failure; 
  add_ptr if_add;

  width = AddArray_get_size(_then);

  /* flags else if size is 1 (may be a failure node when width > 1) */
  is_else_failure = AddArray_get_size(_else) == 1;
  nusmv_assert(is_else_failure || width == AddArray_get_size(_else));

  /* prepares if_add */
  if_add = AddArray_make_disjunction(dd, _if);

  res = AddArray_create(width);
  /* process every pair of bits */
  for (i = 0; i < width; ++i) {
    add_ptr add = add_ifthenelse(dd,
                                 if_add, 
                                 AddArray_get_n(_then, i),
                                 is_else_failure ? AddArray_get_add(_else)
                                 : AddArray_get_n(_else, i));
    /* if ELSE is a failure node then only one ADD will be there */
    AddArray_set_n(res, i, add);
  }

  add_free(dd, if_add);
  return res;
}


/**Function********************************************************************

  Synopsis           [Performs bit-selection operation
  on a Word expression represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [The high-bit and low-bit of selections
  are specified by "range". "range" must
  be ADD leafs with a RANGE node (holding two integer constants,
  and these constant must be in the range [width-1, 0]).
  NB: The invoker should destroy the returned array.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_bit_selection(DdManager* dd,
                                         AddArray_ptr word,
                                         AddArray_ptr range)
{
  const int width = AddArray_get_size(word);
  int i;
  int highBit;
  int lowBit;
  AddArray_ptr res;

  /* convert the ADD representation of high and low bits into integers */
  {
    add_ptr range_add = AddArray_get_add(range);/*one ADD may be in range*/
    node_ptr range_node;

    nusmv_assert(add_isleaf(range_add));/* range must be a value, not ADD tree */

    range_node = add_get_leaf(dd, range_add);
    nusmv_assert(node_get_type(range_node) == RANGE &&
                 node_get_type(car(range_node)) == NUMBER &&
                 node_get_type(cdr(range_node)) == NUMBER);

    highBit = NODE_TO_INT(car(car(range_node)));
    lowBit = NODE_TO_INT(car(cdr(range_node)));
   
    nusmv_assert(0 <= lowBit && lowBit <= highBit && highBit < width);
  }

  i = highBit - lowBit + 1;
  res = AddArray_create(i);
  for ( --i; i >= 0; --i){
    add_ptr add = AddArray_get_n(word, i + lowBit);
    add_ref(add);
    AddArray_set_n(res, i, add);
  }
  return res;
}


/**Function********************************************************************

  Synopsis           [Performs concatenation operation
  on two Word expressions represented as arrays of ADD.
  Every ADD corresponds to a bit of a Word expression]

  Description        [
  NB: The invoker should destroy the returned array.
  ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
AddArray_ptr AddArray_word_concatenation(DdManager* dd,
                                         AddArray_ptr arg1, AddArray_ptr arg2)
{
  const int width1 = AddArray_get_size(arg1);
  const int width2 = AddArray_get_size(arg2);
  int i;

  AddArray_ptr res = AddArray_create(width1 + width2);

  for (i = 0; i < width2; ++i) {
    add_ptr add = AddArray_get_n(arg2, i);
    add_ref(add);
    AddArray_set_n(res, i, add);
  }

  for (i = 0; i < width1; ++i) {
    add_ptr add = AddArray_get_n(arg1, i);
    add_ref(add);
    AddArray_set_n(res, i + width2, add);
  }
  return res;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Performes the full-adder operation on the arguments arg1, arg2
  and carry_in. The returned sum-bit and carry-bit are returned in
  sum and carry_out.]

  Description [The returned ADD (sum and carry_out) are referenced.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void add_array_full_adder(DdManager* dd,
                                 add_ptr arg1,
                                 add_ptr arg2,
                                 add_ptr carry_in,
                                 add_ptr* sum,
                                 add_ptr* carry_out)
{
  add_ptr arg1_xor_arg2;
  add_ptr arg1_and_arg2;
  add_ptr arg1_xor_arg2_and_carry_in;
    
  nusmv_assert( sum != (add_ptr*)NULL && carry_out != (add_ptr*)NULL);
  
  /* sum = arg1 XOR arg2 XOR carry_in */
  arg1_xor_arg2 = add_xor(dd, arg1, arg2);
  *sum = add_xor(dd, arg1_xor_arg2, carry_in);
  
  /* curry_out = (arg1 and arg2) OR ((arg1 XOR arg2) AND carry_in) */
  arg1_and_arg2 = add_and(dd, arg1, arg2);
  arg1_xor_arg2_and_carry_in = add_and(dd, arg1_xor_arg2, carry_in);
  *carry_out = add_or(dd, arg1_and_arg2, arg1_xor_arg2_and_carry_in);

  add_free(dd, arg1_xor_arg2);
  add_free(dd, arg1_and_arg2);
  add_free(dd, arg1_xor_arg2_and_carry_in);
  return;
}


/**Function********************************************************************

  Synopsis    [Performes the addition of two Word expressions and 
  a carry-in bit.]
  
  Description [The sum is returned by the parameter res (the invoker
  must destroy this array), and the final carry-bit is returned by the
  parameter carry_out (the ADD is referenced).
  The size of input arrays must be equal(and positive). ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void add_array_adder(DdManager* dd, AddArray_ptr arg1, AddArray_ptr arg2,
                            add_ptr carry_in,
                            AddArray_ptr* res,  add_ptr* carry_out)
{
  const int width = AddArray_get_size(arg1);
  int i;

  nusmv_assert( res != (AddArray_ptr*)NULL && carry_out != (add_ptr*)NULL);
  nusmv_assert(AddArray_get_size(arg2) == width && width > 0);
  
  
  *res = AddArray_create(width);
  add_ref(carry_in);/* it will be de-referenced in the loop or in an outter fun */

  for (i = 0; i < width; ++i) {
    add_ptr sum;
    add_ptr tmp_carry;
    
    add_array_full_adder(dd, AddArray_get_n(arg1, i), AddArray_get_n(arg2, i),
                         carry_in, &sum, &tmp_carry);
    AddArray_set_n(*res, i, sum);
    add_free(dd, carry_in);
    carry_in = tmp_carry;
  } /* for */

  *carry_out = carry_in;
  return;
}


/**Function********************************************************************

  Synopsis    [Performs division operations on two Word expressions (which 
  are encoded as arrays of bits)]

  Description [The quotient and the remainder is returned in the 
  parameters "quotient" and "remainder" respectively.
  The invoker should free the returned arrays.

  The size of arguments should be the same (and positive).

  Every bit of the resulting arrays is wrapped in ITE which check
  the second argument (of the operation) for not being zero.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void add_array_division_remainder(DdManager* dd,
                                         AddArray_ptr arg1,
                                         AddArray_ptr arg2,
                                         AddArray_ptr* quotient,
                                         AddArray_ptr* remainder)
{
  int index;
  const int N = AddArray_get_size(arg1);
  add_ptr isNotZero;
  AddArray_ptr quot;
  AddArray_ptr rem;

  nusmv_assert(N > 0 && AddArray_get_size(arg2) == N);

  /* Check the second argument for being NOT zero */
  {
    add_ptr tmp;
    
    for (isNotZero = add_zero(dd), index = 0; index < N; ++index) {
      tmp = add_or(dd, isNotZero, AddArray_get_n(arg2, index));
      add_free(dd, isNotZero);
      isNotZero = tmp;
    }
  }

  /* create remainder and quotient and set the remainder to the zero value */
  quot = AddArray_create(N);
  rem = AddArray_create(N);
  for (index = 0; index < N; ++index) {
    AddArray_set_n(rem, index, add_zero(dd));
  }
  
  /* calculate the division operation */
  for (index = N - 1; index >= 0; --index) {
    add_ptr isDividable;

    /* shift remainder left by 1 bit and add the bit number 'index' from arg1 */
    {
      int k;
      add_ptr tmp;
      add_free(dd, AddArray_get_n(rem, N-1));
      for (k = N - 1; k > 0; --k) {
        AddArray_set_n(rem, k, AddArray_get_n(rem, k - 1));
      }
      tmp = AddArray_get_n(arg1, index);
      add_ref(tmp);
      AddArray_set_n(rem, 0, tmp);
    }
      
    /* calculate ADD for (remainder >= arg2), i.e. flag that 
       subtraction can be performed. Set the quotinet[index] to this value .
    */
    {
      AddArray_ptr tmp = AddArray_word_greater_equal(dd, rem, arg2);
      isDividable = AddArray_get_add(tmp);
      add_ref(isDividable);
      AddArray_destroy(dd, tmp);
      AddArray_set_n(quot, index, isDividable);
    }
    
    /* set remainder to (remainder - (arg2 AND (remainder >= arg2))) */
    {
      AddArray_ptr diff;
      AddArray_ptr guardedArg = AddArray_create(N);
      int k;
      
      for (k = 0; k < N; ++k) {
        add_ptr add = add_and(dd, AddArray_get_n(arg2, k), isDividable);
        AddArray_set_n(guardedArg, k, add);
      }
      
      diff = AddArray_word_minus(dd, rem, guardedArg);

      AddArray_destroy(dd, rem);
      AddArray_destroy(dd, guardedArg);
      rem = diff;
    }
  } /* for index */
  
  /* Calculation of the quotient and the remainder is finished.
     Now guard every bit of quotient and remainder by the condition that
     the divisor is not equal to zero 
  */
  { 
    /* create an error message */
    add_ptr error = add_leaf(dd, 
                     failure_make("Division by zero", 
                                  FAILURE_DIV_BY_ZERO, 
                                  (int) node_get_lineno(get_the_node())));
    
    for (index = 0; index < N; ++index) {
      add_ptr guarded;
      /* quotient */
      guarded = add_ifthenelse(dd, isNotZero, AddArray_get_n(quot, index), 
                               error);
      add_free(dd, AddArray_get_n(quot, index));
      AddArray_set_n(quot, index, guarded);
      /* remainder */
      guarded = add_ifthenelse(dd, isNotZero, AddArray_get_n(rem, index), 
                               error);
      add_free(dd, AddArray_get_n(rem, index));
      AddArray_set_n(rem, index, guarded);
    }

    add_free(dd, error);
  }

  add_free(dd, isNotZero);
  *remainder = rem;
  *quotient = quot;
  return;
}


/**Function********************************************************************

  Synopsis    [Create an ADD of the default case in shift operations, i.e. ADD of
  ITE(nubmer >=0 && number <= width, zero, FAILURE).
  ]

  Description [This function is used in shift operations.
  See, for example, AddArray_word_left_shift.

  The 'number' is ADD of the number of bit the Word is shifted.
  'width' is the width of the given Word expression.
  'errMessage' is the error message to print if number is out of range,
  for example, "Right operand of left-shift is out of range".

  NB: The returned ADD is referenced.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static add_ptr
add_array_create_default_value_of_shift_operation(DdManager* dd,
                                                  AddArray_ptr number,
                                                  int width,
                                                  const char* errMessage)
{
  int nw = AddArray_get_size(number);
  add_ptr res = (add_ptr) NULL;

  add_ptr def_val = add_zero(dd);
  add_ptr err_val = add_leaf(dd, 
                        failure_make(errMessage, FAILURE_UNSPECIFIED,
                        (int) node_get_lineno(get_the_node())));

  if (nw > 1) {
    AddArray_ptr adef_val, aerr_val, zero, nge0, nlew, nge0_lew, ite;

    adef_val = AddArray_from_add(add_dup(def_val));
    aerr_val = AddArray_from_add(add_dup(err_val));
    zero = AddArray_from_word_number(dd, WordNumber_from_integer(0, nw));

    nge0 = AddArray_word_greater_equal(dd, number, zero);
    if ((2 << (nw-1)) > width) { /* does it need to check number <= width? */
      AddArray_ptr aw;
      aw = AddArray_from_word_number(dd, WordNumber_from_integer(width, nw));
      nlew = AddArray_word_less_equal(dd, number, aw);
      nge0_lew = AddArray_word_apply_binary(dd, nge0, nlew, add_and);
      AddArray_destroy(dd, nlew);
      AddArray_destroy(dd, aw);
      AddArray_destroy(dd, nge0);
    }
    else {
      nge0_lew = nge0;
    }
    ite = AddArray_word_ite(dd, nge0_lew, adef_val, aerr_val);
    res = add_dup(AddArray_get_add(ite));

    AddArray_destroy(dd, ite);
    AddArray_destroy(dd, nge0_lew);

    AddArray_destroy(dd, zero);    
    AddArray_destroy(dd, aerr_val);
    AddArray_destroy(dd, adef_val);
  }
  else {
    add_ptr zero, aw, nge0, nlew, nge0_lew;

    zero = add_zero(dd);
    aw = add_leaf(dd, find_node(NUMBER, NODE_FROM_INT(width), Nil));
    nge0 = add_apply(dd, node_ge, AddArray_get_add(number), zero);
    nlew = add_apply(dd, node_le, AddArray_get_add(number), aw);
    nge0_lew = add_and(dd, nge0, nlew);
    res = add_ifthenelse(dd, nge0_lew, def_val, err_val);

    add_free(dd, nge0_lew);
    add_free(dd, nlew);
    add_free(dd, nge0);
    add_free(dd, aw);
    add_free(dd, zero);
  }

  add_free(dd, err_val);
  add_free(dd, def_val);

  return res;
}

/**Function********************************************************************

  Synopsis    [A function takes an array and applies ADD's
  "add_monadic_apply" with the operation "op" on every bit of the array.]

  Description [the result of the functions is a new array
        [add_monadic_apply(op, arg[0]), 
         add_monadic_apply(op, arg[1]), 
         ....
         add_monadic_apply(op, arg[width-1])].

  NB: The invoker should free the returned array.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static AddArray_ptr add_array_apply_on_bits(DdManager* dd, NPFNN/*NPFCVT*/ op,
                                            AddArray_ptr arg)
{
  AddArray_ptr res;
  int i;
  const int width = AddArray_get_size(arg);
  /* array should be of the positive size */
  nusmv_assert(width > 0);
  
  res = AddArray_create(width);
  for (i = 0; i < width; ++i) {
    add_ptr add = add_monadic_apply(dd, op, AddArray_get_n(arg, i));
    AddArray_set_n(res, i, add);
  }
  return res;
}



/**Function********************************************************************

  Synopsis    [Private service]

  Description [op can be: AddArray_word_less or AddArray_word_less_equal]

  SideEffects []

  SeeAlso     [AddArray_word_signed_less, AddArray_word_signed_less_equal]

******************************************************************************/
static AddArray_ptr 
add_array_word_signed_operator(DdManager* dd, APFDAA op, 
                               AddArray_ptr arg1, AddArray_ptr arg2)
{
  /* A <s B 
     === (A[msb] & !B[msb]) | ((A[msb] = B[msb]) & (A op B))
  */
  
  const int width = AddArray_get_size(arg1);

  add_ptr arg1_msb, arg2_msb; /* Not to be freed */
  add_ptr tmp1, tmp2;
  add_ptr not_arg2_msb;
  AddArray_ptr less_op;
  add_ptr res;

  nusmv_assert(width == AddArray_get_size(arg2) && width > 0);

  /* prepare the arguments */
  arg1_msb = AddArray_get_n(arg1, width-1);
  arg2_msb = AddArray_get_n(arg2, width-1);
  not_arg2_msb = add_monadic_apply(dd, node_not, arg2_msb);
  less_op = op(dd, arg1, arg2);

  /* Calculates tmp2 := ((A[msb] = B[msb]) & (A <u B)) */
  tmp1 = add_apply(dd, node_equal, arg1_msb, arg2_msb);
  tmp2 = add_and(dd, tmp1, AddArray_get_add(less_op));
  AddArray_destroy(dd, less_op);
  add_free(dd, tmp1);

  /* Calculates tmp1 := (A[msb] & !B[msb]) */
  tmp1 = add_and(dd, arg1_msb, not_arg2_msb);

  /* The final result */
  res = add_or(dd, tmp1, tmp2);

  add_free(dd, tmp1);
  add_free(dd, tmp2);
  add_free(dd, not_arg2_msb);
  
  return AddArray_from_add(res);
}


