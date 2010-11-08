%{
//#include <stdio.h>
#include "CTL_Parse.hh"
#include <string.h>

//extern YYLTYPE yylloc;
//#define YY_USER_INIT yylloc.first_line=1;

enum {
   NONE, FORMULA, DFA, PDA, REGEX
} expect;

%}

%option nounput
%x comment regex formula dfa pda

%%

FORMULA           { /* Declaration commands */
   expect = FORMULA;
   return FORMULA_DECLARATION;
}
DFA               {
   expect = DFA;
   return DFA_DECLARATION;
}
PDA               {
   expect = PDA;
   return PDA_DECLARATION;
}
REGULAR           { 
   expect = REGEX;
   return REGEX_DECLARATION;
}

\{[ \t\n]*        {
   switch(expect) {
      case REGEX:
         BEGIN(regex);
         break;
      case FORMULA:
         BEGIN(formula);
         break;
      case DFA:
         BEGIN(dfa);
         break;
      case PDA:
         BEGIN(pda);
         break;
      default:
         BEGIN 0; // TODO ?
         break;
   }
   return LBRACE;
}
<regex>\}                {
   expect = NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<formula>\}                {
   expect = NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<dfa>\}                {
   expect = NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<pda>\}                {
   expect = NONE;
   BEGIN(INITIAL);
   return RBRACE;
}

:load             return LOAD; /* Other commands */
:show             return SHOW;
:quit             return QUIT;

^\n               return EMPTY;
\(                return LPAREN;
\)                return RPAREN;
[a-z_]+           {
   yylval.string=strdup(yytext); 
   return IDENTIFIER;
}

<formula>0                 return FORMULA_FALSE; /* Propositional syntax */
<formula>1                 return FORMULA_TRUE;

<formula>[a-z_]+           {
   yylval.string=strdup(yytext); 
   return FORMULA_PVAR;
}
<formula>!                 return FORMULA_NOT;
<formula>\&                return FORMULA_AND;
<formula>\|                return FORMULA_OR;
<formula>->                return FORMULA_IMPLICATION;
<formula>\(                return FORMULA_LPAREN;
<formula>\)                return FORMULA_RPAREN;

<formula>A                 return FORMULA_A; /* CTL specific syntax */
<formula>E                 return FORMULA_E;
<formula>U                 return FORMULA_U;
<formula>R                 return FORMULA_R;
<formula>G                 return FORMULA_G;
<formula>X                 return FORMULA_X;


<regex>\(         return REGEX_LPAREN; /* For regular expressions */
<regex>\)         return REGEX_RPAREN;
<regex>\|         return REGEX_OR;
<regex>\.         return REGEX_ANY;
<regex>\*         return REGEX_KLEENE;
<regex>;          return REGEX_SEQ;
<regex>[a-z_]+           {
   yylval.string=strdup(yytext); 
   return REGEX_ACTION;
}
<regex>[\s]+    /* Ignore whitespace */

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
