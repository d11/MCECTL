/*****************************************************************************/

#include <stdio.h>
#include <wpds.h>
#include "print.h"
#include "bdd.h"

int main()
{
	bddPds *pds;
	wFA    *fa, *fa_post;
	wIdent  p0,p1,p2,y0,y1,y2,s1,s2;
	DdNode *weight;

	wInit();
	bddInit();

	p0 = wIdentCreate("p0");   y0 = wIdentCreate("y0");
	p1 = wIdentCreate("p1");   y1 = wIdentCreate("y1");
	p2 = wIdentCreate("p2");   y2 = wIdentCreate("y2");
	s1 = wIdentCreate("_s1_"); s2 = wIdentCreate("__s2__");
	
	pds = bddPdsCreate(0,0,NULL,NULL);

	Cudd_Ref(weight = Cudd_ReadOne(bddGetManager()));
	bddPdsInsert(pds,p0,y0,p1,y1,y0,weight);
	bddPdsInsert(pds,p1,y1,p2,y2,y0,weight);
	bddPdsInsert(pds,p2,y2,p0,y1, 0,weight);
	bddPdsInsert(pds,p0,y1,p0, 0, 0,weight);

	fa = bddFaCreate(pds,SAT_POST);
	bddFaInsert(fa,p0,y0,s1,weight);
	bddFaInsert(fa,s1,y0,s2,weight);
	Cudd_RecursiveDeref(bddGetManager(),weight);

	print_automaton(fa,"before post*");
	fa_post = bddPoststar(pds,fa);
	print_automaton(fa_post,"after post*");

	bddFaDelete(fa);
	bddFaDelete(fa_post);
	bddPdsDelete(pds);

	bddFinish();
	wFinish();

	return 0;
}
