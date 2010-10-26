%{
//#include <stdio.h>
#include "y.tab.h"
%}

%%
0        return FALSE;
1        return TRUE;
[a-z]+   yylval.string=strdup(yytext); return PVAR;
!        return NOT;
\&       return AND;
\|       return OR;
\(       return LPAREN;
\)       return RPAREN;
AX       return AX;
[ \t\n]+   /* ignore whitespace */

%%
