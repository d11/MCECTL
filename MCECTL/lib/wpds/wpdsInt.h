/*****************************************************************************/
/* Internal functions and data structures of the WPDS library.		     */

#ifndef WPDSINT_H
#define WPDSINT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "alloc.h"
#include "wpds.h"

/***************************************************************************
                DATA STRUCTURES
 ***************************************************************************/

/***************************************************************************
 Generic hash table:

 These structures are used to implement a hash table. The hash table is
 generic in the sense that the hashed elements can be any C struct, provided
 that the first two fields in the struct are compatible with the wStub struct.
 The size of the hash table will grow automatically as more elements are
 inserted, but will not shrink if elements are removed.
*/ 

#define wHashElement void

typedef struct wStub
{
	struct wStub *next;
	unsigned long hash;
} wStub;

typedef struct wHash
{
	wStub** buckets;
	int min_buckets;
	int num_buckets;
	int num_entries;
	int mask;
	int (*cmp)(wHashElement*,wHashElement*);
} wHash;


/***************************************************************************/
/* Identifier hash table:						   */

extern char **wident_array;		/* maps integers to strings */

typedef struct wIdentCell		/* identifier hash entry */
{
	struct wIdentCell *next;	/* next and hash needed by hash.c */
	unsigned long hash;
	wIdent ident;
} wIdentCell;


/***************************************************************************
 Head tables:
 Saturation processes use this auxiliary look-up table for quick access to
 transitions and pushdown rules satisfying certain conditions. Entries in
 the table consist of a pair of names (p,y) and can point to a list of
 transitions via the trans field (such lists will be connected via the
 'stack' field of the transitions).

 In pre* and post*, 
  - entry (0,0) points to the worklist (i.e. transitions that were modified
    but not yet removed using wSatPopTrans);
  - entry (p,y) points to transitions (p,y,?).

 Moreover, in post* only,
  - entry (p,0) points to transitions (p,?,?) if p is a non-initial state
    (i.e. transitions starting at p);
  - entry (0,q) points to epsilon transitions leading to q.

 In pre* only,
  - entry (p,0) points to some transition starting at p (if any); the
    'trans' field is NULL if there is no such transition.

 In addition, table entries (wHeads) can be connected by edges (wHeadEdges).
 Every edge corresponds to a pushdown rule whose left-hand side is the
 pair in the 'from' field and whose right-hand side is the pair in the 'to'
 field. The 'tail' field records which stack symbol gets pushed (if any).
 An entry (p,y) can iterate over the chain of rules where it stands on the
 left-hand (right-hand) side by following the post/postnext (pre/prenext)
 pointers.  In pre*, entries (p,0) use the pre/prenext pointers to point
 to rules of the form <p',y'> -> <p,empty> (these are applied once a
 transition starting at p exists and are then removed from the table).
*/

typedef struct wHead
{
	struct wHead *next;
	unsigned long hash;

	wIdent state;
	wIdent name;

	struct wHeadEdge *pre;	/* old: rules */
	struct wHeadEdge *post;	/* old: fwd   */
	wTrans *trans;		/* old: trans */
} wHead;

typedef struct wHeadEdge
{				/* corresponds to a rule from -> to<tail> */
	wHead *from;			/* old: head    */
	wHead *to;			/* old: headfwd */
	wIdent tail;
	struct wHeadEdge *prenext;	/* old: next    */
	struct wHeadEdge *postnext;	/* old: fwdnext */

	wSRvalue  *label;
	wSRvalue  *delta;
	wIdent midstate;	/* used for new rules in post* method */

	wRule *origin;			/* the PDS rule causing this edge */
	struct wWitnessNode *wnode;	/* for artificial rules in pre*,
		(origin,trans) are the rule/trans pair causing this edge */
} wHeadEdge;

typedef struct wHeadTable
{
	wSat *prcs;
	wHash *hash;
} wHeadTable;


