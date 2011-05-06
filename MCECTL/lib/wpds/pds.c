/***************************************************************************/
/* Data structures and functions for maintaining weighted pushdown systems */

#include "wpdsInt.h"

wRule *wRuleCreate (wIdent p0, wIdent y0, wIdent p1, wIdent y1, wIdent y2,
			void *label)
{
	wRule *rule = Malloc(sizeof(struct wRule));

	rule->from_state = p0;
	rule->from_stack = y0;
	rule->to_state   = p1;
	rule->to_stack1  = y1;
	rule->to_stack2  = y2;
	rule->label	 = label;

	return rule;
}

/***************************************************************************/

wPDS* wPDSCreate (wSemiring *sr)
{
	wPDS *pds = Malloc(sizeof(struct wPDS));
	pds->rules = NULL;
	pds->sr = wSRCopy(sr);
	pds->user = NULL;
	return pds;
}

void wPDSDelete (wPDS *pds)
{
	while (pds->rules)
	{
		wRule *rule = pds->rules;
		pds->rules = rule->next;
		pds->sr->deref(rule->label);
		Free(rule);
	}
	Free(pds->sr);
	Free(pds);
}

wRule* wPDSInsert (wPDS* pds, wIdent p0, wIdent y0,
			wIdent p1, wIdent y1, wIdent y2, void *label)
{
	wRule *rule = wRuleCreate(p0,y0,p1,y1,y2,label);
	rule->next = pds->rules;
	pds->rules = rule;
	pds->sr->ref(label);
	return rule;
}

// Added by Remy
int* wPDSSharedRule (wPDS *pds) {
	
	wRule *rule;
	int *sharedRule = Malloc(128);
	int i = 0;
	for (rule = pds->rules; rule; rule = rule->next) {
		if (rule->shared) {
			sharedRule[i++] = (int) rule;
		}
	}
	
	return sharedRule;
}
