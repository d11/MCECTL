%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chkLang.h"

int chklex(void);
void chkerror(char* info);
char current_letter = 65;
FILE *output;

fpos_t pos;
char* current_module;
char* current_name;
char* init_name;



int init_nummer = 0;
int one_call = 0;
int check_errors = 0;
int after_label = 0;


IrLang ir_lang = NULL;
Alpha alphabet = NULL;
InitList init_list = NULL;
int lineno = 1;
//for debugging information
int debuginfo = 0;




%}

%union
{
	char *string;
	struct IrDefAlphabet *y_alpha;
	struct IrDefLanguageStruct *y_lang;
}




%token DEFINE_LANGUAGES
%token END_DEFINE 
%token CHECK MODULE INIT
%token ERROR
%token ALL SPEC

%token <string> IDENT
%token <string> CALLIDENT  
%token <string> LETTER
%token <string> TYPE
%token <string> OPERATOR
%token <string> NEW_LETTER
%token <string> NEW_LANGUAGE
%token <string> LABEL
%token <string> ARGUMENT
%token <string> LANGUAGE

%type <string> expression
%type <string> konk
//%type <string> koma
//%type <string> operator
%type <string> stern
%type <string> klammer
%type <string> arglist
%type <string> optarglist
%type <string> varlist
%type <string> optvarlist
%type <string> checkstmt
%type <string> optformula
%type <string> formula
%type <string> varexpr

%right CONCAT
%left '+'
%left '*'
%left '-'
%nonassoc NOT
%nonassoc NEXT
%nonassoc EXIST
%nonassoc GLOBAL
%left AND
%left OR
%left UNTIL





%%
 

startsym: language optformula moduledecl init rest { //printf("startsym\n");
	InitList initl = init_list;
	while(initl != NULL)
	{
		//printf("initl->name = %s\n",initl->initName);
		if(initl->ifModule)
		{
			fprintf(output, "\nstart_init_%d:\t%s(", initl->initNummer, initl->initName);
			printInitialParameters();
			fprintf(output, ");\n");
		}
		else
		{
			fprintf(output, "\nstart_init_%d:\tgoto %s;\n", initl->initNummer, initl->initName);
		}
		initl = initl->next;
	}
	
	if(!check_errors)printf("the program was translated successfully\n");
 	}
	
	;

optformula: /*empty*/	{}
	| SPEC IDENT formula ';'  { // at the end the result is stored on the top of the stack
				  fprintf(output, "\nspec %s %s;\n", $2, $3);
			  	  if(debuginfo) printf("output formel = %s\n", $3);
				  free($2);
				  free($3); }
	;


