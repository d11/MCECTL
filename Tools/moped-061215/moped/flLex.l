%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define YY_NO_UNPUT



%}

%option noyywrap


%%


"||"		{ if(debugmodus) printf("OR returned\n");
		  return OR; }
"&&"		{ if(debugmodus) printf("AND returned\n");
		  return AND; }
"!"		{ if(debugmodus) printf("NOT returned\n"); 
		  return NOT; }
"true"		{ if(debugmodus) printf("TRUE returned\n");
		  return TR; }
"false"		{ if(debugmodus) printf("FALSE returned\n");
		  return FA; }
"G"		{ if(debugmodus) printf("GLOBAL returned\n");
		  return GLOBAL; }
"X"		{ if(debugmodus) printf("NEXT returned\n");
		  return NEXT; }
"U"		{ if(debugmodus) printf("UNTIL returned\n");
		  return UNTIL; }
"F"		{ if(debugmodus) printf("EXIST returned\n");
		  return EXIST; }
["]		{ if(debugmodus) printf("\" returned\n"); return '"'; }

[/{/}/(/)]	{ if(debugmodus) printf("%c returned\n", fltext[0]); return fltext[0]; }
"="|">"|"<"|">="|"<="|"!="	{ fllval.string = strdup(fltext);
				  if(debugmodus) printf("OPERATOR %s returned\n", fllval.string);
				  return OPERATOR; }

[A-Za-z0-9_][A-Za-z0-9_]*	{ fllval.string = strdup(fltext);
			  if(debugmodus) printf("IDENT %s returned\n", fllval.string);
			  return IDENT; }
[ /t/n]		{}
.		{ printf("unexpected symbol %c was found", fltext[0]); }



