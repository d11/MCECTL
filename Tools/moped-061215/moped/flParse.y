%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int fllex();
void flerror(char* info);

int linenum = 1;
int debugmodus = 0;
int check_er= 0;
extern FILE *flout;
extern FILE *flin;

%}

%union
{
	char *string;
}




%token TR
%token FA
%token NOT 
%token OR
%token AND
%token NEXT
%token UNTIL
%token GLOBAL
%token EXIST 
%token ERROR
%token LESSEQ
%token MOREEQ
%token UNEQ

%token <string> IDENT
%token <string>	LANGUAGE
%token <string> OPERATOR

%type <string> formel
%type <string> varexpr

%nonassoc NOT
%nonassoc NEXT
%nonassoc EXIST
%nonassoc GLOBAL
%left AND
%left OR
%left UNTIL

%%

startsym: formel		{ // at the end the result is stored on the top of the stack
				  flout = fopen("output_formel", "w");
				  fprintf(flout, "%s", $1);
				  printf("output formel = %s\n", $1);
				  free($1); 
				}
	;

formel: IDENT			{ $$ = strdup($1); 
				  free($1);  }
	| TR			{ $$ = strdup("true"); }
	| FA			{ $$ = strdup("false");}
	| formel AND formel	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%s&&%s", $1, $3);	
				  free($1);
				  free($3);}
	| formel OR formel	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%s||%s", $1, $3);	
				  free($1);
				  free($3);}
	| NOT formel		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "!%s", $2);	
				  free($2);}
	| NEXT formel		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "X%s", $2);	
				  free($2);}
	| formel UNTIL formel	{ $$ = (char*)malloc(strlen($1) + strlen($3) + 3); 
				  sprintf($$, "%sU%s", $1, $3);	
				  free($1);
				  free($3);}
	| EXIST formel		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "E%s", $2);	
				  free($2);}
	| GLOBAL formel		{ $$ = (char*)malloc(strlen($2) + 3); 
				  sprintf($$, "G%s", $2);	
				  //if(debugmodus) printf("global rule used, current formula = %s\n", $$);
				  free($2);}
	| '(' formel ')'	{ $$ = (char*)malloc(strlen($2) + 5); 
				  sprintf($$, "(%s)", $2);	
				  free($2);}
	| '{' varexpr '}'	{ $$ = (char*)malloc(strlen($2) + 5); 
				  sprintf($$, "{%s}", $2);	
				  free($2);}
	| '"' IDENT '"'		{ char *name = (char*)malloc(strlen($2) + 20);
				  sprintf(name, "Languages/%s", $2);
				  FILE *to_read = fopen(name, "r");
				  if(to_read == NULL){
					printf("Error: there is no language %s\n", $2);
					flerror("couldn't translate the given formel");
		 	  	  }else{				  	
					  char *row;
					  row = (char*)malloc(30);
					  
					  int check_first = 1;
					  $$ = (char*)malloc(strlen($2) + 25);
					  sprintf($$, "(");
					  while(fgets(row, 25, to_read) != NULL)			
		  			  {
						if(debugmodus) printf("row = %s\n", row);
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

varexpr:  IDENT OPERATOR IDENT	{ $$ = (char*)malloc(strlen($1) + strlen($2) + strlen($3) + 1); 
				  sprintf($$, "%s%s%s", $1, $2, $3);	
				  free($1);
				  free($2);
				  free($3);}
	;



%%

#include "flLex.c"

//extern FILE *flin;
//extern FILE *flout;

void flParse(char *filename){
	//INITIAL;
	 
	if(debugmodus)printf("start parsing formula\n");
	if(!(flin = fopen(filename,"r"))) {
		printf("formel %s not found\n", filename);
		exit(1);
	}
	//yyrestart(flin);
	if(debugmodus)printf("input file %s was opened\n", filename);
	//flout = fopen("output_formel", "w");
	//if(debugmodus)printf("output file was opened\n");
	
	if(flparse())
		printf("parse errors encountered while reading input formel\n");

	if(!check_er)
		printf("the formel was translated successfully");


}

void flerror(char *info)
{	
	printf("%s\n ", info);
	//exit(1);
	check_er = 1;
	
}




