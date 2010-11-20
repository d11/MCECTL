/* =========================================================================

   Structs for Boolean Programs

   ====================================================================== */


#ifndef _BP_H_
#define _BP_H_

#include <wpds.h>
#include "bdd.h"

typedef struct bp_ident_element *bp_ident_t;
typedef struct bp_idref_element *bp_idref_t;
typedef struct bp_fun_element *bp_fun_t;
typedef struct bp_stmt_element *bp_stmt_t;
typedef struct bp_expr_element *bp_expr_t;
typedef struct bp_tree_element *bp_tree_t;
typedef struct bp_int_element *bp_int_t;
typedef struct bp_dep_element *bp_dep_t;
typedef struct bp_ptr_element *bp_ptr_t;
typedef struct bp_flow_element *bp_flow_t;
typedef struct bp_edge_element *bp_edge_t;
typedef struct bp_funnode_element *bp_funnode_t;
typedef struct bp_funedge_element *bp_funedge_t;
typedef struct bp_mask_element *bp_mask_t;

struct bp_ident_element
{
	wIdent varname;
	int index;
	short fun;
	bp_ident_t next;
};

struct bp_idref_element
{
	bp_ident_t ident;
	char primed;
	bp_expr_t expr;
	bp_idref_t next;
};

struct bp_fun_element
{
	wIdent funname;
	char num_returns;
	char num_args;
	short num_locals;
	short num_globals;
	bp_ident_t locals;
	bp_ident_t returns;
	bp_ident_t parms;
	bp_ident_t callbuf;
	bp_expr_t enforce;
	bp_funnode_t funnode;
	bp_stmt_t stmt;
	bp_fun_t next;
	bp_mask_t pe_mask;
	bp_mask_t se_mask;
	bp_mask_t is_mask;
};

struct bp_exprstmt_element
{
	bp_idref_t asgnlist;
	bp_idref_t fmllist;
	char no_label_given;
	wIdent label;
};

struct bp_control_element
{
	bp_expr_t expr;
	bp_stmt_t thenstmt;
	bp_stmt_t elsestmt;
};

typedef union {
	struct bp_exprstmt_element a;
	struct bp_control_element c;
} bp_stmtdata;
	
struct bp_stmt_element
{
	wIdent label;
	char no_label_given;
	char type;
	bp_stmtdata e;
	char *live;
	bp_flow_t fn1;
	bp_stmt_t next;
};

struct bp_expr_element
{
	char token;
	bp_expr_t left, right;
};

enum {
	BP_CONST, BP_IDENT,
	BP_NOT, BP_EQ, BP_NE,
	BP_AND, BP_OR, BP_IMP,
	BP_ND, BP_CHOOSE
};

enum {
	BP_ASSIGN, BP_CALL,
	BP_ASSERT, BP_ASSUME, BP_CONSTRAIN,
	BP_PRINT, BP_IF, BP_WHILE,
	BP_RETURN, BP_SKIP, BP_GOTO
};

struct bp_tree_element
{
	wIdent name;
	signed char bal;
	short fun;
	bp_ident_t id;
	bp_tree_t left;
	bp_tree_t right;
};

struct bp_int_element
{
	int index;
	bp_int_t next;
};

struct bp_dep_element
{
	int weight;
	char marked;
	short inedges;
	short companion;
	bp_ptr_t edges;
};

struct bp_ptr_element
{
	bp_dep_t dep;
	bp_ptr_t next;
};

struct bp_flow_element
{
	int distance;
	bp_edge_t edges;
	char *live;
	char in_worklist;
	bp_flow_t next_in_wl;
	bp_fun_t fun;
	wIdent label;
};

struct bp_edge_element
{
	bp_stmt_t stmt;
	bp_flow_t target;
	bp_edge_t next;
};

struct bp_funnode_element
{
	bp_fun_t fun;
	int lowpt, dfsnum, comp;
	char *usedglob;
	bp_funedge_t edges;
	bp_funnode_t stack;
	bp_funedge_t calls;
	bp_edge_t returns;
};

