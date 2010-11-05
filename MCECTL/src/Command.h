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
#include "LTS.h"
#include "Automata.h"

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
class DeclareDFACommand : public Command {
private:
   string _dfa_name;
   dfa_ref _dfa;
public:
   DeclareDFACommand(string name, dfa_ref dfa) : _dfa_name(name), _dfa(dfa) { }
   virtual string ToString() const {
      string s = "DFA ";
      return s + _dfa_name + " := " + _dfa->ToString();
   }
   virtual void Execute(Environment &environment, GlobalOptions &options) const {
      cout << "[DFA "  << _dfa_name << "]" << endl;
      cout << "Declaring DFA... "  << ToString() << endl;
      // TODO
   }
   virtual ~DeclareDFACommand() {
      delete _dfa;
   }
};

class QuitCommand : public Command {
   public:
      QuitCommand() { }
      virtual string ToString() const {
         return ":quit";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
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

class ShowCommand : public Command {
   private:
      string _identifier;
   public:
      ShowCommand(string identifier) : _identifier(identifier) { }
      virtual string ToString() const {
         string s = ":show(";
         return s + _identifier + ")" ;
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {

      }
};

class DeclareRegexCommand : public Command {
   public:
      DeclareRegexCommand(string name, dfa_ref dfa) { } // TODO
      virtual string ToString() const {
         return "REGEX [blah]";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
         cout << "TODO" << endl;
      }
};
#endif
