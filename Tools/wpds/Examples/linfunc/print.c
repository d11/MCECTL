/* Functions used by both test programs */

#include <stdio.h>
#include "wpds.h"
#include "linfunc.h"

extern void printLFValue(void *a);

void print_automaton (wFA *fa, char *s)
{
	wTrans *t;

	printf("Automaton %s:\n",s);
	for (t = fa->transitions; t; t = t->next) {
		printf("  %s -- %s --> %s ",
			wIdentString(t->from),wIdentString(t->name),
			wIdentString(t->to));
                printf("\t\t");
                printLFValue(t->label);
                printf("\n");
        }
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

void print_conf (wFA *fa, wPath *p)
{
    wConfig *conf = wPathConfig(fa,p);
    wIdent *id;
	
    printLFValue(p->value);
    printf(" <%s,%s",wIdentString(conf->state),wIdentString(conf->stack[0]));
    id = conf->stack;
    while (*++id) printf(" %s",wIdentString(*id));
    printf(">");

    wConfigDelete(conf);
}

void print_indent(int i)
{
    int j;
    for (j = 0; j < i; j++) {
        printf("|  ");
    }
}

void print_trace_aux (wFA *fa, wPath *p, int indent)
{
		wPathTrans *pt;

		if (p == NULL) return;

                print_indent(indent);
		print_conf(fa,p);
		printf("\n");
		pt = p->transitions;
		while (pt) {
	                print_indent(indent);
			if (pt->rule) print_rule(pt->rule);
			else if (pt->target == NULL)
				printf("accepted by query automaton");
			printf("\n");
			print_trace_aux(fa, pt->target, indent + 1);
			pt = pt->next;
		}
}

void print_trace (wFA *fa, wPath *p)
{
  wPathTraceAll(fa,p);
  print_trace_aux(fa, p, 0);
}
