%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include "y.tab.h"
//#include "chkLang.h"
#define YY_NO_UNPUT


int def_lang = 0;
int init_decl = 0;
int arguments = 0;
int in_check = 0;
int in_module = 0;
int module_decl = 0; 
int module_impl = 0;
int in_struct = 0;
int after_letter = 0;
int callmod = 0;
int clause_n = 0;
int in_spec = 0;


%}

%option noyywrap


%%

"//"(.)*\n	{ lineno++; ECHO;}

"init"		{ ECHO; init_decl = 1; return INIT; }
"module"	{ ECHO; in_module = 1; module_decl = 1; return MODULE; }
"ALL"		{ return ALL; }
"spec"		{ in_spec = 1;  return SPEC; }


"define_languages"	{ def_lang = 1; return DEFINE_LANGUAGES; }
"end_define"		{ def_lang = 0; return END_DEFINE; }
"check"			{ in_check = 1; return CHECK; }

"int"|"bool"|"void"|"unsigned int"|"signed int"			{ if(!arguments) ECHO;
			  					  else { chklval.string = strdup(chktext); 
									 if(debuginfo)printf("type = %s\n", chktext);
									 return TYPE;}
								}

"G"		{ if(debuginfo) printf("GLOBAL returned\n");
		  return GLOBAL; }
"X"		{ if(debuginfo) printf("NEXT returned\n");
		  return NEXT; }
"U"		{ if(debuginfo) printf("UNTIL returned\n");
		  return UNTIL; }
"F"		{ if(debuginfo) printf("EXIST returned\n");
		  return EXIST; }
["]		{ if(debuginfo) printf("\" returned\n"); return '"'; }	
		
("struct"[ /t]+[A-Za-z_][A-Za-z0-9_]*|"enum"[ /t]+[A-Za-z_][A-Za-z0-9_]*)[ 	]*"{"	{in_struct = 1; ECHO;}
"struct"[ /t]+[A-Za-z_][A-Za-z0-9_]*|"enum"[ /t]+[A-Za-z_][A-Za-z0-9_]*	{if(!arguments) ECHO;
								 else { chklval.string = strdup(chktext);
									if(debuginfo)printf("type = %s\n", chktext);
									return TYPE;}}
								 									
	/*[A-Za-z_][A-Za-z0-9_]*[+*]	{if(def_lang) {

				 yyless(chkleng-1); 
				 chklval.string = strdup(chktext);  
				 
				 Alpha alpha;
				 alpha = alphabet; 
				 while(alpha != NULL)
				 {
					if(!strcmp(alpha->word, chktext))
					{					
						
						return LETTER;
					}	
					alpha = alpha->next;
				 }
				 return IDENT;
				 }
				 else ECHO;}	*/



[A-Za-z_][A-Za-z0-9_'/.\[\]]*[ ]*[=][ ]*[A-Za-z_][A-Za-z0-9_]*	{
				if(debuginfo)printf("in callident = %s\n", chktext);
				char* text = strdup(chktext);
				int l1 =  strcspn(chktext, "= ");
				yyless(l1);
				if(debuginfo)printf("chktext = %s\n", chktext);
				chklval.string = strdup(chktext);
				if(def_lang||in_spec) {//free(text);
						 return IDENT;}
				else{ 	
					char* equ = strrchr(text, '=');
					char* part2 = (char*)malloc(strlen(text) - l1 +1);
					//free(text);

					strcpy(part2, equ+1);
					equ = strrchr(part2, ' ');
					strcpy(part2, equ+1);
					if(debuginfo)printf("part2 = %s\n", part2);

					Alpha alpha;
					alpha = alphabet; 
					while(alpha != NULL)
					{
						if(!strcmp(alpha->word, part2)){
							callmod = 1;
							if(debuginfo)printf("callident %s returned\n", chklval.string);
							//free(part2);
							return CALLIDENT;}
						alpha = alpha->next;
					}
					//free(part2);
				    }
				ECHO;
			}								 				
[A-Za-z_][A-Za-z0-9_]*	{	IrLang lang;
				after_letter = 0;
				
				lang = ir_lang; 
				while(lang != NULL)
				{
					if(!strcmp(lang->name, chktext))
					{
						chklval.string = strdup(chktext);
						if(debuginfo)printf("language = %s\n", chktext);
						return LANGUAGE;
					}
					lang = lang->next;
					
					
				}	
				
				if(in_spec){	chklval.string = strdup(chktext);
						if(debuginfo)printf("ident in specification = %s\n", chktext);
						return IDENT;}


				Alpha alpha;
				alpha = alphabet; 
				while(alpha != NULL)
				{
					if(!strcmp(alpha->word, chktext))
					{
						chklval.string = strdup(chktext);
						
						if(!(def_lang||init_decl)) {arguments = 1;}
						if(in_module) in_module = 0;
						
						after_letter = 1;
						if(debuginfo)printf("letter = %s\n", chktext);
						return LETTER;
					}	
					alpha = alpha->next;
				}
				
				if(in_module) { 
						in_module = 0; 
						arguments = 1; 
						chklval.string = strdup(chktext); 
						
						if(debuginfo)printf("new_letter = %s\n", chktext);
						return NEW_LETTER;}
					
				if(arguments) { 
						chklval.string = strdup(chktext); 
						if(debuginfo)printf("argument = %s\n", chktext);
						return ARGUMENT;}
									
				
				if(def_lang)  {	
						chklval.string = strdup(chktext);
						if(debuginfo)printf("ident in lang = %s\n", chktext);
						return IDENT;}
					
				if(init_decl) { 
						chklval.string = strdup(chktext);
						if(debuginfo)printf("ident in init = %s\n", chktext);
						return IDENT;}
				
				
				ECHO;
			}
			

	/*(([+*]*[A-Za-z_][A-Za-z0-9_]*[+*][A-Za-z0-9_]*)+)|([+*][A-Za-z_][A-Za-z0-9_]*)	{if(def_lang) {
				
				 char* string;
				 string = strdup(chktext);
				 
				 if((string[0] == '*')||(string[0] == '+'))
				 {
				 	yyless(1);
					
					return string[0];
				 }
				 char* str;
				 str = strtok(string, "+*");
				 
				 
				 yyless(strlen(str)); 
				 chklval.string = strdup(chktext);
				 			 				 
				 free(string);
				 Alpha alpha;
				 alpha = alphabet; 
				 while(alpha != NULL)
				 {
					if(!strcmp(alpha->word, chktext))
					{
						
						return LETTER;
					}	
					alpha = alpha->next;
				 }
				 return IDENT;
				 }
				 else ECHO;}*/