formula: IDENT			{ $$ = strdup($1); 
				  free($1);  }
	| formula AND formula	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%s&&%s", $1, $3);	
				  free($1);
				  free($3);}
	| formula OR formula	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%s||%s", $1, $3);	
				  free($1);
				  free($3);}
	| NOT formula		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "!%s", $2);	
				  free($2);}
	| NEXT formula		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "X%s", $2);	
				  free($2);}
	| formula UNTIL formula	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%sU%s", $1, $3);	
				  free($1);
				  free($3);}
	| EXIST formula		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "E%s", $2);	
				  free($2);}
	| GLOBAL formula		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "G%s", $2);	
				  free($2);}
	| '(' formula ')'	{ $$ = (char*)malloc(strlen($2) + 5); 
				  sprintf($$, "(%s)", $2);	
				  free($2);}
	| '{' varexpr '}'	{ $$ = (char*)malloc(strlen($2) + 5); 
				  sprintf($$, "{%s}", $2);	
				  free($2);}
	| '"' LANGUAGE '"'		{ char *name = (char*)malloc(strlen($2) + 20);
				  sprintf(name, "Languages/%s", $2);
				  FILE *to_read = fopen(name, "r");
				  if(to_read == NULL){
					printf("Error: there is no language %s\n", $2);
					chkerror("couldn't translate the given formel");
		 	  	  }else{				  	
					  char *row;
					  row = (char*)malloc(30);
					  
					  int check_first = 1;
					  $$ = (char*)malloc(strlen($2) + 25);
					  sprintf($$, "(");
					  while(fgets(row, 25, to_read) != NULL)			
		  			  {
						if(debuginfo) printf("row = %s\n", row);
						char *first_state = strtok(row, " ");
						char *second_state = strtok(NULL, " ");
						second_state = strtok(NULL, " \n");
					
						if(!strcmp(second_state, "(FINAL)"))
						{
							if(!strcmp(first_state, "0")){
								if(!check_first){
									 char *tmp;
									 if ((tmp = realloc($$, strlen($$) + strlen($2) + 30)) == NULL) fprintf(stderr, "ERROR: realloc failed\n");
									 $$ = tmp;
									 sprintf($$, "%s||", $$);	
								}
								sprintf($$, "%s{state_of_%s=0}", $$, $2);
								check_first = 0;
							
							}else if(!atoi(first_state))
								printf("Cannot convert string %s to integer\n", first_state);
							else{
								if(!check_first){
									 char *tmp;
									 if ((tmp = realloc($$, strlen($$) + strlen($2) + 30)) == NULL) fprintf(stderr, "ERROR: realloc failed\n");
									 $$ = tmp;
									 
									 sprintf($$, "%s||", $$);	
								}
								
								sprintf($$, "%s{state_of_%s=%d}", $$, $2, atoi(first_state));
								check_first = 0;
							}
						}
						first_state = NULL;
						second_state = NULL;
					  }
					
					
					  free(row);
					  sprintf($$, "%s)", $$);
				   }
				   if(to_read)fclose(to_read);
				   to_read = NULL;
				   free(name);
				   free($2);
					  
					 
					
				}
				;

varexpr:  varexpr OPERATOR IDENT	{ $$ = (char*)malloc(strlen($1) + strlen($2) + strlen($3) + 1); 
				  sprintf($$, "%s%s%s", $1, $2, $3);	
				  free($1);
				  free($2);
				  free($3);}
	| IDENT
	;





language: DEFINE_LANGUAGES languages END_DEFINE 	{}
	//| DEFINE_LANGUAGES languages error	{ chkerror("error in language definition, probably 'end_define' missing");}
	//| error		{ chkerror("'define_languages' was expected");}
	;

