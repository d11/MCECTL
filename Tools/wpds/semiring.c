/****************************************************************************/
/* Just a couple of functions concerning semirings (internal use).	    */

#include "wpdsInt.h"

void wSRNothing (void *a)
{
}

wSemiring* wSRCopy (wSemiring *sr)
{
	wSemiring *copy = Malloc(sizeof(struct wSemiring));
	memcpy(copy,sr,sizeof(struct wSemiring));
	if (!copy->ref) copy->ref = wSRNothing;
	if (!copy->deref) copy->deref = wSRNothing;
	return copy;
}
