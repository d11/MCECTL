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
#include <boost/algorithm/string/trim.hpp>

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
   class StateCreator<A, State> : public binary_function<const AST::State *, DeclareAutomatonCommand<A, State> &, void> {
   public:
      void operator()(const AST::State *ast_state, DeclareAutomatonCommand<A, State> &command) const {

         if (ast_state->GetType() == AST::State::BASIC) {
            cout << "creating normal state" << endl;
            State *state = new State(ast_state->GetName(), ast_state->GetAccepting());
            command.CreateState(state);
         }
         else {
            throw runtime_error("Bad state type");
         }
      }
   };

   template <class A>
   class StateCreator<A, KripkeState>  : public binary_function<const AST::State *, DeclareAutomatonCommand<A, KripkeState> &, void> {
   public:
      void operator()(const AST::State *ast_state, DeclareAutomatonCommand<A, KripkeState> &command) const {
         cout << "["<< ast_state->GetName() << "]" << endl;
         if (ast_state->GetType() == AST::State::KRIPKE) {
            cout << "creating kripke state" << endl;
            KripkeState *state = new KripkeState(ast_state->GetName(), ast_state->GetName(), "_", ast_state->GetPropositions());
            command.CreateState(state);
         }
         else if (ast_state->GetType() == AST::State::PUSHDOWN_KRIPKE) {
            cout << "creating pushdown kripke state" << endl;
				ostringstream s;
				s << "<" << ast_state->GetName() << "," 
				  << ast_state->GetSymbol() << ">";
            KripkeState *state = new KripkeState(ast_state->GetName(), s.str(), ast_state->GetSymbol(), ast_state->GetPropositions());
            command.AddStackSymbol(ast_state->GetSymbol());
            command.CreateState(state);
         }
         else {
            throw runtime_error("Bad state type");
         }
      }
   };


   template <class A, class S>
   class RuleCreator : public binary_function< AST::Automaton::Rule,  DeclareAutomatonCommand<A, S> &, void> {
   public:
      void operator()( AST::Automaton::Rule rule, DeclareAutomatonCommand<A, S> &command ) const {
        // throw runtime_error("don't know how to convert rule");
         cout << "DOING GENERAL RULE CREATION" << endl;
         command.CreateRule(rule._config, rule._action);
      }
   };
   template <class S>
   class RuleCreator<RegularAction, S> : public binary_function< AST::Automaton::Rule,  DeclareAutomatonCommand<RegularAction, S> &, void> {
   public:
      void operator()( AST::Automaton::Rule rule, DeclareAutomatonCommand<RegularAction, S> &command ) const {

         //RegularAction *action = new RegularAction(rule._action->GetName());
         command.CreateRule(rule._config, rule._action);
         //const AA *ast_action(rule_iter->action->Convert<AA>());
         //A *action = ConvertRule<A>(*rule_iter);

      }
   };

   template <class A, class S>
   class DeclareAutomatonCommand : public Command {
      private:
         string _automaton_name;
         const AST::Automaton *_ast_automaton;

         // Used during construction
         map<string, S*> _state_map;
         vector<S*> _states;
         S* _initial_state;
         FiniteAutomaton<A,S> *_automaton;

         set<string> _state_names;
         set<string> _stack_alphabet;
         ConfigurationSpace *_config_space;
      public:
         DeclareAutomatonCommand(const string &name, const AST::Automaton *dfa)
            : _automaton_name(name), _ast_automaton(dfa), _config_space(NULL) { }

         virtual string ToString() const {
            stringstream s;
            s << "Automaton " << _automaton_name << " { " << _ast_automaton->ToString() << " }";
            return s.str();
         }

         void CreateState(S *state) {
            cout << "Adding state " << state->GetName() << "(" << state << ")"  << endl;
            _state_names.insert(state->GetName());
            _state_map.insert(make_pair<string, S*>(state->GetName(), state));
            _states.push_back(state);
         }

         void CreateAction(RegularAction **action, AST::Action *ast_action) {
            cout << "CREATING REGULAR ACTION" << endl;
            if (ast_action->GetType() != AST::Action::REGULAR) {
               throw runtime_error("Action type is not regular");
            }
            *action = new RegularAction(
               ast_action->GetName(),
               ast_action->GetStateID(*_config_space)
            );
         }

         void AddStackSymbol(const string &symbol) {
            string trimmed_symbol = boost::algorithm::trim_copy(symbol);

            if (trimmed_symbol != "") {
               _stack_alphabet.insert(trimmed_symbol);
            }
         }
         void CreateAction(PushDownAction **action, AST::Action *ast_action) {
            cout << "CREATING PUSHDOWN ACTION" << endl;
            switch(ast_action->GetType()) {
               case AST::Action::REGULAR:
                  throw runtime_error("Action type shouldn't be regular");
                  break;
               case AST::Action::PUSH:
                  *action = new PushAction(
                     ast_action->GetName(),
                     ast_action->GetStateID(*_config_space),
                     ast_action->GetSymbol()
                  );
                  break;
               case AST::Action::REWRITE:
                  *action = new RewriteAction(
                     ast_action->GetName(),
                     ast_action->GetStateID(*_config_space),
                     ast_action->GetSymbol()
                  );
                  break;
               case AST::Action::POP:
                  *action = new PopAction(
                     ast_action->GetName(),
                     ast_action->GetStateID(*_config_space)
                  );
                  break;
               default:
                  throw runtime_error("Bad action type");
                  break;
            }

         }

         void CreateRule(const AST::Configuration *config, AST::Action *ast_action ) {
            int start_id = config->GetID(*_config_space);
            cout << "Creating rule" << endl;
            cout << "From config: " << config->ToString() << endl;
            cout << "with action: " << ast_action->ToString() << endl;
            A *action = NULL;
            CreateAction(&action, ast_action);
            if (!action) { throw runtime_error("failed to create action"); }
            cout << "created action object: " << action->ToString() << endl;

            _automaton->AddRule(start_id, action);
         }

         void CreateAutomaton() {

            _stack_alphabet.insert("_");

            for_each(
               _ast_automaton->GetStates().begin(),
               _ast_automaton->GetStates().end(),
               bind2nd(StateCreator<A, S>(), *this)
            );

            typename vector<AST::Automaton::Rule>::const_iterator iter;
            for (iter = _ast_automaton->GetRules().begin();
                 iter != _ast_automaton->GetRules().end();
                 ++iter) {
               AddStackSymbol(iter->_config->GetSymbol());
               AddStackSymbol(iter->_action->GetSymbol());
            }


            _initial_state = *(_states.begin()); // TODO

            vector<string> state_names_ordered;
            copy(_state_names.begin(), _state_names.end(), back_inserter(state_names_ordered));
            sort(state_names_ordered.begin(), state_names_ordered.end());
            vector<string> stack_alphabet_ordered;
            copy(_stack_alphabet.begin(), _stack_alphabet.end(), back_inserter(stack_alphabet_ordered));

            copy( _state_names.begin(), _state_names.end(), std::ostream_iterator<string>(std::cout, " "));

            copy( _stack_alphabet.begin(), _stack_alphabet.end(), std::ostream_iterator<string>(std::cout, " "));
            cout << endl;

            sort(_states.begin(), _states.end(), less_state_name());
            typename vector<S*>::const_iterator itr;
            for (itr = _states.begin(); itr != _states.end(); ++itr) {
               cout << (*itr)->GetName() << " ";
            }
            cout << endl;

            copy( state_names_ordered.begin(), state_names_ordered.end(), std::ostream_iterator<string>(std::cout, " "));

            /*
            cout << _states.size() << endl;
            // Check names and states are ordered the same way
            typename vector<S*>::const_iterator i1;
            vector<string>::const_iterator i2;
            i2 = state_names_ordered.begin();
            for(i1 = _states.begin(); i1 != _states.end(); ++i1) {
               if (i2 == state_names_ordered.end()) {
                  throw runtime_error("state mismatch! - duplicate state names? [1]");
               }
               cout << (*i1)->GetName() << " | " << *i2 << endl;
               if (0 != (*i1)->GetName().compare(*i2)) {
                  throw runtime_error("state name mismatch!");
               }
               ++i2;
            }
            if (i2 != state_names_ordered.end()) {
               throw runtime_error("state mismatch! - duplicate state names? [2]");
            }
            */

            _config_space = new ConfigurationSpace(state_names_ordered, stack_alphabet_ordered);

            _automaton = new FiniteAutomaton<A, S>( _states, _initial_state, _config_space );

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

            if (!_automaton) {
               throw runtime_error("Failed to construct automaton");
            }
            cout << _automaton->ToString() << endl;
            environment.SetAutomaton( _automaton_name, _automaton );
         }
         virtual ~DeclareAutomatonCommand() {
            delete _ast_automaton;
         }
   };

}

#endif