languages:/*empty*/		{}
	| error { chkerror("error in language description"); }
	| languages IDENT error	{ chkerror("error in language syntax, probably '=' fails");}
	| languages IDENT '=' expression error	{ chkerror("error in language syntax, probably ';' fails");}
	| languages IDENT '=' expression ';' { 
		
		// fill the all-clause
		
		char* expr_to_search = strdup($4);
		
		char* after_expr;
		char* cur_expr = NULL;
		if(debuginfo)printf("expression before all = %s\n", $4);

		char* end_expr = strdup($4);
		char* all = strpbrk(expr_to_search, "A");
		if(all != NULL){

 			printf("not null\n");
			while(all != NULL)
			{
				int l = strcspn(expr_to_search, "A");
				if(debuginfo)printf("l = %d\n", l);
	
				char* vor_expr = (char*)malloc(l + 110);
				if(debuginfo)printf("expr_to_search = %s\n", expr_to_search);

				strncpy(vor_expr, expr_to_search, l);
				vor_expr[l] = 0;
				if(debuginfo)printf("vor_expr = %s\n", vor_expr);


				after_expr = (char*)malloc(strlen(expr_to_search)-l+1);
				strcpy(after_expr, all+1);
				if(debuginfo)printf("after_expr = %s\n", after_expr);

						
				sprintf(vor_expr, "%s(A", vor_expr);
				Alpha alpha = alphabet;
				while(alpha != NULL)
				{	sprintf(vor_expr, "%s+%c", vor_expr, alpha->letter);
					alpha = alpha->next;	}
				sprintf(vor_expr, "%s)", vor_expr);
				if(debuginfo)printf("vor_expr = %s\n", vor_expr);

				all = NULL;
				free(expr_to_search);
				expr_to_search = strdup(after_expr);
				if(end_expr != NULL)free(end_expr);
				if(cur_expr != NULL){
					end_expr = (char*)malloc(strlen(cur_expr)+strlen(vor_expr)+2);
					sprintf(end_expr, "%s%s", cur_expr, vor_expr);
					free(cur_expr);
				}else end_expr = strdup(vor_expr);
				
				cur_expr = strdup(end_expr);
				if(debuginfo)printf("end_expr = %s\n", end_expr);
				free(after_expr);
				free(vor_expr);
				all = strpbrk(expr_to_search, "A");
			}
			free(end_expr);
			end_expr = (char*)malloc(strlen(cur_expr)+strlen(expr_to_search)+2);
			sprintf(end_expr, "%s%s", cur_expr, expr_to_search);
			free(cur_expr);
		}
		
		free(expr_to_search);

		if(debuginfo)printf("expression after all = %s\n", end_expr);
		IrLang lang = irLangCreate($2, end_expr, ir_lang);
		if(debuginfo)printf("lang->expression = %s\n", lang->expression);
		ir_lang = lang;	
		free(end_expr);
  		char* execute = (char*)malloc(strlen(lang->expression) + 100);
		
		sprintf(execute, "echo \"%s\" | retofm |fmdeterm |fmmin >Languages/%s", lang->expression, lang->name);

		if(debuginfo)printf("execute = %s\n", execute);
		system(execute);
		free(execute);
		
		if(debuginfo)printf("before fopen, filename = %s\n", lang->name);
		
		char* name = (char*)malloc(50);
		sprintf(name, "Languages/%s", lang->name);

		FILE *automat = fopen(name, "r");
		free(name);
		if(automat == NULL)
			{printf("Error while translating the expression  %s into the automat\n", lang->expression); exit(1);}
		//else printf("file opened successfully\n");
		
		int unaccept = 0;
		//else printf("file opened\n");		
		char *row;
		row = (char*)malloc(30);
					
		  while(fgets(row, 25, automat) != NULL)
		  {
		  	if(debuginfo)printf("row = %s", row);
			int n = 0;
			char *state = strtok(row, " ");
			//printf("state = %s\n", state);
			if(atoi(state))
				n = atoi(state);
			
			if(n > unaccept) unaccept = n;

			free(row);
			row = (char*)malloc(30);
		  }
		
		free(row);
		lang->unaccept = unaccept + 1;
		
		

		if(automat)fclose(automat);
		automat = NULL;
		
		
			
		free($2);
		free($4);
		
		}
	; 
		
		

expression: expression '+' konk	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 2); 
					  sprintf($$, "%s+%s", $1, $3);	
					  free($1);
					  free($3);
				}
	| konk			{ $$ = strdup($1);
				  free($1);	
				}
	;
	
konk:	konk stern		{ $$ = (char*)malloc(strlen($1) + strlen($2) + 1);
					  sprintf($$, "%s%s", $1, $2);
					  free($1);
					  free($2);
				}
	| stern			{ $$ = strdup($1);
				  free($1);	
				}
	;
	
stern:	stern '*'		{ $$ = (char*)malloc(strlen($1) + 2);
					  sprintf($$, "%s*", $1);
					  free($1);
				}	
	| klammer		{ $$ = strdup($1);
				  free($1);	
				}
	;
	
