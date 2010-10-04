/**CFile***********************************************************************

  FileName    [compileBEval.c]

  PackageName [compile]

  Synopsis    []

  Description [Conversion from scalar expressions to boolean expressions.]

  SeeAlso     [optional]

  Author      [Alessandro Cimatti and Marco Roveri]

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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "compile/compile.h"
#include "compileInt.h"
#include "enc/enc.h"
#include "enc/operators.h"
#include "parser/symbols.h"
#include "parser/mbp_symbols.h"
#include "utils/error.h"
#include "utils/utils_io.h" /* for indent_node */
#include "utils/range.h"
#include "utils/assoc.h"
#include "utils/WordNumber.h"

static char rcsid[] UTIL_UNUSED = "$Id: compileBEval.c,v 1.13.4.12.2.1.2.18.4.21 2007/04/06 11:06:32 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

static hash_ptr expr2bexpr_hash = (hash_ptr) NULL;

void init_expr2bexp_hash() 
{
  nusmv_assert(expr2bexpr_hash == (hash_ptr) NULL);
  expr2bexpr_hash = new_assoc();
}

void clear_expr2bexp_hash()
{
  if (expr2bexpr_hash != (hash_ptr) NULL) {
    free_assoc(expr2bexpr_hash);
    expr2bexpr_hash = (hash_ptr) NULL;
  }
}

static node_ptr make_key(node_ptr expr, boolean a, boolean b) 
{
  nusmv_ptrint j = 0;  

  j += a? 1 : 0;
  j += b? 2 : 0;

  return find_node(CONTEXT, expr, (node_ptr) j);
}

static void 
expr2bexpr_hash_insert_entry(node_ptr expr, node_ptr ctx, 
                             node_ptr bexpr, boolean a, boolean b)
{
  node_ptr cexp;
  nusmv_assert(expr2bexpr_hash != (hash_ptr) NULL);

  if (ctx == Nil) cexp = expr;
  else cexp = find_node(CONTEXT, ctx, expr);
  insert_assoc(expr2bexpr_hash, make_key(cexp, a, b), bexpr);
}

static node_ptr 
expr2bexpr_hash_lookup_entry(node_ptr expr, node_ptr ctx, 
                             boolean a, boolean b)
{
  node_ptr cexp;
  nusmv_assert(expr2bexpr_hash != (hash_ptr) NULL);

  if (ctx == Nil) cexp = expr;
  else cexp = find_node(CONTEXT, ctx, expr);

  return find_assoc(expr2bexpr_hash, make_key(cexp, a, b));
}


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
 
static node_ptr 
expr2bexpr_recur ARGS((BddEnc_ptr enc, SymbLayer_ptr,
                       node_ptr, node_ptr, boolean));
static node_ptr 
scalar_atom2bexpr ARGS((BddEnc_ptr, SymbLayer_ptr,
                        node_ptr, node_ptr, boolean));

static node_ptr 
expr2bexpr_ite ARGS((BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                     node_ptr expr, node_ptr context, boolean in_next));

static node_ptr 
expr2bexpr_word_ite_aux ARGS((BddEnc_ptr enc, SymbLayer_ptr det_layer,
                              node_ptr expr, node_ptr context, 
                              boolean in_next));


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Converts a scalar expression into a boolean expression.]

  Description        [Takes an scalar expression intended to evaluate
  to boolean, maps through booleans down to the atomic scalar
  propositions, builds the corresponding boolean function, and returns
  the resulting boolean expression.

  The conversion of atomic scalar proposition is currently performed
  by generating the corresponding ADD, and then printing it in terms
  of binary variables.

  If one or more determinization variable must be created
  (i.e. non-determinism must be allowed) then det_layer is the
  SymbLayer instance to be filled with the newly created
  determinization variables. If non-determinism is not allowed, specify 
  NULL as det_layer value. In this case you can use detexpr2bexpr as well.

  The input expression will be processed with Nil context (for
  flattened expr this does not matter).]

  SideEffects        [None]

  SeeAlso            [Compile_detexpr2bexpr, expr2bexpr_recur]

******************************************************************************/
Expr_ptr Compile_expr2bexpr(BddEnc_ptr enc, 
                            SymbLayer_ptr det_layer, 
                            Expr_ptr expr)
{
  node_ptr res;
  int temp = yylineno;

  if (expr == EXPR(NULL)) return expr;

  yylineno = node_get_lineno( NODE_PTR(expr) );
  res = expr2bexpr_recur(enc, det_layer, expr, Nil, false);
  yylineno = temp;
  return EXPR(res);
}


