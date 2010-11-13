%{
/*
 * =====================================================================================
 *
 *       Filename:  CTL.l
 *
 *    Description:  Lexer input file
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <vector>
using namespace std;
#include "Command.h"
#include "AST/Formula.h"
#include "AST/Regex.h"

#include "CTL_Parse.hh"
#include <string.h>

//extern YYLTYPE yylloc;
//#define YY_USER_INIT yylloc.first_line=1;

enum LexModes {
   LEX_NONE, LEX_FORMULA, LEX_DFA, LEX_PDA, LEX_REGEX
} expect;

%}

%option nounput
%x comment regex formula dfa pda

%%

FORMULA           { /* Declaration commands */
   expect = LEX_FORMULA;
   return FORMULA_DECLARATION;
}
DFA               {
   expect = LEX_DFA;
   return DFA_DECLARATION;
}
PDA               {
   expect = LEX_PDA;
   return PDA_DECLARATION;
}
REGULAR           { 
   expect = LEX_REGEX;
   return REGEX_DECLARATION;
}

\{[ \t\n]*        {
   switch(expect) {
      case LEX_REGEX:
         BEGIN(regex);
         break;
      case LEX_FORMULA:
         BEGIN(formula);
         break;
      case LEX_DFA:
         BEGIN(dfa);
         break;
      case LEX_PDA:
         BEGIN(pda);
         break;
      default:
         BEGIN 0; // TODO ?
         break;
   }
   return LBRACE;
}
<regex>\}                {
   expect = LEX_NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<formula>\}                {
   expect = LEX_NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<dfa>\}                {
   expect = LEX_NONE;
   BEGIN(INITIAL);
   return RBRACE;
}
<pda>\}                {
   expect = LEX_NONE;
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
<formula>\[                return FORMULA_LBRACKET;
<formula>\]                return FORMULA_RBRACKET;

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
<regex>[a-z_]+    {
   yylval.string=strdup(yytext); 
   return REGEX_ACTION;
}
<regex>[ \t\n]+    /* Ignore whitespace */

<dfa>\(           return DFA_LPAREN; /* Deterministic finite automata */
<dfa>\)           return DFA_RPAREN;
<dfa>STATE        return DFA_STATE;
<dfa>:            return DFA_COLON;
<dfa>,            return DFA_COMMA;
<dfa>ACTION       return DFA_ACTION;
<dfa>->           return DFA_TRANSITION;
<dfa>[a-z_]+      {
   yylval.string=strdup(yytext); 
   return DFA_IDENTIFIER;
}
<dfa>[ \t\n]+    /* Ignore whitespace */

#                 BEGIN(comment); /* Comment start */
<regex>#          BEGIN(comment); /* Comment start */
<formula>#        BEGIN(comment); /* Comment start */
<dfa>#            BEGIN(comment); /* Comment start */
<pda>#            BEGIN(comment); /* Comment start */
<comment>[^\n]*                   /* Comment body */
<comment>\n       BEGIN(INITIAL); /* Comment end */

\"(\\.|[^\"])*\"  { /* Quoted strings */
   int length = strlen(yytext) - 2;
   char *buf = (char*)malloc((length+1)*sizeof(char)); // TODO use new 
   yylval.string = strncpy(buf, yytext+sizeof(char), length);
   yylval.string[length] = '\0';
   return QUOTED;
}

[ \t\n]+          /* Ignore whitespace */

%%