/***************************************************************************
 Witness graph structures:
 These structures are used to represent the nodes and edges of the graph
 G_pop (or G_push), which is used to reconstruct witness traces (if enabled).
 Each witness node holds information for a (transition,value) pair that has
 arisen during a saturation process. The 'wnode' field of the wTrans structure
 points to the witness node for the value of a transition at the end of the
 process; however, other nodes may exist for the same transition paired with
 other (larger) values, which may (directly or transitively) be the target
 of edges starting at other transitions. Reference counting is used to release
 nodes that are no longer reachable from the 'wnode' field of any transition
 (note that the witness graph is acyclic).
*/

typedef struct wWitnessNode {
	wTrans  *trans;			/* the transition                   */
	wSRvalue value;			/* the value explained by this node */
	int refcount;			/* reference count                  */
	struct wWitnessEdge *edges;	/* list of outgoing hyperedges      */
	struct wWitnessNode *previous;	/* previous wnode for same trans    */
	/* change Stefan K. */
	void *user;                    /* free for use */
	/* end change */
} wWitnessNode;

typedef struct wWitnessEdge {		/* A witness edge is a triple       */
	wWitnessNode *source;		/* (source,rule,target1 target2),   */
	wSRvalue value;			/* where rule, target1, target2 may */
	wRule *rule;			/* be empty if appropriate. value   */
	wWitnessNode *target1;		/* records the (partial) value con- */
	wWitnessNode *target2;		/* tributed to source by this edge. */
	struct wWitnessEdge *next;	/* (next outgoing edge from source) */
} wWitnessEdge;


/***************************************************************************
 An auxiliary data structure used in the function wReadout; for initial
 states, lists the outgoing transitions of that state; for non-initial
 states, lists the incoming transitions from other non-initial states.
 The 'value' field is used to compute the combination of all paths leading
 from that state to some final state.
*/

typedef struct wState
{
	struct wHead *next;
	unsigned long hash;

	wIdent name;		/* name of the automaton state     */
	wSRvalue value;		/* values on paths from here to final states */
	wSRvalue delta;		/* for differential computation    */
	wTrans *trans;		/* list of transitions (see above) */

	char in_queue;		/* worklist queue used in wReadout */
	struct wState *next_state;
} wState;


/***************************************************************************
 Saturation processes:
 The main items of interest are the finite automaton (fa), in which the
 result is kept; the head table (heads), which is used to index important
 parts of the rules and transitions (see above), and an output queue of
 transitions that were produced by the process.
*/

struct wSat
{
	wSemiring *sr;		/* the semiring		 */
	char dir;		/* pre* or post*	 */
	char diff;		/* batch or differential */
	char trace;		/* keep witness traces?  */
	wFA *fa;		/* the resulting fa	 */
	wHeadTable* heads;	/* temporary hash table  */
	wHead *queue;		/* the worklist		 */
	/* change Stefan K. */
	int special;            /* keep additional
				   witness edges         */
	/* end change */
};


/***************************************************************************
                FUNCTION DECLARATIONS
 ***************************************************************************/

/***************************************************************************/
/* Copy a semiring structure and replace NULL pointers for ref/deref
   with dummy entries. */
extern wSemiring* wSRCopy (wSemiring*);


/***************************************************************************/
/* Generic hash table:							   */

#define WHASH_MIN 256  /* initial buckets in new table; must be power of 2 */

/* Create a new hash table. Initially, the table has WHASH_MIN buckets. The
   table will grow automatically if the fill rate exceeds a certain ratio.
   wHashCreate takes a pointer to a user-supplied function as its argument.
   The function is expected to compare two hash elements and to return -1,0,1
   if the elements are smaller, equal, larger to each other. This function is
   used by whash_find_element and whash_insert_element to determine the
   correct position of elements. */
extern wHash* wHashCreate (int(*)(wHashElement*,wHashElement*));

/* Delete the table, i.e. free its memory. The function pointer will be
   called on every entry in the hash and is expected to release the memory
   associated with it. */
