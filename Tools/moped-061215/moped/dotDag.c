void dotHeader ()
{
	printf("======================\n");
	printf("digraph DAG {\n");
	printf(" edge [dir=back]\n"); /* see switch in order in dotEdge */
}

void dotFooter ()
{
	printf("}\n");
	printf("======================\n");
}

void dotNodes (dag *d)
{
	dagNodeList *dnl;
	dagNode *n;
	for (dnl = d->allNodes; dnl; dnl = dnl->next)
	{
		n = dnl->n;
		printf(" n%d [label=\"%s\",color=%s];\n",
		       n->id,
		       wIdentString(getStackSymbol(n)),
		       inDagNodeList(n,d->top)? "blue"
		       : (n->type == LOC_NORMAL || n->type == LOC_ENTRY)? "black"
		       : (n->type == LOC_CALL)? "red"
		       : (n->type == LOC_GLOBAL)? "yellow"
		       : (n->type == LOC_EVAL)? "green"
		       : "violet");
	}
}

void dotEdge (dagNode *n1, dagNode *n2)
{
	assert (n1 != NULL);
	if (n2 == NULL) return;
	printf("  n%d -> n%d;\n", n2->id, n1->id);/*edge direction is set to back*/
}

void dotEdges (dag *d)
{
	dagEdgeList *del;
	dagEdge *e;
	for (del = d->allEdges; del; del = del->next)
	{
		e = del->e;
		dotEdge(e->down, e->up1);
		dotEdge(e->down, e->up2);
	}
}

void dotDag (dag *d)
{
	dotHeader();
	dotNodes(d);
	dotEdges(d);
	dotFooter();
}
