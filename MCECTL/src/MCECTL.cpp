/*
 * =====================================================================================
 *
 *       Filename:  MCECTL.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/10/10 15:45:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <getopt.h>
#include <iostream>

void usage() {

}
/*
        struct option {
              const char *name;
              int has_arg;
              int *flag;
              int val;
          };
          */

void get_options(int argc, char *argv[]) {
   struct option longopts[] = {
      { "file", required_argument, 0, 'f' },
      { 0, 0, 0, 0 }
   };
   int index = 0;
   int result = getopt_long (argc, argv, "f:", longopts, &index);
}

int main(int argc, char *argv[]) {



   return 0;
}