"^"|"<=>"|"<<"|">>"|"<="|"=="|"!="|">=" {if(arguments||in_spec) { chklval.string = strdup(chktext); 
							 if(debuginfo)printf("operator %s returned\n", chktext);
							 return OPERATOR;}
							 else ECHO;	}
"||"		{ if(arguments) { chklval.string = strdup(chktext); 
				if(debuginfo)printf("operator %s returned\n", chktext);
				return OPERATOR;}
		  if(in_spec){if(debuginfo) printf("OR returned\n");
		  		return OR;}
		  else ECHO;	 }
"&&"		{ if(arguments) { chklval.string = strdup(chktext); 
				if(debuginfo)printf("operator %s returned\n", chktext);
				return OPERATOR;}
		  if(in_spec){if(debuginfo) printf("AND returned\n");
		  		return AND;}
		  else ECHO;	 }
"!"		{ if(arguments) { chklval.string = strdup(chktext); 
				if(debuginfo)printf("operator %s returned\n", chktext);
				return OPERATOR;}
		  if(in_spec){if(debuginfo) printf("NOT returned\n");
		  		return NOT;}
		  else ECHO;	  }


			  
"("		{if(arguments){ clause_n = clause_n + 1; after_letter = 0; return '(';}
		 else if(in_check||def_lang||in_spec) return '(';
		 else ECHO;}
		
")"		{if(arguments){ 
			clause_n = clause_n - 1;
			if(clause_n <= 0)arguments = 0;
			return ')';}
		 else if(def_lang||in_check||in_spec) return chktext[0];
		 else ECHO;		}

[+*\-&|\/<>]	{if(def_lang){ callmod = 0; return chktext[0];}
		 if(arguments||in_spec){ chklval.string = strdup(chktext);
				if(debuginfo)printf("operator %s returned\n", chktext); 
				return OPERATOR;}
		 ECHO;	}

"="		{if(def_lang||callmod) { callmod = 0; return '=';}
		 if(in_spec){ chklval.string = strdup(chktext); return OPERATOR; }	
		 else ECHO;	}
		 
[ 	]	{ ECHO; } 

		 
","		{if(arguments) {chklval.string = strdup(chktext); if(debuginfo)printf("operator = %s\n", chktext); return OPERATOR;}}

"{"		{if(module_decl) {module_decl = 0; module_impl = 1; ECHO; return '{';}
		 if(in_spec) return '{';
		 ECHO;	}

	
[a-zA-Z_][a-zA-Z_0-9\[\]'/.]+	{
			if(debuginfo) printf("structure or array element found = %s\n", chktext);
			if(arguments){ 
			chklval.string = strdup(chktext);
			if(debuginfo)printf("argument %s returned\n", chktext);
		 	return ARGUMENT;}
			if(in_spec){ chklval.string = strdup(chktext); return IDENT; }
		 	ECHO;}	

[0-9]+		{if(arguments){ 
			chklval.string = strdup(chktext);
		 	return ARGUMENT;}
		 if(in_spec){ chklval.string = strdup(chktext);
			  	return IDENT; }
		 ECHO;}	
		 
"}"		{ if(in_struct){in_struct = 0; ECHO;}
		  else if(module_impl) {module_impl = 0; ECHO; return '}';} 
		  if(in_spec) return '}'; } 
		 
";"		{if(init_decl) { init_decl = 0; ECHO; return ';';}
		 if(module_decl) { module_decl = 0; ECHO; return ';';} 	
		 if(in_check) { in_check = 0; return ';';}
		 if(def_lang) return ';';
		 if(in_spec){in_spec = 0; return ';';}
		 ECHO;	}	
":"		{if(after_letter){ after_letter = 0; return ':';}
		 if(in_spec){ if(debuginfo) printf(": returned in specifikation\n"); 
				chklval.string = strdup(chktext);
				return OPERATOR;}
		 ECHO;	}
		 
		 		  					 

		 
.		{ ECHO; }
	 
		 
		 
\n		{init_decl = 0; lineno++; ECHO;}		 
	  

