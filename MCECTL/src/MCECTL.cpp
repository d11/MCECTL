/*
 * =====================================================================================
 *
 *       Filename:  MCECTL.cpp
 *    Description:  Main entry point
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <getopt.h>
#include <iostream>

#include <cstdlib>
#include <map>
#include <string>
#include "Environment.h"
#include "REPL.h"

#include "CTL.h"
#include "Command.h"
#include "command/LoadFile.h"

using namespace std;

void usage(int exit_code) {
   cout << "Usage:" << endl
             << "\tMCECTL [--file blah.ectl]" << endl;
   exit(exit_code);
}
/*
        struct option {
              const char *name;
              int has_arg;
              int *flag;
              int val;
          };
          */

map<string, string> get_options(int argc, char *argv[]) {
   static struct option longopts[] = {
      { "file", required_argument, 0, 'f' },
      { "verbose", 0, 0, 'v' },
      { 0, 0, 0, 0 }
   };

   map<string, string> options;

   int result = 0;
   int index = 0;
   while (1) {
      result = getopt_long (argc, argv, "f:", longopts, &index);
      
      if ( result < 0 )
      {
         break;
      }

      if ( result == '?' ) {
         usage(1);
      }

      options[ longopts[index].name ] = optarg;
      cout << "Result: " << (char)result << endl;
      cout << "Option: " << longopts[index].name << endl;
      cout << "Value:  " << (optarg != NULL ? optarg : "[none]") << endl;
   }

   return options;
}

int main(int argc, char *argv[]) {

   map<string, string> options = get_options(argc, argv);
   map<string, string>::const_iterator iter;
   for ( iter = options.begin(); iter != options.end(); iter++ ) {
      pair<string, string> p = *iter;
      cout << "Got option: " << p.first << ", " << p.second << endl;
   }


   Environment env;
   GlobalOptions global_options;
   CommandProcessor command_processor(env, global_options);
   CommandParser command_parser;

   REPL repl(command_processor, command_parser, global_options);

   iter = options.find("file");
   if(iter != options.end()) {
      Command::LoadFile command(iter->second);
      repl.SendCommand(command);
   }
   else {
   //   cout << "No file option found" << endl;
   }

   repl.Run();

   cout << "Goodbye" << endl;

   return 0;
}
