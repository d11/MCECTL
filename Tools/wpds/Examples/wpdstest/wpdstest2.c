/*****************************************************************************/
/* WPDS library usage example no.2: post* while counting the number of rules */

/*****************************************************************************
   Example taken from:
   J. Esparza, D. Hansel, P. Rossmanith, and S. Schwoon.
   Efficient algorithms for model checking pushdown systems.
   Technical Report TUM-I0002, SFB-Bericht 342/1/00 A,
   Technische Universität München, Institut für Informatik, February 2000.
   http://www.fmi.uni-stuttgart.de/szs/publications/info/schwoosn.EHRS00b.shtml
 *****************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <wpds.h>
#include <compat.h>
#include "print.h"

void* plusfn (void *a, void *b) { return (void*)((int)a+(int)b); }
void* minfn  (void *a, void *b) { return (a<b)? a : b; }
int   eqfn   (void *a, void *b) { return a==b; }
void* onefn ()		 	{ return NULL; }
void* zerofn ()		 	{ return (void*)INT_MAX; }

int main()
{
	wPDS  *pds;
	wFA   *fa, *fa_post;
	wPath *p;
	wConfig *c;
	wSemiring sr = { &plusfn, &minfn, NULL,		/* ext, comb, diff  */
			  &onefn, &zerofn, NULL,	/* one, zero, q-one */
			  &eqfn, NULL, NULL };		/* eq, ref, deref   */
	wSemiring *s = &sr;
	wIdent p0,p1,p2,y0,y1,y2,s1,s2,m1;

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
	wPDSInsert(pds,p0,y0,p1,y1,y0,(void*)1);
	wPDSInsert(pds,p1,y1,p2,y2,y0,(void*)1);
	wPDSInsert(pds,p2,y2,p0,y1, 0,(void*)1);
	wPDSInsert(pds,p0,y1,p0, 0, 0,(void*)1);

	fa = wFACreate(s);
	wFAInsert(fa,p0,y0,s1,NULL,NULL);
	wFAInsert(fa,s1,y0,s2,NULL,NULL);

	print_automaton(fa,"before post*");
	fa_post = wPoststar(pds,fa,TRACE_YES);
	print_automaton(fa_post,"after post*");

	m1 = get_midstate(p1,y1);
	c = wConfigCreate(p0,y0,y0,y0,y0,0);
	printf("value of <p0,y0 y0 y0 y0> is %d\n\n",
			(int)wConfigValue(fa_post,c));
	printf("trace example:\n");
	p = wPathFind(fa_post,c);
	print_trace(fa_post,p->transitions->target);
	wPathDeref(fa_post,p);
	wConfigDelete(c);

	wPDSDelete(pds);
	wFADelete(fa);
	wFADelete(fa_post);
	wFinish();

	return 0;
}
