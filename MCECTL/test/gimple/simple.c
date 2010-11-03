#include <stdio.h>

int hello() {
   printf("Hello world\n");
   return 0;
}

int main( int argc, char *argv[] ) {
   int result = hello();
   return result;
}
