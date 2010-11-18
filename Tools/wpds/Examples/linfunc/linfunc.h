#ifndef LINFUNC_H
#define LINFUNC_H

#include <limits.h>

#define INT_TOP INT_MAX
#define INT_BOTTOM INT_MIN

/* Semiring of "linear functions" -- really linear functions meet constant functions */

typedef struct linfunc {
  int a, b, c;     /* represents lambda x . (a*x + b) meet c */
  int rc;
} *LINFUNC;

extern LINFUNC linfunc_id;
extern LINFUNC linfunc_top;
extern LINFUNC linfunc_bottom;
extern int linfunc_hash(LINFUNC key);
extern void linfunc_init();
extern void printLFValue(void *a);
extern void *mkLFValue(int a, int b, int c);
extern void *linfunc_extender (void *a, void *b);  /*  Return b compose a. [The order is important.] */
extern void *linfunc_combiner (void *a, void *b);
extern int linfunc_eq (void *a, void *b);
extern void *linfunc_diff (void *a, void *b);
extern void *linfunc_one();  /* Identity element for extender operation */
extern void *linfunc_zero(); /* Identity element for combiner operation */
extern void linfunc_rcincr(void *a);
extern void linfunc_rcdecr(void *a);
extern void linfunc_stats();

#endif
