/***************************************************************************/
/* Data structures and functions for maintaining identifiers.		   */

#include "wpdsInt.h"

char **wident_array;
long wident_globals_alloc;
long wident_globals_used;
wHash *wident_table;

#define NAME_DELTA 1024

int wIdentCellCompare (void *va, void *vb)
{
	wIdentCell *a = va, *b = vb;
	return strcmp(wident_array[a->ident],wident_array[b->ident]);
}

wIdent wIdentCreate (char *new_ident)
{
	wIdentCell *entry;
	static wIdentCell *new_entry = NULL;
	unsigned long hash = 1;
	char *c;

	/* Note that new_entry is static. If new_ident is already known,
	   we wll not deallocate new_entry; instead, we keep it around
	   for the next call.
	   FIXME: new_entry will not be freed by wIdentFinish().
		  (probably a minor concern). */
	if (!new_entry) new_entry = Malloc(sizeof(struct wIdentCell));

	for (c = new_ident; *c; c++) hash = (hash + *c) << 1;
	new_entry->hash = hash % 200003;
	new_entry->ident = wident_globals_used;

	/* enlarge the array if we are out of space */
	if (wident_globals_used == wident_globals_alloc)
	{
		if (wident_array)
			wident_array = Realloc(wident_array,
			  (wident_globals_alloc += NAME_DELTA) * sizeof(char*));
		else
			wident_array = Malloc(
			  (wident_globals_alloc += NAME_DELTA) * sizeof(char*));
	}

	/* add new_ident to wident_array temporarily for wIdentCellCompare */
	wident_array[wident_globals_used] = new_ident;
	entry = wHashInsert(wident_table,new_entry);

	if (entry == new_entry)
	{
		new_entry = NULL; /* will allocate new memory next time */
		wident_array[wident_globals_used++] = Strdup(new_ident);
	}
	return entry->ident;
}

void wIdentInit ()
{
	/* initialize the hash table */
	wident_table = wHashCreate(wIdentCellCompare);
	wident_array = NULL;
	wident_globals_alloc = 0;
	wident_globals_used  = 0;

	/* Create a dummy identfier with index 0. This is done so
	   that zeros for missing stack symbols cannnot be confused
	   with actual stack symbols. */
	wIdentCreate("*");
}

char* wIdentString (wIdent i)
{
	return wident_array[i];
}

void wIdentFree (void *ptr)
{
	Free(ptr);
}

void wIdentFinish ()
{
	int i;

	/* Delete the hash table. */
	wHashDelete(wident_table,wIdentFree);

	/* Delete the array of names. */
	for (i = 0; i < wident_globals_used; i++)
		Free(wident_array[i]);

	if (wident_array) Free(wident_array);
}

char wIsInitial (wIdent i)
{
	char c = *wIdentString(i);
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
				      || (c >= '0' && c <= '9');
}

char wIsFinal (wIdent i)
{
	char *name = wIdentString(i);
	//return name[0] == '_' && name[1] == '_';
	if (strlen(name) < 2)
		return 0;
	else
		return name[strlen(name)-2] == '_' && name[strlen(name)-1] == '_';
}