/**Function********************************************************************

  Synopsis           [Converts a scalar expression into a boolean expression.]

  Description        [Takes an scalar expression intended to evaluate
  to boolean, maps through booleans down to the atomic scalar
  propositions, builds the corresponding boolean function, and returns
  the resulting boolean expression.
  
  The conversion of atomic scalar proposition is currently performed
  by generating the corresponding ADD, and then printing it in terms
  of binary variables.

  An error is returned if determinization variables are introduced in
  the booleanization process.
  
  The input expression will be processed with Nil context (for
  flattened expr this does not matter).]

  SideEffects        [None]

  SeeAlso            [Compile_expr2bexpr, expr2bexpr_recur]

******************************************************************************/
Expr_ptr Compile_detexpr2bexpr(BddEnc_ptr enc, Expr_ptr expr)
{
  node_ptr res;
  int temp = yylineno;

  if (expr == EXPR(NULL)) return expr;
  
  yylineno = node_get_lineno(expr);
  res = expr2bexpr_recur(enc, SYMB_LAYER(NULL), expr, Nil, false);

  yylineno = temp;
  return EXPR(res);
}



/**Function********************************************************************

  Synopsis [Cleans those hashed entries that are about a symbol that
  is being removed]

  Description        [Called by BoolEnc class when removing a layer]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Compile_cleanup_booleanizer_cache_about(SymbTable_ptr st, 
                                             NodeList_ptr symbs)
{
  node_ptr all_keys = assoc_get_keys(expr2bexpr_hash, true);
  node_ptr iter;
  
  for (iter=all_keys; iter != Nil; iter = cdr(iter)) {
    node_ptr expr = car(car(iter));    
    NodeList_ptr list = Compile_get_expression_dependencies(st, expr);
    ListIter_ptr sym_iter;
    
    for (sym_iter=NodeList_get_first_iter(symbs); !ListIter_is_end(sym_iter); 
         sym_iter=ListIter_get_next(sym_iter)) {
      node_ptr name = NodeList_get_elem_at(symbs, sym_iter);
      if (NodeList_belongs_to(list, name)) {
        /* removes the corresponding entry */
        insert_assoc(expr2bexpr_hash, car(iter), Nil);
      }
    }

    NodeList_destroy(list);
  }
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis [Creates the encoding of the left-shifting circuit for
  words]

  Description        [ wb is -1 if b is not a word (can be a number)]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr expr2bexpr_shift_left(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                      node_ptr context, boolean in_next, 
                                      node_ptr a, node_ptr b, node_ptr def_case, 
                                      int wb)
{ 
  /* creates shift circuit */
  array_t* va;
  node_ptr res;
  int maxb, wa, i;

  wa = node_get_int(cdr(a));
  va = node_word_to_array(a);
  if (wb > 0) maxb = (2 << (wb-1)) - 1;
  else maxb = wa;

  res = Nil;
  for (i=0; i<wa; ++i) {
    node_ptr bit = def_case;
    int k;
    
    for (k=MIN(i,maxb); k >= 0; --k) {
      node_ptr beqk;

      if (wb > 0) {
        node_ptr wk = node_word_create_from_integer(k, wb);
        beqk = node_word_equal(b, wk);
      }
      else {    
        beqk = expr2bexpr_recur(enc, det_layer, 
                                find_node(EQUAL, b, 
                                          find_node(NUMBER, NODE_FROM_INT(k), Nil)),
                                context, in_next);
      }

      bit = find_node(CASE, find_node(COLON, beqk, 
                                      array_fetch(node_ptr, va, i-k)), 
                      bit);
    }
    
    res = cons(bit, res); /* msb at the top */
  }
  array_free(va);

  return node_word_create_from_list(res, wa);
}  


/**Function********************************************************************

  Synopsis [Creates the encoding of the right-shifting circuit for
  words]

  Description        []

  SideEffects        [wb is -1 if b is not a word (can be a number)]

  SeeAlso            []

******************************************************************************/
static node_ptr expr2bexpr_shift_right(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                       node_ptr context, boolean in_next, 
                                       node_ptr a, node_ptr b, node_ptr def_case, 
                                       int wb)
{ 
  /* creates shift circuit */
  array_t* va;
  node_ptr res;
  int maxw, wa, i;

  wa = node_get_int(cdr(a));
  va = node_word_to_array(a);
  if (wb > 0) maxw = MIN((2 << (wb-1)), wa);
  else maxw = wa;
  
  res = Nil;
  for (i=0; i<wa; ++i) {
    node_ptr bit = def_case;
    int k;
    
    for (k=maxw-1; k >= 0; --k) { 
      node_ptr beqk;

      if (k+i < wa) {
        /* a valid index */
        if (wb > 0) {
          node_ptr wk = node_word_create_from_integer(k, wb);
          beqk = node_word_equal(b, wk);
        }
        else {  
          beqk = expr2bexpr_recur(enc, det_layer, 
                                  find_node(EQUAL, b, 
                                            find_node(NUMBER, NODE_FROM_INT(k), Nil)), 
                                  context, in_next);
        }

        bit = find_node(CASE, 
                        find_node(COLON, beqk, array_fetch(node_ptr, va, i+k)),
                        bit);
      }
    } /* for k */
    
    res = cons(bit, res); /* msb at the top */
  } /* for i */

  array_free(va);

  return node_word_create_from_list(res, wa);
}  