klammer: '(' expression ')'	{ $$ = (char*)malloc(strlen($2) + 3);
					  sprintf($$, "(%s)", $2);	
					  free($2);
				}
	| LETTER 		{ $$ = (char*)malloc(2);
					  
					  char let;
					  let = getLetter($1);
					  
					  $$[0]=let;
					  $$[1]=0;
					  free($1);
					  				  
				}
	| IDENT			{ if(current_letter == 122) chkerror("Maximum 51 letters are possible in the alphabet ");
					  else	if(current_letter == 90) current_letter = 97;
						else current_letter = current_letter + 1;
						
					  Alpha alpha = irAlphaCreate($1, current_letter, alphabet);
					  alphabet = alpha;
					  	 
					  $$ = (char*)malloc(2);
					  $$[0]=current_letter;
					  $$[1]=0;
					  free($1);
				}
	| ALL			{ $$ = (char*)malloc(2);
				  $$[0] = 'A';
				  $$[1] = 0;
				}
	;
	

	
rest: 	/* empty */		{}
	| callstmt rest		{}
	| checkstmt rest	{}
	| label rest		{}
	| moduleimpl rest	{}
	| error		{chkerror("error in program structure"); }
	;	
	
moduledecl: module ';' moduledecl 	{}
	|/*empty*/	{}
	| module error	{ chkerror("error in module declaration, probably ';' expected"); }
	;

moduleimpl: modulehead modulebody '}'	{ if(current_module) free(current_module);
					  current_module = NULL;
					  one_call = 0;
					  
					}			
	;

modulehead: module '{'	{ if(!current_name){ chkerror("error within module: couldn't define the name of the current module"); exit(1);}
			  if(current_module) free(current_module);
			  current_module = strdup(current_name);
			  fprintf(output, "\n\n");
			  IrLang lang = ir_lang;
			  while(lang != NULL)
			  {
			  	fprintf(output, "int local_state_of_%s;\n", lang->name);
			  	lang = lang->next;
			  }
			}
	| module error	{ chkerror("error in modulehead, '{' is expected"); }
	;



modulebody: modulebody callstmt 	{}	
	| modulebody label		{}	
	| modulebody checkstmt		{}
	| /*empty*/			{}
	| modulebody error 	{chkerror("unexpected symbol in modulebody found"); }
	;
	


init: 	  INIT initlist ';'		{ }
	| INIT initlist error		{ chkerror("error in init list declaration, probably ';' fails"); }
	;

initlist: /*empty*/		{ }
	| initlist LETTER	{ InitList new_init = InitListIdentCreate(init_nummer, $2, init_list);
				  init_list = new_init;
				  fprintf(output, " start_init_%d", init_nummer);
				  init_nummer++;
				  free($2);
				}
			  	
	| initlist IDENT	{ InitList new_init = InitListIdentCreate(init_nummer, $2, init_list);
				  init_list = new_init;
				  fprintf(output, " start_init_%d", init_nummer);
				  init_nummer++;
				  
				  free($2);
				}
	;
	

module:  MODULE LETTER '(' optvarlist ')' { 
					    if(current_name)free(current_name);
					    current_name = NULL;
					    if(debuginfo)printf("varlist = '%s'\n", $4);
					    current_name = strdup($2);
					    if(current_name == NULL) printf("malloc error in module (letter)!\n");
					    if(debuginfo)printf("in module '%s'\n", $2);
					    if(debuginfo)printf("cur_name (letter) = '%s'\n", current_name);
					    fprintf(output, " %s( %s", $2, $4);
					    	
					if(strlen($4) != 0) fprintf(output, ",");	
							 								
					InitList initl = init_list; 
					while(initl != NULL)						
					{
						if(!strcmp(initl->initName, $2))
							initl->ifModule = 1;
						initl = initl->next;
					}			
							  
					IrLang lang = ir_lang;
					while(lang != NULL)	
					{
					  	if(lang->next == NULL)
							fprintf(output, "int state_of_%s ", lang->name);
						else
					 		fprintf(output, "int state_of_%s, ", lang->name);
						lang = lang->next;
					}
					fprintf(output, " )");
					
					free($2);
					free($4);
						 
					}
	| MODULE NEW_LETTER '(' optvarlist ')'	{  
						if(current_name) free(current_name);
						current_name = NULL;
						if(debuginfo)printf("varlist = '%s'\n", $4);
						if(debuginfo)printf("in new module %s\n", $2);
						current_name = strdup($2);
						if(current_name == NULL) printf("malloc error!\n");
						//printf("in new module %s\n", $2);
						if(debuginfo)printf("cur_name (new_letter) = %s\n", current_name);
						Alpha alpha = irAlphaCreate($2, '1', alphabet);
						
						alphabet = alpha;
						
						fprintf(output, " %s( %s", $2, $4);
					    	
					    		  if(strlen($4) != 0) fprintf(output, ",");	
							 						  
							  InitList initl = init_list; 
							  while(initl != NULL)						
							  {
							  	if(!strcmp(initl->initName, $2))
									initl->ifModule = 1;
							  	initl = initl->next;
							  }			
							  							  
							  IrLang lang = ir_lang;
					  		  while(lang != NULL)	
							  {
							  	if(lang->next == NULL)
					  				fprintf(output, "int state_of_%s ", lang->name);
								else
									fprintf(output, "int state_of_%s, ", lang->name);
								lang = lang->next;
							  }
					  		  fprintf(output, " )");
							  
						free($2);
						free($4);
						}	
						
	|MODULE LETTER error	{chkerror("error in module definition, probably '(' fails");}		
				
							
	;
	
