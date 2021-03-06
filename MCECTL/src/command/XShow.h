/*
 * =====================================================================================
 *
 *       Filename:  XShow.h
 *    Description:  Show automata graphically, via GraphViz
 *
 * =====================================================================================
 */

#ifndef  _XSHOW_H_
#define  _XSHOW_H_

#include <string>
#include <iostream>
#include "Command.h"
#include "Environment.h"
#include <stdio.h>

struct GlobalOptions;

namespace Command {

   class XShowCommand : public Command {
      private:
         string _identifier;
      public:
         XShowCommand(const string &identifier) : _identifier(identifier) { }
         virtual string ToString() const {
            string s = ":xshow(";
            return s + _identifier + ")" ;
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) {

				const Automaton *automaton = NULL;
            try {
               automaton = environment.GetDFA(_identifier);
            } catch (NonExistentAutomatonException e) {
               try {
                  automaton = environment.GetPDA(_identifier);
               } catch (NonExistentAutomatonException e) {
                  try {
                     automaton = environment.GetLTS(_identifier);
                  } catch (NonExistentAutomatonException e) {
                     automaton = environment.GetPDS(_identifier);
                  }
               }
            }
				string s(automaton->ToDot());
	
            // If verbose, display the DOT used
            if (options.IsVerbose()) {
               cout << s.c_str() << endl;
            }

				FILE *dot = popen("dot -Tx11", "w");
				if (NULL == dot) {
					throw CommandFailed("Couldn't open pipe to 'dot' command");
				}
				fputs(s.c_str(), dot);
				pclose(dot);

         }
   };

}

#endif
