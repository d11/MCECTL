/****************************************************************************/
/* Memory-allocation functions						    */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/***************************************************************************/

void* Malloc_(size_t amount, char *file, int line)
{
	void *result = malloc(amount);
	printf("malloc on %p at %s:%d\n",result,file,line);
	return result;
}

void* Calloc_(size_t nmemb, size_t amount, char *file, int line)
{
	void *result = calloc(nmemb,amount);
	printf("malloc on %p at %s:%d\n",result,file,line);
	return result;
}

char* Strdup_(char *s, char *file, int line)
{
	char *result = strdup(s);
	printf("malloc on %p at %s:%d\n",result,file,line);
	return result;
}

void* Realloc_(void *ptr, size_t amount, char *file, int line)
{
	void *result = realloc(ptr,amount);
	printf("free on %p at %s:%d\n",ptr,file,line);
	printf("malloc on %p at %s:%d\n",result,file,line);
	return result;
}

void Free_(void *ptr, char *file, int line)
{
	printf("free on %p at %s:%d\n",ptr,file,line);
	free(ptr);
}

