/***************************************************************************/
/* External functions and data structures of the WPDS library.		   */

#ifndef WPDS_H
#define WPDS_H

/***************************************************************************/
/* "Compatibility" macros for earlier versions of the library              */
/* These may be eliminated in future versions.                             */
/* #include "compat.h" */

/*************************************************************************** 
		DATA STRUCTURES
 ***************************************************************************/

/***************************************************************************
 Semirings:

 A semiring is a collection of function pointers with the following meanings:
 * The extend, combine, and diff functions take two semiring elements as
   arguments and return the result of applying the extender, combiner, or
   difference operator, respectively.
 * Function one() should return the neutral element of the extender.
 * Function zero() should return the neutral element of the combiner
   (which must also be an annihilator of the extender).
 * The quasione function takes a semiring element (say d) as its argument
   and should return a 'quasione' of d, i.e. some element e satisfying
   the property extend(d,e) = d.
 * The value returned by eq(a,b) should be non-zero if semiring elements
   a and b are equal, and zero otherwise.

 For cases where semiring values are complex memory structures, the WPDS
 library supports reference counting. The user can keep reference counts on
 each semiring element and discard an element when its count reaches zero.
 ref should increase the count, deref should decrease it. Both functions
 may be NULL in which case the WPDS library will not use them. It is assumed
 that the functions extend, combine, diff, one, zero, and quasione bump the
 reference count of the elements they return!

 It is good practice to have reference counts saturate at some fixed
 quantity (related to the amount of space provided in each value to hold
 the reference count) and to have one and zero be elements whose reference
 counts are already at the saturated value.

 The function diff is used only in differential mode; otherwise, the pointer
 for it may be NULL. The quasione pointer can safely be set to NULL; in that
 case, the one() function is used instead.
*/

typedef void* wSRvalue;

typedef struct wSemiring
{
	wSRvalue (*extend)(wSRvalue,wSRvalue);
	wSRvalue (*combine)(wSRvalue,wSRvalue);
	wSRvalue (*diff)(wSRvalue,wSRvalue);
	wSRvalue (*one)();
	wSRvalue (*zero)();
	wSRvalue (*quasione)(wSRvalue);
	int      (*eq)(wSRvalue,wSRvalue);
	void     (*ref)(wSRvalue);
	void     (*deref)(wSRvalue);
} wSemiring;


/***************************************************************************
 Identifiers:

 In the library, identifiers serve to identify automaton states, control
 locations, stack symbols etc. Internally, these are represented by integers.
 For convenience, the library provides hash functions from integers to strings
 and back, allowing the user to associate a descriptive name with each
 identifier. Each distinct string is associated with a unique identifier.
*/

typedef long wIdent;


/***************************************************************************

 The core functionality of the library consists of two functions: weighted
 pre* and weighted post*. The library provides two interfaces to them, a
 ready-made, easy-to-use interface, and a more sophisticated one.

 The "ready-made" interface allows to define a weighted PDS and a weighted
 automaton (w.r.t. to the same semiring), fill these with pushdown rules
 and transitions, apply a pre* / post* primitive, and obtain the resulting
 automaton. For more information on this interface, refer to the following
 sections on pushdown systems, finite automata, and the ready-made interface.

 Occasionally, the user may wish to fine-tune the way pre* and post* work,
 usually in order to increase efficiency. This is what the more sophisticated
 interface is for. The basic concept here is that of a "saturation process":
 a saturation process can be fed with pushdown rules and automaton transitions
 (in arbitrary order), and produces automaton transitions as a result. You
 may prefer to use this interface if you want to, e.g.
  * change the order in which transitions are processed (the ready-made
    procedures use a LIFO queue); this makes sense if your semiring has a
    total ordering, or if you want to implement search guidance heuristics;
  * construct the pushdown system on the fly;
  * stop the computation as soon as the interesting part of the result
    is known (e.g. a particular transition is produced).
 For more information on this interface, see "saturation processes" below.

 ***************************************************************************/

