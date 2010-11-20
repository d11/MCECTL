#include "absCache.h"
#include "crc32.h"

static int cacheMisses;
static int cacheHits;
static int hitMax;

wHash *andAbsHash;
andAbs *front, *back;
int andAbsHashSize;

static int andAbsCompare (void *va, void *vb)
{
	andAbs *a = va, *b = vb;
	#define andAbsHashCompare(field) \
		if (a->field != b->field) return (a < b)? -1 : 1;
	andAbsHashCompare(x);
	andAbsHashCompare(y);
	andAbsHashCompare(cube);
	return 0;
}

static void andAbsDelete (void *vv)
{
	andAbs *el = vv;
	Cudd_RecursiveDeref(mgr,el->x);
	Cudd_RecursiveDeref(mgr,el->y);
	Cudd_RecursiveDeref(mgr,el->cube);
	Cudd_RecursiveDeref(mgr,el->result);
	Free(el);
}

/* what does extern mean? */
void andAbsHashCreate ()
{
	andAbsHash = wHashCreate(andAbsCompare);
	front = NULL;
	back = NULL;
	andAbsHashSize = 0;
	cacheHits = cacheMisses = 0;
	hitMax = 0;
}

void unlink (andAbs *el)
{
	if (el->backwardLRU)
	{
		el->backwardLRU->forwardLRU = el->forwardLRU;
		if (el->forwardLRU)
		{
			el->forwardLRU->backwardLRU = el->backwardLRU;
		}
		else /* |el-x-x... */
		{
			assert (front == el);
			front = el->backwardLRU;
		}
	}
	else
	{
		assert (back == el);
		if (el->forwardLRU) /* ---x-x-el| */
		{
			el->forwardLRU->backwardLRU = NULL;
			back = el->forwardLRU;
		}
		else /* |el| */
		{
			front = back = NULL;
		}
	}
	el->forwardLRU = NULL;
	el->backwardLRU = NULL;
}

void unlinkAndDelete (andAbs *el)
{
	unlink(el);
	wHashRemove(andAbsHash,el);
	andAbsHashSize--;
	andAbsDelete(el);
}

void clearSpace ()
/* make sure that andAbsHashSize < CACHE,
   possibly by deleting the front element */
{
	assert (andAbsHashSize <= CACHE);
	if (andAbsHashSize == CACHE)
	{
		if (!hitMax)
		{
			printf("**************************************** ");
			printf("Hit Max. Cache Size %d\n", CACHE);
			hitMax = 1;
		}
		assert (front != NULL);
		unlinkAndDelete(front);
	}
	assert (andAbsHashSize < CACHE);
}

void push_back (andAbs *el)
{
	if (front == NULL)
	{
		assert (back == NULL);
		front = el;
		back = el;
		el->forwardLRU = NULL;
		el->backwardLRU = NULL;
	} 
	else
	{
		assert (back != NULL);
		el->forwardLRU = back;
		el->backwardLRU = NULL;
		back->backwardLRU = el;
		back = el;
	}
}

DdNode* cachedAndAbstract (DdNode *x, DdNode *y, DdNode *cube)
{
	if (CACHE == 0) return Cudd_bddAndAbstract(mgr,x,y,cube);
	andAbs *v, *nv = Malloc(sizeof(struct andAbs));
	
	nv->x = x; /* they might get refed below */
	nv->y = y;
	nv->cube = cube;
	nv->result = NULL;
	
	nv->hash = crcCompute((char*)&(nv->x), 3*sizeof(DdNode*));
	nv->next = NULL;

	v = wHashInsert(andAbsHash,nv);
	if (v == nv) /* this is a new element */
	{
		/* printf("Cache Miss. Size = %d\n",andAbsHashSize); */
		cacheMisses++;
		andAbsHashSize++;
		Cudd_Ref(x);
		Cudd_Ref(y);
		Cudd_Ref(cube);
		Cudd_Ref(v->result = Cudd_bddAndAbstract(
				 mgr,
				 nv->x, nv->y, nv->cube));
		clearSpace();
	}
	else
	{
		cacheHits++;
		/* printf("Cache Hit. Size = %d\n",andAbsHashSize); */
		Free(nv);
		/* now shift v back to mark it as recently used */
		unlink(v);
	}
	push_back(v);
	return v->result;
}

void andAbsHashDelete ()
{
	fprintf(stderr, "CacheHitRatio: %2.2f  ", 1.0*cacheHits/(cacheHits+cacheMisses));
	fprintf(stderr, "andAbsHashSize: %d   ", andAbsHashSize);
	wHashDelete(andAbsHash,andAbsDelete);
}
