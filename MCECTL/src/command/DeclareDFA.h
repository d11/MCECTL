/*
 * =====================================================================================
 *
 *       Filename:  DeclareDFA.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef _DECLARE_DFA_H
#define _DECLARE_DFA_H

#include "Command.h"
#include "Automata.h"

namespace Command {

   class DeclareDFACommand : public Command {
      private:
         string _dfa_name;
         const DFA *_dfa;
      public:
         DeclareDFACommand(string name, const DFA *dfa) : _dfa_name(name), _dfa(dfa) { }
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

}

#endif

