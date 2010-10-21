%{
#include <stdio.h>
%}

%%
0        printf("FALSE ");
1        printf("TRUE ");
[a-z]+   printf("PVAR ");
!        printf("NOT ");
\&       printf("AND ");
\|       printf("OR ");
\(       printf("LPAREN ");
\)       printf("RPAREN ");
AX       printf("AX ");
[ \t\n]+   /* ignore whitespace */

%%
