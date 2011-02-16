/*
 * =====================================================================================
 *
 *       Filename:  Show.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */
#ifndef _SHOW_COMMAND_H_
#define _SHOW_COMMAND_H_

#include <string>
#include <iostream>

#include "Command.h"
#include "Environment.h"
#include "exception/NonExistentFormula.h"
#include "exception/NonExistentAutomaton.h"
#include "exception/NonExistentTransitionSystem.h"

struct GlobalOptions;


namespace Command {

   class ShowCommand : public Command {
      private:
         string _identifier;
         bool TryShowAll(const Environment &environment) const {
				// :show() prints whole environment info
				if (_identifier.empty()) {
					cout << environment.ToString() << endl;
					return true;
				}
            return false;
         }
      public:
         ShowCommand(const string &identifier) : _identifier(identifier) { }
         virtual string ToString() const {
            string s = ":show(";
            return s + _identifier + ")" ;
         }


         virtual void Execute(Environment &environment, GlobalOptions &options) {
            if (TryShowAll(environment)) {
               return;
            }
            
            bool found = false;
            try {
               const DFA *automaton = environment.GetDFA(_identifier);
               cout << "Found automaton '" << _identifier << "':" << endl;
               cout << automaton->ToString() << endl;
               found = true;
            }
            catch (NonExistentAutomatonException e) { }
            try {
               const PDA *automaton = environment.GetPDA(_identifier);
               cout << "Found automaton '" << _identifier << "':" << endl;
               cout << automaton->ToString() << endl;
               found = true;
            }
            catch (NonExistentAutomatonException e) { }
            try {
               const PushDownSystem *automaton = environment.GetPDS(_identifier);
               cout << "Found pushdown system '" << _identifier << "':" << endl;
               cout << automaton->ToString() << endl;
               found = true;
            }
            catch (NonExistentAutomatonException e) { }

            try {
               const KripkeStructure *transition_system = environment.GetLTS(_identifier);
               cout << "Found kripke system '" << _identifier << "':" << endl;
               cout << transition_system->ToString() << endl;
               found = true;
            }
            catch (NonExistentTransitionSystemException e) { }

            try {
               Formula::Formula::const_reference formula(environment.GetFormula(_identifier));
               cout << "Found formula '" << _identifier << "':" << endl;
               cout << formula.ToString() << endl;
               found = true;
            }
            catch (NonExistentFormulaException e) { }

            if (!found) {
               cout << "Couldn't find anything with the name '" << _identifier << "'." << endl;
               return;
            }

         }
   };

}
#endif
