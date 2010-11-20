#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "alloc.h"
#include "irInt.h"

char *default_stack_symbol_name = "DEFAULT";
int default_stack_symbol_count = 1;
int name_count = 1;

wIdent name_create_fresh(char *string, int count) {
	
	char *name;
	wIdent result;
	
	name = (char *) Malloc((strlen(string) + ((int) log10(count)) + 3) 
		* sizeof(char *));
	sprintf(name, "%s_%i_", string, count);
	result = wIdentCreate(name);
	Free(name);
	
	return result;
}

wIdent irNameStackFresh() {
	
	return name_create_fresh(default_stack_symbol_name
		, default_stack_symbol_count++);
}

wIdent irNameCreateFresh(wIdent name) {
	
	return name_create_fresh(wIdentString(name), name_count++);
}

wIdent irNameStructCreate(wIdent varname, wIdent field) {
	
	char *string = Malloc((strlen(wIdentString(varname))
		+ strlen(wIdentString(field)) + 2) * sizeof(char *));
	sprintf(string, "%s_%s", wIdentString(varname), wIdentString(field));
	wIdent name = wIdentCreate(string);
	Free(string);

	return name;
}

wIdent irNameRetStructCreate(wIdent varname, wIdent field) {
	
	char *string = Malloc((strlen(wIdentString(varname))
		+ strlen(wIdentString(field)) + 6) * sizeof(char *));
	sprintf(string, "ret_%s_%s", wIdentString(varname), wIdentString(field));
	wIdent name = wIdentCreate(string);
	Free(string);

	return name;
}
