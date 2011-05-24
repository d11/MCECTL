/*
 * =====================================================================================
 *
 *       Filename:  REPL.cpp
 *    Description:  
 *
 * =====================================================================================
 */

#include <string>
#include <vector>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "ModelChecker.h"
#include "Environment.h"
#include "CTL.h"
#include "GlobalOptions.h"
#include "exception/ParseException.h"
#include "exception/CommandFailed.h"
#include "command/CommandProcessor.h"
#include "command/Quit.h"
#include "Command.h"

using namespace std;
using namespace Command;

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

   void SendCommand( Command::Command &command ) {
      cout << Prompt() << command.ToString() << endl;
      try {
         _command_processor.ExecuteCommand(command);
      } catch (CommandFailed e) {
         cout << e.what() << endl;
      }
	}

   void Run() {

      while (!_global_options.IsQuitting()) {

         char *line = readline( Prompt() );

         vector<CommandRef> commands;
         if (!line) {
            commands.push_back( CommandRef(new Command::QuitCommand()) );
            cout << ":quit" << endl;
         }
         else {
            string input(line);
            add_history (line);
            free(line);

            try {
               commands = _command_parser.ParseString( input );
            } catch (ParseException e) {
               cout << e.what() << endl;
               continue;
            }
         }
         try {
         _command_processor.ExecuteCommandList(commands);
         } catch (CommandFailed e) {
            cout << e.what() << endl;
            continue;
         }

      }
   }

};
