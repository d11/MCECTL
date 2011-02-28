/*****************************************************************************/
/* WPDS library usage example no.1: normal pre*                              */

/*****************************************************************************
   Example taken from:
   J. Esparza, D. Hansel, P. Rossmanith, and S. Schwoon.
   Efficient algorithms for model checking pushdown systems.
   Technical Report TUM-I0002, SFB-Bericht 342/1/00 A,
   Technische Universität München, Institut für Informatik, February 2000.
   http://www.fmi.uni-stuttgart.de/szs/publications/info/schwoosn.EHRS00b.shtml
 *****************************************************************************/

#include <stdio.h>
#include <wpds.h>
#include "print.h"

void *nullfn (void *a, void *b) { return NULL; }
int   eqfn (void *a, void *b)   { return a==b;    }
void *null ()			{ return NULL; }

int main()
{
	wPDS *pds;
	wFA *fa, *fa_pre;
	wPath *p;
	wConfig *c;
	wSemiring sr = { &nullfn, &nullfn, NULL,	/* ext, comb, diff  */
			  &null, &null, NULL,		/* one, zero, q-one */
			  &eqfn, NULL, NULL };		/* eq, ref, deref   */
	wSemiring *s = &sr;
	wIdent p0,p1,p2,y0,y1,y2,s1,s2;

	wInit();

	p0 = wIdentCreate("p0");
	p1 = wIdentCreate("p1");
	p2 = wIdentCreate("p2");

	y0 = wIdentCreate("y0");
	y1 = wIdentCreate("y1");
	y2 = wIdentCreate("y2");

	s1 = wIdentCreate("_s1_");
	s2 = wIdentCreate("__s2__");
	
	pds = wPDSCreate(s);
	wPDSInsert(pds,p0,y0,p1,y1,y0,NULL);
	wPDSInsert(pds,p1,y1,p2,y2,y0,NULL);
	wPDSInsert(pds,p2,y2,p0,y1, 0,NULL);
	wPDSInsert(pds,p0,y1,p0, 0, 0,NULL);

	fa = wFACreate(s);
	wFAInsert(fa,p0,y0,s1,NULL,NULL);
	wFAInsert(fa,s1,y0,s2,NULL,NULL);

	print_automaton(fa,"before pre*");
	fa_pre = wPrestar(pds,fa,TRACE_YES);
	print_automaton(fa_pre,"after pre*");

	printf("trace example:\n");
	c = wConfigCreate(p1,y1,0);
	p = wPathFind(fa_pre,c);
	print_trace(fa_pre,p->transitions->target);
	wConfigDelete(c);
	wPathDeref(fa_pre,p);

	wFADelete(fa);
	wFADelete(fa_pre);
	wPDSDelete(pds);
	wFinish();

	return 0;
}
