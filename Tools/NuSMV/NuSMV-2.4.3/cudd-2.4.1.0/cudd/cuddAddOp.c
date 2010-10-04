/**CFile***********************************************************************

  FileName    [cuddAddOp.c]

  PackageName [cudd]

  Synopsis    [Apply function for ADDs and some operations on ADD.]

  Description [Apply function for ADDs and some operations on ADD.]

  Author      [Marco Roveri]

  Copyright   [ Copyright (c) 1998 by ITC-IRST and Carnegie Mellon University.
  All Rights Reserved. This software is for educational purposes only.
  Permission is given to academic institutions to use, copy, and modify
  this software and its documentation provided that this introductory
  message is not removed, that this software and its documentation is
  used for the institutions' internal research and educational purposes,
  and that no monies are exchanged. No guarantee is expressed or implied
  by the distribution of this code.
  Send bug-reports and/or questions to: nusmv@irst.itc.it. ]

******************************************************************************/

#include    "util.h"
#include    "st.h"
#include    "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#ifndef lint
static char rcsid[] DD_UNUSED = "$Id: cuddAddOp.c,v 1.1.2.1.2.4 2007/03/25 22:03:15 nusmv Exp $";
#endif


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define CUDD_ELSE_CNST (CUDD_VALUE_TYPE)(-100)

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static CUDD_VALUE_TYPE Cudd_type_error (DdManager *, CUDD_VALUE_TYPE);
static CUDD_VALUE_TYPE Cudd_fatal_error (DdManager *, const char *);
static void cudd_addWalkLeavesRecur (void (*op)(CUDD_VALUE_TYPE), DdNode *, st_table *);
static int addCheckPositiveCube (DdManager *manager, DdNode *cube);
static DdNode * addAbstractRecur (DdManager * manager, CUDD_VALUE_TYPE (*op)(), DdNode * f, DdNode * cube);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Applies op to the corresponding discriminants of f and g.]

  Description [Applies op to the corresponding discriminants of f and g.
  Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addApply(
DdManager       *dd,
CUDD_VALUE_TYPE (*op)(),
DdNode          *f,
DdNode          *g)
{
    DdNode *res;

    do {
	dd->reordered = 0;
	res = cudd_addApplyRecur(dd,op,f,g);
    } while (dd->reordered == 1);
    return(res);

} /* end of Cudd_addApply */

/**Function********************************************************************

  Synopsis    [Applies AND to the corresponding discriminants of f and g.]

  Description [Applies logical AND to the corresponding discriminants
  of f and g. f and g must have only 0 or 1 as terminal nodes. Returns
  a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode * 
Cudd_addAnd(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode *res;

  do {
    dd->reordered = 0;
    res = cudd_addAndRecur(dd, f, g);
  } while (dd->reordered == 1);
  return(res);

}

/**Function********************************************************************

  Synopsis    [Applies AND to the corresponding discriminants of f and g.]

  Description [Applies logical AND to the corresponding discriminants
  of f and g and stores the result in f. f and g must have only 0 or 1
  as terminal nodes.]

  SideEffects [The result is stored in the first operand.]

  SeeAlso     [Cudd_addAnd]

******************************************************************************/
void
Cudd_addAccumulateAnd(
DdManager * dd,
DdNode **f,
DdNode *g)
{
  DdNode *res;

  res = Cudd_addAnd(dd,*f,g);
  if ( res != NULL) {
    cuddRef(res);
    Cudd_RecursiveDeref(dd,*f);
  }
  *f = res;
}

/**Function********************************************************************

  Synopsis    [Applies OR to the corresponding discriminants of f and g.]

  Description [Applies logica OR to the corresponding discriminants of
  f and g.  f and g must have only 0 or 1 as terminal nodes. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode * 
Cudd_addOr(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode *res;

  do {
    dd->reordered = 0;
    res = cudd_addOrRecur(dd,f,g);
  } while (dd->reordered == 1);
  return(res);

}

/**Function********************************************************************

  Synopsis    [Applies XOR to the corresponding discriminants of f and g.]

  Description [Applies logical XOR to the corresponding discriminants
  of f and g.  f and g must have only 0 or 1 as terminal
  nodes. Returns a pointer to the result if successful; NULL
  otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addXor(
DdManager *dd,
DdNode *f,
DdNode *g)
{
    DdNode *res;

    do {
	dd->reordered = 0;
	res = cudd_addXorRecur(dd,f,g);
    } while (dd->reordered == 1);
    return(res);

}

/**Function********************************************************************

  Synopsis    [Applies NOT to the corresponding discriminants of f.]

  Description [Applies logical NOT to the corresponding discriminants of f.
  f must have only 0 or 1 as terminal nodes. Returns a pointer to the
  result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode * 
Cudd_addNot(
DdManager * dd,
DdNode * f)
{
  DdNode * res;
  DdNode * one = DD_ONE(dd);

  do {
    dd->reordered = 0;
    res = cudd_addXorRecur(dd,one,f);
  } while (dd->reordered == 1);
  return(res);

}

/**Function********************************************************************

  Synopsis    [Existentially abstracts all the variables in cube from f.]

  Description [Abstracts all the variables in cube from f by applying
  a given function over all possible values taken by the variables. 
  Returns the abstracted ADD.]

  SideEffects [None]

  SeeAlso     [Cudd_addExistAbstract]

******************************************************************************/
DdNode *
Cudd_addAbstract(
  DdManager * manager,
  CUDD_VALUE_TYPE (*op)(),
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = addAbstractRecur(manager, op, f, cube);
    } while (manager->reordered == 1);
    return(res);

} /* end of Cudd_addAbstract */

