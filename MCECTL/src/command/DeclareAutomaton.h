/*
 * =====================================================================================
 *
 *       Filename:  DeclareAutomaton.h
 *    Description:  Command for creating automaton/system objects from their ASTs
 *
 * =====================================================================================
 */

#ifndef _DECLARE_AUTOMATON_H
#define _DECLARE_AUTOMATON_H

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

	// Foreward declaration
   template <class A, class S>
   class DeclareAutomatonCommand;

	// Default functor for creating states
   template <class A, class S>
   class StateCreator
	: public binary_function<
					const AST::State *,
					DeclareAutomatonCommand<A, S> &,
					void
				>
	{
   public:
      void operator()(
			const AST::State *ast_state,
			DeclareAutomatonCommand<A, S> &command
		) const {
			// This default functor shouldn't be used, so if called it indicates a
			// template error
         throw CommandFailed("don't know how to convert");
      }
   };

	// Functor for creating BASIC states (i.e. not Kripke; the states used by
	// normal automata)
   template <class A>
   class StateCreator<A, State>
	: public binary_function<
					const AST::State *,                  // first argument type
					DeclareAutomatonCommand<A, State> &, // second argument type
					void >                               // return type
	{
   public:
      void operator()(
				const AST::State *ast_state,                
				DeclareAutomatonCommand<A, State> &command           
			) const
		{
			// Check the AST is the expected type, then use it to create the
			// state object
         if (ast_state->GetType() == AST::State::BASIC) {
            State *state = new State(
						ast_state->GetName(),
						ast_state->GetAccepting()
					);
            command.CreateState(state);
         }
         else {
            throw CommandFailed("Bad state type");
         }
      }
   };

	// Functor for creating Kripke or Pushdown Kripke states. A pushdown Kripke
	// state represents an assignation of a propositional valuation to some
	// particular *head configuration* of a pushdown system; a plain Kripke
	// state is what is used in a regular Labelled Transition System.
   template <class A>
   class StateCreator<A, KripkeState> 
	: public binary_function<
					const AST::State *,                        // first arg type
					DeclareAutomatonCommand<A, KripkeState> &, // second arg type
					void >                                     // return type
	{
   public:
      void operator()(
				const AST::State *ast_state, 
				DeclareAutomatonCommand<A, KripkeState> &command
			) const 
		{
         if (ast_state->GetType() == AST::State::KRIPKE) {
				// Create the state object
            KripkeState *state = new KripkeState(
						ast_state->GetName(),        // name
						ast_state->GetName(),        // configuration name
						"_",                         // stack_symbol
						ast_state->GetPropositions() // valuation
					);
            command.CreateState(state);
         }
         else if (ast_state->GetType() == AST::State::PUSHDOWN_KRIPKE) {
				// Construct the head configuration name
				ostringstream s;
				s << "<" << ast_state->GetName() << "," 
				  << ast_state->GetSymbol() << ">";

				// Create the state object
            KripkeState *state = new KripkeState(
						ast_state->GetName(),        // name
						s.str(),                     // configuration name
						ast_state->GetSymbol(),      // stack_symbol
						ast_state->GetPropositions() // valuation
					);

				// Also record the stack symbol
            command.AddStackSymbol(ast_state->GetSymbol());
            command.CreateState(state);
         }
         else {
            throw CommandFailed("Bad state type");
         }
      }
   };


   template <class A, class S>
   class RuleCreator
		: public binary_function<
						AST::Automaton::Rule,
						DeclareAutomatonCommand<A, S> &,
						void > {
   public:
		void operator()(
			AST::Automaton::Rule rule,
			DeclareAutomatonCommand<A, S> &command
		) const {
         command.CreateRule(rule._config, rule._action);
      }
   };

	// Functor for creating a regular (non-pushdown) transition rule
   template <class S>
   class RuleCreator<RegularAction, S>
	: public binary_function<
		AST::Automaton::Rule,
		DeclareAutomatonCommand<RegularAction, S> &,
		void>
	{
   public:
		void operator()(
				AST::Automaton::Rule rule,
				DeclareAutomatonCommand<RegularAction, S> &command
			) const
		{
         command.CreateRule(rule._config, rule._action);
      }
   };

   template <class A, class S>
	class DeclareAutomatonCommand : public Command {
	private:
		// AST of the automaton/system to be created
		const AST::Automaton *_ast_automaton;
		// Name under which the result will be stored
		string _automaton_name;
		// Map from state names to state objects
		map<string, S*> _state_map;
		// Additional list of state object
		vector<S*> _states;
		// Pointer to the initial state object
		S* _initial_state;
		// Result automaton
		FiniteAutomaton<A,S> *_automaton;
		// Set of state names (ensure uniqueness)
		set<string> _state_names;
		// Set of stack symbols
		set<string> _stack_alphabet;
		// Head configurations of the automaton/system
		ConfigurationSpace *_config_space;
	public:
		// Constructor: store the AST and the name
		DeclareAutomatonCommand(const string &name, const AST::Automaton *dfa)
			: _ast_automaton(dfa), _automaton_name(name), _config_space(NULL)
		{ }

		// Description of this declaration command
		virtual string ToString() const {
			stringstream s;
			s  << "Automaton " << _automaton_name
			   << " { " << _ast_automaton->ToString() << " }";
			return s.str();
		}

		// Record a state in all of the necessary structures
		void CreateState(S *state) {
			_state_names.insert(state->GetName());
			_state_map.insert(make_pair<string, S*>(state->GetName(), state));
			_states.push_back(state);
		}

		// Add a symbol to the stack alphabet
		void AddStackSymbol(const string &symbol) {
			string trimmed_symbol = boost::algorithm::trim_copy(symbol);
			if (trimmed_symbol != "") {
				_stack_alphabet.insert(trimmed_symbol);
			}
		}

		// Create a regular action object
		void CreateAction(RegularAction **action, AST::Action *ast_action) {
			if (ast_action->GetType() != AST::Action::REGULAR) {
				throw CommandFailed("Action type is not regular");
			}
			*action = new RegularAction(
					ast_action->GetName(),
					ast_action->GetStateID(*_config_space)
				);
		}

		// Create an pushdown action object of the desired type
		void CreateAction(PushDownAction **action, AST::Action *ast_action) {
			switch(ast_action->GetType()) {
				case AST::Action::REGULAR:
					throw CommandFailed("Action type shouldn't be regular");
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
					throw CommandFailed("Bad action type");
					break;
			}

		}

		// Add a rule to the automaton: first find the source configuration for
		// the rule, and then create the rule's action
		void CreateRule(const AST::Configuration *config, AST::Action *ast_action ) {
			int start_id = config->GetID(*_config_space);
			A *action = NULL;
			CreateAction(&action, ast_action);
			if (!action) { throw CommandFailed("failed to create action"); }
			_automaton->AddRule(start_id, action);
		}

		// Main work of the command - the stages of creating the automaton object
		void CreateAutomaton() {

			// All automaton and systems have '_', the stack bottom, in their
			// alphabet (even if they do not use the stack at all)
			_stack_alphabet.insert("_");

			// Create all states from the AST
			for_each(
				_ast_automaton->GetStates().begin(),
				_ast_automaton->GetStates().end(),
				bind2nd(StateCreator<A, S>(), *this)
			);

			// Create all symbols mentioned in any rule
			typename vector<AST::Automaton::Rule>::const_iterator iter;
			for (iter = _ast_automaton->GetRules().begin();
					iter != _ast_automaton->GetRules().end();
					++iter) {
				AddStackSymbol(iter->_config->GetSymbol());
				AddStackSymbol(iter->_action->GetSymbol());
			}

			// The first state listed is taken as the initial one
			_initial_state = *(_states.begin()); 

			// Take the state names and stack symbols to work out the possible
			// head configurations - this set is represented by a Configuration
			// Space. We sort them, for consistency.
			vector<string> state_names_ordered;
			vector<string> stack_alphabet_ordered;
			copy(
				_state_names.begin(), _state_names.end(),
				back_inserter(state_names_ordered)
			);
			copy(
				_stack_alphabet.begin(), _stack_alphabet.end(), 
				back_inserter(stack_alphabet_ordered)
			);
			sort(state_names_ordered.begin(), state_names_ordered.end());
			sort(_states.begin(), _states.end(), less_state_name());

			_config_space = new ConfigurationSpace(
				state_names_ordered, 
				stack_alphabet_ordered
			);

			// Create the actual automaton object, using the above
			_automaton = new FiniteAutomaton<A, S>( 
				_states, 
				_initial_state, 
				_config_space 
			);

			// Add each of the rules from the AST to the automaton/system
			for_each(
				_ast_automaton->GetRules().begin(),
				_ast_automaton->GetRules().end(),
				bind2nd(RuleCreator<A, S>(), *this)
			);

		}

		// Attempt to create an automaton or system, and if successful, store
		// the result in the environment under the appropriate name.
		virtual void Execute(Environment &environment, GlobalOptions &options) {
			CreateAutomaton();
			if (!_automaton) {
				throw CommandFailed("Failed to construct automaton");
			}
			environment.SetAutomaton( _automaton_name, _automaton );
		}

		// Destructor: AST is no longer needed
		virtual ~DeclareAutomatonCommand() {
			delete _ast_automaton;
		}
	};

}

#endif