optarglist: 	arglist			{ $$ = strdup($1);}
	| /*empty*/			{ $$ = strdup("");}
	;
	
optvarlist: 	varlist			{ $$ = strdup($1);}
	| /*empty*/			{ $$ = strdup("");}
	;
							
arglist: /*arglist ',' ARGUMENT 		{ $$ = (char*)malloc(strlen($1)+strlen($3) + 10);
					  sprintf($$, "%s, %s", $1, $3);
					  free($1);
					  free($3);
					  
					} */
	 ARGUMENT			{ $$ = (char*)malloc(strlen($1) + 1);
					  sprintf($$, "%s", $1);
					 
					  free($1);
					}
	| arglist OPERATOR ARGUMENT	{ $$ = (char*)malloc(strlen($1)+strlen($2) + strlen($3) + 5);
					  sprintf($$, "%s %s %s", $1, $2, $3);
					  free($1);
					  free($2);
					  free($3);}
	| '(' arglist ')'		{ $$ = (char*)malloc(strlen($2) + 5);
					  sprintf($$, "(%s)", $2);
					  free($2);}

	| error				{ chkerror("error in list of parameters found");}
	 
	;	


			
	
varlist: varlist OPERATOR TYPE ARGUMENT 	{ $$ = (char*)malloc(strlen($1)+strlen($3)+strlen($4) + 10);
					  sprintf($$, "%s, %s %s", $1, $3, $4);	
					  
					  free($1);
					  free($3);
					  free($4);
					} 
	|TYPE ARGUMENT			{ $$ = (char*)malloc(strlen($1)+strlen($2)+10);
					  
					  sprintf($$, "%s %s", $1, $2);
					  
					  free($1);
					  free($2);
					}
	| error				{ chkerror("error in list of arguments found");}				
	
	;						
	

				
								
