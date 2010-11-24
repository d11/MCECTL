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
#include "TransitionSystem.h"
#include "AST/TransitionSystem.h"

#include "Util.h"

#include <exception>
#include <numeric>

namespace Command {

   class DeclareAutomatonCommand : public Command {
      private:
         string _automaton_name;
         const AST::Automaton *_ast_automaton;
      public:
         DeclareAutomatonCommand(const string &name, const AST::Automaton *dfa) : _automaton_name(name), _ast_automaton(dfa) { }
         virtual string ToString() const {
            stringstream s;

            s << "Automaton " << _automaton_name << " { " << _ast_automaton->ToString() << " }";
            return s.str();
         }
         Automaton *CreateDFA() const {
            map<string, State*> state_map;

            const vector<const AST::State *> &ast_states(_ast_automaton->GetStates());
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

            const vector<AST::Automaton::Rule> &ast_rules(_ast_automaton->GetRules());
            vector<AST::Automaton::Rule>::const_iterator rule_iter;
            for (rule_iter = ast_rules.begin(); rule_iter != ast_rules.end(); ++rule_iter) {
               // TODO unhack!!!
               const AST::RegularAction *ast_action(dynamic_cast<const AST::RegularAction *>(rule_iter->action));
               State *state1(state_map.find(rule_iter->state1)->second);
               State *state2(state_map.find(rule_iter->state2)->second);
               if (!state1 || !state2) { throw runtime_error("Action refers to nonexistent state"); }
               RegularAction *action(new RegularAction(ast_action->GetName()));
               dfa->AddRule(state1, action, state2);
            }
            return dfa;
         }

         // TODO reduce code duplication !!

         Automaton *CreateLTS() const {
            map<string, KripkeState*> state_map;

            const vector<const AST::State *> &ast_states(_ast_automaton->GetStates());
            vector<KripkeState*> states;

            vector<const AST::State*>::const_iterator state_iter;
            for (state_iter = ast_states.begin(); state_iter != ast_states.end(); ++state_iter) {

               cout << "Transforming state ";

               const AST::KripkeState *ast_state(dynamic_cast<const AST::KripkeState *>(*state_iter));
               if (!ast_state) {
                  throw runtime_error("LTS contains non-kripke states..!");
               }
               string name(ast_state->GetName());
               cout << name << ": ";
               vector<string> propositions = ast_state->GetPropositions();

               cout << accumulate(propositions.begin(), propositions.end(), string(""), JoinWithComma) << endl;
//               transform(propositions.begin(), propositions.end()-1, propositions.begin()+1, cout, JoinWithComma);
               KripkeState *state = new KripkeState(name, propositions);
               state_map.insert( make_pair<string, KripkeState*>(name, state) );
               states.push_back(state);
            }

            KripkeState *initial_state = *(states.begin()); // TODO

            KripkeStructure *lts = new KripkeStructure(states, initial_state);

            const vector<AST::Automaton::Rule> &ast_rules(_ast_automaton->GetRules());
            vector<AST::Automaton::Rule>::const_iterator rule_iter;
            for (rule_iter = ast_rules.begin(); rule_iter != ast_rules.end(); ++rule_iter) {
               // TODO unhack!!!
               const AST::RegularAction *ast_action(dynamic_cast<const AST::RegularAction *>(rule_iter->action));
               if (!ast_action) {
                  throw runtime_error("LTS contains non-regular actions..!");
               }
               KripkeState *state1(state_map.find(rule_iter->state1)->second);
               KripkeState *state2(state_map.find(rule_iter->state2)->second);
               if (!state1 || !state2) { throw runtime_error("Action refers to nonexistent state"); }
               RegularAction *action(new RegularAction(ast_action->GetName()));
               lts->AddRule(state1, action, state2);
            }
            return lts;
         }

         virtual void Execute(Environment &environment, GlobalOptions &options) {
            cout << "[Automaton "  << _automaton_name << "]" << endl;
            cout << "Declaring ... "  << ToString() << endl;

            Automaton *automaton;

            switch (_ast_automaton->GetType()) {

               case AST::Automaton::DFA:
                  automaton = CreateDFA();
                  break;
               case AST::Automaton::LTS:
                  automaton = CreateLTS();
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

            environment.SetAutomaton( _automaton_name, automaton );
            cout << automaton->ToString() << endl;
         }
         virtual ~DeclareAutomatonCommand() {
            delete _ast_automaton;
         }
   };

}

#endif