/****************************************************************************
 Pushdown rules / Pushdown systems:

 The data structure for pushdown rules includes (apart from identifiers for
 control locations and stack symbols) a semiring value and a 'user' field
 which the library user can use to store additional information.

 A (weighted) pushdown system is simply a collection of rules with a common
 semiring (the set of control locations and stack symbols is derived
 implicitly). The rules/next pointers can be used to iterate over the rules.
*/

typedef struct wRule
{
	wIdent from_state;	/* left-hand-side control location	*/
	wIdent from_stack;	/* left-hand-side stack symbol		*/
	wIdent to_state;	/* right-hand-side control location	*/
	wIdent to_stack1;	/* right-hand-side first stack symbol	*/
	wIdent to_stack2;	/* right-hand-side second stack symbol	*/
				/* to_stack1 and to_stack2 are expected
				     to be 0 if not used		*/
	wSRvalue label;		/* semiring value associated with rule  */
	struct wRule *next;	/* use this to iterate over transitions */

        void  *user;            /* Set to NULL by wPDSInsert;
                                   otherwise, free for application use. */

	wIdent midstate;	/* internal use (by sat processes) */
	
	// Added by Remy
	int shared;			/* shared rule? (for ADPN) */
} wRule;

typedef struct wPDS
{
	wSemiring *sr;		/* the semiring  */
	wRule *rules;		/* list of rules */
	void *user;		/* set to NULL by wPDSCreate;
					available for application use.  */
} wPDS;


/***************************************************************************
 Automaton rules / Finite automata:

 An automaton transition includes identifiers for source state, input symbol,
 target state, and an associated semiring value. The 'user' field is not
 used by the library, and the user may store additional information in it.

 A (weighted) automaton is a collection of transitions with a common
 semiring. The transitions/next pointers can be used to iterate over the
 transitions.
*/

typedef struct wTrans
{
	struct wTrans *next_hash;	/* internal use (hash table) */
	unsigned long hash;		/* internal use (hash table) */

	wIdent from, name, to;	/* source state, symbol, target state	   */
	wSRvalue label;		/* semiring value 			   */
	wSRvalue delta;		/* delta value for differential mode	   */
	struct wTrans *next;	/* for listing all transitions in the FA   */
	struct wWitnessNode *wnode;	/* witness information for 'label' */

	void *user;		/* Set to NULL by wFAInsert;
				   otherwise, free for application use.    */

	struct wTrans *stack;		/* internal use (by sat processes) */
	struct wTrans **stack_prev;	/* ditto			   */
} wTrans;

typedef struct wFA
{
	wSemiring *sr;			/* the semiring                    */
	wTrans    *transitions;		/* list of transitions             */
	char	   dir;			/* pre* or post* automaton         */
	struct wHash *hash;		/* hash for transitions (internal) */
	struct wHash *paths;		/* hash for paths (internal)       */
	void *user;			/* set to NULL by wFACreate;
					   available for application use.  */
} wFA;


/***************************************************************************
 Saturation processes:
 The elements of the data structure are mostly for internal usage, but
 see wpdsInt.h if you want to know more.
*/

typedef struct wSat wSat;


/***************************************************************************
 Configurations and Path (witness generation):
 A configuration simply consists of a control location and a zero-terminated
 array of stack symbols. The wPath structure represents an accepting path in
 some finite automaton, corresponding to a configuration accepted by the
 automaton. In general, a configuration may have multiple accepting paths.

 More precisely, a wPath is a path coupled with a semiring value for each
 transition on the path that has arisen at some point in the computation
 history of the automaton. In general, transitions on the path may have
 assumed different values during the computation.

 The 'value' field of the structure contains the sum (w.r.t. to the extender
 operation) of the transitions on the path. If a path is generated multiple
 times by the library operations, its memory address is reused. (This is
 important because in general, a witness set may be a dag.) Therefore, the
 library supports reference counts on paths.

 A path may be equipped with a list of wPathTrans structures. These contain
 pointers to successor/predecessor paths (depending on whether the automaton
 in question is the result of a pre* or a post* automaton). Such lists are
 generated by the wPathTraceStep/All functions.

 A wPath* value of NULL represents an empty path. If wPathTraceStep/All
 return a transition to a NULL path (with a NULL rule), it means that the
 source path was part of the initial automaton (the one to which a pre* or
 post* operation has been applied).
*/