checkstmt: CHECK '(' LANGUAGE ')'';'	{ if(debuginfo)printf("check statement with language %s was parsed\n", $3);
					  fprintf(output, "\n skip(");
					  
					  
					  char* filename;
					  filename = strdup($3);
					  char* name = (char*)malloc(20 + strlen(filename));
					  sprintf(name, "Languages/%s", filename);
					  FILE *to_read = fopen(name, "r");
					  free(name);
					  					  					  
					  if(to_read == NULL)
						printf("Error while reading the file  %s", filename);
					 
						
						
						
					  	
					  char *row;
					  row = (char*)malloc(30);
					  
					  int check_first = 1;
					  while(fgets(row, 25, to_read) != NULL)			
		  			{
					
		  			char *first_state = strtok(row, " ");
					char *second_state = strtok(NULL, " ");
					second_state = strtok(NULL, " \n");
					
					if(!strcmp(second_state, "(FINAL)"))
					{
						
						if(!strcmp(first_state, "0")){
							if(!check_first) fprintf(output, "||");
							fprintf(output, "(state_of_%s == 0)", filename);
							check_first = 0;
						}else if(!atoi(first_state))
							printf("Cannot convert string %s to integer\n", first_state);
						else{
						  if(!check_first) fprintf(output, "||");
				fprintf(output, "(state_of_%s == %d)", filename, atoi(first_state));
				check_first = 0;}
					}
					first_state = NULL;
					second_state = NULL;
					
			
		  			}
					
					
					  free(row);
					  
					  if(to_read)fclose(to_read);
					  to_read = NULL;
					  free(filename);
					  fprintf(output, ");\n");
					 
					  				  
					  //free($3);
					  	
						
					}
	| CHECK error		{chkerror("error in check statement"); }
		
	;

label: LETTER ':'	{ if(current_module == NULL){ 
				char* er = (char*)malloc(100);
				sprintf(er, "you can not use the label: '%s' out of any module", $1);
				chkerror(er);
				free(er);}
			  else {
				fprintf(output, "%s: ", $1);
				after_label = 1;
				char let = getLetter(current_module);	
			  	

				/*if(let == '1')
				{
					fprintf(output, "if\n");
					IrLang lang = ir_lang;
					while(lang != NULL){
						int find = printForLetter(lang->name, "state_of_", 'A');							
						if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
						else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
						fprintf(output, "fi;\n");
						lang = lang->next;
					}
				}
			  	else*/
				printStateTransfer("state_of_", let);
				let = getLetter($1);
				//if(let != '1') -- if we know it's a LETTER => let != '1'
				printStateTransfer("local_state_of_", let);
				free($1);
			   }

			}
	;

callstmt: LETTER '(' optarglist ')'	{ 
					  char let;
					  				  
					  if(current_module == NULL)
					  { 
					  	fprintf(output, "%s(", $1);
						if(strlen($3) > 0)
					  	{	fprintf(output, "%s,", $3);
							free($3);
						}
						printInitialParameters();
						fprintf(output, ")");
					  }
					  else {
					  if(!after_label)
					  {
					  	let = getLetter(current_module);
					  	/*if(let == '1')
					  	{
							fprintf(output, "if\n");
							IrLang lang = ir_lang;
							while(lang != NULL){
								int find = printForLetter(lang->name, "state_of", 'A');								
								if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								fprintf(output, "fi;\n");
								lang = lang->next;
							}
					  							
					  	}
						else*/ 
							printStateTransfer("state_of_", let);
					   }
						
						after_label = 0;					
						fprintf(output, "%s(", $1);
						
					  	if(strlen($3) > 0)
					  	{	
							fprintf(output, "%s,", $3);
							free($3);
						}
						printLocalStates();
						fprintf(output, ")");

					}
					free($1);
					  
				} 

	| CALLIDENT '=' LETTER '(' optarglist ')'	{
								char let;
					  				  
					  if(current_module == NULL)
					  { 
					  	fprintf(output, "%s = %s(", $1, $3);
						if(strlen($5) > 0)
					  	{	fprintf(output, "%s,", $5);
							free($5);
						}
						printInitialParameters();
						fprintf(output, ")");
					  }
					  else{ 
					  if(!after_label)
					  {
					  	let = getLetter(current_module);
					  	/*if(let == '1')
					  	{
							fprintf(output, "if\n");
							IrLang lang = ir_lang;
							while(lang != NULL)
							{
								int find = printForLetter(lang->name, "local_state_of", 'A');
							
								if(find)fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								else fprintf(output, "\t::true -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
								fprintf(output, "fi;\n");
								lang = lang->next;
							}
					  		
						}
						else */
							printStateTransfer("state_of_", let);
					   }	
						
						after_label = 0;
					  	fprintf(output, "%s = %s(", $1, $3);
						
					  	if(strlen($5) > 0)
					  	{	
							fprintf(output, "%s,", $5);
							free($5);
						}
						printLocalStates();
						fprintf(output, ")");							
					  
					  }
						free($1);
						free($3);
				}
					
	;

