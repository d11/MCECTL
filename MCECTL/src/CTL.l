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

0                 return FALSE; /* Propositional syntax */
1                 return TRUE;
[a-z_]+           {
   yylval.string=strdup(yytext); 
   return IDENTIFIER;
}
!                 return NOT;
\&                return AND;
\|                return OR;
\(                return LPAREN;
\)                return RPAREN;

A                 return A; /* CTL specific syntax */
E                 return E;
U                 return U;
R                 return R;
G                 return G;
X                 return X;



FORMULA           return FORMULA_DECLARATION; /* Declaration commands */
DFA               return DFA_DECLARATION;
PDA               return PDA_DECLARATION;
REGULAR           return REGEX_DECLARATION;
\{                 return LBRACE;
\}                 return RBRACE;

:load             return LOAD; /* Other commands */
:show             return SHOW;
:quit             return QUIT;

:=                return ASSIGNMENT;
^\n               return EMPTY;

\.                return FULLSTOP; /* For regular expressions */
\*                return ASTERISK;

#                 BEGIN(comment); /* Comment start */
<comment>[^\n]*                   /* Comment body */
<comment>\n       BEGIN(INITIAL); /* Comment end */

\"(\\.|[^\"])*\"  { /* Quoted strings */
   int length = strlen(yytext) - 2;
   char *buf = malloc(length*sizeof(char));
   yylval.string = strncpy(buf, yytext+1, length);
   return QUOTED;
}

[ \t\n]+          /* Ignore whitespace */

%%
