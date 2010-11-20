#include "bdd.h"

void bwngDfs1(wWitnessNode*);

int bwngCount;

void bwngPrintHeader ()
{
	printf("======================\n");
	printf("digraph test {\n");
	printf(" edge [dir=back]\n"); /* see switch in order in dotEdge */
}

void bwngPrintFooter ()
{
	printf("}\n");
	printf("======================\n");
}

void bwngPrintNode (wWitnessNode *n)
{
	printf(" n%d [label=\"%s %s %s\",color=%s];\n",
		(int)(n->user),
		wIdentString(n->trans->from),
		wIdentString(n->trans->name),
		wIdentString(n->trans->to),
		bddEmpty(n->value)? "red" : "black"
		);
}

void bwngPrintEdge (wWitnessNode *n1, wWitnessNode *n2)
{
	if (!n2) return;
	bwngDfs1(n2);
	printf("  n%d -> n%d;\n",(int)(n2->user),(int)(n1->user));
}

void bwngDfs1 (wWitnessNode *n)
{
	wWitnessEdge *e;
	if (n->user) return;

	n->user = (void*)(++bwngCount);
	bwngPrintNode(n);
	for (e = n->edges; e; e = e->next)
	{
		bwngPrintEdge(n,e->target1);
		bwngPrintEdge(n,e->target2);
	}
}

void bwngDfs2 (wWitnessNode *n)
{
	wWitnessEdge *e;
	if (!n || !n->user) return;

	n->user = NULL;
	for (e = n->edges; e; e = e->next)
	{
		bwngDfs2(e->target1);
		bwngDfs2(e->target2);
	}
}

void bddPrintwNodeGraph (wSat *prcs)
{
	wTrans *t;

	bwngCount = 0;
	bwngPrintHeader();
        for (t = prcs->fa->transitions; t; t = t->next)
		bwngDfs1(t->wnode);
	bwngPrintFooter();

        for (t = prcs->fa->transitions; t; t = t->next)
		bwngDfs2(t->wnode);
}
