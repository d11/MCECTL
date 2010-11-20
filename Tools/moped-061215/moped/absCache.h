#ifndef ABSCACHE_H
#define ABSCACHE_H

#include "absRef.h"

static const int CACHE = 10000;

typedef struct andAbs
{
        struct absHash *next;		/* These two fields are needed     */
        unsigned long hash;		/* by the hashing procedures.	   */

	DdNode *x;
	DdNode *y;
	DdNode *cube;
	DdNode *result;
	struct andAbs *forwardLRU;
	struct andAbs *backwardLRU;
} andAbs;

void andAbsHashCreate ();
void andAbsHashDelete ();
DdNode* cachedAndAbstract (DdNode *x, DdNode *y, DdNode *cube);

#endif
