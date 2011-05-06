/*****************************************************************************
   Example taken from:

     S. Schwoon, S. Jha, T. Reps, and S. Stubblebine, On generalized
     authorization problems. To appear in Proc. 16th IEEE Computer
     Security Foundations Workshop, (June 30 - July 2, 2003, Asilomar,
     Pacific Grove, CA).  Tech. Report version: TR-1469, Computer Sciences
     Department, University of Wisconsin, Madison, WI, Jan. 2003.
     http://www.cs.wisc.edu/~reps/#csfw03
 *****************************************************************************/

#include <stdio.h>
#include "wpds.h"
#include "print.h"

#define NONSENSITIVE 0
#define SENSITIVE 1


void *extenderFn (void *a, void *b) { 

  int left_val;

  left_val = (int)a;

  if (left_val == SENSITIVE) {
    return((void *)SENSITIVE);
  }
  else {
    return(b);

  }

} /* end of extender **/
    

void *combinerFn (void *a, void *b) {

  int left_val;

  left_val = (int)a;

  if (left_val == NONSENSITIVE) {
    return((void *)NONSENSITIVE);
  }
  else {
    return(b);
  }
} /** end of combiner **/

int   eqFn (void *a, void *b)  { 
  return a==b;
} /** end of eqfn **/

void *oneFn ()	 { 

return ((void *)NONSENSITIVE); 
} /** end of nullFn **/

void *zeroFn ()	 { 

return ((void *)SENSITIVE); 
} /** end of nullFn **/

int main()
{
  wPDS  *pds;
  wFA   *fa, *fa_pre;
  wPath *p;
  wSemiring sr = { &extenderFn, &combinerFn, NULL,
			&oneFn, &zeroFn, NULL, &eqFn, NULL, NULL };
  wSemiring *s = &sr;
  wIdent k_x,k_h,k_aids,k_im,k_alice;
  wIdent y_patient, y_square, y_bsquare;
  wIdent final_state;

  wInit();

  k_x = wIdentCreate("KX");
  k_h = wIdentCreate("KH");
  k_aids = wIdentCreate("KH-AIDS");
  k_im = wIdentCreate("KH-IM");
  k_alice = wIdentCreate("Alice");

  y_patient = wIdentCreate("patient");
  y_square = wIdentCreate("square");
  y_bsquare = wIdentCreate("b_square");

  final_state = wIdentCreate("__accepting_state__");

  pds = wPDSCreate(s);

  wPDSInsert(pds,k_x, y_square, k_h, y_patient, y_bsquare, (void*)NONSENSITIVE);
  wPDSInsert(pds,k_h, y_patient, k_im, y_patient, 0, (void*)NONSENSITIVE);
  wPDSInsert(pds,k_h, y_patient, k_aids, y_patient, 0, (void*)NONSENSITIVE);
  wPDSInsert(pds,k_im, y_patient, k_alice, 0, 0, (void*)NONSENSITIVE);
  wPDSInsert(pds,k_aids, y_patient, k_alice, 0, 0, (void*)SENSITIVE);

  fa = wFACreate(s);
  wFAInsert(fa,k_alice, y_square , final_state, (void *)NONSENSITIVE, NULL);
  wFAInsert(fa,k_alice, y_bsquare , final_state, (void *)NONSENSITIVE, NULL);

  print_automaton(fa,"before");
  fa_pre = wPrestar(pds,fa,TRACE_YES);
  print_automaton(fa_pre,"after");

  printf("trace example:\n");
  p = wPathCreate(fa_pre,wFAFind(fa_pre,k_x,y_square,final_state),NULL);
  print_trace(fa_pre,p);

  wPDSDelete(pds);
  wFADelete(fa);
  wFADelete(fa_pre);
  wFinish();

  return 0;
} /** end of main **/