/**Function********************************************************************

  Synopsis [Private service for shifting operations]

  Description [creates a default error case. wb is -1 if b is not
  a word.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr 
expr2bexpr_get_shift_def_value(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                               node_ptr context, boolean in_next, 
                               node_ptr a, node_ptr b, int wb)
{
  node_ptr error, bge0, blew, bge0_lew;
  int wa;

  wa = node_get_int(cdr(a));

  error = failure_make("Right operand of shift operator is out of range", 
                       FAILURE_UNSPECIFIED, node_get_lineno(get_the_node()));

  if (wb > 0) {
    bge0 = node_word_greater_equal(b, node_word_create_from_integer(0, wb));
    blew = (wb>=wa) ? node_word_less_equal(b, 
                                           node_word_create_from_integer(wa, wb)) 
      : find_node(TRUEEXP, Nil, Nil);

  }
  else {
    bge0 = expr2bexpr_recur(enc, det_layer, 
                            find_node(GE, b, 
                                      find_node(NUMBER, (node_ptr) 0, Nil)),
                            context, in_next);
    blew = expr2bexpr_recur(enc, det_layer, 
                            find_node(LE, b, 
                                      find_node(NUMBER, NODE_FROM_INT(wa), Nil)),
                            context, in_next); 
  }

  bge0_lew = find_node(AND, bge0, blew); 
  return find_node(CASE, 
                   find_node(COLON, bge0_lew, find_node(FALSEEXP, Nil, Nil)), 
                   error);
}


/**Function********************************************************************

  Synopsis           [High-level function for shifting operations]

  Description        [This function is called directly by the booleanizer]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr expr2bexpr_shift(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                 node_ptr expr, node_ptr context, 
                                 boolean in_next)
{
  TypeChecker_ptr tc;
  SymbType_ptr ta, tb;
  node_ptr a, b;
  int wa, wb;
  int etype = node_get_type(expr);

  nusmv_assert(etype == LSHIFT || etype == RSHIFT);

  tc = BddEnc_get_type_checker(enc);
  ta = TypeChecker_get_expression_type(tc, car(expr), context);

  if (ta == SYMB_TYPE(NULL) || SymbType_is_error(ta)) {
    internal_error("expr2bexpr_recur_unary: operand has invalid type");
  }

  nusmv_assert(SymbType_is_word(ta));
  a = expr2bexpr_recur(enc, det_layer, car(expr), context, in_next);
  _CHECK_WORD(a);
  wa = node_get_int(cdr(a));

  tb = TypeChecker_get_expression_type(tc, cdr(expr), context);
  if (SymbType_is_word(tb)) {
    b = expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next);
    _CHECK_WORD(b);
    wb = node_get_int(cdr(b));
  }
  else {
    b = cdr(expr);
    wb = -1;
  }

  {
    node_ptr def_case;
    node_ptr res;

    set_the_node(expr);
    def_case = expr2bexpr_get_shift_def_value(enc, det_layer, context, in_next, 
                                              a, b, wb);
    if (etype == LSHIFT) {
      res = expr2bexpr_shift_left(enc, det_layer, context, in_next, 
                                  a, b, def_case, wb);
    }
    else {
      res = expr2bexpr_shift_right(enc, det_layer, context, in_next, 
                                   a, b, def_case, wb);
    }

    return res;
  }
}


/**Function********************************************************************

  Synopsis           [High-level function for rotating words]

  Description        [This function is called directly by the booleanizer]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static node_ptr expr2bexpr_rotate(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                  node_ptr expr, node_ptr context, 
                                  boolean in_next)
{
  TypeChecker_ptr tc;
  SymbType_ptr ta, tb;
  node_ptr a, b;
  int wa, wb;
  int etype = node_get_type(expr);
  array_t* va;

  nusmv_assert(etype == LROTATE || etype == RROTATE);

  tc = BddEnc_get_type_checker(enc);
  ta = TypeChecker_get_expression_type(tc, car(expr), context);

  if (ta == SYMB_TYPE(NULL) || SymbType_is_error(ta)) {
    internal_error("expr2bexpr_recur_unary: operand has invalid type");
  }

  nusmv_assert(SymbType_is_word(ta));
  a = expr2bexpr_recur(enc, det_layer, car(expr), context, in_next);
  _CHECK_WORD(a);
  wa = node_get_int(cdr(a));
  va = node_word_to_array(a);

  tb = TypeChecker_get_expression_type(tc, cdr(expr), context);
  if (SymbType_is_word(tb)) {
    b = expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next);
    _CHECK_WORD(b);
    wb = node_get_int(cdr(b));
  }
  else {
    b = cdr(expr);
    wb = -1;
  }

  {
    node_ptr err_case;
    node_ptr res;
    int maxb, i;

    err_case = failure_make("Right operand of rotate operation is out of range",
                            FAILURE_UNSPECIFIED, (int) node_get_lineno(expr));
    
    if (wb > 0) maxb = MIN(((2 << (wb-1)) - 1), wa);
    else maxb = wa;

    res = Nil;
    /* performs the rotation */
    for (i=0; i < wa; ++i) {
      node_ptr bit = err_case;
      int k; 

      for (k=maxb; k >= 0; --k) {
        node_ptr beqk;
        int p;

        if (wb > 0) {
          node_ptr wk = node_word_create_from_integer(k, wb);
          beqk = node_word_equal(b, wk);
        }
        else { /* b is not a word */
          beqk = expr2bexpr_recur(enc, det_layer, 
                                  find_node(EQUAL, b, 
                                            find_node(NUMBER, NODE_FROM_INT(k), Nil)), 
                                  context, in_next);              
        }

        if (etype == LROTATE) {
          p = (i >= k) ? i-k : i-k+wa;
        } 
        else { /* RROTATE */
          p = ((i+k) % wa);
        }
        bit = find_node(CASE, 
                        find_node(COLON, beqk, array_fetch(node_ptr, va, p)), 
                        bit);                                   
      } /* for k */

      res = cons(bit, res); /* msb at the top */
    } /* for i */
    
    array_free(va);

    return node_word_create_from_list(res, wa);
  }
}


