#ifndef ABSREF_H
#define ABSREF_H

#include "bdd.h"

bddResult* bddPostReachability (bddPds *pds, wIdent state, wIdent stack);
wFA* bddComputePoststar (bddPds *pds, wFA *fa,
			 wIdent state, wIdent stack, wTrans **found);
bddTrans* bddPickConfig (wFA* fa, wTrans *start);

long wident_globals_used;

typedef enum ruleType {
	RULE_NORMAL,
	RULE_PUSH,
	RULE_POP,
	RULE_COMBINE,
	RULE_EVAL,
	RULE_ENTRY,
} ruleType;

ruleType *pRULE_NORMAL, *pRULE_PUSH, *pRULE_POP,
	*pRULE_COMBINE, *pRULE_EVAL, *pRULE_ENTRY;


typedef enum locType {
	LOC_NOLOC,
	LOC_EVAL,
	LOC_NORMAL,
	LOC_ENTRY,
	LOC_CALL,
	LOC_GLOBAL
} locType;

typedef struct predicateList
{
	DdNode* J;       /* concretization */
	int id;
	struct predicateList* next; 
} predicateList;

typedef struct locationInfo
{
	int id;
	locType type;        /* the type of the location   */
        int nPreds;          /* the number of local preds  */
	predicateList *preds;/* the list of local preds    */
	DdNode *coll0, *coll1; /* collected abstract-concrete equivalences */
	int collFlag;
} locationInfo;
	
typedef struct wIdentList
{
	wIdent wId;
	struct wIdentList *next;
} wIdentList;

typedef enum analyseResult
{
	ABS_RES_NO_ERROR,
	ABS_RES_SPURIOUS_CE,
	ABS_RES_REAL_CE
} analyseResult;

typedef struct absRefHelpers
{
	DdNode *eqv_G0G1_L0L1;
	DdNode *eqv_G1G2_L1L3;
	int *c_perm_G0_G1;
	int *c_perm_G0G1L1_G1G2L3;
	DdNode *cube_G0;
	DdNode *cube_G1;
	DdNode *cube_G0L0;
	DdNode *cube_G1L1;
	DdNode *cube_G2L3;
	DdNode *cube_G0L0L2;
	DdNode *cube_G1L1L2;
	DdNode *cube_G2L0L3;
	DdNode *cube_G0G1L1;
	DdNode *cube_G1G2L3;
	DdNode *cube_G1G2L1L3;

	DdNode *cube_g0g1g2l0l1l2l3;
	DdNode *cube_g2l3;
	/* int *a_perm_l1_l2; */
} absRefHelpers;


bddResult* absRef (bddPds *concretePds, wIdent state, wIdent stack);

void assert_(int cond, char* file, int line);
#define assert(cond) assert_(cond,__FILE__,__LINE__)

absRefHelpers *helpers;

wIdentList *evalSymbols;
wIdentList *entrySymbols;

bddPds *concPds;
wHeadTable *concHeadTable;

locationInfo **locInf; /* data relevant to each location (stack symbol) */
int NLocs;             /* the number of different locations */
wIdent *locs;          /* array of length NLocs */

locationInfo *globInf; /* data relevant to the global preds */
int maxLPreds;         /* max number of local predicates */

DdManager *mgr;
DdNode *one;
DdNode *zero;

static const int DEBUG = 0;
static const int VERBOSE = 0;
static const int SHORTCUT = 0;

static const int I_VALUES = 0;
static const int J_VALUES = 1;

static const int LAZY = 1;

static const int PRESERVE_HEUR     = 2;
/* 0 means no preserve
   1 means general preserve (as often as possible)
   2 means DAG preserve
*/ 
static const int STRONG_J_HEUR     = 2;
/* 0 means no strengthening
   1 means a single strengthening
   2 means conciliating strengthening
   3 means maximal strengthening (depending on variable order)
*/
static const int WEAK_I_HEUR       = 0;


#endif
