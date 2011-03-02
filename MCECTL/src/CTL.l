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
#include <string>
using namespace std;
#include "Command.h"
#include "AST/Formula.h"
#include "AST/Regex.h"
#include "AST/TransitionSystem.h"
#include "Automata.h"

#include "CTL_Parse.hh"
//#include <string.h>

//extern YYLTYPE yylloc;
//#define YY_USER_INIT yylloc.first_line=1;

enum LexModes {
   LEX_NONE, LEX_FORMULA, LEX_AUTOMATON, LEX_REGEX
} expect;

%}

%option nounput
%x comment regex formula automaton

%%

FORMULA { expect = LEX_FORMULA;   return FORMULA_DECLARATION; } 
DFA     { expect = LEX_AUTOMATON; return DFA_DECLARATION;     } 
PDA     { expect = LEX_AUTOMATON; return PDA_DECLARATION;     } 
LTS     { expect = LEX_AUTOMATON; return LTS_DECLARATION;     } 
PDS     { expect = LEX_AUTOMATON; return PDS_DECLARATION;     } 
REGULAR { expect = LEX_REGEX;     return REGEX_DECLARATION;   } 

\{[ \t\n]* {
   switch(expect) {
      case LEX_REGEX:     BEGIN(regex);     break;
      case LEX_FORMULA:   BEGIN(formula);   break;
      case LEX_AUTOMATON: BEGIN(automaton); break;
      default:
         BEGIN 0; // TODO ?
         break;
   }
   return LBRACE;
}
            
<regex>\}     { expect = LEX_NONE; BEGIN(INITIAL); return RBRACE; }
<formula>\}   { expect = LEX_NONE; BEGIN(INITIAL); return RBRACE; } 
<automaton>\} { expect = LEX_NONE; BEGIN(INITIAL); return RBRACE; } 

:load       return LOAD; /* Other commands */
:show       return SHOW;
:xshow      return XSHOW;
:quit       return QUIT;
:check      return CHECK;

^\n         return EMPTY;
\(          return LPAREN;
\)          return RPAREN;
,           return COMMA;

[a-z_][a-z_1-9]*      {
   yylval.string=new string(yytext); 
   return IDENTIFIER;
}

<formula>0        return FORMULA_FALSE; /* Propositional syntax */
<formula>1        return FORMULA_TRUE;

<formula>[a-z_][a-z_1-9]*  { yylval.string=new string(yytext); return FORMULA_PVAR; }
<formula>!        return FORMULA_NOT;
<formula>\&       return FORMULA_AND;
<formula>\|       return FORMULA_OR;
<formula>->       return FORMULA_IMPLICATION;
<formula>\(       return FORMULA_LPAREN;
<formula>\)       return FORMULA_RPAREN;
<formula>\[       return FORMULA_LBRACKET;
<formula>\]       return FORMULA_RBRACKET;

<formula>A        return FORMULA_A; /* CTL specific syntax */
<formula>E        return FORMULA_E;
<formula>U        return FORMULA_U;
<formula>R        return FORMULA_R;
<formula>G        return FORMULA_G;
<formula>X        return FORMULA_X;


<regex>\(         return REGEX_LPAREN; /* For regular expressions */
<regex>\)         return REGEX_RPAREN;
<regex>\|         return REGEX_OR;
<regex>\.         return REGEX_ANY;
<regex>\*         return REGEX_KLEENE;
<regex>;          return REGEX_SEQ;
<regex>[a-z_][a-z_1-9]*    {
   yylval.string=new string(yytext); 
   return REGEX_ACTION;
}
<regex>[ \t\n]+    /* Ignore whitespace */

<automaton>\(           return DFA_LPAREN; /* Automata */
<automaton>\)           return DFA_RPAREN;
<automaton>\*           return DFA_ASTERISK;
<automaton>STATE        return DFA_STATE;
<automaton>:            return DFA_COLON;
<automaton>,            return DFA_COMMA;
<automaton>ACTION       return DFA_ACTION;
<automaton>->           return DFA_TRANSITION;
<automaton>[a-z_][a-z_1-9]*      {
   yylval.string= new string (yytext); 
   return DFA_IDENTIFIER;
};
<automaton>\[           return AUTOMATON_LBRACKET;
<automaton>\]           return AUTOMATON_RBRACKET;
<automaton>[ \t\n]+    /* Ignore whitespace */
<automaton>\.           return AUTOMATON_STACK_BOTTOM;
<automaton>PUSH         return AUTOMATON_PUSH;
<automaton>POP          return AUTOMATON_POP;
<automaton>REWRITE      return AUTOMATON_REWRITE;

#                 BEGIN(comment); /* Comment start */
<regex>#          BEGIN(comment); /* Comment start */
<formula>#        BEGIN(comment); /* Comment start */
<automaton>#      BEGIN(comment); /* Comment start */
<comment>[^\n]*                   /* Comment body */
<comment>\n       {
   switch(expect) {
      case LEX_REGEX:     BEGIN(regex);     break;
      case LEX_FORMULA:   BEGIN(formula);   break;
      case LEX_AUTOMATON: BEGIN(automaton); break;
      default:            BEGIN(INITIAL);   break;
   }
}

BEGIN(INITIAL); /* Comment end */

\"(\\.|[^\"])*\"  { /* Quoted strings */
   string s(yytext);
   size_t length = strlen(yytext) - 2;
   yylval.string = new string(s.substr(1,length));
   return QUOTED;
}

[ \t\n]+          /* Ignore whitespace */

%%