/**Function********************************************************************

  Synopsis           [Applied unary operand to booleanize depending 
  on the operand type. Operation is passed through 'op'. Operation can 
  be a closure W->W]

  Description []

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static node_ptr expr2bexpr_recur_unary(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                       node_ptr expr, node_ptr context, 
                                       boolean in_next,
                                       NPFN op)
{
  TypeChecker_ptr tc = BddEnc_get_type_checker(enc);
  SymbType_ptr ta;
  
  ta = TypeChecker_get_expression_type(tc, car(expr), context);

  if (ta == SYMB_TYPE(NULL) || SymbType_is_error(ta)) {
    internal_error("expr2bexpr_recur_unary: operand has invalid type");
  }

  /* if word, applies the given operation to its encoding */
  if (!opt_encode_word_monolithic(options) && SymbType_is_word(ta)) {
    node_ptr a = expr2bexpr_recur(enc, det_layer, car(expr), context, in_next);
    nusmv_assert(node_get_type(a) == WORD);     

    set_the_node(expr);
    return op(a);
  }
  
  /* not a leaf, applies the standard encoding */
  return find_node(node_get_type(expr),
                   expr2bexpr_recur(enc, det_layer, car(expr),
                                    context, in_next),
                   Nil); 
}


/**Function********************************************************************

  Synopsis           [Applied to binary operands to booleanize depending 
  on the operands types. Operation is passed through 'op'. Operation can 
  be a closure WxW->W or a predicate over words WxW->Bool]

  Description [If operands are not words and the expression is not a
  leaf, the expression is converted down to an ADD, and then back to a
  node_ptr to booleanize it.]

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
static node_ptr 
expr2bexpr_recur_binary(BddEnc_ptr enc, SymbLayer_ptr det_layer,
                        node_ptr expr, node_ptr context, 
                        boolean in_next,
                        NPFNN op)
{
  TypeChecker_ptr tc = BddEnc_get_type_checker(enc);
  SymbType_ptr ta, tb;
  node_ptr res;
  
  ta = TypeChecker_get_expression_type(tc, car(expr), context);
  tb = TypeChecker_get_expression_type(tc, cdr(expr), context);

  if (ta == SYMB_TYPE(NULL) || tb == SYMB_TYPE(NULL) || 
      SymbType_is_error(ta) || SymbType_is_error(tb)) {
    internal_error("expr2bexpr_recur_binary: operands have invalid types");
  }

  /* if words, applies the given operation to their encodings */
  if (!opt_encode_word_monolithic(options) && 
      SymbType_is_word(ta) && SymbType_is_word(tb)) {
    node_ptr a = expr2bexpr_recur(enc, det_layer, car(expr), context, in_next);
    node_ptr b = expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next);
    _CHECK_WORD(a);
    _CHECK_WORD(b);

    set_the_node(expr);
    return op(a,b);
  }

  /* not words, proceeds as usual. Some require to be booleanized
     through ADDs */
  switch (node_get_type(expr)) {
    /* leaves that require to be booleanization */  
  case EQUAL:
  case LT:
  case GT:
  case LE:
  case GE:      
    /* Signed predicates over scalar terms (guaranteed to return boolean) */
  case SLT:
  case SGT:
  case SLE:
  case SGE:      
    /* Predicates over possibly scalar terms (guaranteed to return boolean) */
    /* Function symbols over scalar terms Might return boolean, but
       check is needed Assumption: if boolean is returned, then the
       function is determinized by introducing a variable on the {0,1}
       leaves. */
  case PLUS:
  case MINUS:
  case TIMES:
  case DIVIDE:
  case MOD:
  case UNION:
  case SETIN:      
  case EQDEF:
    res = scalar_atom2bexpr(enc, det_layer, expr, context, in_next);
    break;

    /* !(a=b) is smaller than (a!=b) */
  case NOTEQUAL:     
    res = find_node(NOT, scalar_atom2bexpr(enc, det_layer, 
                                           find_node(EQUAL, car(expr), cdr(expr)), 
                                           context, in_next), Nil);
    break;
                    
  default:
    /* not a leaf */
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, car(expr), context, in_next), 
                    expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next));
  }
  
  return res;
}


