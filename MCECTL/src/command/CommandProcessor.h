/*
 * =====================================================================================
 *
 *       Filename:  CommandProcessor.h
 *    Description:  Harness for executing command objects in a controlled manner
 *
 * =====================================================================================
 */

#ifndef _COMMAND_PROCESSOR_H_
#define _COMMAND_PROCESSOR_H_

#include <vector>
#include <exception>

#include "Environment.h"
#include "GlobalOptions.h"
#include "Command.h"

class CommandProcessor {
private:
   Environment &_environment;
   GlobalOptions &_global_options;
public:
   CommandProcessor(Environment &env, GlobalOptions &options) 
		: _environment(env), _global_options(options)
	{ }

	// Attempt to execute the given command, printing an error message upon
	// failure.
   bool ExecuteCommand( Command::Command &command ) {
		bool success = false;
		try {
			command.Execute(_environment, _global_options);
			success = true;
		} catch (exception &e) {
         cerr << "## Error: " << e.what() << " ##" << endl;
		}

      delete &command;
		return success;
   }

	// Run a series of commands
   void ExecuteCommandList( vector<Command::CommandRef> &commands) {
      vector<Command::CommandRef>::iterator iter;
      for (iter = commands.begin(); iter != commands.end(); ++iter) {
			// Stop early if one command fails
         if (!ExecuteCommand(**iter)) {
				break;
			}
      }
   }
};

#endif
