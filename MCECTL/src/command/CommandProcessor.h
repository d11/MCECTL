/*
 * =====================================================================================
 *
 *       Filename:  CommandProcessor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/11/10 20:32:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
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
   void ExecuteCommand( const Command::Command &command ) {
      cout << "Executing command: " << command.ToString() << endl;
      command.Execute(_environment, _global_options);
   }
   void ExecuteCommandList( const vector<Command::CommandRef> &commands) {
      vector<Command::CommandRef>::const_iterator iter;
      for (iter = commands.begin(); iter != commands.end(); ++iter) {
         ExecuteCommand(**iter);
      }
   }
};


#endif