/**Function********************************************************************

  Synopsis    [Applies IfThen to the corresponding discriminants of f and g.]

  Description [Applies IfThen to the corresponding discriminants of f and g.
  Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addIfThen(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode * res;

  do {
    dd->reordered = 0;
    res = cudd_addIfThenRecur(dd,f,g);
  } while (dd->reordered == 1);
  return(res);

}

/**Function********************************************************************

  Synopsis    [Applies Else to the corresponding discriminants of f and g.]

  Description [Applies Else to the corresponding discriminants of f and g.
  Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addElse(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode *res;

  do {
    dd->reordered = 0;
    res = cudd_addElseRecur(dd,f,g);
  } while (dd->reordered == 1);
  return(res);
}

/**Function********************************************************************

  Synopsis    [Applies IfThenElse to the corresponding discriminants of f,
  g and h.]

  Description [Applies IfThenElse to the corresponding discriminants of f,
  g and h. Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
Cudd_addIfThenElse(
DdManager *dd,
DdNode *f,
DdNode *g,
DdNode *h)
{
  DdNode *res_then, *res;

  do {
    dd->reordered = 0;
    res_then = cudd_addIfThenRecur(dd,f,g);
    if(res_then == NULL) return(NULL);
    cuddRef(res_then);
    res = cudd_addElseRecur(dd,res_then,g);
    if(res == NULL) {
      Cudd_RecursiveDeref(dd,res_then);
      return(NULL);
    }
    cuddDeref(res_then);
  } while (dd->reordered == 1);
  return(res);
}    

/**Function********************************************************************

  Synopsis    [Computes the difference between two sets.]

  Description [Computes the set difference between two sets.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/

DdNode *
Cudd_addCubeDiff(
DdManager * dd,
DdNode * a,
DdNode * b)
{
  DdNode * res;

  do {
    dd->reordered = 0;
    res = cudd_addCubeDiffRecur(dd,a,b);
  } while (dd->reordered == 1);
  return(res);
}

/**Function********************************************************************

  Synopsis    [Applies a generic function to constant nodes.]

  Description [Applies a generic function (void)(*op)(CUDD_VALUE_TYPE) to
  constants nodes of f.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/

void 
Cudd_addWalkLeaves(
void (*op)(CUDD_VALUE_TYPE),
DdNode * f)
{
  st_table *tmp_table;

  tmp_table = st_init_table(st_ptrcmp,st_ptrhash);
  if (tmp_table == NULL) {
    fprintf(stderr,"Cudd_addWalkLeaves: tmp_table == NULL\n");
    exit(1);
  }
  cudd_addWalkLeavesRecur(op,f,tmp_table);
  st_free_table(tmp_table);
}

static void
cudd_addWalkLeavesRecur(
void (*op)(CUDD_VALUE_TYPE),
DdNode * f,
st_table * tmp_table)
{
  long mark;

  if (st_lookup(tmp_table, (char *)f, (char **)&mark)) return;
  if (st_insert(tmp_table, (char *)f, (char *)1) == ST_OUT_OF_MEM) {
    fprintf(stderr,"cudd_addWalkLeavesRecur: ST_OUT_OF_MEM\n");
    exit(1);
  }
  if (cuddIsConstant(f)) op((CUDD_VALUE_TYPE)cuddV(f));
  else {
    cudd_addWalkLeavesRecur(op, (DdNode *)cuddT(f), tmp_table); 
    cudd_addWalkLeavesRecur(op, (DdNode *)cuddE(f), tmp_table);
  }
}

/**Function********************************************************************

  Synopsis    [Given an ADD, this function extracts its "constant" value.]

  Description [Given an ADD, this function extracts its "constant" value.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
CUDD_VALUE_TYPE
Cudd_add_value(
DdNode * fn)
{
  CUDD_VALUE_TYPE res;

  if (cuddIsConstant(fn)) return((CUDD_VALUE_TYPE)cuddV(fn));
  res = Cudd_add_value(cuddE(fn));
  if (res == CUDD_ELSE_CNST)
    res = Cudd_add_value(cuddT(fn));
  return(res);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addApply.]

  Description [Performs the recursive step of Cudd_addApply. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cudd_addApplyRecur(
DdManager *dd,
CUDD_VALUE_TYPE (*op)(),
DdNode *f,
DdNode *g)
{
    DdNode *res,
	   *fv, *fvn, *gv, *gvn,
	   *T, *E;
    unsigned int ford, gord;
    unsigned int index;

    /*
      Check for terminals. If it's the case then "op" is applied to
      the operands f and gmay swap f and g.
    */
    if(cuddIsConstant(f) && cuddIsConstant(g)){
      CUDD_VALUE_TYPE res_n;

      res_n = (*op)(cuddV(f),cuddV(g));
      if (res_n == NULL) return(NULL);
      return(cuddUniqueConst(dd,res_n));
    }

    /* Check cache */
    res = cuddCacheLookup2(dd,(DdNode * (*)())op,f,g);
    if (res != NULL) return(res);

    /* Recursive Step */
    ford = cuddI(dd,f->index);
    gord = cuddI(dd,g->index);
    if (ford <= gord) {
	index = f->index;
	fv = cuddT(f);
	fvn = cuddE(f);
    } else {
	index = g->index;
	fv = fvn = f;
    }
    if (gord <= ford) {
	gv = cuddT(g);
	gvn = cuddE(g);
    } else {
	gv = gvn = g;
    }

    T = cudd_addApplyRecur(dd,op,fv,gv);
    if (T == NULL) return(NULL);
    cuddRef(T);

    E = cudd_addApplyRecur(dd,op,fvn,gvn);
    if (E == NULL) {
	Cudd_RecursiveDeref(dd,T);
	return(NULL);
    }
    cuddRef(E);

    /* Necessary to have ROBDD */
    res = (T == E) ? T : cuddUniqueInter(dd,(int)index,T,E);
    if (res == NULL) {
	Cudd_RecursiveDeref(dd, T);
	Cudd_RecursiveDeref(dd, E);
	return(NULL);
    }
    cuddDeref(T);
    cuddDeref(E);

    /* Store result */
    cuddCacheInsert2(dd,(DdNode * (*)())op,f,g,res);

    return(res);

} /* end of cudd_addApplyRecur */

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addAnd.]

  Description [Performs the recursive step of Cudd_addAnd. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cudd_addAndRecur(
DdManager *manager,
DdNode    *f,
DdNode    *g)
{
    DdNode *fv, *fnv, *gv, *gnv;
    DdNode *one, *zero, *r, *t, *e;
    unsigned int topf, topg, index;

    one  = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Terminal cases. */
    if (f == zero || g == zero) return(zero);
    if (f == g || g == one) return(f);
    if (f == one) return(g);

    HANDLE_FAILURE(manager, f); HANDLE_FAILURE(manager, g);

    if (cuddIsConstant(f)) Cudd_type_error(manager, cuddV(f));
    if (cuddIsConstant(g)) Cudd_type_error(manager, cuddV(g));
    
    /* At this point f and g are not constant. */
    if (f < g) { /* Try to increase cache efficiency. */
	DdNode *tmp = f;
	f = g;
	g = tmp;
    }

    /* Check cache. */
    r = cuddCacheLookup2(manager, Cudd_addAnd, f, g);
    if (r != NULL) return(r);

    topf = manager->perm[f->index];
    topg = manager->perm[g->index];

    /* Compute cofactors. */
    if (topf <= topg) {
	index = f->index;
	fv = cuddT(f);
	fnv = cuddE(f);
    } else {
	index = g->index;
	fv = fnv = f;
    }

    if (topg <= topf) {
	gv = cuddT(g);
	gnv = cuddE(g);
    } else {
	gv = gnv = g;
    }

    t = cudd_addAndRecur(manager, fv, gv);
    if (t == NULL) return(NULL);
    cuddRef(t);

    e = cudd_addAndRecur(manager, fnv, gnv);
    if (e == NULL) {
	Cudd_RecursiveDeref(manager, t);
	return(NULL);
    }
    cuddRef(e);

    if (t == e) { /* Necessary to have ROBDD */
	r = t;
    } else {
      r = cuddUniqueInter(manager,(int)index,t,e);
      if (r == NULL) {
        Cudd_RecursiveDeref(manager, t);
        Cudd_RecursiveDeref(manager, e);
        return(NULL);
      }
    }
    cuddDeref(e);
    cuddDeref(t);
    cuddCacheInsert2(manager, Cudd_addAnd, f, g, r);
    return(r);
}


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addOr.]

  Description [Performs the recursive step of Cudd_addOr. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cudd_addOrRecur(
DdManager *manager,
DdNode    *f,
DdNode    *g)
{
    DdNode *fv, *fnv, *gv, *gnv;
    DdNode *one, *zero, *r, *t, *e;
    unsigned int topf, topg, index;

    one  = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Terminal cases. */
    if (f == one || g == one) return(one);
    if (f == g || g == zero) return(f);
    if (f == zero) return(g);

    HANDLE_FAILURE(manager, f); HANDLE_FAILURE(manager, g);

    if (cuddIsConstant(f)) Cudd_type_error(manager, cuddV(f));
    if (cuddIsConstant(g)) Cudd_type_error(manager, cuddV(g));
    
    /* At this point f and g are not constant. */
    if (f < g) { /* Try to increase cache efficiency. */
	DdNode *tmp = f;
	f = g;
	g = tmp;
    }

    /* Check cache. */
    r = cuddCacheLookup2(manager, Cudd_addOr, f, g);
    if (r != NULL) return(r);

    /* Here we can skip the use of cuddI, because the operands are known
    ** to be non-constant.
    */
    topf = manager->perm[f->index];
    topg = manager->perm[g->index];

    /* Compute cofactors. */
    if (topf <= topg) {
	index = f->index;
	fv = cuddT(f);
	fnv = cuddE(f);
    } else {
	index = g->index;
	fv = fnv = f;
    }

    if (topg <= topf) {
	gv = cuddT(g);
	gnv = cuddE(g);
    } else {
	gv = gnv = g;
    }

    t = cudd_addOrRecur(manager, fv, gv);
    if (t == NULL) return(NULL);
    cuddRef(t);

    e = cudd_addOrRecur(manager, fnv, gnv);
    if (e == NULL) {
	Cudd_RecursiveDeref(manager, t);
	return(NULL);
    }
    cuddRef(e);

    if (t == e) {  /* Necessary to have ROBDD */
	r = t;
    } else {
      r = cuddUniqueInter(manager,(int)index,t,e);
      if (r == NULL) {
        Cudd_RecursiveDeref(manager, t);
        Cudd_RecursiveDeref(manager, e);
        return(NULL);
      }
    }
    cuddDeref(e);
    cuddDeref(t);
    cuddCacheInsert2(manager, Cudd_addOr, f, g, r);
    return(r);
}

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addXor.]

  Description [Performs the recursive step of Cudd_addXor. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cudd_addXorRecur(
DdManager *manager,
DdNode    *f,
DdNode    *g)
{
    DdNode *fv, *fnv, *gv, *gnv;
    DdNode *one, *zero, *r, *t, *e;
    unsigned int topf, topg, index;

    one  = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Terminal cases. */
    if ((f == one) && (g == one)) return(zero);
    if (f == zero) return(g);
    if (g == zero) return(f);
    if (f == g) return(zero);

    HANDLE_FAILURE(manager, f); HANDLE_FAILURE(manager, g);

    if (f < g) { /* Try to increase cache efficiency and simplify tests. */
	DdNode *tmp = f;
	f = g;
	g = tmp;
    }

    /* Check cache. */
    r = cuddCacheLookup2(manager, Cudd_addXor, f, g);
    if (r != NULL) return(r);

    /* Here we can skip the use of cuddI, because the operands are known
    ** to be non-constant.
    */
    topf = cuddI(manager, f->index);
    topg = cuddI(manager, g->index);

    /* Compute cofactors. */
    if (topf == topg){
      if (cuddIsConstant(f) && (f != one) && (f != zero))
        Cudd_type_error(manager, cuddV(f));
      if (cuddIsConstant(g) && (g != one) && (g != zero))
        Cudd_type_error(manager, cuddV(g));
      index = f->index;
      fv  = cuddT(f);
      fnv = cuddE(f);
      gv  = cuddT(g);
      gnv = cuddE(g);
    } else {
      if (topf < topg) {
        if (cuddIsConstant(f) && (f != one) && (f != zero))
          Cudd_type_error(manager, cuddV(f));
	index = f->index;
	fv = cuddT(f);
	fnv = cuddE(f);
	gv = gnv = g;
      } else { /* topf > topg */
        if (cuddIsConstant(g) && (g != one) && (g != zero))
          Cudd_type_error(manager, cuddV(g));
	index = g->index;
	fv = fnv = f;
	gv = cuddT(g);
	gnv = cuddE(g);
      }
    }

    t = cudd_addXorRecur(manager, fv, gv);
    if (t == NULL) return(NULL);
    cuddRef(t);

    e = cudd_addXorRecur(manager, fnv, gnv);
    if (e == NULL) {
	Cudd_RecursiveDeref(manager, t);
	return(NULL);
    }
    cuddRef(e);

    if (t == e) { /* Necessary to have RODD. */
	r = t;
    } else {
      r = cuddUniqueInter(manager,(int)index,t,e);
      if (r == NULL) {
        Cudd_RecursiveDeref(manager, t);
        Cudd_RecursiveDeref(manager, e);
        return(NULL);
      }
    }
    cuddDeref(e);
    cuddDeref(t);
    cuddCacheInsert2(manager, Cudd_addXor, f, g, r);
    return(r);

}

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addIfThen.]

  Description [Performs the recursive step of Cudd_addIfThen. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/

DdNode *
cudd_addIfThenRecur(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode       *one, *zero, *fv, *fnv, *gv, *gnv;
  DdNode       *e, *t, *res;
  unsigned int topf, topg;
  int          index;

  one =  DD_ONE(dd);
  zero = DD_ZERO(dd);

  if( f == zero ) return(cuddUniqueConst(dd, CUDD_ELSE_CNST));
  if( f == one  ) return(g);

  HANDLE_FAILURE(dd, f); HANDLE_FAILURE(dd, g);
        
  if( cuddIsConstant(f) ) Cudd_type_error(dd, cuddV(f)); 

  /* Check cache */
  res = cuddCacheLookup2(dd,Cudd_addIfThen,f,g);
  if (res != NULL) return(res);
  
  topf = cuddI(dd,f->index);
  topg = cuddI(dd,g->index);

  if(topf == topg) {
    index = f->index;
    fv  = cuddT(f);
    fnv = cuddE(f);
    gv  = cuddT(g);
    gnv = cuddE(g);
  } else {
    if (topf < topg) {
      index = f->index;
      fv  = cuddT(f);
      fnv = cuddE(f);
      gv  = gnv = g;
    } else { /* topf > topg */
      index = g->index;
      fv  = fnv = f;
      gv  = cuddT(g);
      gnv = cuddE(g);
    }
  }

  t = cudd_addIfThenRecur(dd,fv,gv);
  if (t == NULL) return(NULL);
  cuddRef(t);

  e = cudd_addIfThenRecur(dd,fnv,gnv);
  if (e == NULL) {
    Cudd_RecursiveDeref(dd,t);
    return(NULL);
  }
  cuddRef(e);

  res = (t == e) ? t : cuddUniqueInter(dd,index,t,e);
  if (res == NULL) {
    Cudd_RecursiveDeref(dd,t);
    Cudd_RecursiveDeref(dd,e);
    return(NULL);
  }
  cuddDeref(t);
  cuddDeref(e);

  /* Store result */
  cuddCacheInsert2(dd,Cudd_addIfThen,f,g,res);

  return(res);

}

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addElse.]

  Description [Performs the recursive step of Cudd_addElse. Returns a
  pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cudd_addElseRecur(
DdManager *dd,
DdNode    *f,
DdNode    *g)
{
  DdNode       *fv, *fnv, *gv, *gnv;
  DdNode       *t, *e, *res;
  unsigned int topf, topg;
  int          index;

  if (cuddIsConstant(f)) return(((cuddV(f) != CUDD_ELSE_CNST) ? f : g));

  res = cuddCacheLookup2(dd,Cudd_addElse,f,g);
  if (res != NULL) return(res);

  topf = cuddI(dd,f->index);
  topg = cuddI(dd,g->index);
  if (topf == topg){
    index = f->index;
    fv  = cuddT(f);
    fnv = cuddE(f);
    gv  = cuddT(g);
    gnv = cuddE(g);
  } else {
    if (topf < topg) {
      index = f->index;
      fv  = cuddT(f);
      fnv = cuddE(f);
      gv  = gnv = g;
    } else { /* topf > topg */
      index = g->index;
      fv  = fnv = f;
      gv  = cuddT(g);
      gnv = cuddE(g);
    }
  }

  t = cudd_addElseRecur(dd,fv,gv);
  if (t == NULL) return(NULL);
  cuddRef(t);

  e = cudd_addElseRecur(dd,fnv,gnv);
  if (e == NULL) {
    Cudd_RecursiveDeref(dd,t);
    return(NULL);
  }
  cuddRef(e);

  res = (t == e) ? t : cuddUniqueInter(dd,index,t,e);
  if (res == NULL) {
    Cudd_RecursiveDeref(dd,t);
    Cudd_RecursiveDeref(dd,e);
    return(NULL);
  }
  cuddDeref(t);
  cuddDeref(e);

  /* Store result */
  cuddCacheInsert2(dd,Cudd_addElse,f,g,res);

  return(res);

}

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addCubeDiff.]

  Description [Performs the recursive step of Cudd_addCubeDiff.
  Returns a pointer to the result if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/

DdNode *
cudd_addCubeDiffRecur(
DdManager * dd,
DdNode    * f,
DdNode    * g)
{
  DdNode       * t,  * e , * res;
  DdNode       * tf, * tg;
  unsigned int topf, topg;
  DdNode       * zero = DD_ZERO(dd);
  DdNode       * one  = DD_ONE(dd);

  if ((f == zero) || (g == zero))
    Cudd_fatal_error(dd, "cudd_addCubeDiff: f == ZERO || g == ZERO");
  if(f == one) return(f);

  topf = cuddI(dd,f->index);
  topg = cuddI(dd,g->index);

  if (topf < topg) {
    e = zero;
    cuddRef(e);
    tf = cuddT(f);
    t = cudd_addCubeDiffRecur(dd, tf, g);
    if (t == NULL) {
      cuddDeref(e);
      return(NULL);
    }
    cuddRef(t);
    res = (t == e) ? t : cuddUniqueInter(dd,f->index, t, e);
    if (res == NULL) {
      cuddDeref(e);
      Cudd_RecursiveDeref(dd, t);
      return(NULL);
    }
    cuddDeref(t);
    cuddDeref(e);
    return(res);
  }
  else if (topf == topg) {
    tf = cuddT(f);
    tg = cuddT(g);
    res = cudd_addCubeDiffRecur(dd, tf, tg);
    return(res);
  }
  else {
    tg = cuddT(g);
    res = cudd_addCubeDiffRecur(dd, f, tg);
    return(res);
  }
}



/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
static CUDD_VALUE_TYPE Cudd_type_error(DdManager *dd, CUDD_VALUE_TYPE f)
{
  start_parsing_err();
  fprintf(dd->err,"\ntype error: value = ");
  print_node(dd->err, f);
  fprintf(dd->err,"\nExpected a boolean expression\n");
  finish_parsing_err();
  return(NULL);
}

static CUDD_VALUE_TYPE Cudd_fatal_error(DdManager *dd, const char * message)
{
  start_parsing_err();
  fprintf(dd->err,"\nFatal error: %s\n", message);
  finish_parsing_err();
  return(NULL);
}

/**Function********************************************************************

  Synopsis    [Checks whether cube is an ADD representing the product
  of positive literals.]

  Description [Checks whether cube is an ADD representing the product of
  positive literals. Returns 1 in case of success; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
addCheckPositiveCube(
  DdManager * manager,
  DdNode * cube)
{
    if (Cudd_IsComplement(cube)) return(0);
    if (cube == DD_ONE(manager)) return(1);
    if (cuddIsConstant(cube)) return(0);
    if (cuddE(cube) == DD_ZERO(manager)) {
        return(addCheckPositiveCube(manager, cuddT(cube)));
    }
    return(0);

} /* end of addCheckPositiveCube */

/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addAbstract.]

  Description [Performs the recursive step of Cudd_addAbstract.
  Returns the ADD obtained by abstracting the variables of cube from f,
  if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
addAbstractRecur(
  DdManager * manager,
  CUDD_VALUE_TYPE (*op)(),
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;

    statLine(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point. */	
    if (f == zero || cuddIsConstant(cube)) {  
        return(f);
    }

    /* Abstract a variable that does not appear in f */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = addAbstractRecur(manager, op, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cudd_addApplyRecur(manager,op,res1,res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup2(manager, (DdNode * (*)())(op+1), f, cube)) != NULL) {
	return(res);
    }

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = addAbstractRecur(manager, op, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = addAbstractRecur(manager, op, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cudd_addApplyRecur(manager, op, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, (DdNode * (*)())(op+1), f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = addAbstractRecur(manager, op, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = addAbstractRecur(manager, op, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, (DdNode * (*)())(op+1), f, cube, res);
        return(res);
    }	    

} /* end of addAbstractRecur */
