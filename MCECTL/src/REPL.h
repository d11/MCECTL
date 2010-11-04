/*
 * =====================================================================================
 *
 *       Filename:  REPL.cpp
 *
 *    Description:  
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <string>
#include <iostream>
#include <readline/readline.h>

#include "ModelChecker.h"
#include "Environment.h"
#include "CTL.h"
#include "GlobalOptions.h"
#include "ParseException.h"

using namespace std;
class CommandProcessor {
private:
   Environment &_environment;
   GlobalOptions &_global_options;
public:
   CommandProcessor(Environment &env, GlobalOptions &options) : _environment(env), _global_options(options) { }
   void ExecuteCommand( const Command &command ) {
      cout << "Executing command: " << command.ToString() << endl;
      command.Execute(_environment, _global_options);
   }
};

class REPL {
   private:
      CommandProcessor &_command_processor;
      CommandParser &_command_parser;
      GlobalOptions &_global_options;

   const char *Prompt() {
      return "MCECTL> ";
   }
   void InitMessage() {
      cout << "Model Checker for Extended Computation Tree Logic\n"
           << "Dan Horgan\n\n";
   }
public:
   REPL(
      CommandProcessor &command_processor,
      CommandParser &command_parser,
      GlobalOptions &global_options) :
   _command_processor(command_processor), 
   _command_parser(command_parser),
   _global_options(global_options) {
      InitMessage();
   }

   void SendCommand( const Command &command ) {
      //cout << "REPL recieved command: " << command.ToString() << endl;
      cout << Prompt() << command.ToString() << endl;
      _command_processor.ExecuteCommand(command);
   }

   void Run() {

      char *line; 
      while (!_global_options.IsQuitting() && NULL != (line = readline( Prompt() ))) {

         string input(line);
         free(line);

         try {
            boost::shared_ptr<Command> command = _command_parser.ParseString( input );
            _command_processor.ExecuteCommand(*command);
         } catch (ParseException e) {
            cout << e.what() << endl;
         }
      }

      if (!_global_options.IsQuitting()) {
         cout << ":quit" << endl;
      }
   }

};
