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
#include "AST/TransitionSystem.h"

#include <exception>

namespace Command {

   class DeclareAutomatonCommand : public Command {
      private:
         string _dfa_name;
         const AST::Automaton *_dfa;
      public:
         DeclareAutomatonCommand(string name, const AST::Automaton *dfa) : _dfa_name(name), _dfa(dfa) { }
         virtual string ToString() const {
            stringstream s;

            s << "Automaton " << _dfa_name << " { " << _dfa->ToString() << " }";
            return s.str();
         }
         Automaton *CreateDFA() const {
            map<string, State*> state_map;

            const vector<const AST::State *> &ast_states(_dfa->GetStates());
            vector<State*> states;

            vector<const AST::State*>::const_iterator state_iter;
            for (state_iter = ast_states.begin(); state_iter != ast_states.end(); ++state_iter) {
               const AST::State *ast_state(*state_iter);
               string name(ast_state->GetName());
               State *state(new State(name));
               state_map.insert( make_pair<string, State*>(name, state) );
               states.push_back(state);
            }

            State *initial_state = state_map.begin()->second; // TODO

            DFA *dfa = new DFA(states, initial_state);

            const vector<AST::Automaton::Rule> &ast_rules(_dfa->GetRules());
            vector<AST::Automaton::Rule>::const_iterator rule_iter;
            for (rule_iter = ast_rules.begin(); rule_iter != ast_rules.end(); ++rule_iter) {
               const AST::State         *ast_state1(rule_iter->state1);
               const AST::State         *ast_state2(rule_iter->state2);
               // TODO unhack!!!
               const AST::RegularAction *ast_action(static_cast<const AST::RegularAction *>(rule_iter->action));
               State *state1(state_map.find(ast_state1->GetName())->second);
               State *state2(state_map.find(ast_state2->GetName())->second);
               RegularAction *action(new RegularAction(ast_action->GetName()));
               dfa->AddRule(state1, action, state2);
            }
            return dfa;
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[Automaton "  << _dfa_name << "]" << endl;
            cout << "Declaring ... "  << ToString() << endl;

            Automaton *automaton;

            switch (_dfa->GetType()) {

               case AST::Automaton::DFA:
                  automaton = CreateDFA();
                  break;
               case AST::Automaton::LTS:
                  throw runtime_error("not implemented");
                  break;
               case AST::Automaton::PDA:
                  throw runtime_error("not implemented");
                  break;
               case AST::Automaton::PDS:
                  throw runtime_error("not implemented");
                  break;
               default:
                  throw runtime_error("Bad automaton type");
                  break;
            }

            environment.SetAutomaton( _dfa_name, automaton );
            cout << automaton->ToString() << endl;
         }
         virtual ~DeclareAutomatonCommand() {
            delete _dfa;
         }
   };

}

#endif

