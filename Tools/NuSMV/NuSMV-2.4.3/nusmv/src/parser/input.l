/***************************************************************  -*-C-*-  ***/
%{
/**CFile***********************************************************************

  FileName    [input.l]

  PackageName [parser]

  Synopsis    [Lexical analyzer for the NuSMV input language]

  SeeAlso     [grammar.y]

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``parser'' package of NuSMV version 2. 
  Copyright (C) 1998-2005 by CMU and ITC-irst. 

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information on NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/
 
#include "util.h"

#include "utils/utils.h"
#include "utils/ustring.h"
#include "node/node.h"
#include "utils/error.h"
#include "utils/WordNumber.h"

#include <ctype.h>
#include <stdlib.h> /* for strtol */
#include <limits.h> /* for LLONG_MAX */
#include <errno.h>

#include "grammar.h" /* For yacc/lex interaction */

/* For direct interpretation of atomic (ATOM and NUMBER)
   and boolan (TRUE and FALSE) constants. */
#include "symbols.h" 

static char rcsid[] UTIL_UNUSED = "$Id: input.l,v 1.12.2.4.4.30.4.6 2007/05/11 08:37:48 nusmv Exp $";

static int nusmv_skip_comment ARGS((void));

EXTERN boolean parser_parse_real  ARGS((void));
EXTERN void Parser_switch_to_psl ARGS((void));

/* 
   The following are needed in order to allow parsing of long
   comments, that otherwise will cause errors since flex thinks that
   it is using REJECT even if it isn't.  The "%option noreject" seems
   to be ignored in the latest (and previous) versions of flex.
*/
#ifdef YY_USES_REJECT
#undef YY_USES_REJECT
#endif


#define YY_INPUT(buf, result, max_size) \
	if ( YY_CURRENT_BUFFER->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for (n=0; n<max_size && \
		     (c = getc(yyin)) != EOF && \
                     c!=' ' && c!='\n' && c!='\t' && c!='\r' && c!='\f'; ++n ) \
			buf[n] = (char) c; \
		if (c==' ' || c=='\n' || c=='\t' || c=='\r' || c=='\f') buf[n++] = (char) c; \
		if (c == EOF && ferror(yyin)) YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else \
		{ \
		errno=0; \
		while ( (result = fread(buf, 1, max_size, yyin))==0 && ferror(yyin)) \
			{ \
			if(errno != EINTR) \
				{ \
				YY_FATAL_ERROR( "input in flex scanner failed" ); \
				break; \
				} \
			errno=0; \
			clearerr(yyin); \
			} \
		}


%}
%a	15000
%o	25000
%option noreject
%option always-interactive

%%
[ \n\t\r\f]             ;
"--"                    nusmv_skip_comment();
^"#"" "[0-9]+.*\n       sscanf(yytext,"# %d",&yylineno);
^"#".*\n                rpterr("Unexpected preprocessor directive:\n %s\n",yytext);
"MODULE"                {yylval.lineno = yylineno; return(TOK_MODULE);}
"process"               {yylval.lineno = yylineno; return(TOK_PROCESS);}
"DEFINE"                {yylval.lineno = yylineno; return(TOK_DEFINE);}
"VAR"                   {yylval.lineno = yylineno; return(TOK_VAR);}
"IVAR"                  {yylval.lineno = yylineno; return(TOK_IVAR);}
"INIT"                  {yylval.lineno = yylineno; return(TOK_INIT);}
"TRANS"                 {yylval.lineno = yylineno; return(TOK_TRANS);}
"INVAR"                 {yylval.lineno = yylineno; return(TOK_INVAR);}
"SPEC"                  {yylval.lineno = yylineno; return(TOK_SPEC);}
"CTLSPEC"               {yylval.lineno = yylineno; return(TOK_CTLSPEC);}
"LTLSPEC"               {yylval.lineno = yylineno; return(TOK_LTLSPEC);}
"PSLSPEC"               {yylval.lineno = yylineno; Parser_switch_to_psl(); return(TOK_PSLSPEC);}
"COMPUTE"               {yylval.lineno = yylineno; return(TOK_COMPUTE);}
"INVARSPEC"             {yylval.lineno = yylineno; return(TOK_INVARSPEC);}
"CONSTRAINT"            {yylval.lineno = yylineno; return(TOK_CONSTRAINT);}
"CONSTANTS"             {yylval.lineno = yylineno; return(TOK_CONSTANTS);}
"SIMPWFF"               {yylval.lineno = yylineno; return(TOK_SIMPWFF);}
"CTLWFF"                {yylval.lineno = yylineno; return(TOK_CTLWFF);}
"LTLWFF"                {yylval.lineno = yylineno; return(TOK_LTLWFF);}
"COMPWFF"               {yylval.lineno = yylineno; return(TOK_COMPWFF);}
"IN"                    {yylval.lineno = yylineno; return(TOK_INCONTEXT);}
"FAIRNESS"              {yylval.lineno = yylineno; return(TOK_FAIRNESS);}
"JUSTICE"               {yylval.lineno = yylineno; return(TOK_JUSTICE);}
"COMPASSION"            {yylval.lineno = yylineno; return(TOK_COMPASSION);}
"ISA"                   {yylval.lineno = yylineno; return(TOK_ISA);}
"ASSIGN"                {yylval.lineno = yylineno; return(TOK_ASSIGN);}
"GOTO"                  {yylval.lineno = yylineno; return(TOK_GOTO);}

"GAME"                  {yylval.lineno = yylineno; return(TOK_GAME);}
"PLAYER_1"              {yylval.lineno = yylineno; return(TOK_PLAYER_1);}
"PLAYER_2"              {yylval.lineno = yylineno; return(TOK_PLAYER_2);}
"REACHTARGET"           {yylval.lineno = yylineno; return(TOK_REACHTARGET);}
"AVOIDTARGET"           {yylval.lineno = yylineno; return(TOK_AVOIDTARGET);}
"REACHDEADLOCK"         {yylval.lineno = yylineno; return(TOK_REACHDEADLOCK);}
"AVOIDDEADLOCK"         {yylval.lineno = yylineno; return(TOK_AVOIDDEADLOCK);}
"BUCHIGAME"             {yylval.lineno = yylineno; return(TOK_BUCHIGAME);}
"GENREACTIVITY"         {yylval.lineno = yylineno; return(TOK_GENREACTIVITY);}

"array"                 {yylval.lineno = yylineno; return(TOK_ARRAY);}
"of"                    {yylval.lineno = yylineno; return(TOK_OF);}
"boolean"               {yylval.lineno = yylineno; return(TOK_BOOLEAN);}
"integer"               {yylval.lineno = yylineno; return(TOK_INTEGER);}
"Integer"               {yylval.lineno = yylineno; return(TOK_INTEGER);}
"real"                  {yylval.lineno = yylineno; return(TOK_REAL);}
"Real"                  {yylval.lineno = yylineno; return(TOK_REAL);}
"word1"                 {yylval.lineno = yylineno; return(TOK_WORD1);}
"word"                  {yylval.lineno = yylineno; return(TOK_WORD);}
"Word"                  {yylval.lineno = yylineno; return(TOK_WORD);}
"bool"                  {yylval.lineno = yylineno; return(TOK_BOOL);}

"READ"                  {yylval.lineno = yylineno; return(TOK_WAREAD);}
"WRITE"                 {yylval.lineno = yylineno; return(TOK_WAWRITE);}

"EX"                    {yylval.lineno = yylineno; return(TOK_EX);}
"AX"                    {yylval.lineno = yylineno; return(TOK_AX);}
"EF"                    {yylval.lineno = yylineno; return(TOK_EF);}
"AF"                    {yylval.lineno = yylineno; return(TOK_AF);}
"EG"                    {yylval.lineno = yylineno; return(TOK_EG);}
"AG"                    {yylval.lineno = yylineno; return(TOK_AG);}
"E"                     {yylval.lineno = yylineno; return(TOK_EE);}
"F"                     {yylval.lineno = yylineno; return(TOK_OP_FUTURE);}
"O"                     {yylval.lineno = yylineno; return(TOK_OP_ONCE);}
"G"                     {yylval.lineno = yylineno; return(TOK_OP_GLOBAL);}
"H"                     {yylval.lineno = yylineno; return(TOK_OP_HISTORICAL);}
"X"                     {yylval.lineno = yylineno; return(TOK_OP_NEXT);}
"Y"                     {yylval.lineno = yylineno; return(TOK_OP_PREC);}
"Z"                     {yylval.lineno = yylineno; return(TOK_OP_NOTPRECNOT);}
"A"                     {yylval.lineno = yylineno; return(TOK_AA);}
"U"                     {yylval.lineno = yylineno; return(TOK_UNTIL);}
"S"                     {yylval.lineno = yylineno; return(TOK_SINCE);}
"V"                     {yylval.lineno = yylineno; return(TOK_RELEASES);}
"T"                     {yylval.lineno = yylineno; return(TOK_TRIGGERED);}
"BU"                    {yylval.lineno = yylineno; return(TOK_BUNTIL);}
"EBF"                   {yylval.lineno = yylineno; return(TOK_EBF);}
"ABF"                   {yylval.lineno = yylineno; return(TOK_ABF);}
"EBG"                   {yylval.lineno = yylineno; return(TOK_EBG);}
"ABG"                   {yylval.lineno = yylineno; return(TOK_ABG);}
"MIN"                   {yylval.lineno = yylineno; return(TOK_MMIN);}
"MAX"                   {yylval.lineno = yylineno; return(TOK_MMAX);}
"("                     {yylval.lineno = yylineno; return(TOK_LP);}
")"                     {yylval.lineno = yylineno; return(TOK_RP);}
"["                     {yylval.lineno = yylineno; return(TOK_LB);}
"]"                     {yylval.lineno = yylineno; return(TOK_RB);}
"{"                     {yylval.lineno = yylineno; return(TOK_LCB);}
"}"                     {yylval.lineno = yylineno; return(TOK_RCB);}
"FALSE"                 {
				yylval.node = new_node(FALSEEXP,Nil,Nil);
				return(TOK_FALSEEXP);
			}
"TRUE"                  {
				yylval.node = new_node(TRUEEXP,Nil,Nil);
				return(TOK_TRUEEXP);
			}
"case"                  {yylval.lineno = yylineno; return(TOK_CASE);}
"esac"                  {yylval.lineno = yylineno; return(TOK_ESAC);}
":="                    {yylval.lineno = yylineno; return(TOK_EQDEF);}
"+"                     {yylval.lineno = yylineno; return(TOK_PLUS);}
"-"                     {yylval.lineno = yylineno; return(TOK_MINUS);}
"*"                     {yylval.lineno = yylineno; return(TOK_TIMES);}
"/"                     {yylval.lineno = yylineno; return(TOK_DIVIDE);}
"mod"                   {yylval.lineno = yylineno; return(TOK_MOD);}
"<<"                    {yylval.lineno = yylineno; return(TOK_LSHIFT);}
">>"                    {yylval.lineno = yylineno; return(TOK_RSHIFT);}
"<<<"                   {yylval.lineno = yylineno; return(TOK_LROTATE);}
">>>"                   {yylval.lineno = yylineno; return(TOK_RROTATE);}
"="                     {yylval.lineno = yylineno; return(TOK_EQUAL);}
"!="                    {yylval.lineno = yylineno; return(TOK_NOTEQUAL);}
"<="                    {yylval.lineno = yylineno; return(TOK_LE);}
">="                    {yylval.lineno = yylineno; return(TOK_GE);}
"<"                     {yylval.lineno = yylineno; return(TOK_LT);}
">"                     {yylval.lineno = yylineno; return(TOK_GT);}
"next"                  {yylval.lineno = yylineno; return(TOK_NEXT);}
"init"                  {yylval.lineno = yylineno; return(TOK_SMALLINIT);}
"self"                  {yylval.lineno = yylineno; return(TOK_SELF);}
"union"                 {yylval.lineno = yylineno; return(TOK_UNION);}
"in"                    {yylval.lineno = yylineno; return(TOK_SETIN);}
".."                    {yylval.lineno = yylineno; return(TOK_TWODOTS);}
"."                     {yylval.lineno = yylineno; return(TOK_DOT);}
"->"                    {yylval.lineno = yylineno; return(TOK_IMPLIES);}
"<->"                   {yylval.lineno = yylineno; return(TOK_IFF);}
"|"                     {yylval.lineno = yylineno; return(TOK_OR);}
"&"                     {yylval.lineno = yylineno; return(TOK_AND);}
"xor"                   {yylval.lineno = yylineno; return(TOK_XOR);}
"xnor"                  {yylval.lineno = yylineno; return(TOK_XNOR);}
"!"                     {yylval.lineno = yylineno; return(TOK_NOT);}

","                     {yylval.lineno = yylineno; return(TOK_COMMA);}
":"                     {yylval.lineno = yylineno; return(TOK_COLON);}
";"                     {yylval.lineno = yylineno; return(TOK_SEMI);}
"::"                    {yylval.lineno = yylineno; return(TOK_CONCATENATION);}
"PRED"                  {yylval.lineno = yylineno; return(TOK_PRED);}
"PREDICATES"            {yylval.lineno = yylineno; return(TOK_PREDSLIST);}
"MIRROR"                {yylval.lineno = yylineno; return(TOK_MIRROR);}

 /* word constants */
"0"[bBoOdDhH][0-9]*"_"[0-9a-fA-F][0-9a-fA-F_]* {
                 char* errorString;
                 WordNumber_ptr wordConstant
		        = WordNumber_from_parsed_string(yytext, &errorString);
                 /* check for errors */
                 if (WORD_NUMBER(NULL) == wordConstant) rpterr(errorString);

                 yylval.node = new_node(NUMBER_WORD,(node_ptr)wordConstant,Nil);
                 return(TOK_NUMBER_WORD);
               }

 /* real, fractional and exponential constants */
[fF]"'"[0-9]+"/"[0-9]+  { 
                  yylval.node = new_node(NUMBER_FRAC,
					 (node_ptr)find_string(yytext),Nil);
                  return(TOK_NUMBER_FRAC);
                 }

[0-9]+("."[0-9]+)?[eE][+-]?[0-9]+ { 
                  yylval.node = new_node(NUMBER_EXP,
					 (node_ptr)find_string(yytext),Nil);
                  return(TOK_NUMBER_EXP);
                 }

[0-9]+"."[0-9]+  { 
                  if (parser_parse_real()) {
                    yylval.node = new_node(NUMBER_REAL,
                                           (node_ptr)find_string(yytext),Nil);
                    return(TOK_NUMBER_REAL);
                  }
                  else {
                    REJECT;
                  }
                 }

 /* integer number */
[0-9]+           {
                  int i;
                  sscanf(yytext,"%d",&i);
                  yylval.node = new_node(NUMBER, NODE_FROM_INT(i), Nil);
                  return(TOK_NUMBER);
                 }

 /* identifier */
[A-Za-z_][A-Za-z0-9_\$#-]*  { 
                  yylval.node = new_node(ATOM,
					 (node_ptr)find_string(yytext),Nil);
                  return(TOK_ATOM);
                 }
\"[^\"]*\"       {
                  yylval.node = new_node(ATOM,(node_ptr)find_string(yytext),Nil);
                  return(TOK_ATOM);
                 } /* Unused token */

.		 rpterr("illegal character : \"%s\"", yytext);
%%

static int nusmv_skip_comment() {
  register int c;

  do {
    c = input();
  } while ( c != '\n' && c != EOF );
  return(0);
}