typedef struct wConfig {
	wIdent state;			/* the control location   */
	wIdent *stack;			/* array of stack symbols */
} wConfig;

typedef struct wPath {
        struct wPath *hashnext;
        unsigned long hash;

        struct wWitnessNode *wnode;	/* pointer to transition/value pair */
        wSRvalue *value;		/* 'sum' of values in path  	    */
        int refcount;			/* reference counter		    */
        void *user;     		/* free for application use 	    */

        struct wPath *next_path;	/* the rest of the path     	    */
	struct wPathTrans *transitions;	/* list of predecessors/successors  */
} wPath;

typedef struct wPathTrans {
	struct wPath *target;		/* the predecessor/succession config */
	struct wRule *rule;		/* rule used to get (from) there     */
	struct wPathTrans *next;	/* next transition (of the source)   */
} wPathTrans;


/*************************************************************************** 
		FUNCTION DECLARATIONS
 ***************************************************************************/

/***************************************************************************/
/* General functions:							   */

/* wInit initializes the package and should be called once before any other
   function of the library. */
extern void wInit ();

/* wFinish reclaims the memory used by the package; this will usually be
   called at the end of the program. */
extern void wFinish ();


/***************************************************************************/
/* Identifier functions:						   */

/* Return an identifier associated with the given string. Calling the function
   twice with the same argument will produce the same identifier. The function
   calls strdup() on the argument, which means that the user can release the
   string afterwards without causing harm. */
extern wIdent wIdentCreate (char*);

/* Map an identifier to its string (the reverse of wIdentCreate). */
extern char* wIdentString (wIdent);

/* Some functions (such as wReadout and wConfigValue) need to know which
   automaton states are initial or final. By convention, identifiers are
   deemed to designate initial states if their names start with an alpha-
   numeric character; the names of final states are assumed to end with
   two underscores (__). */
extern char wIsInitial (wIdent);
extern char wIsFinal (wIdent);

/* Returns the number of identifiers in the hash */
extern long wIdentNum();


/***************************************************************************/
/* Functions for maintaining (weighted) pushdown systems.                  */

/* Create an empty pushdown system (i.e. one without any rules), which is
   associated with a semiring. The operations specified in the semiring will
   be used in wPDSInsert and wPDSDelete. 				   */
extern wPDS* wPDSCreate (wSemiring*);

/* Create a rule inside a weighted PDS. The wIdent arguments are the left- 
   hand-side and right-hand-side symbols in the same order as in wRule, the
   last argument is the semring value. Zero (0) should be used to indicate
   missing stack symbols. The function will create a reference to the
   semiring value. The user field of the rule will be NULL. */
extern wRule *wPDSInsert (wPDS*, wIdent, wIdent,
				 wIdent, wIdent, wIdent, wSRvalue);

/* Free the memory used by the PDS and its rules; semiring values
   associated with rules will be dereferenced. */
extern void wPDSDelete (wPDS*);

extern int* wPDSSharedRule(wPDS*);

/***************************************************************************/
/* Functions for maintaining (weighted) automata.                          */

/* Create an empty automaton (initially without any transitions). The
   given semiring will be used in all future operations on transitions. */
extern wFA* wFACreate (wSemiring*);

/* Create a transition inside an automaton with the specified semiring value.
   If transition is already in the automaton, the new value will be combined
   with the one that is already there. The function returns a pointer to the
   transition inside the automaton. Unless the last argument is NULL, upon
   return the integer pointed to by it will be 1 if and only if either the
   transition was not already in the automaton, or the previous value of
   the automaton did not subsume the new value (i.e. the operation did
   actually change the automaton). Moreover, if the semiring has a diff
   operation, the 'delta' field of the returned transition contains the
   difference between the given semiring value and the previous value of
   the transition. */
extern wTrans* wFAInsert (wFA*, wIdent, wIdent, wIdent, wSRvalue, int*);