/**Function********************************************************************

  Synopsis           [Creates an encoding for CASE node. If CASE evaluates to 
  a word, a WORD encoding is created.]

  Description        [Private sesrvice called by expr2bexpr_recur]

  SideEffects        [None]

  SeeAlso            [expr2bexpr_word_ite_aux]

******************************************************************************/
static node_ptr expr2bexpr_ite(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                               node_ptr expr, node_ptr context, 
                               boolean in_next)
{
  node_ptr res;

  nusmv_assert(node_get_type(expr) == CASE || 
               node_get_type(expr) == IFTHENELSE);

  if (!opt_encode_word_monolithic(options) && 
      SymbType_is_word(TypeChecker_get_expression_type(
                                                       BddEnc_get_type_checker(enc), expr, context))) {
    res = expr2bexpr_word_ite_aux(enc, det_layer, expr, context, in_next);

  }
  else { /* not a word */
    res = find_node(CASE,
                    find_node(COLON, 
                              expr2bexpr_recur(enc, det_layer, car(car(expr)),
                                               context, in_next),
                              expr2bexpr_recur(enc, det_layer, cdr(car(expr)),
                                               context, in_next)),
                    expr2bexpr_recur(enc, det_layer, cdr(expr), 
                                     context, in_next));
  }

  return res;
}

/**Function********************************************************************

 Synopsis [Service of expr2bexpr_word_ite, that creates the word  encoding]

 Description        [Creates the resulting WORD encoding as:
 <textarea>
                     WORD
                  /        \
                 /          NUMBER(size)
                /
             _____ CONS ________________
            /                           \ 
       _   ITE                         CONS
      |  /  |  \                      /    \
      |c1 t1.0  ITE                 ITE     ...
  bit0|        /  |  \            _ ...
  |          c2  t2.0 ITE       | 
  .           ...      |        |
  .                    .    bit1|
  |_                   .
  |_
</textarea>
Encoding complexity is N*C (N=word width, C=num of cases)]

SideEffects        []

SeeAlso            []

******************************************************************************/
static node_ptr 
expr2bexpr_word_ite_aux(BddEnc_ptr enc, SymbLayer_ptr det_layer,
                        node_ptr expr, node_ptr context, boolean in_next)
{
  node_ptr res;
  node_ptr bcase, iter, failure;
  int size = -1;

  /* extracts and booleanizes the case */
  bcase = Nil; /* a list of pairs <bcond, bthen as array> */
  failure = Nil; /* to hold possible FAILURE node */
  iter = expr;
  while (true) {
    node_ptr bcond = expr2bexpr_recur(enc, det_layer, car(car(iter)), 
                                      context, in_next);
    node_ptr bthen = expr2bexpr_recur(enc, det_layer, cdr(car(iter)), 
                                      context, in_next);
    _CHECK_WORD(bthen);

    /* all then nodes have the same width: */
    if (size == -1) size = node_word_get_width(bthen);
    else { nusmv_assert(node_word_get_width(bthen) == size); } 
    
    bcase = cons(find_node(COLON, bcond, (node_ptr) node_word_to_array(bthen)),
                 bcase);
    
    iter = cdr(iter);

    /* terminating conditions */
    if (node_get_type(iter) == FAILURE) { failure = iter; break; }
    if (node_get_type(iter) != CASE || node_get_type(iter) != IFTHENELSE) {
      /* non-standard last case */
      failure = expr2bexpr_recur(enc, det_layer, iter, context, in_next);
      break;
    }
  }
  
  { /* creates the encoding */

    int i;
    node_ptr wbits = Nil;

    for (i=0; i < size; ++i) {
      node_ptr cases = failure;
      for (iter=bcase; iter!=Nil; iter=cdr(iter)) {
        nusmv_assert(node_get_type(car(iter)) == COLON);
        node_ptr bcond = car(car(iter));
        node_ptr biti = array_fetch(node_ptr, (array_t*) cdr(car(iter)), i);
        
        cases = find_node(CASE, find_node(COLON, bcond, biti), cases);
      }
      wbits = cons(cases, wbits);
    }

    res = node_word_create_from_list(wbits, size);
  }

  /* frees the bcase structure */
  for (iter=bcase; iter!=Nil; iter=cdr(iter)) { 
    array_free((array_t*) cdr(car(iter))); 
  }

  return res;
}


