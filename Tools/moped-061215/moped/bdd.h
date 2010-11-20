/***************************************************************************/
/* Data structures and functions related to BDDs.			   */

#ifndef BDD_H
#define BDD_H

#include <stdio.h>
#include <cudd.h>
#include <wpds.h>
#include <wpdsInt.h>

#include "alloc.h"

/*************************************************************************** 
		DATA STRUCTURES
 ***************************************************************************/

/* The following data structure represents a 'weighted Pds with
   BDDs as weights. A structure of this type should be generated
   using the bddPdsCreate procedure (see below). */

typedef struct bddPds
{
	wPDS	*pds;		/* The pds (essentially a list of rules). */
	wConfig *start;		/* Initial configuration. We assume that
				   the BDDs at the initial configuration
				   are `true'. */

	/* Additional information about the BDD encoding: */
	DdManager *mgr;		/* The BDD manager. */
	int	num_globals;	/* Number of local and global boolean vars. */
	int	num_locals;	  

	/* The BDD variables used to encode all the BDDs: */
	DdNode  **g0, **g1, **g2;	/* Three copies for each global var. */
	DdNode  **l0, **l1, **l2, **l3; /* Four copies for each local var.   */
	DdNode  *one, *zero;	/* The BDD nodes for 'true' and 'false',
				   respectively. */
	struct bddHelpers *helpers;
				/* Additional structures that may be helpful.
				   These are set up by bddPdsCreate. */
} bddPds;

/* A procedure of this type can be passed to bddCreatePds to generate
   a user-defined variable ordering. The bddPds parameter specifies a
   number of global and local variables. The procedure is expected
   to generate three copies of each global and four copies of each local
   variable, and set the g0..g2 and l0..l3 fields to the generated
   BDD variables. The second argument is user-defined data (see bddCreatePds).
*/
typedef void (*bddShuffleProc)(bddPds*,void*);

/***************************************************************************/

/* Specification of a model-checking problem: */
/* Added by Remy: PROBLEM_REACHABILITY_DPN */

typedef enum { PROBLEM_REACHABILITY, PROBLEM_REACHABILITY_PRE, PROBLEM_LTL,
	PROBLEM_REACHABILITY_DPN, PROBLEM_REACHABILITY_ADPN,
	PROBLEM_REACHABILITY_2, PROBLEM_REACHABILITY_PRE_2} bddProblemType;

typedef struct bddProblem
{
	bddProblemType type;	/* LTL not yet implemented */
	wIdent state;		/* For reachability, the state/stack pair */
	wIdent stack;		/* whose reachability is to be tested.    */
} bddProblem;

/***************************************************************************/

/* The result of the model-checking algorithm: (see bddResult below) */

typedef enum { RESULT_YES, RESULT_NO } bddResultType;

/* Data attached to a stack element of a configuration in a counterexample.
   refcount is for internal use, do not touch.
   The stack is formed by following the rest_path pointer. */
typedef struct bddPath
{
	wIdent stack;
	DdNode *bdd;	/* values encoded with l0 variables */
	int refcount;
	void *user;	/* set to NULL; free for application use */
	struct bddPath *rest_path;
} bddPath;

/* A configuration in a counterexample.
   Configurations form a linked list connected by the next_config field.
   stack points to the stack contents (another linked list, see above). */
   
typedef struct bddConfig
{
	wIdent state;
	DdNode *bdd;	/* values encoded with g0 variables */
	wRule *rule;	/* the rule used to go to the next configuration;
			   NULL if next_config is also NULL */
	struct bddPath *stack;
	struct bddPath *prefix;
	struct bddConfig *next_config;
} bddConfig;

/* The data returned by the model-checking algorithms. */
typedef struct bddResult
{
	bddResultType yesno;	/* either RESULT_YES or RESULT_NO */
	bddConfig *prefix;	/* prefix of counterexample, if appropriate */
	bddConfig *loop;	/* loop of counterexample, if appropriate */
				/* loop==NULL for reachability */
} bddResult;

/***************************************************************************/

#include "bddInt.h"

/*************************************************************************** 
		FUNCTION DECLARATIONS
 ***************************************************************************/

/* bddInit must be called once before any other function
   from this module. */
extern void bddInit ();
/* bddFinish should be called in the end to release
   global data structures allocated by bddInit. */
extern void bddFinish ();

/* bddDebug runs CUDD's debugging functions and returns their results.
   In addition, some message are directly printed to the standard output.

   After bddDebug, the integers pointed to by the first and second argument
   contain the result of Cudd_CheckDebug and Cudd_CheckKeys, respectively.
   Non-zero values indicate inconsistencies in the BDD tables (and a bug
   in the program).

   The third argument gets the result of Cudd_CheckZeroRef, which is the
   number of BDD nodes with a non-zero reference count. This number ought
   to be zero immediately before calling bddFinish. Non-zero values indicate
   potential memory leaks.

   Any of the three arguments may be NULL, in which case the corresponding
   function is executed, but its result is not evaluated. */
extern void bddDebug (int*,int*,int*);

/* bddPdsCreate generates a new (empty) "pushdown system with BDDs".
   The first argument is the initial configuration (the procedure 
   generates a new copy of the configuration). The two integer arguments
   are the numbers of global and local BDD variables, respectively.
   The third argument is a procedure that is expected to generate the
   variable ordering. The fourth argument is user-defined data;
   bddPdsCreate will pass it on to the shuffle procedure. If
   the shuffle procedure is NULL, a default ordering will be used. */
extern bddPds* bddPdsCreate (wConfig*,int,int,bddShuffleProc,void*);

/* Add a new rule to a pushdown system. The first two identifiers are
   the (state,stack symbol) pair on the left-hand side, the following
   three identifiers are the right-hand side. One or both of the stack
   symbols on the right-hand side may be 0 to indicate a right-hand side
   of length 1 or 0, respectively.

   The last argument is the BDD attached to this rule. The BDD is expected
   to be encoded using the variables given in the bddPds structure. More
   precisely, the
    the left-hand control state   should be encoded with variables from g0,
    the left-hand stack symbol    should be encoded with variables from l0,
    the right-hand control state  should be encoded with variables from g1,
    the first right stack symbol  should be encoded with variables from l1,
    the second right stack symbol should be encoded with variables from l2.
   bddPdsInsert will generate its own reference to the BDD, therefore the user
   may release the BDD after calling the procedure.
*/
extern wRule* bddPdsInsert (bddPds*,wIdent,wIdent,wIdent,wIdent,wIdent,DdNode*);

/* The main model-checking procedure: */
extern bddResult* bddAnalyse (bddPds*,bddProblem*);

/* Print a result. */
extern void bddPrintResult (bddPds*,bddResult*);

/* Delete a result (release all the memory claimed by it). */
extern void bddDeleteResult (bddPds*,bddResult*);

/* Delete the PDS (release all the memory claimed by it).
   (This will not remove the generated BDD variables from the BDD manager.) */
extern void bddPdsDelete (bddPds*);

#endif
