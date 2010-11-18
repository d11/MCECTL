/****************************************************************************/
/* Memory-allocation functions						    */

#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>

#ifdef ALLOC_DEBUG
 #define Malloc(amount) Malloc_(amount,__FILE__,__LINE__)
 #define Calloc(nmemb,amount) Calloc_(nmemb,amount,__FILE__,__LINE__)
 #define Strdup(s) Strdup_(s,__FILE__,__LINE__)
 #define Realloc(ptr,amount) Realloc_(ptr,amount,__FILE__,__LINE__)
 #define Free(ptr) Free_(ptr,__FILE__,__LINE__)

 extern void* Malloc_(size_t,char*,int);
 extern void* Calloc_(size_t,size_t,char*,int);
 extern char* Strdup_(char*,char*,int);
 extern void* Realloc_(void*,size_t,char*,int);
 extern void Free_(void*,char*,int);

#else
 #define Malloc(amount) malloc(amount)
 #define Calloc(nmemb,amount) calloc(nmemb,amount)
 #define Strdup(s) strdup(s)
 #define Realloc(ptr,amount) realloc(ptr,amount)
 #define Free(ptr) free(ptr)
#endif

#endif

/***************************************************************************/
