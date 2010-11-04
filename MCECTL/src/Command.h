/*
 * =====================================================================================
 *
 *       Filename:  Command.h
 *
 *    Description:  
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */
#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <iostream>
#include "Environment.h"
#include "GlobalOptions.h"
#include "ctl_ast.h"

using namespace std;


class Command {
public:
   virtual string ToString() const = 0;
   virtual void Execute(Environment &environment, GlobalOptions &options) const = 0;
   virtual ~Command() { };
};

typedef Command *command_ref;
class LoadFileCommand : public Command {
private:
   string _filename;
public:
   LoadFileCommand(string filename) : _filename(filename) { };
   virtual string ToString() const { string s = ":load(\""; return s + _filename + "\")"; };
   virtual void Execute(Environment &environment, GlobalOptions &options) const {
      cout << "Loading file: " << _filename << endl;
      //CommandProcessor command_processor(environment);
      //
   }
};

class DeclareFormulaCommand : public Command {
private:
   string _formula_name;
   formula_ref _formula;
public:
   DeclareFormulaCommand(string name, formula_ref formula) : _formula_name(name), _formula(formula) { }
   virtual string ToString() const {
      string s = "FORMULA ";
      return s + _formula_name + " := " + _formula->ToString();
   }
   virtual void Execute(Environment &environment, GlobalOptions &options) const {
      cout << "[FORMULA "  << _formula_name << "]" << endl;
      cout << "Declaring formula... "  << ToString() << endl;
      // TODO
   }
   virtual ~DeclareFormulaCommand() {
      delete _formula;
   }
};

class QuitCommand : public Command {
   public:
      QuitCommand() { }
      virtual string ToString() const {
         return ":quit";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
         cout << "Quitting!" << endl;
         options.SetQuitting();
      }
};

class NullCommand : public Command {
   public:
      virtual string ToString() const {
         return "";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
      }
};
#endif