%%

#include "chkLex.c"

extern FILE *chkin;
extern FILE *chkout;
//void FreeStruct();



/******************************* Constructor ********************************/

IrLang irLangCreate(char* name, char* expression, IrLang next)
{
	IrLang lang = malloc(sizeof(struct IrDefLanguageStruct));
	lang->name = strdup(name);
	lang->expression = strdup(expression);
	lang->next = next;
	
	return lang;
}


Alpha irAlphaCreate(char* word, char letter, Alpha next)
{
	Alpha alpha = malloc(sizeof(struct IrDefAlphabet));
	
	alpha->word = malloc(strlen(word)+1);
	strcpy(alpha->word, word);
	
	alpha->letter = letter;
	alpha->next = next;

	return alpha;
}


InitList InitListIdentCreate(int nummer, char* name, InitList next)
{
	InitList new_init = malloc(sizeof(struct InitListStruct));
	
	new_init->initNummer = nummer;
	new_init->initName = strdup(name);
	new_init->ifModule = 0;
	new_init->next = next; 
	
	
	return new_init;

}

/****************************************************************************/
char getLetter(char *word)
{
	Alpha alpha;
	alpha = alphabet;
	
	
	
	while(alpha != NULL)
	{
		if(!strcmp(alpha->word, word))
		{
			
			return alpha->letter;
		}
		alpha = alpha->next;
	
	}
	
	return 0;

}

char* getWord(char letter) 
{
	Alpha alpha = alphabet;
	
	while(alpha != NULL)
	{
		if(alpha->letter == letter)
			return alpha->word;
		alpha = alpha->next;
	
	}
	
	return NULL;
}
/****************************************************************************/

void printUnacceptStates(){

	IrLang lang = ir_lang;
	while(lang != NULL)
	{
		fprintf(output, "%d", lang->unaccept);
		lang = lang->next;
		if(lang != NULL) fprintf(output, ", ");
	}

	return;
}
/****************************************************************************/

void printLocalStates(){

	IrLang lang = ir_lang;
	while(lang != NULL)
	{
		fprintf(output, "local_state_of_%s", lang->name);
		lang = lang->next;
		if(lang != NULL) fprintf(output, ", ");
	}

	return;
}
/****************************************************************************/

void printStateTransfer(const char* stateName, char let)
{
	IrLang lang = ir_lang;
	int find;
	while(lang != NULL)
	{
		fprintf(output, "if\n");

		find = printForLetter(lang->name, stateName, let);
		if(!find){ find = printForLetter(lang->name, stateName, 'A');}

		fprintf(output, "\t::else -> local_state_of_%s = %d;\n", lang->name, lang->unaccept);
		fprintf(output, "fi;\n");

		lang = lang->next;
	}
	
	
	return;
}

/****************************************************************************/

int printForLetter(char* lang_name, const char* stateName, char let)
{
	char* name = (char*)malloc(50);
		sprintf(name, "Languages/%s", lang_name);
		FILE *autom = fopen(name, "r");
		free(name);
		if(autom == NULL)
			printf("Error while reading the file  %s\n ", lang_name);
			
		
		
				
		char *row;
		row = (char*)malloc(20);
		
		int find = 0;	
		while(fgets(row, 20, autom) != NULL)			
		{
			char *first_state = strtok(row, " ");
			char *symbol = strtok(NULL, " ");
			char sym = symbol[0];
			if(sym == let)
			{
				char *second_state = strtok(NULL, " \n");
				int s1 = atoi(first_state);
				int s2 = atoi(second_state);
				fprintf(output, "\t::(%s%s == %d) -> local_state_of_%s = %d;\n", stateName, lang_name, s1, lang_name, s2);		
				find = 1;			
			}
					
			
		 }
		
		free(row);
		if(autom)fclose(autom);
		autom = NULL;
		return find;
}