struct bp_funedge_element
{
	bp_funnode_t fn;
	bp_stmt_t stmt;
	bp_funedge_t next;
	bp_funedge_t nextcall;
};

struct bp_mask_element
{
	int nv;
	int *vars;
	int *perm;
};

extern bp_ident_t bp_globals;
extern bp_ident_t bp_decls;
extern bp_ident_t bp_allglobids;
extern bp_ident_t bp_globids;
extern bp_ident_t bp_locids;
extern bp_fun_t bp_functions;
extern bp_tree_t bp_nametree;
extern bp_tree_t bp_funtree;
extern bp_tree_t bp_labeltree;
extern int bp_numglobals;
extern int bp_maxrets;
extern int bp_maxlocs;
extern int bp_labelcount;
extern int bp_stepsize;
extern int *bp_savedglobals;
extern short bp_funcount;
extern DdNode **bddvars;
extern DdNode **pvars, **qvars, **ppvars;
extern DdNode **yvars, **ypvars, **yppvars, **ypppvars;
extern int *mc_shuffle, *mc_rev_shuffle;
extern int GLOBALS,LOCALS;

extern int mc_globals;
extern int mc_encoding;
extern int mc_elimdead;
extern int mc_sepvar;
extern int mc_queue;
extern wIdent mc_reach_target_label;

#define alloc_bp_ident() (malloc(sizeof(struct bp_ident_element)))
#define alloc_bp_idref() (malloc(sizeof(struct bp_idref_element)))
#define alloc_bp_fun() (malloc(sizeof(struct bp_fun_element)))
#define alloc_bp_stmt()  (malloc(sizeof(struct bp_stmt_element)))
#define alloc_bp_expr() (malloc(sizeof(struct bp_expr_element)))
#define alloc_bp_tree() (malloc(sizeof(struct bp_tree_element)))
#define alloc_bp_int() (malloc(sizeof(struct bp_int_element)))
#define alloc_bp_ptr() (malloc(sizeof(struct bp_ptr_element)))
#define alloc_bp_flow() (malloc(sizeof(struct bp_flow_element)))
#define alloc_bp_edge() (malloc(sizeof(struct bp_edge_element)))
#define alloc_bp_funnode() (malloc(sizeof(struct bp_funnode_element)))
#define alloc_bp_funedge() (malloc(sizeof(struct bp_funedge_element)))
#define alloc_bp_mask() (malloc(sizeof(struct bp_mask_element)))

#define new_bp_stmt(var,typ) \
	((var = alloc_bp_stmt())->type = typ, \
	  var->next = NULL, var->live = NULL)
#define new_bp_expr(var,tok,lt,rt) \
	(var = alloc_bp_expr(), var->token = tok, \
	var->left = lt, var->right = rt)

#define call_crosses_comp(fun,target) \
	(mc_globals && fun->num_globals != target->num_globals)
#define stepwise(num) (bp_stepsize && num > bp_stepsize)
//#define stepwise(num) 0

extern signed char bp_insert_decl (bp_tree_t*,wIdent,short,bp_ident_t);
extern bp_ident_t bp_lookup (bp_tree_t,wIdent,short);
extern bp_ident_t bp_find_var (bp_tree_t,wIdent,short);
extern bp_ident_t bp_find_decl (wIdent,short);
extern int bp_assign_glob_index (wIdent);
extern int bp_assign_loc_index (wIdent, short);
extern int bp_assign_parm_index (wIdent, short);
extern int bp_assign_ret_index (wIdent, int);
extern int bp_assign_cb_index (wIdent, short, int);

extern void bp_elim_dead_simple ();
extern void bp_mod_ref ();
extern void bp_build_call_graph ();
extern void bp_build_flow_graph ();
extern void bp_compute_distances ();

extern wIdent bp_create_label (bp_fun_t,bp_stmt_t,wIdent);
extern bddPds* bpReadBP (char*);
extern bddPds* bp_create_pds ();
void bpError (char*,...);

extern void sort_int (int*,int,int);
extern void sort_int_rev (int*,int,int);
extern void bp_reorder_dep (bddPds*,void*);

#endif
