#include <stdio.h>
#include <malloc.h>
#include "linfunc.h"
#include <assert.h>

/* counter for the total number of active references
   - for debugging purposes */
int lf_refs = 0;

/* Operations on extended integers ---------------------------------- */

/*
static int uminus(int a) {
  if (a == INT_TOP) return(INT_TOP);
  if (a == INT_BOTTOM) return(INT_BOTTOM);
  return(-a);
}
*/

static int plus(int a, int b) {
  if (a == INT_TOP || b == INT_TOP) return(INT_TOP);
  if (a == INT_BOTTOM || b == INT_BOTTOM) return(INT_BOTTOM);
  return(a+b);
}

static int minus(int a, int b) {
  if (a == INT_TOP || b == INT_TOP) return(INT_TOP);
  if (a == INT_BOTTOM || b == INT_BOTTOM) return(INT_BOTTOM);
  return(a-b);
}

static int times(int a, int b) {
  if (a == INT_TOP || b == INT_TOP) return(INT_TOP);
  if (a == INT_BOTTOM || b == INT_BOTTOM) return(INT_BOTTOM);
  return(a*b);
}

static int meet(int a, int b) {
  if (a == b) return(a);
  if (a == INT_TOP) return(b);
  if (b == INT_TOP) return(a);
  return(INT_BOTTOM);
}

/* LINFUNC abstract datatype ---------------------------------------------- */

#define RC_MAX INT_MAX
LINFUNC linfunc_id;
LINFUNC linfunc_top;
LINFUNC linfunc_bottom;

void linfunc_init() {
  linfunc_id = (LINFUNC)malloc(sizeof(struct linfunc));
  linfunc_id->a = 1;
  linfunc_id->b = 0;
  linfunc_id->c = INT_TOP;
  linfunc_id->rc = RC_MAX;

  linfunc_top = (LINFUNC)malloc(sizeof(struct linfunc));
  linfunc_top->a = INT_TOP;
  linfunc_top->b = INT_TOP;
  linfunc_top->c = INT_TOP;
  linfunc_top->rc = RC_MAX;

  linfunc_bottom = (LINFUNC)malloc(sizeof(struct linfunc));
  linfunc_bottom->a = 1;
  linfunc_bottom->b = 0;
  linfunc_bottom->c = INT_BOTTOM;
  linfunc_bottom->rc = RC_MAX;

  return;
}

void printLFValue(void *a) {
  LINFUNC x = (LINFUNC)a;
  if ( (x->a == 1) && (x->b == 0) && (x->c == INT_TOP) )
    printf("ID");
  else if ( (x->a == INT_TOP) && (x->b == INT_TOP) && (x->c == INT_TOP) )
    printf("TOP");
  else if ( (x->a == 1) && (x->b == 0) && (x->c == INT_BOTTOM) )
    printf("BOTTOM");
  else if ( (x->a == 0) && (x->c == INT_TOP) )
    printf("<%d>", x->b);
  else if ((x->a == 1) && (x->c == INT_TOP) )
    printf("<\\x.x + %d>", x->b);
  else if (x->c == INT_TOP)
    printf("<\\x.%d*x + %d>", x->a, x->b);
  else 
    printf("<(\\x.%d*x + %d) /\\ %d>", x->a, x->b, x->c);
}

void *mkLFValue(int a, int b, int c) {
  LINFUNC val;

  assert(a != INT_TOP);
  assert(b != INT_TOP);
  assert(a != INT_BOTTOM);
  assert(b != INT_BOTTOM);

  if (c == INT_BOTTOM)
    return(linfunc_bottom);
  if (a == 1 && b == 0 && c == INT_TOP)
    return(linfunc_id);
  if (a == 0 && c != INT_TOP && b != c)
    return(linfunc_bottom);
  val = (LINFUNC)malloc(sizeof(struct linfunc));
  val->a = a;
  val->b = b;
  val->c = c;
  val->rc = 0;
  if (a == 0 && b == c)
    val->c = INT_TOP;
  return((void *)val);
}

void *mkLFValueRC1(int a, int b, int c) {
  void *temp = mkLFValue(a, b, c);
  linfunc_rcincr(temp);
  return(temp);
}

/*
 * linfunc_extender(a,b)
 *
 * Return b compose a. [The order is important.]
 */
void *linfunc_extender (void *a, void *b) {
  LINFUNC x = (LINFUNC)a;
  LINFUNC y = (LINFUNC)b;
  int temp_a;
  int temp_b;
  int temp_c;

  if (a == linfunc_top || b == linfunc_top)
{
lf_refs++;
    return(linfunc_top);
}
  temp_a = times(y->a, x->a);
  temp_b = plus(times(y->a, x->b), y->b);
  temp_c = meet(plus(times(y->a, x->c), y->b), y->c);
  return(mkLFValueRC1(temp_a, temp_b, temp_c));
} /* end of linfunc_extender **/

