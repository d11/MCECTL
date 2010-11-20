#ifndef PRE_H
#define PRE_H

#include <wpds.h>
#include "int.h"

/** Data structures used to construct call graph. **/
typedef struct callNode {

	wIdent  name;
	struct callNodeList *callTo;
	struct callNodeList *calledBy;
} callNode;

typedef struct callNodeList {

	callNode *node;
	struct callNodeList *next;
} callNodeList;
/***************************************************/

/* A call graph is actually a linked list of all call nodes. */
#define callGraph callNodeList

/* Perform pre* analysis when problem->type == PROBLEM_REACHABILITY_PRE_2.
   The procedure is supposed to be called from intAnalyse. */
extern bddResult* twoAnalyse(intPds*, intProblem*);

#endif
