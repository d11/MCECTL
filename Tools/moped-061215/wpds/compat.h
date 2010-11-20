/***************************************************************************/
/* "Compatibility" macros for earlier versions of the library              */
/* These may be eliminated in future versions.                             */

#define fa_t       wFA
#define pds_t      wPDS
#define rule_t     wRule
#define trans_t    wTrans
#define semiring_t wSemiring
#define conf_t     wPath
#define clist_t    wPathTrans

#define str2int    wIdentCreate
#define int2str    wIdentString

#define from_state from
#define to_state   to

#define fa_first(fa) fa->transitions
#define fa_next(fa,t) t->next

#define init_wpds    wInit
#define pds_create   wPDSCreate
#define fa_create    wFACreate
#define fa_find	     wFAFind

#define pds_add_rule(pds,s,p0,y0,p1,y1,y2,d) wPDSInsert(pds,p0,y0,p1,y1,y2,d)
#define fa_add_trans(fa,s,p,y,q,d) wFAInsert(fa,p,y,q,d,NULL)
#define fa_prestar(pds,fa,s) wPrestar(pds,fa,1)
#define fa_poststar(pds,fa,s) wPoststar(pds,fa,1)

#define get_midstate wsGetMidstate

extern unsigned long wsGetMidstate (unsigned long,unsigned long);
