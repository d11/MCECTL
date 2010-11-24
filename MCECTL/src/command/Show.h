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
         void TryShowAll(const Environment &environment) const {
				// :show() prints whole environment info
				if (_identifier.empty()) {
					cout << environment.ToString() << endl;
					return;
				}
         }
      public:
         ShowCommand(const string &identifier) : _identifier(identifier) { }
         virtual string ToString() const {
            string s = ":show(";
            return s + _identifier + ")" ;
         }


         virtual void Execute(Environment &environment, GlobalOptions &options) {
            TryShowAll(environment);
            
            bool found_automaton = true;
            try {
               const Automaton *automaton(environment.GetAutomaton(_identifier));
               cout << "Found automaton '" << _identifier << "':" << endl;
               cout << automaton->ToString() << endl;
            }
            catch (NonExistentAutomatonException e) {
               found_automaton = false;
            }

            bool found_formula = true;
            try {
               Formula::Formula::const_reference formula(environment.GetFormula(_identifier));
               cout << "Found formula '" << _identifier << "':" << endl;
               cout << formula.ToString() << endl;
            }
            catch (NonExistentFormulaException e) {
               found_formula = false;
            }

            bool found_system = true;
            try {
               KripkeStructure::const_reference transition_system(environment.GetSystem(_identifier));
               cout << "Found system '" << _identifier << "':" << endl;
               cout << transition_system.ToString() << endl;
            }
            catch (NonExistentTransitionSystemException e) {
               found_system = false;
            }

            if (!found_automaton && !found_formula && !found_system) {
               cout << "Couldn't find anything with the name '" << _identifier << "'." << endl;
               return;
            }

         }
   };

}
#endif
