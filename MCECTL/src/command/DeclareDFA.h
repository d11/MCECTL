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
#include <functional>

namespace Command {

   template <class A, class S>
   class DeclareAutomatonCommand;

   template <class A, class S>
   class StateCreator : public binary_function<const AST::State *, DeclareAutomatonCommand<A, S> &, void> {
   public:
      void operator()(const AST::State *ast_state, DeclareAutomatonCommand<A, S> &command) const {
         throw runtime_error("don't know how to convert");
      }
   };

   template <class A>
   class StateCreator<A, State>  : public binary_function<const AST::State *, DeclareAutomatonCommand<A, State> &, void> {
   public:
      void operator()(const AST::State *ast_state, DeclareAutomatonCommand<A, State> &command) const {
         cout << "creating normal state" << endl;

         if (ast_state->GetType() != AST::State::BASIC)
            throw runtime_error("Bad state type");

         State *state = new State(ast_state->GetName());
         command.CreateState(state);
      }
   };

   template <class A>
   class StateCreator<A, KripkeState>  : public binary_function<const AST::State *, DeclareAutomatonCommand<A, KripkeState> &, void> {
   public:
      void operator()(const AST::State *ast_state, DeclareAutomatonCommand<A, KripkeState> &command) const {
         cout << "creating kripke state" << endl;
         if (ast_state->GetType() != AST::State::KRIPKE)
            throw runtime_error("Bad state type");

         KripkeState *state = new KripkeState(ast_state->GetName(), ast_state->GetPropositions());
         command.CreateState(state);
      }
   };

   template <class A, class S>
   class RuleCreator : public binary_function< AST::Automaton::Rule,  DeclareAutomatonCommand<A, S> &, void> {
   public:
      void operator()( AST::Automaton::Rule rule, DeclareAutomatonCommand<A, S> &command ) const {
         throw runtime_error("don't know how to convert rule");
      }
   };
   template <class S>
   class RuleCreator<RegularAction, S> : public binary_function< AST::Automaton::Rule,  DeclareAutomatonCommand<RegularAction, S> &, void> {
   public:
      void operator()( AST::Automaton::Rule rule, DeclareAutomatonCommand<RegularAction, S> &command ) const {

         RegularAction *action = new RegularAction(rule.action->GetName());
         command.CreateRule(rule.state1, action, rule.state2);
         //const AA *ast_action(rule_iter->action->Convert<AA>());
         //A *action = ConvertRule<A>(*rule_iter);

      }
   };

   // Parameters are: 
   // Action, State
   template <class A, class S>
   class DeclareAutomatonCommand : public Command {
      private:
         string _automaton_name;
         const AST::Automaton *_ast_automaton;

         // Used during construction
         map<string, S*> _state_map;
         vector<S*> _states;
         S *_initial_state;
         FiniteAutomaton<A, S> *_automaton;
      public:
         DeclareAutomatonCommand(const string &name, const AST::Automaton *dfa) : _automaton_name(name), _ast_automaton(dfa) { }
         virtual string ToString() const {
            stringstream s;

            s << "Automaton " << _automaton_name << " { " << _ast_automaton->ToString() << " }";
            return s.str();
         }
         /*
         Automaton *CreateDFA() const {
            map<string, State*> state_map;

            const vector<AST::State *> &ast_states(_ast_automaton->GetStates());
            vector<State*> states;

            vector<AST::State*>::const_iterator state_iter;
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

            const vector<AST::State *> &ast_states(_ast_automaton->GetStates());
            vector<KripkeState*> states;

            vector<AST::State*>::const_iterator state_iter;
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
         */

         void CreateState(S *state) {
            cout << "Adding state " << state->GetName() << "(" << state << ")"  << endl;
            _state_map.insert( make_pair<string, S*>(state->GetName(), state) );
            _states.push_back(state);
         }

         void CreateRule(const string &state1_name, A *action, const string &state2_name ) {
            S *state_1 = _state_map.find(state1_name)->second;
            S *state_2 = _state_map.find(state2_name)->second;
            cout << "Adding rule " << state1_name << " (" << state_1 << ") " << action->ToString() << " " << state2_name << " (" << state_2 << ") " << endl;
            if (!state_1 || !state_2) { throw runtime_error("Action refers to nonexistent state"); }
            _automaton->AddRule(state_1, action, state_2);
         }

        // S *CreateState(const AS &ast_state);
         void CreateAutomaton() {
            for_each(
               _ast_automaton->GetStates().begin(),
               _ast_automaton->GetStates().end(),
               bind2nd(StateCreator<A, S>(), *this)
            );

            _initial_state = *(_states.begin()); // TODO

            _automaton = new FiniteAutomaton<A, S>( _states, _initial_state );

            for_each(
               _ast_automaton->GetRules().begin(),
               _ast_automaton->GetRules().end(),
               bind2nd(RuleCreator<A, S>(), *this)
            );
         }

         virtual void Execute(Environment &environment, GlobalOptions &options) {
            cout << "[Automaton "  << _automaton_name << "]" << endl;
            cout << "Declaring ... "  << ToString() << endl;

            CreateAutomaton();

            environment.SetAutomaton( _automaton_name, _automaton );
            cout << _automaton->ToString() << endl;
         }
         virtual ~DeclareAutomatonCommand() {
            delete _ast_automaton;
         }
   };

   //template <class A>
   //State *DeclareAutomatonCommand<A, State, AA, AST::State>::CreateState(AST::State *) {
   //   return new State(ast_state.GetName());
   //}

   //template <class A, AA>
   //KripkeState *DeclareAutomatonCommand<A, State, AA, AST::State>::CreateState(AST::KripkeState *) {
   //   return new KripkeState(ast_state.GetName(), ast_state.GetPropositions());
   //}

}

#endif