/****************************************************************************/

void printInitialParameters()
{
			IrLang lang;
			lang = ir_lang;
			
			while(lang != NULL)
			{
				char* name = (char*)malloc(50);
				sprintf(name, "Languages/%s", lang->name);
				FILE *automat1 = fopen(name, "r");
				free(name);
				if(automat1 == NULL)
				printf("Error while reading the file  %s\n ", lang->name);
		
		
				char *row;
				row = (char*)malloc(30);
				
				while(fgets(row, 25, automat1) != NULL)			
		  		{
					
		  			char *first_state = strtok(row, " ");
					
					if(!strcmp(first_state, "(START)"))
					{
						char *second_state = strtok(NULL, " ");
						second_state = strtok(NULL, " \n");
						
						if(!strcmp(second_state, "0"))
							fprintf(output, "0");
						else if(!atoi(second_state))
							printf("Cannot convert string %s to integer\n", second_state);
						     else fprintf(output, "%d", atoi(second_state));
						second_state = NULL;
					}
					 first_state = NULL;
					
			
		  		}
		
				
				free(row);
				if(automat1)fclose(automat1);
				automat1 = NULL;
				lang = lang->next;
				if(lang != NULL) fprintf(output, ",");
			}
			
	
	return;
}

/******************************* Destructor ********************************/

void irLangFree(IrLang irlang)
{
	if (!irlang) return;
	
	if (irlang->next) irLangFree(irlang->next);
	if(irlang->name)free(irlang->name);
	if(irlang->expression)free(irlang->expression);
	free(irlang);
	
	return;
}

void irAlphaFree(Alpha alpha)
{
	if(!alpha) return;
	//printf("alpha word = %s\n", alpha->word);
	if(alpha->next) irAlphaFree(alpha->next);
	
	if(alpha->word)free(alpha->word);
	free(alpha);
	
	return;
}

void InitListFree(InitList initl)
{
	if(!initl) return;
	
	if(initl->next) InitListFree(initl->next);
	free(initl->initName);
	
	free(initl);
	
	return;
}


void FreeStruct() 
{

	irLangFree(ir_lang);
	irAlphaFree(alphabet);
	InitListFree(init_list);


}


void chkerror(char *info)
{	
	printf("line %d: %s\n ", lineno, info);
	check_errors = 1;
	
}

void chkParse (char *filename) {
	if (!(chkin = fopen(filename,"r"))) {
		printf("modelfile %s not found\n", filename);
		exit(1);
	}
	
	char* exitfile = malloc(strlen(filename) + 7);
	char* newp = strrchr(filename, '/');
	char* exfile = malloc(strlen(filename) + 1);
	if(newp != NULL){ newp++; strcpy(exfile, newp);}
	else strcpy(exfile, filename);
	sprintf(exitfile, "%s%s",  "check_", exfile);
	free(exfile);
	printf("\nexitfile = %s\n", exitfile);
	output = fopen(exitfile, "w");
	chkout = output;
	if (chkparse())
		printf("parse errors encountered while reading input\n");

	//yyterminate();
	fclose(chkin);
	fclose(chkout);
	fclose(output);
	free(exitfile);
	IrLang lang = ir_lang;
	char* name = (char*)malloc(50);
	while(lang != NULL){
		sprintf(name, "Languages/%s", lang->name);
		remove(name);
		lang = lang->next;
	}
	free(name);
	FreeStruct();
	
}

/*int main( int argc, char *argv[])
{

	//chkin = fopen(argv[1], "r");
	//chkout = fopen("rest.c", "w"); 
	output = fopen("new_file_1.c", "w");
	chkout = output;
	//alphabet = NULL;
	//ir_lang = NULL;
	chkParse(argv[1]);
	//chkParse("test1.c");
	//free(current_module);
	FreeStruct();
	
}*/
