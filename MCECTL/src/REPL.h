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

#include "ModelChecker.h"
#include "Environment.h"
#include <string>
#include <iostream>
using namespace std;

class Command {
public:
   virtual string ToString() const = 0;
   virtual void Execute(Environment &environment) const = 0;
   virtual ~Command() { };
};

class LoadFileCommand : public Command {
private:
   string _filename;
public:
      LoadFileCommand(string filename) : _filename(filename) { };
      virtual string ToString() const { string s = ":load("; return s + _filename + ")"; };
      virtual void Execute(Environment &environment) const {
         cout << "Loading file: " << _filename << endl;
         //CommandProcessor command_processor(environment);
         //
      }
};

class CommandProcessor {
private:
   Environment &_environment;
public:
   CommandProcessor(Environment &env) : _environment(env) { }
   void ExecuteCommand( const Command &command ) {
      cout << "Executing command: " << command.ToString() << endl;
      command.Execute(_environment);
   }
};

class REPL {
   private:
      CommandProcessor _command_processor;

   void Prompt() {
      cout << "MCECTL> ";
   }
   void InitMessage() {
      cout << "Model Checker for Extended Computation Tree Logic\n"
           << "Dan Horgan\n\n";
   }
public:
   REPL(CommandProcessor &cp) : _command_processor(cp) {

   }

   void SendCommand( const Command &command ) {
      //cout << "REPL recieved command: " << command.ToString() << endl;
      Prompt();
      cout << command.ToString() << endl;
      _command_processor.ExecuteCommand(command);
   }

   void Run() {
      cout << "Starting REPL" << endl;

      cout << "Ending REPL" << endl;
   }

};