/**Function********************************************************************

 Synopsis           [Converts a generic expression into a boolean expression.]

 Description        [Takes an expression intended to evaluate to boolean,
 maps through booleans down to the atomic scalar propositions,
 builds the corresponding boolean function,
 and returns the resulting boolean expression.

 The conversion of atomic scalar proposition is currently performed
 by generating the corresponding ADD, and then printing it in terms
 of binary variables.

 The introduction of determinization variables is allowed only if flag
 <tt>allow_nondet</tt> is set to true.

 The input expression may be normal (not flattened), flattened or
 expanded. Parameter 'context' is used if the expression is not flattened.]

 SideEffects        [None]

 SeeAlso            [Compile_expr2bexpr, detexpr2bexpr]

******************************************************************************/
static node_ptr expr2bexpr_recur(BddEnc_ptr enc, 
                                 SymbLayer_ptr det_layer, 
                                 node_ptr expr,
                                 node_ptr context,
                                 boolean in_next)
{
  SymbTable_ptr symb_table;
  TypeChecker_ptr tc;
  node_ptr res; 

  if (expr == Nil) return Nil;

  res = expr2bexpr_hash_lookup_entry(expr, context, in_next, 
                                     det_layer != SYMB_LAYER(NULL));
  if (res != (node_ptr) NULL) return res;

  symb_table = BaseEnc_get_symbol_table(BASE_ENC(enc));
  tc = BddEnc_get_type_checker(enc);

  switch (node_get_type(expr)) {

  case FAILURE:
  case TRUEEXP:
  case FALSEEXP:
    res = expr;
    break;

  case NUMBER: 
    if ((NODE_TO_INT(car(expr)) | 1) != 1) { /* not boolean value */
      rpterr("Number can not be casted to boolean (except 0 and 1)");
    }
    res = NODE_TO_INT(car(expr)) ? 
      find_node(TRUEEXP, Nil, Nil) : find_node(FALSEEXP, Nil, Nil);
    break;
    
  case NUMBER_WORD: 
    res = node_word_create_from_wordnumber(WORD_NUMBER(car(expr)));
    break;

  case NUMBER_FRAC: 
  case NUMBER_REAL:
  case NUMBER_EXP:
    rpterr("Real constant cannot be casted to boolean");

  case BIT:
    {
      node_ptr name = CompileFlatten_resolve_name(symb_table, expr, context);
      nusmv_assert(SymbTable_is_symbol_var(symb_table, name)); 
      
      res = name;
      break;
    }

  case DOT:
  case ATOM:
  case ARRAY: 
    {
      node_ptr name = CompileFlatten_resolve_name(symb_table, expr, context);
      node_ptr param = lookup_param_hash(name);

      /* no check for ambiguity is required, since type checker did it */

      /* parameter */
      if (param != (node_ptr) NULL) {
        /* process the value of the parameter */
        res = expr2bexpr_recur(enc, det_layer, param, context, in_next);
        break;
      }

      /* define */
      if (SymbTable_is_symbol_define(symb_table, name)) {
        /* define (rhs must be boolean, recur to check) */
        node_ptr body = SymbTable_get_define_flatten_body(symb_table, name);
        if (body == (node_ptr) NULL) error_undefined(name); 
        
        res = expr2bexpr_recur(enc, det_layer, body, context, in_next);
      }
      /* variable */
      else if (SymbTable_is_symbol_var(symb_table, name)) {
        SymbType_ptr vtype = SymbTable_get_var_type(symb_table, name);
        /* variable, must be boolean or word */
        if (SymbType_is_boolean_enum(vtype)) {
          if (in_next) res = find_node(NEXT, name, Nil);
          else res = name;        
        }
        else if (SymbType_is_word(vtype)) {
          BoolEnc_ptr benc = BoolEncClient_get_bool_enc(BOOL_ENC_CLIENT(enc));

          res = BoolEnc_get_var_encoding(benc, name);
          nusmv_assert(node_get_type(res) == WORD);
          if (in_next) res = node_word_apply_unary(res, NEXT);
        }
        else {
          rpterr("Unexpected non boolean variable");
        }           
      }

      else {
        /* unknow identifier. This code should be impossible since the 
           type checker already checked the expr for being correct
        */
        print_sexp(nusmv_stderr, expr);
        rpterr("Unexpected data structure"); 
      }
    }
    break;
    
  case CONS:
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, 
                                     car(expr), context, in_next),
                    expr2bexpr_recur(enc, det_layer,
                                     cdr(expr), context, in_next));
    break;

  case CAST_BOOL:
    res = expr2bexpr_recur_unary(enc, det_layer, expr, context, in_next, 
                                 node_word_cast_bool);
    break;

  case CAST_WORD1:
    {
      TypeChecker_ptr tc = BddEnc_get_type_checker(enc);
      node_ptr a = car(expr);

      nusmv_assert(SymbType_is_boolean_enum(
                                            TypeChecker_get_expression_type(tc, a, context)));

      a = expr2bexpr_recur(enc, det_layer, a, context, in_next);
      res = node_word_create(a, 1);
      break;
    }

  case LSHIFT:
  case RSHIFT:
    res = expr2bexpr_shift(enc, det_layer, expr, context, in_next);
    break;    
    
  case LROTATE:
  case RROTATE:    
    res = expr2bexpr_rotate(enc, det_layer, expr, context, in_next);
    break;    
    
  case CONCATENATION:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_concat);
    break;

  case BIT_SELECTION:
    {
      TypeChecker_ptr tc = BddEnc_get_type_checker(enc);
      SymbType_ptr ta;
      node_ptr a;
  
      ta = TypeChecker_get_expression_type(tc, car(expr), context);
      nusmv_assert(SymbType_is_word(ta));

      a = expr2bexpr_recur(enc, det_layer, car(expr), context, in_next);
      
      res = node_word_selection(a, cdr(expr));
      break;
    }
     
  case SIGN_EXTEND:
    rpterr("Sign extend cannot be casted to boolean");
     
  case NOT:
    res = expr2bexpr_recur_unary(enc, det_layer, expr, context, in_next, 
                                 node_word_not);
    break;

  case UMINUS:
    res = expr2bexpr_recur_unary(enc, det_layer, expr, context, in_next, 
                                 node_word_uminus);
    break;
  
  case AND:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_and);
    break;

  case OR:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_or);
    break;

  case XOR:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_xor);
    break;

  case XNOR:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_xnor);
    break;

  case IMPLIES:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_implies);
    break;

  case IFF:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_iff);
    break;

  case CASE:
    res = expr2bexpr_ite(enc, det_layer, expr, context, in_next);
    break;
    
  case NEXT: 
    nusmv_assert(!in_next);
    res = expr2bexpr_recur(enc, det_layer, car(expr), context, true);
    break;
    
  case EQDEF:
    {
      node_ptr var, var_name;
      node_ptr lhs = car(expr);
      node_ptr rhs = cdr(expr);

      switch (node_get_type(lhs)) {
      case SMALLINIT:
        var_name = car(lhs);
        var = car(lhs);
        break;
        
      case NEXT:
        var_name = car(lhs);
        var = lhs;
        break;
        
      default:
        var_name = lhs;
        var = lhs;
      }

      {
        node_ptr name = CompileFlatten_resolve_name(symb_table, var_name, 
                                                    context);

        if (!SymbTable_is_symbol_declared(symb_table, name)) {
          error_undefined(name);
        } 
        
        if (SymbTable_is_symbol_var(symb_table, name)) {
          if (SymbType_is_boolean_enum(
                                       SymbTable_get_var_type(symb_table, name))) {
            /* boolean variable, rhs will be determinized */
            if (node_get_type(lhs) == NEXT) var = find_node(NEXT, name, Nil);
            else var = name;
            
            res = find_node(IFF, var, 
                            expr2bexpr_recur(enc, det_layer, rhs, context, in_next));
          }
          else { /* scalar variable */                  
            node_ptr fixed_expr;
            nusmv_assert(!in_next);

            /* var here is unflatten left node, with SMALLINIT filtered out */
            fixed_expr = find_node(EQDEF, var, rhs);
            res = expr2bexpr_recur_binary(enc, det_layer, fixed_expr, context, 
                                          in_next, node_word_iff);
            break;
          }
        }
        else { rpterr("Unexpected data structure, variable was expected"); }
      }
      break;
    } /* end of case EQDEF */

  case EQUAL: 
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_equal);
    break;

    /* Predicates over possibly scalar terms (guaranteed to return boolean) */
  case NOTEQUAL:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_notequal);
    break;

  case PLUS:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_plus);
    break; 

  case MINUS:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_minus);
    break;
    
    /* Predicates over scalar terms (guaranteed to return boolean) */
  case LT:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_less);
    break;    
    
  case LE:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_less_equal);
    break;    

  case GT:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_greater);
    break;    

  case GE:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_greater_equal);
    break;    

    /* Signed predicates over scalar terms (guaranteed to return boolean) */
  case SLT:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_sign_less);
    break;    

  case SLE:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_sign_less_equal);
    break;    

  case SGT:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_sign_greater);
    break;    

  case SGE:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_sign_greater_equal);
    break;
  
    /* Function symbols over scalar terms Might return boolean, but
       check is needed Assumption: if boolean is returned, then the
       function is determinized by introducing a variable on the {0,1}
       leaves. */
  case TIMES:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_times);
    break;

  case DIVIDE:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_divide);
    break;

  case MOD:
    res = expr2bexpr_recur_binary(enc, det_layer, expr, context, in_next, 
                                  node_word_mod);
    break;

  case UNION:
  case SETIN: 
    res = scalar_atom2bexpr(enc, det_layer, expr, context, in_next);
    break;
  
    /* UNARY CTL/LTL OPERATORS */
  case EX:
  case AX:
  case EG:
  case AG:
  case EF:
  case AF:
  case OP_NEXT:
  case OP_PREC:
  case OP_NOTPRECNOT:
  case OP_FUTURE:
  case OP_ONCE:
  case OP_GLOBAL:
  case OP_HISTORICAL:
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, car(expr), context, in_next),
                    Nil);
    break;
  
    /* BINARY CTL/LTL OPERATORS */
  case EU:
  case AU:
  case MINU:
  case MAXU:
  case UNTIL:
  case RELEASES:
  case SINCE: 
  case TRIGGERED: 
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, car(expr), context, in_next),
                    expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next));
    break;
    
    /* BOUNDED TEMPORAL OPERATORS (cdr is range, no recursion needed) */
  case EBF:
  case ABF:
  case EBG:
  case ABG:
  case EBU:
  case ABU: 
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, car(expr), context, in_next),
                    cdr(expr));
    break;
  
  case CONTEXT:
    res = expr2bexpr_recur(enc, det_layer, cdr(expr), car(expr), in_next);
    break;

  case TWODOTS: 
    rpterr("Unexpected TWODOTS node");
    res = (node_ptr) NULL;
    break;

