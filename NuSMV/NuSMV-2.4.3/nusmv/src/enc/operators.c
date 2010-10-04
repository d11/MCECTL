/**CFile***********************************************************************

  FileName    [operators.c]

  PackageName [enc]

  Synopsis    [These operators are used by dd package]

  Description [Functions like add_plus, add_equal, etc., call these operators]

  SeeAlso     [operators.h]

  Author      [Marco Roveri, Roberto Cavada]

  Copyright   [
  This file is part of the ``enc'' package of NuSMV version 2. 
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

#include "operators.h"
#include "parser/symbols.h"
#include "utils/error.h"
#include "utils/ustring.h"
#include "opt/opt.h"
#include "utils/WordNumber.h"

static char rcsid[] UTIL_UNUSED = "$Id: operators.c,v 1.1.2.5.4.17.4.12 2007/05/18 17:24:22 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef int (*INTPFII)(int, int);


/*---------------------------------------------------------------------------*/
/* macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN int yylineno;
EXTERN options_ptr options;

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static node_ptr 
node_word_full_adder ARGS((node_ptr ai, node_ptr bi, node_ptr carry_in, 
                           node_ptr* carry_out));

static node_ptr node_word_divide_reminder ARGS((node_ptr a, node_ptr b, 
                                                node_ptr* reminder));

static node_ptr node_word_sign_op ARGS((node_ptr a, node_ptr b, NPFNN op));


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [ performs logical or bitwise AND on two nodes.]

  Description        [ Nodes can be integers with values 0 and 1 (logical AND) 
  or Word constants (bitwise AND). All other combinations are illegal.]

  SideEffects        []

******************************************************************************/
node_ptr node_and(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) & NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_and(WORD_NUMBER(car(n1)),
                                              WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("&", n1, n2);

  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [ performs logical or bitwise OR on two nodes.]

  Description        [ Nodes can be integers with values 0 and 1 (logical OR) 
  or Word constants (bitwise OR). All other combinations are illegal.]

  SideEffects        []

******************************************************************************/
node_ptr node_or(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) | NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_or(WORD_NUMBER(car(n1)),
                                             WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("|", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [ performs logical or bitwise NOT on a node.]

  Description        [ Node can be an integer with values 0 or 1 (logical NOR) 
  or a Word constant (bitwise OR). All other combinations are illegal.

  NOTE: At the momement, CUDD does not have unary 'apply', so
  you have to write a unary operator in the form of a binary one which
  actually applies to the first operand only]

  SideEffects        []

******************************************************************************/
node_ptr node_not(node_ptr n, node_ptr this_node_is_not_used)
{ 
  if (node_get_type(n) == FAILURE) return n; /* error in previous expr */

  if (node_get_type(n) == NUMBER) {
    return NODE_TO_INT(car(n)) ? zero_number : one_number;
  }
  else if(node_get_type(n) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_not(WORD_NUMBER(car(n))), Nil);
  }
  else error_not_proper_number("", n);
  return (node_ptr) NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [ performs logical or bitwise IFF on two nodes.]

  Description        [ Nodes can be integers with values 0 and 1 (logical IFF) 
  or Word constants (bitwise IFF). All other combinations are illegal.]

  SideEffects        []

******************************************************************************/
node_ptr node_iff(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if ((node_get_type(n1) == NUMBER) && (node_get_type(n2) == NUMBER)) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) == NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_iff(WORD_NUMBER(car(n1)),
                                              WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("iff", n1, n2);

  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [ performs logical or bitwise XOR on two nodes.]

  Description        [ Nodes can be integers with values 0 and 1 (logical XOR) 
  or Word constants (bitwise XOR). All other combinations are illegal.]

  SideEffects        []

******************************************************************************/
node_ptr node_xor(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) ^ NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_xor(WORD_NUMBER(car(n1)),
                                              WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("xor", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis        [ performs logical or bitwise IMPLIES on two nodes.]

  Description     [Nodes can be integers with values 0 and 1 (logical IMPLIES) 
  or Word constants (bitwise IMPLIES). All other combinations are illegal.]

  SideEffects     []

******************************************************************************/
node_ptr node_implies(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
      NODE_FROM_INT((!NODE_TO_INT(car(n1))) | NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_implies(WORD_NUMBER(car(n1)),
                                                  WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("implies", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 (symbol one_number) if 
  the nodes are the same, and value 0 (symbol zero_number) otherwise]

  Description        [
  In NuSMV an constant is equal to another constant then this 
  constants are actually the same and representable by the same node.
  The only exception is if one operand is boolean and the other 
  is Word -- they also may be equal]

  SideEffects        []

  SeeAlso            [node_setin]

******************************************************************************/
node_ptr node_equal(node_ptr n1, node_ptr n2)
{
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(n1 == n2) return(one_number);

  /* additional equality test: Word[1] * boolean. 
     This test is required only if any Word expression is represented 
     as one ADD */
  /* n1 is boolean and n2 is Word[1] and they are equal. */
  if (NUMBER == node_get_type(n1) && NUMBER_WORD == node_get_type(n2) &&
      1 == WordNumber_get_width(WORD_NUMBER(car(n2))) &&
      NODE_TO_INT(car(n1)) == WordNumber_get_bit(WORD_NUMBER(car(n2)), 0)) {
    return(one_number);
  }

  /* n1 is Word[1] and n2 is boolean and they are equal */
  if (NUMBER == node_get_type(n2) && NUMBER_WORD == node_get_type(n1) &&
      1 == WordNumber_get_width(WORD_NUMBER(car(n1))) &&
      NODE_TO_INT(car(n2)) == WordNumber_get_bit(WORD_NUMBER(car(n1)), 0)) {
    return(one_number);
  }

  return(zero_number);
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 (symbol one_number) if 
  the nodes are of different values, and value 0 (symbol zero_number) otherwise]

  Description        [
  In NuSMV an constant is equal to another constant then this 
  constants are actually the same and representable by the same node.
  The only exception is if one operand is boolean and the other 
  is Word -- they also may be equal]

  SideEffects        []

  SeeAlso            [node_setin]

******************************************************************************/
node_ptr node_not_equal(node_ptr n1, node_ptr n2)
{
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(n1 == n2) return(zero_number);

  /* additional equality test: Word[1] * boolean.
     This test is required only if any Word expression is represented 
     as one ADD */
  /* n1 is boolean and n2 is Word[1] and they are equal */
  if (NUMBER == node_get_type(n1) && NUMBER_WORD == node_get_type(n2) &&
      1 == WordNumber_get_width(WORD_NUMBER(car(n2))) &&
      NODE_TO_INT(car(n1)) == WordNumber_get_bit(WORD_NUMBER(car(n2)), 0)) {
    return(zero_number);
  }

  /* n1 is Word[1] and n2 is boolean and they are equal */
  if (NUMBER == node_get_type(n2) && NUMBER_WORD == node_get_type(n1) &&
      1 == WordNumber_get_width(WORD_NUMBER(car(n1))) &&
      NODE_TO_INT(car(n2)) == WordNumber_get_bit(WORD_NUMBER(car(n1)), 0)) {
    return(zero_number);
  }

  return(one_number);
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is less than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER or both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_lt(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return (NODE_TO_INT(car(n1)) < NODE_TO_INT(car(n2))) ? 
      one_number : zero_number;
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_less(WORD_NUMBER(car(n1)), WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("<", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is greater than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER or both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_gt(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return NODE_TO_INT(car(n1)) > NODE_TO_INT(car(n2)) ? 
      one_number : zero_number;
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_greater(WORD_NUMBER(car(n1)), WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers(">", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is less or equal than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER or both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_le(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return (NODE_TO_INT(car(n1)) <= NODE_TO_INT(car(n2))) ? 
      one_number : zero_number;
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_less_or_equal(WORD_NUMBER(car(n1)), WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("<=", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is greater or equal than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER or both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_ge(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return NODE_TO_INT(car(n1)) >= NODE_TO_INT(car(n2)) ? 
      one_number : zero_number;
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_greater_or_equal(WORD_NUMBER(car(n1)),
                                       WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers(">=", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is signed less than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_slt(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_signed_less(WORD_NUMBER(car(n1)), WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("signed <", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is signed greater than the second one, and 0 - otherwise.]

  Description        [Nodes should be both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_sgt(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_signed_greater(WORD_NUMBER(car(n1)), WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("signed >", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is signed less or equal than the second one, and 0 - otherwise.]

  Description        [Nodes should be  both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_sle(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_signed_less_or_equal(WORD_NUMBER(car(n1)),
                                           WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("signed <=", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [returns NUMBER with value 1 if 
  the first node is signed greater or equal than the second one,
  and 0 - otherwise.]

  Description        [Nodes should be both NUMBER_WORD]

  SideEffects        []

******************************************************************************/
node_ptr node_sge(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return WordNumber_signed_greater_or_equal(WORD_NUMBER(car(n1)),
                                              WORD_NUMBER(car(n2)))
      ? one_number : zero_number;
  }
  else error_not_proper_numbers("signed >=", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Negates the operand (unary minus)]

  Description        [Left node can be NUMBER or NUMBER_WORD, and the
  right one is Nil.]

  SideEffects        []

******************************************************************************/
node_ptr node_unary_minus(node_ptr n, node_ptr this_node_is_not_used)
{ 
  if (node_get_type(n) == FAILURE) return n; /* error in previous expr */

  if (node_get_type(n) == NUMBER) {
    return find_node(NUMBER, NODE_FROM_INT(-NODE_TO_INT(car(n))), Nil);
  }
  else if(node_get_type(n) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
             (node_ptr)WordNumber_unary_minus(WORD_NUMBER(car(n))), Nil);
  }
  else error_not_proper_number("- (unary)", n);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Adds two nodes]

  Description        [Nodes can be both NUMBER or both NUMBER_WORD.]

  SideEffects        []

******************************************************************************/
node_ptr node_plus(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) + NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_plus(WORD_NUMBER(car(n1)),
                                               WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("+", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Subtract two nodes]

  Description        [Nodes can be both NUMBER or both NUMBER_WORD.]

  SideEffects        []

******************************************************************************/
node_ptr node_minus(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return  find_node(NUMBER, 
              NODE_FROM_INT(NODE_TO_INT(car(n1)) - NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_minus(WORD_NUMBER(car(n1)),
                                                WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("-", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Multiplies two nodes]

  Description        [Nodes can be both NUMBER or both NUMBER_WORD.]

  SideEffects        []

******************************************************************************/
node_ptr node_times(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(NUMBER, 
             NODE_FROM_INT(NODE_TO_INT(car(n1)) * NODE_TO_INT(car(n2))), Nil);
  }
  else if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_times(WORD_NUMBER(car(n1)),
                                                WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("*", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Divides two nodes]

  Description        [Nodes can be both NUMBER or both NUMBER_WORD.]

  SideEffects        []

******************************************************************************/
node_ptr node_divide(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  /* check the second operand for being zero */
  if (  (node_get_type(n2) == NUMBER && 0 == NODE_TO_INT(car(n2))) 
     || (node_get_type(n2) == NUMBER_WORD && 
         0L == WordNumber_get_value(WORD_NUMBER(car(n2)))) ) { 
    return failure_make("Division by zero", FAILURE_DIV_BY_ZERO, yylineno);
  }
 
  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    /* here is the check whether usual ANSI semantics of division is used */
    if (opt_use_ansi_c_div_op(options)) {
      return find_node(NUMBER, 
              NODE_FROM_INT(NODE_TO_INT(car(n1)) / NODE_TO_INT(car(n2))), Nil);
    }
    /* the semantics of the division is from old version of NuSMV */
    else {
      int a = NODE_TO_INT(car(n1));
      int b = NODE_TO_INT(car(n2));
      int r = a % b;
      int result = a/b - (r < 0);/*IF r < 0 THEN a/b - 1 ELSE a/b */
      return find_node(NUMBER, NODE_FROM_INT(result), Nil);
    }
  }
  
  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_divide(WORD_NUMBER(car(n1)),
                                                 WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("/", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Computes the remainder of division of two nodes]

  Description        [Nodes can be both NUMBER or both NUMBER_WORD.]

  SideEffects        []

******************************************************************************/
node_ptr node_mod(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  /* check the second operand for being zero */
  if (  (node_get_type(n2) == NUMBER && 0 == NODE_TO_INT(car(n2))) 
     || (node_get_type(n2) == NUMBER_WORD && 
         0ULL == WordNumber_get_value(WORD_NUMBER(car(n2)))) ) { 
    return failure_make("Division by zero", FAILURE_DIV_BY_ZERO, yylineno);
  }

  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    /* here is the check whether usual ANSI semantics of division is used */
    if (opt_use_ansi_c_div_op(options)) {
      return find_node(NUMBER, 
         NODE_FROM_INT(NODE_TO_INT(car(n1)) % NODE_TO_INT(car(n2))), Nil);
    }
    /* the semantics of the division is from old version of NuSMV */
    else {
      int a = NODE_TO_INT(car(n1));
      int b = NODE_TO_INT(car(n2));
      int r = a % b;
      if (r < 0) r += b;
      return find_node(NUMBER, NODE_FROM_INT(r), Nil);
    }
  }
  
  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_mod(WORD_NUMBER(car(n1)),
                                              WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("mod", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Right shifts the first node by the number of bit given 
  in the second node.]

  Description        [First node should be NUMBER_WORD, and the second 
  one should be NUMBER]

  SideEffects        []

******************************************************************************/
node_ptr node_right_shift(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER) {
    WordNumber_ptr w = WORD_NUMBER(car(n1));
    int i = NODE_TO_INT(car(n2));
    if ( i < 0 || i >= WordNumber_get_width(w)) {
      return failure_make("right operand of '>>' is out of range", 
                          FAILURE_UNSPECIFIED, yylineno);
    }
    return find_node(NUMBER_WORD, (node_ptr)WordNumber_right_shift(w, i), Nil);
  }
  else error_not_proper_numbers(">>", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Left shifts the first node by the number of bit given 
  in the second node.]

  Description        [First node should be NUMBER_WORD, and the second 
  one should be NUMBER]

  SideEffects        []

******************************************************************************/
node_ptr node_left_shift(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER) {
    WordNumber_ptr w = WORD_NUMBER(car(n1));
    int i = NODE_TO_INT(car(n2));
    if ( i < 0 || i >= WordNumber_get_width(w)) {
      return failure_make("right operand of '<<' is out of range", 
                          FAILURE_UNSPECIFIED, yylineno);
    }
    return find_node(NUMBER_WORD, (node_ptr)WordNumber_left_shift(w, i), Nil);
  }
  else error_not_proper_numbers("<<", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Right rotates the first node by the number of bit given 
  in the second node.]

  Description        [First node should be NUMBER_WORD, and the second 
  one should be NUMBER]

  SideEffects        []

******************************************************************************/
node_ptr node_right_rotate(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */

  if (node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER) {
    WordNumber_ptr w = WORD_NUMBER(car(n1));
    int i = NODE_TO_INT(car(n2));
    if ( i < 0 || i >= WordNumber_get_width(w)) {
      return failure_make("right operand of '>>>' is out of range", 
                          FAILURE_UNSPECIFIED, yylineno);
    }
    return find_node(NUMBER_WORD, (node_ptr)WordNumber_right_rotate(w, i), Nil);
  }
  else error_not_proper_numbers(">>>", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Left rotates the first node by the number of bit given 
  in the second node.]

  Description        [First node should be NUMBER_WORD, and the second 
  one should be NUMBER]

  SideEffects        []

******************************************************************************/
node_ptr node_left_rotate(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */
  
  if (node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER) {
    WordNumber_ptr w = WORD_NUMBER(car(n1));
    int i = NODE_TO_INT(car(n2));
    if ( i < 0 || i >= WordNumber_get_width(w)) {
      return failure_make("right operand of '<<<' is out of range", 
                          FAILURE_UNSPECIFIED, yylineno);
    }
    return find_node(NUMBER_WORD, (node_ptr)WordNumber_left_rotate(w, i), Nil);
  }
  else error_not_proper_numbers("<<<", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [conatenates the values of two NUMBER_WORD nodes.]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_concatenation(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */
  
  /* cast */

  /* if n1 is boolean => cast to Word[1] */
  if (NUMBER == node_get_type(n1) && (NODE_TO_INT(car(n1)) & ~1) == 0) {
    n1 = find_node(NUMBER_WORD, 
                   (node_ptr)WordNumber_from_integer(NODE_TO_INT(car(n1)), 1), 
                   Nil);
  }

  /* if n2 is boolean => cast to Word[1] */
  if (NUMBER == node_get_type(n2) && (NODE_TO_INT(car(n2)) & ~1) == 0) {
    n2 = find_node(NUMBER_WORD, 
                   (node_ptr)WordNumber_from_integer(NODE_TO_INT(car(n2)), 1), 
                   Nil);
  }
  
  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == NUMBER_WORD){
    return find_node(NUMBER_WORD,
               (node_ptr)WordNumber_concatenate(WORD_NUMBER(car(n1)),
                                                WORD_NUMBER(car(n2))), Nil);
  }
  else error_not_proper_numbers("::", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [creates RANGE node from two NUMBER nodes.]

  Description        [this range is used in bit-selection only]

  SideEffects        [node_bit_selection]

******************************************************************************/
node_ptr node_bit_range(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */
  
  if (node_get_type(n1) == NUMBER && node_get_type(n2) == NUMBER) {
    return find_node(RANGE, n1, n2);
  }
  else error_not_proper_numbers("bit-selection-range", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [perform bit selection operation]

  Description        [first operand must be NUMBER_WORD node and 
  the second one must be RANGE(which holds the two NUMBERs).]

  SideEffects        [node_bits_range]

******************************************************************************/
node_ptr node_bit_selection(node_ptr n1, node_ptr n2)
{ 
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */
  
  /* cast */
  if(node_get_type(n1) == NUMBER_WORD && node_get_type(n2) == RANGE){
    return find_node(NUMBER_WORD,
                     (node_ptr) WordNumber_bit_select(WORD_NUMBER(car(n1)),
                                                     NODE_TO_INT(car(car(n2))),
                                                     NODE_TO_INT(car(cdr(n2)))),
                     Nil);
  }
  else error_not_proper_numbers("bit-selection", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [converts NUMBER_WORD node with 1-bit-width Word constant
  to boolean NUMBER.
 
  NOTE: At the momement, CUDD does not have unary 'apply', so
  you have to write a unary operator in the form of a binary one which
  actually applies to the first operand only]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_bool_cast(node_ptr n, node_ptr this_node_is_not_used)
{ 
  WordNumber_ptr word;

  if (node_get_type(n) == FAILURE) return n; /* error in previous expr */
  
  word = WORD_NUMBER(car(n));

  if (node_get_type(n) == NUMBER_WORD && WordNumber_get_width(word) == 1) {
    return  WordNumber_get_bit(word, 0) ? one_number : zero_number;
  }
  else error_not_proper_number("bool", n);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [ converts boolean NUMBER node to NUMBER_WORD
  with 1-bit-width Word constant.

  NOTE: At the momement, CUDD does not have unary 'apply', so
  you have to write a unary operator in the form of a binary one which
  actually applies to the first operand only]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word1_cast(node_ptr n, node_ptr this_node_is_not_used)
{ 
  if (node_get_type(n) == FAILURE) return n; /* error in previous expr */
  
  if (node_get_type(n) == NUMBER) {
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_from_integer(NODE_TO_INT(car(n)), 1),
                     Nil);
  }
  else error_not_proper_number("word1", n);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Concatenates sign bit of n1 n2 number of times to n1]

  Description        [left exp must be a NUMBER_WORD and the right one
  is a NUMBER]

  SideEffects        []

******************************************************************************/
node_ptr node_sign_extend(node_ptr n1, node_ptr n2) {
  if (node_get_type(n1) == FAILURE) return n1; /* error in previous expr */
  if (node_get_type(n2) == FAILURE) return n2; /* error in previous expr */
  
  if (node_get_type(n1) == NUMBER_WORD && node_get_type(n2) ==NUMBER) {
    return find_node(NUMBER_WORD,
                     (node_ptr)WordNumber_sign_extend(WORD_NUMBER(car(n1)),
                                                      NODE_TO_INT(car(n2))), 
                     Nil);
  }
  else error_not_proper_numbers("sign-extend", n1, n2);
  return (node_ptr)NULL;/* return something to suppress warnings */
}

/**Function********************************************************************

  Synopsis           [Computes the set union of two s_expr.]

  Description        [This function computes the sexp resulting from
  the union of s_expr "n1" and "n2".
  NB: if any of the operands is a FAILURE node, the FAILURE node is returned.]

  SideEffects        []

******************************************************************************/
node_ptr node_union(node_ptr n1, node_ptr n2)
{
  node_ptr tmp;

  if(n1 == Nil) return(n2);
  if(n2 == Nil) return(n1);

  if (node_get_type(n1) == FAILURE) return n1; /* error in operand */
  if (node_get_type(n2) == FAILURE) return n2; /* error in operand */
  
  /* convert singleton elements to lists */
  if(node_get_type(n1) != CONS) n1 = find_node(CONS, n1, Nil);
  if(node_get_type(n2) != CONS) n2 = find_node(CONS, n2, Nil);

  /* create a list merged from the given lists and 
     with all elements ordered (less-comparison)
  */
  tmp = Nil;
  while (n1 != Nil && n2 != Nil) {
    if (car(n1) == car(n2)) {
      tmp = cons(car(n1), tmp);
      n1 = cdr(n1);
      n2 = cdr(n2);
    }
    else if (car(n1) < car(n2)) {/* < is used because the list will be reversed */
      tmp = cons(car(n1), tmp);
      n1 = cdr(n1);
    }
    else { /*car(n2) > car(n1) */
      tmp = cons(car(n2), tmp);
      n2 = cdr(n2);
    }
  }
  if (Nil == n1) n1 = n2; /* remaining elements (they were in n1 or n2) */

  /* reverse the obtained list and apply find_node. The result will be in n1 */
  while (Nil != tmp) {
    n1 = find_node(CONS, car(tmp), n1);

    n2 =  cdr(tmp);
    free_node(tmp);
    tmp = n2;
  }
  return n1;
}

/**Function********************************************************************

  Synopsis           [Set inclusion]

  Description        [Checks if s_expr "n1" is a subset of s_expr
  "n2", if it is the case them <code>one_number</code> is returned,
  else <code>zero_number</code> is returned.

  If "n1" is a list of values then <code>one_number</code> is returned only
  if all elements of "n1" is a subset of "n2".

  NB: if any of the operands is a FAILURE node, the FAILURE node is returned.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
node_ptr node_setin(node_ptr n1, node_ptr n2)
{
  node_ptr iter1;
  node_ptr iter2;

  if (node_get_type(n1) == FAILURE) return n1; /* error in operand */
  if (node_get_type(n2) == FAILURE) return n2; /* error in operand */
  
  /* convert singleton elements to lists */
  if (CONS != node_get_type(n1)) n1 = find_node(CONS, n1, Nil);
  if (CONS != node_get_type(n2)) n2 = find_node(CONS, n2, Nil);
  
  /* check that every element of n1 is equal to some element of n2 */
  for (iter1 = n1; iter1 != Nil; iter1 = cdr(iter1)) {
    for (iter2 = n2; iter2 != Nil; iter2 = cdr(iter2)) {
      if (car(iter1) == car(iter2)) break; /* there is equality */
    }
    /* one of the elements of n1 is not equal to any elements of n2 */
    if (iter2 == Nil) return zero_number;
  }
  return one_number;
}


/* ---------------------------------------------------------------------- */
/*   WORDS releated (encoded as node_ptr)                                 */
/* ---------------------------------------------------------------------- */

/**Function********************************************************************

  Synopsis           [Creates a node_ptr that represents the encoding of a
  WORD.]

  Description        [bitval is the initial value of all bits. w it the
  word width]

  SideEffects        []

******************************************************************************/
node_ptr node_word_create(node_ptr bitval, size_t w)
{
  node_ptr width = find_node(NUMBER, (node_ptr) w, Nil);
  node_ptr enc = Nil;
  for (;w>0; --w) { enc = cons(bitval, enc); }
  
  return find_node(WORD, enc, width);
}


/**Function********************************************************************

  Synopsis           [Creates a node_ptr that represents the encoding of a
  WORD, taking the values of bits from the given list]

  Description        [The list (of CONS nodes) must have length equal to w]

  SideEffects        [node_word_create]

******************************************************************************/
node_ptr node_word_create_from_list(node_ptr l, size_t w)
{
  nusmv_assert(node_get_type(l) == CONS);
  nusmv_assert(llength(l) == w);
  
  return find_node(WORD, l, find_node(NUMBER, (node_ptr) w, Nil));
}


/**Function********************************************************************

  Synopsis           [Creates a node_ptr that represents the encoding of a
  WORD, taking the values of bits from the given WordNumber]

  Description        [Word width is taken from the given WordNumber]

  SideEffects        []

******************************************************************************/
node_ptr node_word_create_from_wordnumber(WordNumber_ptr wn)
{
  node_ptr bits;
  int w, i;

  w = WordNumber_get_width(wn);
  bits = Nil;      
  for (i=0; i<w; ++i) {
    bits = cons(WordNumber_get_bit(wn, i) ? 
                find_node(TRUEEXP, Nil, Nil) 
                : find_node(FALSEEXP, Nil, Nil), 
                bits);        
  }

  return node_word_create_from_list(bits, w);
}


/**Function********************************************************************

  Synopsis           [Creates a node_ptr that represents the encoding of a
  WORD, taking the values of bits from the given integer value]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_create_from_integer(unsigned long long value, 
                                       size_t width)
{
  return node_word_create_from_wordnumber(
                          WordNumber_from_integer(value, width));
}


/**Function********************************************************************

  Synopsis           [Creates a node_ptr that represents the encoding of a
  WORD, taking the values of bits from the given array of nodes.]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_create_from_array(array_t* arr)
{
  node_ptr res = Nil;
  node_ptr bit;
  int i;

  arrayForEachItem(node_ptr, arr, i, bit) { res = cons(bit, res); }
  return node_word_create_from_list(res, array_n(arr));
}


/**Function********************************************************************

  Synopsis           [Returns the width of the given word]

  Description        []

  SideEffects        []

******************************************************************************/
size_t node_word_get_width(node_ptr w)
{
  return node_get_int(cdr(w));
}


/**Function********************************************************************

  Synopsis           [Converts the given word to a dynamic array.]

  Description        [The array must be freed by the caller]

  SideEffects        []

******************************************************************************/
array_t* node_word_to_array(node_ptr w)
{ 
  int wid, i;
  array_t* res;
  node_ptr iter;

  _CHECK_WORD(w);

  wid = node_get_int(cdr(w));  
  res = array_alloc(node_ptr, wid);  
  for (i=wid-1, iter=car(w); i>=0; --i, iter=cdr(iter)) { 
    array_insert(node_ptr, res, i, car(iter)); 
  }
  
  return res;
}


/**Function********************************************************************

  Synopsis           [Private helpers for expr2bexpr_recur (used by 'map' and 
  'map2')]

  Description        []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static int _apply_op = 0;
static node_ptr _node_word_apply_unary_aux(node_ptr e)
{ return find_node(_apply_op, e, Nil); }
static node_ptr _node_word_apply_binary_aux(node_ptr e1, node_ptr e2)
{ return find_node(_apply_op, e1, e2); }


/**Function********************************************************************

  Synopsis           [Traverses the word bits, and foreach bit creates a node
  whose operator is given. The result is returned as a new word encoding]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_apply_unary(node_ptr wenc, int op)
{
  node_ptr res;

  _CHECK_WORD(wenc);

  _apply_op = op;
  res = map(_node_word_apply_unary_aux, car(wenc));
  return find_node(WORD, res, cdr(wenc));
}


/**Function********************************************************************

  Synopsis [Traverses two given words, and creates a new word
  applying to each pair of bits the given operator]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_apply_binary(node_ptr wenc1, node_ptr wenc2, int op)
{ 
  node_ptr res;

  _CHECK_WORDS(wenc1, wenc2);

  _apply_op = op;                                     
  res = map2(_node_word_apply_binary_aux, car(wenc1), car(wenc2));
  return find_node(WORD, res, cdr(wenc1));
}


/**Function********************************************************************

  Synopsis           [Returns an AND node that is the conjuction of all
  bits of the given word]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_make_conjuction(node_ptr w)
{
  node_ptr res = find_node(TRUEEXP, Nil, Nil);
  for (w=car(w); w != Nil; w=cdr(w)) res = find_node(AND, car(w), res);
  return res;
}


/**Function********************************************************************

  Synopsis           [Returns an OR node that is the disjuction of all
  bits of the given word]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_make_disjunction(node_ptr w)
{
  node_ptr res = find_node(FALSEEXP, Nil, Nil);
  for (w=car(w); w != Nil; w=cdr(w)) res = find_node(OR, car(w), res);
  return res;
}

/**Function********************************************************************

  Synopsis           [Casts the given word to boolean]

  Description        [The word must have width 1]

  SideEffects        []

******************************************************************************/
node_ptr node_word_cast_bool(node_ptr w)
{ 
  _CHECK_WORD(w);
  nusmv_assert(node_get_int(cdr(w)) == 1);

  return car(car(w));
}

/**Function********************************************************************

  Synopsis [Returns a new word that is the negation of the given
  word]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_not(node_ptr w)
{ return node_word_apply_unary(w, NOT); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the conjuction of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_and(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, AND); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the disjuction of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_or(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, OR); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the xor of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_xor(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, XOR); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the xnor of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_xnor(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, XNOR); }

/**Function********************************************************************

  Synopsis [Returns a new word that is the logical implication of
  the given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_implies(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, IMPLIES); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the <-> of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_iff(node_ptr a, node_ptr b)
{ return node_word_apply_binary(a, b, IFF); }

/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_equal(node_ptr a, node_ptr b)
{ return node_word_make_conjuction(node_word_iff(a, b)); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the xor of the given
  words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_notequal(node_ptr a, node_ptr b)
{ return node_word_make_disjunction(node_word_xor(a, b)); }

/**Function********************************************************************

  Synopsis           [Returns a new word that is the concatenationof the given
  words]

  Description [The first given word is the most significant word
  of the result]

  SideEffects        []

******************************************************************************/
node_ptr node_word_concat(node_ptr a, node_ptr b)
{
  size_t w;

  _CHECK_WORD(a);
  _CHECK_WORD(b);

  w = node_get_int(cdr(a)) + node_get_int(cdr(b));
  return node_word_create_from_list(append_ns(car(a), car(b)), w);
}

/**Function********************************************************************

  Synopsis           [Performs bit selections of the given word]

  Description        [word must be already booleanized. 
  Range must be compatible with the given word width, and must be a node in the 
  form of COLON(NUMBER, NUMBER)]

  SideEffects        []

******************************************************************************/
node_ptr node_word_selection(node_ptr word, node_ptr range)
{
  int width, high, low, i;
  node_ptr res, iter, tmp;
  
  _CHECK_WORD(word);
  nusmv_assert(node_get_type(range) == COLON &&
               node_get_type(car(range)) == NUMBER &&
               node_get_type(cdr(range)) == NUMBER);

  width = node_get_int(cdr(word));
  high = node_get_int(car(range));
  low = node_get_int(cdr(range));
  nusmv_assert(high >= low && low >= 0 && high < width);

  /* gets rid of higher bits */
  iter = car(word);
  for (i=width-1; i > high; --i) iter = cdr(iter);

  /* Takes only bits until low. Constructs the list top down */  
  res = find_node(CONS, car(iter), Nil); /* at least one bit */
  tmp = res; /* tail of the new list */
  for (--i, iter = cdr(iter); i>=low; --i, iter = cdr(iter)) {
    node_ptr a = find_node(CONS, car(iter), Nil);    
    node_node_setcdr(tmp, a);
    tmp=a;
  }
  
  return node_word_create_from_list(res, high-low+1);
}


/**Function********************************************************************

  Synopsis [Bit-blasts the given words, creating a new word
  encoding that is an added circuit]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_adder(node_ptr a, node_ptr b, node_ptr carry_in, 
                         node_ptr* carry_out)
{
  node_ptr res;
  node_ptr width;

  _CHECK_WORDS(a, b);

  width = cdr(a);
  res = Nil;
  for (a=reverse_ns(car(a)),b=reverse_ns(car(b)); 
       a!=Nil && b != Nil; a=cdr(a), b=cdr(b)) {
    node_ptr bit_carry;
    node_ptr bit = node_word_full_adder(car(a), car(b), carry_in, &bit_carry);
    res = cons(bit, res);    
    carry_in = bit_carry;
  }
  
  *carry_out = carry_in;
  return find_node(WORD, res, width);
}


/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  adds given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_plus(node_ptr a, node_ptr b)
{
  node_ptr carry_out;
  return node_word_adder(a, b, find_node(FALSEEXP, Nil, Nil), &carry_out);
}

/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  subtracts given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_minus(node_ptr a, node_ptr b)
{
  /* a-b ==  a+(not b)+1 */ 
  node_ptr carry_out;
  return node_word_adder(a, node_word_not(b),
                  find_node(TRUEEXP, Nil, Nil), &carry_out);
}

/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  performs unsigned subtraction of given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_uminus(node_ptr a)
{
  /* -a ==  (not a)+1 */
  node_ptr carry_out;
  node_ptr zero, zenc, wzero;
  int w;

  _CHECK_WORD(a);

  /* creates 0b0 */  
  zero = find_node(FALSEEXP, Nil, Nil);
  zenc = Nil;
  for (w = node_get_int(cdr(a)); w > 0; --w) zenc = cons(zero, zenc);
  wzero = find_node(WORD, zenc, cdr(a));

  return node_word_adder(node_word_not(a), wzero, 
                  find_node(TRUEEXP, Nil, Nil), &carry_out);
}

/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  performs multiplication of given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_times(node_ptr a, node_ptr b)
{
  /* A*B = ((B & A[0])<<0) +...+ ((B & A[i])<<i) +...+ ((B & A[N-1])<<N-1) */
  array_t *va, *vb, *vab;
  int w, i;

  _CHECK_WORDS(a,b);

  w = node_get_int(cdr(a));
  
  va = node_word_to_array(a);  
  vb = node_word_to_array(b);    

  vab = array_alloc(node_ptr, w);
  /* prepares (a[0] & b) */
  for (i=0; i<w; ++i) {
    array_insert(node_ptr, vab, i,
                 find_node(AND, array_fetch(node_ptr, va, 0), 
                           array_fetch(node_ptr, vb, i)));    
  }
  
  for (i=1; i<w; ++i) {
    node_ptr cin = find_node(FALSEEXP, Nil, Nil);
    int k;

    for (k=0; k < w-i; ++k) {
      node_ptr sum, ctmp, adder;
      
      adder = find_node(AND, array_fetch(node_ptr, va, i), 
                        array_fetch(node_ptr, vb, k));

      sum = node_word_full_adder(array_fetch(node_ptr, vab, i+k), 
                                 adder, cin, &ctmp);
      array_insert(node_ptr, vab, i+k, sum);
      cin = ctmp;   
    }    
  }  

  array_free(vb);
  array_free(va);
  
  { /* creates the WORD from the bit array */   
    node_ptr res = node_word_create_from_array(vab);
    array_free(vab);
    return res;
  }
}


/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  divides given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_divide(node_ptr a, node_ptr b)
{
  node_ptr rem;
  return node_word_divide_reminder(a, b, &rem);
}

/**Function********************************************************************

  Synopsis           [Creates a new word encoding that is the circuit that
  performs modulo of given words]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_mod(node_ptr a, node_ptr b)
{
  node_ptr rem;
  node_word_divide_reminder(a, b, &rem);
  return rem;
}

/**Function********************************************************************

  Synopsis           [Predicate for a < b]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_less(node_ptr a, node_ptr b)
{ 
  /* "carry bit of" ((not A) + B) */
  node_ptr carry_out;
  node_word_adder(node_word_not(a), b, 
                  find_node(FALSEEXP, Nil, Nil), &carry_out);
  return carry_out;
}

/**Function********************************************************************

  Synopsis           [Predicate for a <= b]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_less_equal(node_ptr a, node_ptr b)
{ 
  /* "carry bit of" ((not A) + (B+1)) */
  node_ptr carry_out;
  node_word_adder(node_word_not(a), b, 
                  find_node(TRUEEXP, Nil, Nil), &carry_out);
  return carry_out;
}

/**Function********************************************************************

  Synopsis           [Predicate for a > b]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_greater(node_ptr a, node_ptr b)
{ return node_word_less(b, a); }
  
/**Function********************************************************************

  Synopsis           [Predicate for a >= b]

  Description        []

  SideEffects        []

******************************************************************************/
node_ptr node_word_greater_equal(node_ptr a, node_ptr b)
{ return node_word_less_equal(b, a); }

/**Function********************************************************************

  Synopsis           [Predicate for a <s b]

  Description        [Signed operation is performed]

  SideEffects        []

******************************************************************************/
node_ptr node_word_sign_less(node_ptr a, node_ptr b)
{
  /* A <s B 
     === (A[msb] & !B[msb]) | ((A[msb] = B[msb]) & (A <u B)) */
  return node_word_sign_op(a, b, node_word_less);
}

/**Function********************************************************************

  Synopsis           [Predicate for a <=s b]

  Description        [Signed operation is performed]

  SideEffects        []

******************************************************************************/
node_ptr node_word_sign_less_equal(node_ptr a, node_ptr b)
{
  /* A <=s B 
     === (A[msb] & !B[msb]) | ((A[msb] = B[msb]) & (A <=u B)) */
  return node_word_sign_op(a, b, node_word_less_equal);
}

/**Function********************************************************************

  Synopsis           [Predicate for a >s b]

  Description        [Signed operation is performed]

  SideEffects        []

******************************************************************************/
node_ptr node_word_sign_greater(node_ptr a, node_ptr b)
{
  /* A >s B 
     === B <s A */
  return node_word_sign_less(b, a);
}

/**Function********************************************************************

  Synopsis           [Predicate for a >=s b]

  Description        [Signed operation is performed]

  SideEffects        []

******************************************************************************/
node_ptr node_word_sign_greater_equal(node_ptr a, node_ptr b)
{
  /* A >=s B 
     === B <=s A */
  return node_word_sign_less_equal(b, a);
}


/**Function********************************************************************

  Synopsis           [A private service for predicates]

  Description        []

  SideEffects        []

******************************************************************************/
static node_ptr node_word_sign_op(node_ptr a, node_ptr b, NPFNN op)
{
  _CHECK_WORDS(a, b);

  node_ptr msb_a = car(car(a));
  node_ptr msb_b = car(car(b));

  node_ptr opres = op(a, b);
  node_ptr tmp1, tmp2;

  /* tmp1 := ((A[msb] = B[msb]) & (A <u B)) */
  tmp1 = find_node(AND, find_node(IFF, msb_a, msb_b), opres);
  
  /* tmp2 := (A[msb] & !B[msb]) */
  tmp2 = find_node(AND, msb_a, find_node(NOT, msb_b, Nil));

  /* result: tmp1 | tmp2 */
  return find_node(OR, tmp1, tmp2);    
}


/**Function********************************************************************

  Synopsis           [Implements a full adder circuit]

  Description        [implements a full adder circuit]

  SideEffects        []

******************************************************************************/
static node_ptr 
node_word_full_adder(node_ptr ai, node_ptr bi, node_ptr carry_in, 
                     node_ptr* carry_out)
{
  node_ptr tmp = find_node(XOR, ai, bi);
  
  /* curry_out = (arg1 and arg2) OR ((arg1 XOR arg2) AND carry_in) */
  *carry_out = find_node(OR, find_node(AND, ai, bi), 
                         find_node(AND, tmp, carry_in));
  
  /* sum = arg1 XOR arg2 XOR carry_in */
  return find_node(XOR, tmp, carry_in);
}


/**Function********************************************************************

  Synopsis           [Implements a divide-with-reminder circuit]

  Description        []

  SideEffects        []

******************************************************************************/
static node_ptr node_word_divide_reminder(node_ptr a, node_ptr b, 
                                          node_ptr* reminder)
{
  int w, i;
  node_ptr quot, rem;
  node_ptr b_nz;
  array_t* va, *vb, *vq;

  _CHECK_WORDS(a,b);

  w = node_get_int(cdr(a));

  /* prepares a check for 'b' not to be zero, to be inserted in the
     resulting encoding */
  b_nz = node_word_make_disjunction(b);

  /* creates remainder and quotient */
  rem = node_word_create(find_node(FALSEEXP, Nil, Nil), w);
  vq = array_alloc(node_ptr, w);

  va = node_word_to_array(a);
  vb = node_word_to_array(b);
  
  /* calculates the division operation */
  for (i=w-1; i>=0; --i) {
    node_ptr is_dividable;

    { /* shifts remainder left by 1 bit and add the i-th bit from 'a' */
      node_ptr rbits = cdr(car(rem)); /* shifts 1 left */
      rbits = reverse(rbits);
      rbits = cons(array_fetch(node_ptr, va, i), rbits); /* pushes a[i] */
      rbits = reverse(rbits);
      setcar(rem, rbits);
    }

    /* calculates (rem >= b), i.e. that subtraction can be performed */
    is_dividable = node_word_greater_equal(rem, b);
    array_insert(node_ptr, vq, i, is_dividable);
    
    { /* sets remainder to (rem - (b AND (rem >= b))) */
      node_ptr guard = Nil;
      int k;

      for (k=0; k<w; ++k) {
        guard = cons(find_node(AND, array_fetch(node_ptr, vb, k), 
                               is_dividable), 
                     guard);        
      }
      
      guard = find_node(WORD, guard, cdr(a)); /* msb(guard) is at the top */
      rem = node_word_minus(rem, guard);
    }
  } /* for i */

  array_free(vb);
  array_free(va);
  
  /* Now guards every bit of quotient and remainder by the condition
     that the divisor is not equal to zero */
  {
    array_t* vr = node_word_to_array(rem);
    node_ptr dbz = failure_make("Division by zero", 
                                FAILURE_DIV_BY_ZERO, 
                                node_get_lineno(get_the_node()));

    for (i=0; i<w; ++i) {
      node_ptr ite;

      /* quotient */
      ite = find_node(CASE, 
                      find_node(COLON, b_nz, array_fetch(node_ptr, vq, i)), 
                      dbz);
      array_insert(node_ptr, vq, i, ite);

      /* reminder */
      ite = find_node(CASE, 
                      find_node(COLON, b_nz, 
                                array_fetch(node_ptr, vr, i)), 
                      dbz);
      array_insert(node_ptr, vr, i, ite);      
    }
  
    rem = node_word_create_from_array(vr);
    array_free(vr);
  }

  *reminder = rem;
  quot = node_word_create_from_array(vq);
  array_free(vq);

  return quot;  
}

