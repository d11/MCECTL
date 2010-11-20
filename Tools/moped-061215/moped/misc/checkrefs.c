#include <stdio.h>
#include <stdlib.h>
#include <cudd.h>
#include <cuddInt.h>

void check_refs (DdManager *unique, char *s)
{
	DdNode **nodelist;
	DdNode *node, *sentinel = &(unique->sentinel);
	int i, j, slots, deleted, totalDeleted = 0;

	printf("%s\n",s);
	if (unique->dead == 0) return;

	for (i = 0; i < unique->size; i++)
	{
		if (unique->subtables[i].dead == 0) continue;
		nodelist = unique->subtables[i].nodelist;

		deleted = 0;
		slots = unique->subtables[i].slots;
		for (j = 0; j < slots; j++)
		{
			node = nodelist[j];
			while (node != sentinel)
			{
				if (node->ref == 0) deleted++;
				node = node->next;
			}
		}
		if ((unsigned) deleted != unique->subtables[i].dead)
		{
			printf("disorder in subtable %i (%u,%u)\n", i,
				(unsigned) deleted, unique->subtables[i].dead);
			exit(1);
		}
		totalDeleted += deleted;
	}

	if (unique->constants.dead != 0)
	{
		nodelist = unique->constants.nodelist;
		deleted = 0;
		slots = unique->constants.slots;
		for (j = 0; j < slots; j++)
		{
			node = nodelist[j];
			while (node != NULL)
			{
				if (node->ref == 0) deleted++;
				node = node->next;
			}
		}
		if ((unsigned) deleted != unique->constants.dead)
		{
			printf("disorder in constants (%u,%u)\n",
				(unsigned) deleted, unique->constants.dead);
			exit(1);
		}
		totalDeleted += deleted;
	}
	if ((unsigned) totalDeleted != unique->dead)
	{
		printf("disorder in table (%u,%u)\n",
				(unsigned) totalDeleted, unique->dead);
		exit(1);
	}

	printf("----\n");
}