#if HAVE_MBP  /* MBP specific expressions. These expressions are converted
                 to boolean only when a boolean module is printed out */
  case MBP_SPEC_WRAPPER: /* skip the player name (left) and process
                            the expression (right child) */
    res = find_node(MBP_SPEC_WRAPPER,
                    car(expr),
                    expr2bexpr_recur(enc, det_layer, cdr(expr), context, in_next));
                    
    break;
                
    /* contain one or two lists. Process them the same way since Nil
       will be skipped */
  case MBP_EXP_LIST:
  case MBP_TWO_EXP_LISTS:
    res = find_node(node_get_type(expr),
                    expr2bexpr_recur(enc, det_layer, 
                                     car(expr), context, in_next),
                    expr2bexpr_recur(enc, det_layer,
                                     cdr(expr), context, in_next));

    break;
#endif /* HAVE_MBP */

  default: 
    internal_error("expr2bexpr_recur: type = %d\n", node_get_type(expr));
    res = (node_ptr) NULL;
  }

  /* updates the results hash  */
  if (res != (node_ptr) NULL) {
    expr2bexpr_hash_insert_entry(expr, context, res, in_next, 
                                 det_layer != SYMB_LAYER(NULL));
  }

  return res;
}


/**Function********************************************************************

  Synopsis [Converts an atomic expression into the corresponding
  (boolean) expression.]

  Description        [Takes an atomic expression and converts it into
  a corresponding boolean expression.

  The introduction of determinization variables is allowed only if the layer 
  <tt>det_layer</tt> is not NULL]
  
  SideEffects        [A new boolean variable might be introduced.]
  
  SeeAlso            []

******************************************************************************/
static node_ptr scalar_atom2bexpr(BddEnc_ptr enc, SymbLayer_ptr det_layer, 
                                  node_ptr expr, node_ptr context,
                                  boolean in_next)
{
  node_ptr res;
  int temp = yylineno;
  add_ptr bool_add = BddEnc_expr_to_add(enc, 
                                        (in_next) ? find_node(NEXT, expr, Nil) : expr, 
                                        context);

  yylineno = node_get_lineno(expr);
  res = BddEnc_add_to_expr(enc, bool_add, det_layer);
  add_free(BddEnc_get_dd_manager(enc), bool_add);
  yylineno = temp;

  return res;
}

