/* Functions used by both test programs */

#include <stdio.h>
#include <wpds.h>

#include "bdd.h"

void print_automaton (wFA *fa, char *s)
{
	wTrans *t;

	printf("Automaton %s:\n",s);
	for (t = fa->transitions; t; t = t->next)
		printf("  %s -- %s --> %s (%p)\n",
			wIdentString(t->from),wIdentString(t->name),
			wIdentString(t->to),((bddDomain*)(t->label))->bdd);
	printf("\n");
}

void print_rule (wRule *r)
{
	printf("rule: <%s,%s> -> <%s",wIdentString(r->from_state),
		wIdentString(r->from_stack),wIdentString(r->to_state));
	if (r->to_stack1) printf(",%s",wIdentString(r->to_stack1));
	if (r->to_stack2) printf(" %s",wIdentString(r->to_stack2));
	printf(">");
}

void print_trace (wFA *fa, wPath *p)
{
	wIdent *id;
	wPath *pnext;
	wConfig *conf;

	do {
		conf = wPathConfig(fa,p);
		printf("[%d] <%s,%s",(int)p->value,wIdentString(conf->state),
				wIdentString(conf->stack[0]));
		id = conf->stack;
		while (*++id) printf(" %s",wIdentString(*id));
		printf(">\t\t");
		wConfigDelete(conf);

		wPathTraceStep(fa,p);
		if (p->transitions->rule) print_rule(p->transitions->rule);
		wPathRef(fa,pnext = p->transitions->target);
		wPathDeref(fa,p);
		p = pnext;

		printf("\n");
	} while (p);

	printf("\n");
}
