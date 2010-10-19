%{
#include <stdio.h>
%}

%%
FALSE    printf("FALSE ");
TRUE     printf("TRUE ");
[a-z]+   printf("PVAR ");
!        printf("NOT ");
\&        printf("AND ");
\|        printf("OR ");
\(        printf("LPAREN ");
\)        printf("RPAREN ");
AX       printf("AX ");
[ \t]+   /* ignore whitespace */

%%
