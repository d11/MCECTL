#include <stdio.h>
#include "wpds.h"
#include "print.h"
#include "linfunc.h"

int main() {
  LINFUNC a, b, c, d, e, f;

  linfunc_init();

  printf("Value-construction tests:\n");
  a = linfunc_top;
  printLFValue(a); printf("\n");
  a = linfunc_id;
  printLFValue(a); printf("\n");
  a = linfunc_bottom;
  printLFValue(a); printf("\n");

  a = mkLFValue(3,1,13);
  printLFValue(a); printf("\n");
  b = mkLFValue(5,-7,13);
  printLFValue(b); printf("\n");
  c = mkLFValue(0,13,INT_TOP);
  printLFValue(c); printf("\n");
  d = mkLFValue(0,5,INT_TOP);
  printLFValue(d); printf("\n");
  e = mkLFValue(1,0,1);
  printLFValue(e); printf("\n");
  f = mkLFValue(3,1,INT_TOP);
  printLFValue(f); printf("\n");

  printf("\nEquality tests:\n");
  printf("%d, (%d expected)\n", linfunc_eq(a,b), 1);
  printf("%d, (%d expected)\n", linfunc_eq(b,a), 1);
  printf("%d, (%d expected)\n", linfunc_eq(a,c), 0);
  printf("%d, (%d expected)\n", linfunc_eq(c,a), 0);
  printf("%d, (%d expected)\n", linfunc_eq(b,c), 0);
  printf("%d, (%d expected)\n", linfunc_eq(c,b), 0);
  printf("%d, (%d expected)\n", linfunc_eq(b,f), 0);
  printf("%d, (%d expected)\n", linfunc_eq(f,b), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,linfunc_top), 1);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,linfunc_bottom), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,linfunc_id), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,a), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,c), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_top,f), 0);

  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,linfunc_top), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,linfunc_bottom), 1);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,linfunc_id), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,a), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,c), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_bottom,f), 0);

  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,linfunc_top), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,linfunc_bottom), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,linfunc_id), 1);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,a), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,c), 0);
  printf("%d, (%d expected)\n", linfunc_eq(linfunc_id,f), 0);

  printf("\nDiff tests:\n");
  printLFValue(linfunc_diff(a,b)); printf("\n");
  printLFValue(linfunc_diff(b,a)); printf("\n");
  printLFValue(linfunc_diff(a,c)); printf("\n");
  printLFValue(linfunc_diff(c,a)); printf("\n");
  printLFValue(linfunc_diff(b,c)); printf("\n");
  printLFValue(linfunc_diff(c,b)); printf("\n");
  printLFValue(linfunc_diff(c,d)); printf("\n");
  printLFValue(linfunc_diff(d,c)); printf("\n");
  printLFValue(linfunc_diff(a,e)); printf("\n");
  printLFValue(linfunc_diff(e,a)); printf("\n");
  printLFValue(linfunc_diff(b,f)); printf("\n");
  printLFValue(linfunc_diff(f,b)); printf("\n");

  printf("\nCombiner tests:\n");
  printLFValue(linfunc_combiner(a,b)); printf("\n");
  printLFValue(linfunc_combiner(b,a)); printf("\n");
  printLFValue(linfunc_combiner(a,c)); printf("\n");
  printLFValue(linfunc_combiner(c,a)); printf("\n");
  printLFValue(linfunc_combiner(b,c)); printf("\n");
  printLFValue(linfunc_combiner(c,b)); printf("\n");
  printLFValue(linfunc_combiner(b,f)); printf("\n");
  printLFValue(linfunc_combiner(f,b)); printf("\n");
  printLFValue(linfunc_combiner(a,linfunc_top)); printf("\n");
  printLFValue(linfunc_combiner(linfunc_top,a)); printf("\n");
  printLFValue(linfunc_combiner(c,linfunc_top)); printf("\n");
  printLFValue(linfunc_combiner(linfunc_top,c)); printf("\n");
  printLFValue(linfunc_combiner(f,linfunc_top)); printf("\n");
  printLFValue(linfunc_combiner(linfunc_top,f)); printf("\n");

  return(0);
}