/* Locate a transition with the given source state, input symbol, target
   state in the automaton (returns NULL if not found). */
extern wTrans* wFAFind (wFA*, wIdent, wIdent, wIdent);

/* Delete an automaton, reclaiming the memory. All transitions will be
   freed and their semiring values dereferenced. */
extern void wFADelete (wFA*);


/***************************************************************************/

/* whether and how to manage witness trace information */
enum { TRACE_NO, TRACE_COPY, TRACE_REF, TRACE_REF_TOTAL };
#define TRACE_YES TRACE_COPY

/* Ready-made functions to compute pre* or post* for the given finite
   automaton, w.r.t. to the given weighted PDS. The resulting automaton
   is returned. The computation will be carried out in differential mode
   if the semiring associated with the PDS and the FA supplies a diff and
   zero operation.

   Unless the third argument is TRACE_NO, the computation maintains witness
   trace information, and the argument describes exactly how witness nodes
   will change when the value of a transition is updated. Suppose transition t
   currently has value d, and its witness node W has incoming edges E. Moreover,
   suppose the transition is updated with d' such that d+d'<d. Thus, t gets a
   new witness node W' with value d+d' and at least one incoming edge with
   label d'.
   
   TRACE_COPY stands for the method from [RSJM05], i.e. for any element e of E
   with value d'' s.t.  d'<d'' does not hold, W' additionally gets a copy of e.

   If TRACE_REF is used, then W' gets no additional edges, and W'->previous
   references W.

   If TRACE_REF_TOTAL is used, then W' gets no additional edges, and
   W'->previous is set to W only if d+d' < d', otherwise to NULL.

   Note that for semirings with a total order, TRACE_COPY and TRACE_REF_TOTAL
   will have the same behaviour, but TRACE_REF_TOTAL skips some unnecessary
   checks. For partially-ordered semirings TRACE_REF is faster but obsolete
   witness nodes will never be freed.

   TRACE_YES is kept around for backward compatibility, but is subject to
   change and should not be used anymore.
*/
extern wFA* wPrestar (wPDS*,wFA*,char);
extern wFA* wPoststar (wPDS*,wFA*,char);


/***************************************************************************/
/* Functions for saturation processes.					   */

/* Essentially, a saturation process keeps track of a set of pushdown rules
   and automaton transitions, and produces new automaton transitions
   according to the pre* or post* saturation rules (see the literature for
   details).

   Seen from the outside, it consists of two parts: an augmentation procedure
   and a worklist. The worklist is a LIFO queue, and the user may add to or
   remove transitions from it. Moreover, the user may invoke the augmentation
   procedure with either a pushdown rule or a transition (previously removed
   from the worklist). The procedure then computes the new transitions made
   possible by that rule or transition, and puts them into the worklist. */

enum { SAT_PRE, SAT_POST };	/* direction of computation */

/* Initialize a saturation process. Operations on values will be carried out
   using the given semiring. The second argument should be one of SAT_PRE
   and SAT_POST, for pre* or post* computations, respectively. Computation
   will be carried out in differential mode if the semiring supplies diff
   and zero operations. The third argument should one of the TRACE_xxx values
   and determines if and how witness trace information will be managed. */
extern struct wSat* wSatCreate (wSemiring*,char,char);

/* Add a transition (with associated semiring value) to the worklist.
   Returns the newly created (or updated) transition. */
extern wTrans* wSatAddTrans (wSat*,wIdent,wIdent,wIdent,wSRvalue);

/* Remove a transition from the worklist. */
extern wTrans* wSatPopTrans (wSat*);

/* Call the augmentation procedure with a transition. The transition must
   be the result of an earlier call to wSatPopTrans. */
extern void wSatAugTrans (wSat*,wTrans*);

/* Call the augmentation procedure with a pushdown rule. The order of
   arguments is the same as in wPDSInsert. The last argument is used
   in witness trace information generated from this rule. */
extern void wSatAugRule (wSat*,wIdent,wIdent,
				wIdent,wIdent,wIdent,wSRvalue,wRule*);

