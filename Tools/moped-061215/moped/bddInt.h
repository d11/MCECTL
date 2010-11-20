/***************************************************************************/
/* Data structures and functions related to BDDs (internal).		   */

#ifndef BDDINT_H
#define BDDINT_H

/*************************************************************************** 
		DATA STRUCTURES
 ***************************************************************************/

struct bddPds;

enum {
	SIG_G0_L0_G1,		/* rules use these three */
	SIG_G0_L0_G1_L1,
	SIG_G0_L0_G1_L1_L2,

	SIG_G1_L1_G2,		/* pre* transitions use this one */

	SIG_G0_L0_G2_L3,	/* post* transition from initial state,
				   L3 is optional  */
	SIG_G1_L1_L0_G2_L3,	/* post* transition from non-initial state,
				   L1 and L3 are optional */
	SIG_G0_G1_L1,		/* epsilon transition, L1 is optional */

	/* The following are needed during trace generation. */
	SIG_G2_L3,		/* for target states, L3 optional */
	SIG_G0_L0_L2_G2_L3,	/* combined values of two transitions (post*) */
	SIG_G1_L1_L2_G2,	/* same for pre* */

	SIG_DONT_CARE		/* this is a hack for "no bdd" */
};

/* alternative names for the above: */
#define SIG_RULE0    SIG_G0_L0_G1
#define SIG_RULE1    SIG_G0_L0_G1_L1
#define SIG_RULE2    SIG_G0_L0_G1_L1_L2
#define SIG_PRETRANS SIG_G1_L1_G2
#define SIG_POST_INI SIG_G0_L0_G2_L3
#define SIG_POST_NON SIG_G1_L1_L0_G2_L3
#define SIG_EPSILON  SIG_G0_G1_L1
#define SIG_STATE    SIG_G2_L3
#define SIG_POST_TWO SIG_G0_L0_L2_G2_L3
#define SIG_PRE_TWO  SIG_G1_L1_L2_G2

typedef struct bddHelpers
{
	DdNode	*cube_g1l1;
	DdNode	*cube_g0l0;
	DdNode	*cube_l0g2l3;
	DdNode  *cube_l2;

	DdNode	*eqv_g0g2_l0l3;

	int	array_size;	/* Cudd_ReadSize at initialisation */
	int	*perm_g0l0_g1l1;
	int	*perm_g2l2_g1l1;
	int	*perm_g0l0g1_g1l1g2;

	int	*perm_g1l1_g0l0;
	int	*perm_g1g2l3_g0g1l1;
	int	*perm_l2_l0;
	int	*perm_l0_l2;

	int	*select_pretrans;
	int	*select_post_ini;
	int	*select_post_non;
	int	*select_state;
} bddHelpers;

typedef struct bddDomain
{
        struct bddDomain *next;		/* These two fields are needed     */
        unsigned long hash;		/* by the hashing procedures.	   */

	int	refcount;

	int	signature;		/* Any change in this structure   */
	struct  bddPds	*pds;		/* must be reflected in the CRC   */
	DdNode	*bdd;			/* computation in bddAddToHash!   */

} bddDomain;

/***************************************************************************/

/* The internal structure for preparing the result. */
typedef struct bddTrans
{
	wTrans *trans;
	bddDomain *values;
	wRule *rule;
	int refcount;
	struct bddTrans *prefix;
	struct bddTrans *rest_path;
	struct bddTrans *next_config;
} bddTrans;

typedef struct bddState
{
	wIdent state;
	bddDomain *values;
	struct bddState *next;
} bddState;

/***************************************************************************/

extern wSemiring bddSemiring;

extern void bddDomInit ();
extern void bddDomFinish ();

extern bddDomain* bddDomGenerate (int,bddPds*,DdNode*);
extern void bddDomRef (wSRvalue);
extern void bddDomDeref (wSRvalue);
extern int bddEmpty (bddDomain*);
extern bddDomain* bddUnion (bddDomain*,bddDomain*);
extern bddDomain* bddIntersect (bddDomain*,bddDomain*);
extern bddDomain* bddDomConvert (bddDomain*);

/***************************************************************************/

extern wIdent bddFinalState ();
extern wFA* bddInitialAutomaton (bddPds*);
extern wFA* bddTargetAutomaton (bddPds*,wIdent,wIdent);
extern wFA* bddDpnAutomaton (bddPds*,wIdent,wIdent);
extern void bddFaDelete (wFA*);

/***************************************************************************/

extern void bddInitCube (DdManager*,int**);
extern void bddAddVarsToCube (DdManager*,int*,DdNode**,int);
extern DdNode* bddCreateCube (DdManager*,int*);
extern void bddInitPermutation (DdManager*,int**);
extern void bddAddToPermutation (DdManager*,int*,DdNode**,DdNode**,int);

extern void bddGenerateData (struct bddPds*,bddShuffleProc,void*);
extern void bddDeleteData (struct bddPds*);

/***************************************************************************/

extern void bddGetPostTrace (bddPds*,wFA*,wTrans*,bddResult*);
extern void bddGetPreTrace (bddPds*,wFA*,wTrans*,bddResult*);
extern void bddGetDpnTrace (bddPds*,wFA*,wTrans*,wTrans*,bddResult*);

/***************************************************************************/

#endif
