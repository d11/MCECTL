/*****************************************************************************/
/* Linear constant propagation example: normal pre*                          */
/*****************************************************************************/

/*****************************************************************************
   Examples taken from:
   T. Reps, S. Schwoon, and S. Jha.
   Weighted pushdown systems and their application to interprocedural
   dataflow analysis. To appear in Proc. 10th Int. Static Analysis Symp (SAS'03)
   Technical report version: TR-1470, Computer Sciences Department,
   University of Wisconsin, Madison, WI, Feb. 2003.
   http://www.cs.wisc.edu/~reps/#sas03
 *****************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include "wpds.h"
#include "print.h"
#include "linfunc.h"

/*
Example program to be analyzed

int x;

void main() {
  x = 5;
  p();           // call site: main_calls_p
  return;
}

void p() {
  if (...) {
    x = x + 1;
    p();         // call site: p_calls_p1
    x = x - 1;
  }
  else if (...) {
    x = x - 1;
    p();         // call site: p_calls_p2
    x = x + 1;
  }
  return;
}
*/

void print_output(wFA *fa, wConfig* c) {
  wPath *p = wPathFind(fa,c);
  
  printf("trace example:\n");
  if (p->transitions == NULL) {
    printf("Goal transition NULL\n");
  }
  else {
    print_trace(fa,p->transitions->target);
  }
  wPathDeref(fa,p);
  printf("\n\n");
}

void print_readout (wFA *fa)
{
  wFA *fa_ro = wReadout(fa);
  print_automaton(fa_ro,"readout");
  wFADelete(fa_ro);
}

void compute_automaton (wPDS *pds, wFA *fa, wIdent from, wIdent name)
{
  wFA *fa_pre;
  wConfig *c;

  print_automaton(fa,"before");
  fa_pre = wPrestar(pds,fa,TRACE_YES);
  print_automaton(fa_pre,"after");

  print_readout(fa_pre);

  c = wConfigCreate(from,name,0);
  print_output(fa_pre,c);
  wConfigDelete(c);

  wFADelete(fa);
  wFADelete(fa_pre);
}