/* Terminate a saturation procedure, releasing all memory except for the
   resulting finite automaton, which is returned by the function. */
extern wFA* wSatDelete(wSat*);


/***************************************************************************/
/* Evaluation functions.						   */

/* Takes an automaton A as input and produces another automaton A' as output.
   A' has one final state (__end__) and a transition (p,y,__end__) for every 
   pair (p,y) such that p is an initial state of the automaton, and there
   exists at least on transition of the form (p,y,q) in A. The value attached
   to (p,y,__end__) is the combination of all values on accepting paths for
   configurations <p,y w>, where w is arbitrary stack content. The computation
   of A' is done differentially when a diff operator is defined.

   In program analysis terms, the result corresponds to that of a conventional
   dataflow analysis, where dataflow facts for a program location are combined
   regardless of stack context.

   Note: For this to work properly, states need to be named according to the
	 conventions stated in the comments for wIsInitial and wIsFinal. */
extern wFA* wReadout (wFA*);

/***************************************************************************/
/* Configurations							   */

/* Create a configuration struct from a sequence of identifiers. The first
   identifier is the control location (state), followed by a zero-terminated
   sequence of identifiers. For example, if p,y1,y2 are identifiers, the
   call wConfigValue(A,p,y1,y2,0) returns the configuration <p,y1 y2>. */
extern wConfig* wConfigCreate (wIdent,wIdent,...);

/* Generate a new copy of a configuration. */
extern wConfig* wConfigCopy (wConfig*);

/* Frees the memory use by a configuration. */
extern void wConfigDelete (wConfig*);

/* Compute the value associated with some configuration in the an automaton.
   The returned value is the sum (w.r.t. combine) over all accepting paths
   for the given configuration. (See wIsFinal for instructions on how to mark
   final states.) The function creates a reference to its return value. */
extern wSRvalue wConfigValue (wFA*,wConfig*);


/***************************************************************************/
/* Witness recovery.							   */

/* The following functions will work properly only if the automaton in
   question has been created by a call to wPrestar, wPostStar, or some
   saturation process with trace generation enabled. */

/* Build a path structure from an array of transitions. The transitions
   should describe an accepting path through the automaton. The last entry
   should be a NULL pointer. The function generates a reference to the path
   it returns. */
extern wPath* wPathFromArray (wFA*,wTrans**);

/* As an alternative to wPathFromArray, wPathCreate generates a path
   structure from a sequence of transitions. The last argument must
   be a NULL pointer. E.g.,
	wPathCreate(fa,wFAFind(fa,p,y1,q),wFAFind(fa,q,y2,__end__),NULL)
   returns a wPath structure for the path p -[y1]-> q -[y2]-> __end__
   in the automaton fa. The function generates a reference to the path
   it returns. */
extern wPath* wPathCreate (wFA*,wTrans*,...);

/* Find all accepting paths for the given configuration in fa. The function
   returns a dummy (or "anchor") wPath struct that does not represent any path,
   but contains a transition to each path that was found. The 'value' field of
   the anchor contains the "sum" (w.r.t. combine) over all paths. Also, a
   reference to the anchor is generated. */
extern wPath* wPathFind (wFA*,wConfig*);

/* Return the configuration accepted by the given path. */
extern wConfig* wPathConfig (wFA*,wPath*);

/* Increase/Decrease the reference count of a path. The user should call
   wPathDeref on the results of wPathCreate/Find once the results are no
   longer needed (otherwise the results will stay in memory).  */ 
extern void wPathRef (wFA*,wPath*);
extern void wPathDeref (wFA*,wPath*);

/* Populate the 'transitions' field of the given path. In a pre* automaton, this
   will identify successors; in a post* automaton, predecessors will be found,
   i.e. the function moves against the direction of the pre* or post* query. */
extern void wPathTraceStep (wFA*,wPath*);

/* Build the complete witness history of the path in question,
   i.e. call wPathTraceStep recursively until initial configurations
   are reached in all brances of the witness tree (or dag). */
extern void wPathTraceAll (wFA*,wPath*);

#endif
