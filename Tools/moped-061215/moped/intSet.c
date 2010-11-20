/***************************************************************************/

/* Functions related to keeping track of which
   stack symbols belong to which modules.	*/

#include "alloc.h"
#include "int.h"

/***************************************************************************/

wSet* wSetCreate ()
{
	return NULL;
}

void wSetInsert (wSet **set, wIdent ident)
{	/* Identifiers in the set are kept in descending order. */
	wSet **position = set;
	wSet *new;

	while (*position && (*position)->ident > ident)
		position = &(*position)->next;

	/* Avoid duplicate entries. */
	if (*position && (*position)->ident == ident) return;

	new = Malloc(sizeof(struct wSet));
	new->ident = ident;
	new->next = *position;
	*position = new;
}

int wSetFind (wSet *set, wIdent ident)
{
	while (set)
	{
		if (set->ident == ident) return 1;
		if (set->ident < ident) return 0;
		set = set->next;
	}
	return 0;
}

void wSetDelete (wSet *set)
{
	while (set)
	{
		wSet *tmp = set->next;
		Free(set);
		set = tmp;
	}
}

/***************************************************************************/
