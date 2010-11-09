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
#include <sstream>

#include "AST/Formula.h"
#include "AST/Regex.h"

#include "Environment.h"
#include "GlobalOptions.h"
#include "LTS.h"
#include "Automata.h"

using namespace std;

namespace Command {

   class Command {
      public:
         virtual string ToString() const = 0;
         virtual void Execute(Environment &environment, GlobalOptions &options) const = 0;
         virtual ~Command() { };
   };

   typedef Command *CommandRef;

   class LoadFileCommand : public Command {
      private:
         string _filename;
      public:
         LoadFileCommand(string filename) : _filename(filename) { };
         virtual string ToString() const { string s = ":load(\""; return s + _filename + "\")"; };
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "Loading file: " << _filename << endl;
            cout << "TODO" << endl;
            // TODO
            //CommandProcessor command_processor(environment);
            //
         }
   };

   class DeclareFormulaCommand : public Command {
      private:
         string _formula_name;
         AST::Formula::FormulaRef _formula;
      public:
         DeclareFormulaCommand(string name, AST::Formula::FormulaRef formula)
            : _formula_name(name), _formula(formula) { }
         virtual string ToString() const {
            string s = "FORMULA ";
            return s + _formula_name + " := " + _formula->ToString();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[FORMULA "  << _formula_name << "]" << endl;
            cout << "Declaring formula... "  << ToString() << endl;
            cout << "TODO" << endl;
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
            stringstream s;
            s << "DFA " << _dfa_name << " { " << _dfa->ToString() << " }";
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[DFA "  << _dfa_name << "]" << endl;
            cout << "Declaring DFA... "  << ToString() << endl;
            cout << "TODO" << endl;
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
            cout << "TODO" << endl;
         }
   };

   class DeclareRegexCommand : public Command {
      private:
         string _identifier;
         AST::Regex::Regex *_regex;
      public:
         DeclareRegexCommand(string name, AST::Regex::Regex *regex)
            : _identifier(name), _regex(regex) { } 
         virtual string ToString() const {
            stringstream s;
            s << "REGULAR " << _identifier << " { "
              << _regex->ToString() << " } ";
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            // TODO
            cout << "TODO" << endl;
            cout << ToString() << endl;
         }
         virtual ~DeclareRegexCommand() {
            delete _regex;
         }
   };

}
#endif
