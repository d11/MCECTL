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

namespace Command {

   class DeclareDFACommand : public Command {
      private:
         string _dfa_name;
         const AST::DFA *_dfa;
      public:
         DeclareDFACommand(string name, const AST::DFA *dfa) : _dfa_name(name), _dfa(dfa) { }
         virtual string ToString() const {
            stringstream s;
            //s << "DFA " << _dfa_name << " { " << _dfa->ToString() << " }";
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[DFA "  << _dfa_name << "]" << endl;
            cout << "Declaring DFA... "  << ToString() << endl;
            cout << "TODO" << endl;

            map<string, DummyState*> state_map;

            const vector<AST::DummyState> &ast_states(_dfa->GetStates());
            vector<DummyState*> states;

            vector<AST::DummyState>::const_iterator state_iter;
            for (state_iter = ast_states.begin(); state_iter != ast_states.end(); ++state_iter) {
               const AST::DummyState &ast_state(*state_iter);
               string name(ast_state.GetName());
               DummyState *state(new DummyState(name));
               state_map.insert( make_pair<string, DummyState*>(name, state) );
               states.push_back(state);
            }

            DummyState *initial_state = state_map.begin()->second; // TODO

            DFA dfa(states, initial_state);

            const vector<AST::DFA::Rule> &ast_rules(_dfa->GetRules());
            vector<AST::DFA::Rule>::const_iterator rule_iter;
            for (rule_iter = ast_rules.begin(); rule_iter != ast_rules.end(); ++rule_iter) {
               const AST::DummyState    &ast_state1(rule_iter->state1);
               const AST::DummyState    &ast_state2(rule_iter->state2);
               const AST::RegularAction &ast_action(rule_iter->action);
               DummyState *state1(state_map.find(ast_state1.GetName())->second);
               DummyState *state2(state_map.find(ast_state2.GetName())->second);
               RegularAction *action(new RegularAction(ast_action.GetName()));
               dfa.AddRule(state1, action, state2);
            }

            environment.SetAutomaton( _dfa_name, dfa );
            cout << dfa.ToString() << endl;
            cout << dfa.ToDot() << endl;
         }
         virtual ~DeclareDFACommand() {
            delete _dfa;
         }
   };

}

#endif