extern void wHashDelete (wHash*,void(*)(wHashElement*));

/* Insert an element into the table. If the element is already present in 
   the table, returns the pointer to the corresponding entry in the table.
   If the element is indeed new, return the element. The element is expected
   to carry its hash value in the 'hash' field of its struct. */
extern wHashElement* wHashInsert (wHash*, wHashElement*);

/* Find an element (return NULL if not present). */
extern wHashElement* wHashFind (wHash*, wHashElement*);

/* Remove an element from the hash. The element itself is not changed. */
extern void wHashRemove (wHash*, wHashElement*);


/***************************************************************************/
/* Identifiers:                                                            */

/* Sets up the hash. This function must be called before the first call
   to wident_create (this is done by wInit()). */
extern void wIdentInit ();			

/* Releases the memory used by the hash. Should be called after the last
   use of wIdentCreate or wIdentString (done by wFinish()). */
extern void wIdentFinish ();

/* This is used by various other comparison functions. */
#define wIdentCompare(i1,i2) (((int) i2)-((int) i1))


/***************************************************************************/
/* Head tables:                                                            */

/* Create an empty table for the process given in the argument. */
extern wHeadTable* wHTCreate (wSat*);

/* Check if an entry is already in the table; if yes, return it;
   if not, extend the table with a new entry and return the new entry. */
extern wHead* wHTInsert (wHeadTable*,wIdent,wIdent);

/* Create an edge (i.e., register a pushdown rule) between the first and
   the second wHead argument. The wIdent argument is the 'tail', the
   semiring value will be associated with the rule, and the function
   will add a reference to it. If the semiring defines a diff operation,
   the 'delta' field of the returned edge contains the difference between
   the given semiring value and the previous value of the edge. The last two
   arguments are used to keep track of the witness information, i.e. they
   should indicate which pushdown rule was responsible for this edge. These
   arguments are ignored when tracing the origins is switched off. */
extern wHeadEdge* wHTInsertEdge (wHeadTable*,wHead*,wHead*,wIdent,wSRvalue,
					wRule*,wWitnessNode*);

/* Add a transition at the front of the 'trans' list of the given head.
   If the transition is already in the 'trans' list of another head,
   the transition is removed from there first. */
extern void wHeadInsertTrans (wHead*,wTrans*);

/* Removes a transition from the 'trans' list of a given head. */
extern void wHeadRemoveTrans (wTrans*);

/* Delete the table and all its entries, including all the edges.
   Semiring  values on edges will be dereferenced. */
extern void wHTDelete (wHeadTable*);


/***************************************************************************/
/* Witness graph:                                                          */

/* Create a witness node for the given (transition,value) pair. This will  */
/* create a reference to the given value. The returned node will have a    */
/* reference count of 1.						   */
extern wWitnessNode* wWNCreate (wFA*,wTrans*,wSRvalue);

/* Increase the reference count of the given witness node.*/
extern void wWNRef (wFA*,wWitnessNode*);

/* Decrease the reference count of the given witness node. If the count
   reaches zero, the node and its outgoing edges will be freed (removing
   references to their semiring values), and the reference counts of its
   successors will be decreased recursively. */
extern void wWNDeref (wFA*,wWitnessNode*);

/* Create an edge in the graph with the given data. Creates a reference
   to the given semiring value. */
extern wWitnessEdge* wWECreate (wFA*,wWitnessNode*,wSRvalue,
				wRule*,wWitnessNode*,wWitnessNode*);

/* Copy a witness edge (creating additional references as appropriate). */
extern wWitnessEdge* wWECopy (wFA*,wWitnessEdge*,wWitnessNode*);

/* Delete a witness edge, deref its targets (used by wWNDeref). */
extern void wWEDelete (wFA*,wWitnessEdge*);

/***************************************************************************/
/* Witness recovery:							   */

/* Free the memory used by some path and dereference all values, paths,
   and wnodes pointed to by it. */
extern void wPathDeleteHash (void*);

#endif