/*
 * linfunc_combiner(a,b)
 */
void *linfunc_combiner (void *a, void *b) {
  LINFUNC x = (LINFUNC)a;
  LINFUNC y = (LINFUNC)b;
  int temp_a, temp_b, temp_c;
  int numerator, denominator;

  if (a == linfunc_top) {
    linfunc_rcincr(b);
    return(b);
  }
  if (b == linfunc_top) {
    linfunc_rcincr(a);
    return(a);
  }
  if ( (x->a == y->a) && (x->b == y->b) ) {
    temp_a = x->a;
    temp_b = x->b;
    temp_c = meet(x->c, y->c);
    return(mkLFValueRC1(temp_a, temp_b, temp_c));
  }
  numerator = minus(x->b, y->b);
  denominator = minus(y->a, x->a);
  if (  numerator == INT_TOP
     || denominator == INT_TOP
     || numerator == INT_BOTTOM
     || denominator == INT_BOTTOM
     || denominator == 0
     || ((numerator/denominator)*denominator != numerator)
     ) {
lf_refs++;
    return((void *)linfunc_bottom);
  }

  if (x->a == 0 && y->a != 0) { /* If x is a constant function and y is not */
    temp_a = y->a;
    temp_b = y->b;
    temp_c = x->b;  /* Use x's constant */
  }
  else if (y->a == 0 && x->a != 0) { /* If y is a constant function and x is not */
    temp_a = x->a;
    temp_b = x->b;
    temp_c = y->b;  /* Use y's constant */
  }
  else {  /* Neither x nor y is a constant function */
    temp_a = (x->a <= y->a) ? x->a : y->a;
    temp_b = (x->a <= y->a) ? x->b : y->b;
    temp_c = meet(temp_a * (numerator/denominator) + temp_b, meet(x->c, y->c));
  }
  return(mkLFValueRC1(temp_a, temp_b, temp_c));
} /** end of linfunc_combiner **/

/*
 * linfunc_eq(a,b)
 */
int linfunc_eq (void *a, void *b) { 
  LINFUNC x = (LINFUNC)a;
  LINFUNC y = (LINFUNC)b;

  if (x->c == INT_BOTTOM || y->c == INT_BOTTOM)
    return (x->c == INT_BOTTOM && y->c == INT_BOTTOM);
  if (x->a == y->a && x->b == y->b && x->c == y->c) return (1);
  if (x->a == 0 || y->a == 0) return(0); /* A constant function can only be equal to */
                                         /* an identical triple */
  if (x->c != INT_TOP && y->c != INT_TOP) { /* Special case for non-identical one-point functions */
    int numerator = minus(x->b, y->b);
    int denominator = minus(y->a, x->a);
    if ( denominator == 0
      || ((numerator/denominator)*denominator != numerator)
       ) {
      return(0);
    }
    else
      return(1);
  }
  else
    return(0);
} /** end of linfunc_eq **/

/*
 * linfunc_diff(a,b)
 *
 * Return the difference between a and b.
 *
 * The return value r has two properties:
 *  o r >= a
 *    (i.e., linfunc_eq(linfunc_combiner(a,r), a) == true)
 *  o b combine r = b combine a
 *    (i.e., linfunc_eq(linfunc_combiner(b,r), linfunc_combiner(b,a)) == true)
 */
void *linfunc_diff (void *a, void *b) {
  LINFUNC x;
  LINFUNC y;
  LINFUNC m;

  x = (LINFUNC)a;
  y = (LINFUNC)b;
  m = linfunc_combiner(x,y);

  if (linfunc_eq(m, y)) { /* y <= x */
    linfunc_rcdecr(m);
lf_refs++;
    return((void *)linfunc_top);
  }
  else {
    linfunc_rcdecr(m);
    linfunc_rcincr(x);
    return((void *)x);
  }
} /** end of linfunc_diff **/

/* Identity element for extender operation */
void *linfunc_one() {
lf_refs++;
  return((void *)linfunc_id);
} /** end of linfunc_one **/

/* Identity element for extender operation */
void *linfunc_zero() {
lf_refs++;
  return((void *)linfunc_top);
} /** end of linfunc_zero **/

/* Reference counting with saturation */
void linfunc_rcincr(void *a)
{
  LINFUNC d = (LINFUNC)a;

lf_refs++;
  if (d->rc != RC_MAX) {
    (d->rc)++;
  }
}

void linfunc_rcdecr(void *a)
{
  LINFUNC d = (LINFUNC)a;

lf_refs--;
  if (d->rc != RC_MAX) {
    (d->rc)--;
    if (d->rc == 0) free(d);
  }
}

void linfunc_stats () {
	printf("refs: %d\n",lf_refs);
}