int main()
{
  wPDS *pds;
  wFA *fa0, *fa3, *fa4;

  wSemiring lf_semiring =  { &linfunc_extender, &linfunc_combiner, NULL,
                             &linfunc_one, &linfunc_zero, NULL, &linfunc_eq,
                             &linfunc_rcincr, &linfunc_rcdecr };
  wSemiring *s = &lf_semiring;

  /* Variables */
    wIdent var_lambda;
    wIdent var_x;

  /* Nodes for procedure main */
    wIdent e_main;
    wIdent n1, n2, n3;
    wIdent x_main;

  /* Nodes for procedure p */
    wIdent e_p;
    wIdent n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14;
    wIdent x_p;

  /* States for a configuration automaton */
    wIdent accepting_state, q1, q2, q3, q4;  

  linfunc_init();

  wInit();

  /* Variables */
     var_lambda = wIdentCreate("Lambda");
     var_x = wIdentCreate("x");

  /* Nodes for procedure main */
     e_main = wIdentCreate("e_main");
     n1 = wIdentCreate("n1");
     n2 = wIdentCreate("n2");
     n3 = wIdentCreate("n3");
     x_main = wIdentCreate("x_main");

  /* Nodes for procedure p */
     e_p = wIdentCreate("e_p");
     n4 = wIdentCreate("n4");
     n5 = wIdentCreate("n5");
     n6 = wIdentCreate("n6");
     n7 = wIdentCreate("n7");
     n8 = wIdentCreate("n8");
     n9 = wIdentCreate("n9");
     n10 = wIdentCreate("n10");
     n11 = wIdentCreate("n11");
     n12 = wIdentCreate("n12");
     n13 = wIdentCreate("n13");
     n14 = wIdentCreate("n14");
     x_p = wIdentCreate("x_p");

  pds = wPDSCreate(s);

  /* Transitions (edges) for procedure main */
     wPDSInsert(pds, var_lambda, e_main, var_lambda, n1,  0, linfunc_id);
     wPDSInsert(pds, var_lambda, e_main, var_x,      n1,  0, linfunc_bottom);

     wPDSInsert(pds, var_lambda, n1,  var_lambda, n2,     0, linfunc_id);
     wPDSInsert(pds, var_lambda, n1,  var_x,      n2,     0, mkLFValue(0,5,INT_TOP));

     wPDSInsert(pds, var_lambda, n2,  var_lambda, n3,     0, linfunc_id);

     wPDSInsert(pds, var_lambda, n3,  var_lambda, x_main, 0, linfunc_id);
     wPDSInsert(pds, var_x,      n3,  var_x,      x_main, 0, linfunc_id);

  /* Transitions (edges) for procedure p */
     wPDSInsert(pds, var_lambda, e_p, var_lambda, n4,     0, linfunc_id);
     wPDSInsert(pds, var_x,      e_p, var_x,      n4,     0, linfunc_id);

     /* if (...) */
     wPDSInsert(pds, var_lambda, n4,  var_lambda, n9,     0, linfunc_id);
     wPDSInsert(pds, var_x,      n4,  var_x,      n9,     0, linfunc_id);

     wPDSInsert(pds, var_lambda, n4,  var_lambda, n5,     0, linfunc_id);
     wPDSInsert(pds, var_x,      n4,  var_x,      n5,     0, linfunc_id);

     wPDSInsert(pds, var_lambda, n5,  var_lambda, n6,     0, linfunc_id);
     wPDSInsert(pds, var_x,      n5,  var_x,      n6,     0, mkLFValue(1,1,INT_TOP));

     wPDSInsert(pds, var_lambda, n6,  var_lambda, n7,     0, linfunc_id);

     wPDSInsert(pds, var_lambda, n7,  var_lambda, n8,     0, linfunc_id);
     wPDSInsert(pds, var_x,      n7,  var_x,      n8,     0, linfunc_id);

     wPDSInsert(pds, var_lambda, n8,  var_lambda, n9,     0, linfunc_id);
     wPDSInsert(pds, var_x,      n8,  var_x,      n9,     0, mkLFValue(1,-1,INT_TOP));

     /* else if (...) */
     wPDSInsert(pds, var_lambda, n9,  var_lambda, n14,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n9,  var_x,      n14,    0, linfunc_id);

     wPDSInsert(pds, var_lambda, n9,  var_lambda, n10,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n9,  var_x,      n10,    0, linfunc_id);

     wPDSInsert(pds, var_lambda, n10, var_lambda, n11,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n10, var_x,      n11,    0, mkLFValue(1,-1,INT_TOP));

     wPDSInsert(pds, var_lambda, n11, var_lambda, n12,    0, linfunc_id);

     wPDSInsert(pds, var_lambda, n12, var_lambda, n13,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n12, var_x,      n13,    0, linfunc_id);

     wPDSInsert(pds, var_lambda, n13, var_lambda, n14,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n13, var_x,      n14,    0, mkLFValue(1,1,INT_TOP));

     wPDSInsert(pds, var_lambda, n14, var_lambda, x_p,    0, linfunc_id);
     wPDSInsert(pds, var_x,      n14, var_x,      x_p,    0, linfunc_id);

  /* Transitions (linkage edges) for main-calls-p and return */
     wPDSInsert(pds, var_lambda, n2,  var_lambda, e_p,    n3, linfunc_id);
     wPDSInsert(pds, var_x,      n2,  var_x,      e_p,    n3, linfunc_id);

  /* Transitions (linkage edges) for p-calls-p1 and return */
     wPDSInsert(pds, var_lambda, n6,  var_lambda,    e_p,    n7, linfunc_id);
     wPDSInsert(pds, var_x,      n6,  var_x,         e_p,    n7, linfunc_id);

  /* Transitions (linkage edges) for p-calls-p2 and return */
     wPDSInsert(pds, var_lambda, n11, var_lambda,    e_p,    n12, linfunc_id);
     wPDSInsert(pds, var_x,      n11, var_x,         e_p,    n12, linfunc_id);

  /* Transitions (linkage edges) for returning from p */
     wPDSInsert(pds, var_lambda, x_p, var_lambda,    0,      0, linfunc_id);
     wPDSInsert(pds, var_x,      x_p, var_x,         0,      0, linfunc_id);

  accepting_state = wIdentCreate("__accepting_state__");
  q1 = wIdentCreate("_q1_");
  q2 = wIdentCreate("_q2_");
  q3 = wIdentCreate("_q3_");
  q4 = wIdentCreate("_q4_");

  /* Example 4.2: <var_x, e_p (n12 n7)* n3> */
  printf("Example 4.2: <var_x, e_p (n12 n7)* n3>\n\n");
  fa0 = wFACreate(s);
  wFAInsert(fa0, var_x, e_p, q1, linfunc_id, NULL);
  wFAInsert(fa0, q1, n12, q2, linfunc_id, NULL);
  wFAInsert(fa0, q2, n7, q1, linfunc_id, NULL);
  wFAInsert(fa0, q1, n3, accepting_state, linfunc_id, NULL);
  compute_automaton(pds,fa0,var_lambda,e_main);

  /* Example 4.3: <var_x, e_p n12 n7 n3> */
  printf("Example 4.3: <var_x, e_p n12 n7 n3>\n\n");
  fa3 = wFACreate(s);
  wFAInsert(fa3, var_x, e_p, q1, linfunc_id, NULL);
  wFAInsert(fa3, q1, n12, q2, linfunc_id, NULL);
  wFAInsert(fa3, q2, n7, q3, linfunc_id, NULL);
  wFAInsert(fa3, q3, n3, accepting_state, linfunc_id, NULL);
  compute_automaton(pds,fa3,var_lambda,e_main);

  /* Example 4.4: <var_x, e_p (n12 + n7)* n3> */
  printf("Example 4.4: <var_x, e_p (n12 + n7)* n3>\n\n");
  fa4 = wFACreate(s);
  wFAInsert(fa4, var_x, e_p, q1, linfunc_id, NULL);
  wFAInsert(fa4, q1, n12, q1, linfunc_id, NULL);
  wFAInsert(fa4, q1, n7, q1, linfunc_id, NULL);
  wFAInsert(fa4, q1, n3, accepting_state, linfunc_id, NULL);
  compute_automaton(pds,fa4,var_lambda,e_main);

  wPDSDelete(pds);
//linfunc_stats();
  wFinish();

  return 0;
} /** end of main **/
