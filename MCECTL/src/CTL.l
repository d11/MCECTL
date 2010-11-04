%{
//#include <stdio.h>
#include "CTL_Parse.hh"
#include <string.h>
//extern YYLTYPE yylloc;
//#define YY_USER_INIT yylloc.first_line=1;
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
:load    return LOAD;
:show    return SHOW;
:quit    return QUIT;
:=       return ASSIGNMENT;
^\n      return EMPTY;
#        BEGIN(comment);
<comment>[^\n]*        
<comment>\n             BEGIN(INITIAL);

\"(\\.|[^\"])*\"       { 
   int length = strlen(yytext) - 2;
   char *buf = malloc(length*sizeof(char));
   yylval.string = strncpy(buf, yytext+1, length);
   return QUOTED;
}

[ \t\n]+   /* ignore whitespace */

%%
