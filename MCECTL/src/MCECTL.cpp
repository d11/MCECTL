/*
 * =====================================================================================
 *
 *       Filename:  MCECTL.cpp
 *    Description:  Executable entry point for using MCECTL via a read-eval-print loop
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

// Print usage information
void usage(int exit_code) {
   cout  << "Usage:" << endl
		   << "\tMCECTL [--file blah.ectl] [--verbose]" << endl;
   exit(exit_code);
}

// Use getopt to read any command line options into a map
map<string, string> get_options(int argc, char *argv[])
{
   map<string, string> options;
   int result = 0; int index = 0;

	// Specification of valid options
	static struct option longopts[] = {
		{ "file",    required_argument,  0, 'f' },
		{ "verbose", no_argument,			0, 'v' },
		{ 0, 0, 0, 0 } // Terminate the list
	};

	// Loop through all found options
   while (true) {
      result = getopt_long(argc, argv, "f:v", longopts, &index);
		// End of options
      if ( result < 0 ) {
         break;
      }
		// Bad option
      if ( result == '?' ) {
         usage(1);
      }
		// Get the option's argument, if there is one
      string value;
      if (optarg) {
         value = optarg;
      }
      else {
         value = "1";
      }
		// Store in the map
      options[ longopts[index].name ] = value;
   }

   return options;
}

// Main entry point for the MCECTL-REPL program - the primary human interface
// to the model-checker.
int main(int argc, char *argv[]) {

	// Set up all necessary objects
   Environment env;
   GlobalOptions global_options;
   CommandProcessor command_processor(env, global_options);
   CommandParser command_parser;
   REPL repl(command_processor, command_parser, global_options);

	// Process command line options
   map<string, string> options = get_options(argc, argv);
   map<string, string>::const_iterator iter;

	// Check whether we were asked to load a file
   iter = options.find("file");
   if (iter != options.end()) {
      Command::LoadFile *command = new Command::LoadFile(iter->second);
      repl.SendCommand(*command);
   }

	// Check for verbosity
   iter = options.find("verbose");
   if(iter != options.end()) {
      global_options.SetVerbose();
   }

	// Launch the read-eval-print loop
   repl.Run();

   cout << "Goodbye" << endl;
   return 0;
}
