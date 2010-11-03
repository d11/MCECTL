%{
//#include <stdio.h>
#include "CTL_Parse.hh"
%}

%option nounput
%x comment

%%
0        return FALSE;
1        return TRUE;
[a-z]+   yylval.string=strdup(yytext); return IDENTIFIER;
!        return NOT;
\&       return AND;
\|       return OR;
\(       return LPAREN;
\)       return RPAREN;
A        return A;
E        return E;
U        return U;
R        return R;
G        return G;
X        return X;
FORMULA  return FORMULA_DECLARATION;
DFA      return DFA_DECLARATION;
PDA      return PDA_DECLARATION;
#        BEGIN(comment);
<comment>[^\n]*        
<comment>\n             BEGIN(INITIAL);

[ \t\n]+   /* ignore whitespace */

%%
