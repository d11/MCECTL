/*****************************************************************************/
/* Generic hash table for WPDS library (internal use)                        */

#include "wpdsInt.h"

wHash* wHashCreate (int(*cmp)(wHashElement*,wHashElement*))
{
	wHash *table = Malloc(sizeof(wHash));

	table->min_buckets = WHASH_MIN;
	table->num_buckets = WHASH_MIN;
	table->num_entries = 0;
	table->mask = WHASH_MIN-1;
	table->buckets = Calloc(1,WHASH_MIN * sizeof(wStub*));
	table->cmp = cmp;

	return table;
}

void wHashDelete (wHash *table, void(*freefn)(wHashElement*))
{
	int i;
	wStub *entry;

	for (i = 0; i < table->num_buckets; i++)
		while ((entry = table->buckets[i]))
		{
			wStub *tmp = entry;
			table->buckets[i] = entry->next;
			freefn(tmp);
		}

	Free(table->buckets);
	Free(table);
}

wHashElement* wHashInsert (wHash *table, wHashElement *e)
{
	int i, cmp = 1;
	wStub *entry = (wStub*) e;
	wStub *last_entry = NULL;
	wStub *next_entry = table->buckets[entry->hash & table->mask];

	/* determine where the new entry should go */
	while (next_entry)
	{
		cmp = table->cmp(next_entry,entry);
		if (cmp >= 0) break;
		last_entry = next_entry;
		next_entry = next_entry->next;
	}

	/* entry already in hash table, return it */
	if (!cmp) return next_entry;

	/* rehash if needed */
	if (table->num_entries++ >= table->num_buckets * 3/4)
	{
		table->mask = table->num_buckets * 2 - 1;
		table->buckets = Realloc(table->buckets,
			table->num_buckets * 2 * sizeof(void*));

		for (i = 0; i < table->num_buckets; i++)
		{
		    wStub *l1 = NULL, *l2 = NULL;

		    /* split bucket number i */
		    last_entry = table->buckets[i];
		    while (last_entry)
		    {
			next_entry = last_entry->next;
			if ((last_entry->hash & table->mask)
					== (unsigned long)i)
			{
				last_entry->next = l1;
				l1 = last_entry;
			}
			else
			{
				last_entry->next = l2;
				l2 = last_entry;
			}
			last_entry = next_entry;
		    }

		    /* revert the split buckets */
		    last_entry = NULL;
		    while (l1)
		    {
			next_entry = l1->next;
			l1->next = last_entry;
			last_entry = l1;
			l1 = next_entry;
		    }

		    table->buckets[i] = last_entry;

		    last_entry = NULL;
		    while (l2)
		    {
			next_entry = l2->next;
			l2->next = last_entry;
			last_entry = l2;
			l2 = next_entry;
		    }

		    table->buckets[i + table->num_buckets] = last_entry;
		}

		table->num_buckets *= 2;
		
		last_entry = NULL;
		next_entry = table->buckets[entry->hash & table->mask];

		/* redetermine position of new entry inside its bucket */
		while (next_entry)
		{
		    cmp = table->cmp(next_entry,entry);
		    if (cmp >= 0) break;
		    last_entry = next_entry;
		    next_entry = next_entry->next;
		}
	}

	/* insert new entry into appropriate bucket... */
	i = entry->hash & table->mask;
	if (!last_entry)
	{
		/* ...at beginning of bucket */
		entry->next = table->buckets[i];
		table->buckets[i] = entry;
	}
	else
	{
		/* ...in the middle or at the end */
		entry->next = next_entry;
		last_entry->next = entry;
	}

	return entry;
}

wHashElement* wHashFind (wHash *table, wHashElement *e)
{
	wStub *entry = (wStub*) e;
	wStub *next_entry = table->buckets[entry->hash & table->mask];
	
	while (next_entry)
	{
		int cmp = table->cmp(next_entry,entry);
		if (cmp > 0) return NULL;
		if (cmp >= 0) return next_entry;
		next_entry = next_entry->next;
	}

	return NULL;
}

void wHashRemove (wHash* table, wHashElement *e)
{
	wStub *entry = (wStub*) e;
	wStub **ptr = table->buckets + (entry->hash & table->mask);

	/* locate the element inside its bucket */
	while (*ptr && *ptr != entry)
		ptr = &((*ptr)->next);

	if (!*ptr) return;	/* if not found, do nothing and return anyway */
	*ptr = (*ptr)->next;	/* remove element from bucket */
	table->num_entries--;
}
