/*
 * =====================================================================================
 *
 *       Filename:  CommandProcessor.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef _COMMAND_PROCESSOR_H_
#define _COMMAND_PROCESSOR_H_

#include <vector>

#include "Environment.h"
#include "GlobalOptions.h"
#include "Command.h"

class CommandProcessor {
private:
   Environment &_environment;
   GlobalOptions &_global_options;
public:
   CommandProcessor(Environment &env, GlobalOptions &options) : _environment(env), _global_options(options) { }
   void ExecuteCommand( Command::Command &command ) {
      cout << "Executing command: " << command.ToString() << endl;
      command.Execute(_environment, _global_options);
      delete &command;
   }
   void ExecuteCommandList( vector<Command::CommandRef> &commands) {
      vector<Command::CommandRef>::iterator iter;
      for (iter = commands.begin(); iter != commands.end(); ++iter) {
         ExecuteCommand(**iter);
      }
   }
};


#endif
