/*
 * =====================================================================================
 *
 *       Filename:  AST/TransitionSystem.h
 *    Description:  AST for systems
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _AST_TRANSITIONSYSTEM_H_
#define  _AST_TRANSITIONSYSTEM_H_

#include <vector>
#include <set>
#include <numeric>
#include <iterator>
#include <boost/checked_delete.hpp>

#include "Automata.h"
#include "Util.h"

namespace AST {

   class Effect : public Showable {
   protected:
      string _destination_state;
   public:
      Effect(const string &dest) : _destination_state(dest) { }
      void SetDestinationState(const string &dest) {
         _destination_state = dest;
      }
      string GetDestinationState() const {
         return _destination_state;
      }
      virtual string ToString() const {
         stringstream s;
         s << "Regular effect: go to state " << _destination_state << endl;
         return s.str();
      }

      virtual StackSymbol GetSymbol() const {
         //throw runtime_error("Can't get symbol; not pushdown effect");
         return "_";
      }
   };
   class PushDownEffect : public Effect {
   public:
      StackSymbol _symbol;
      enum Type { PUSH, POP, REWRITE };
      Type _type;
      PushDownEffect( const Type &type  ) : Effect(""), _symbol(""), _type(type) { }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }
      StackSymbol GetSymbol() const { return _symbol; }

      virtual string ToString() const {
         stringstream s;
         s << "Pushdown effect:" << endl
           << "go to state " << _destination_state << endl
           << "and ";
         if      ( _type == PUSH    )  { s << "push symbol " << _symbol << " onto stack" << endl; } 
         else if ( _type == POP     )  { s << "pop top symbol from stack" << endl;                } 
         else if ( _type == REWRITE )  { s << "rewrite top stack symbol to " << _symbol << endl;  } 
         else                          { s << "???";                                              } 
         return s.str();
      }
      Type GetType() const { return _type; }
      
   };

   class Action : public Showable {
   public:
      enum Type { REGULAR, PUSH, POP, REWRITE } _type;
   private:
      string _action_name;
      const Effect *_effect;
   public:
      Action(const string &name, const Effect *effect) : _action_name(name), _effect(effect) { }
      const string &GetName() const { return _action_name; }
      string ToString() const {
         stringstream s;
         s << "Action of type ";
         if      ( _type == REGULAR )  { s << "REGULAR"; } 
         else if ( _type == PUSH    )  { s << "PUSH";    } 
         else if ( _type == POP     )  { s << "POP";     } 
         else if ( _type == REWRITE )  { s << "REWRITE"; } 
         else                          { s << "???";     } 
         s << endl << "with name " << _action_name
           << endl << "and effect " << _effect->ToString();
         return s.str();
      }
      Type GetType() const { return _type; }
      void SetType(Type type) { _type = type; }

      unsigned int GetStateID(const ConfigurationSpace &config_space) const {
         return config_space.GetStateID(_effect->GetDestinationState());
      }
      StackSymbol GetSymbol() const { return _effect->GetSymbol(); }
   };
   class State : public Showable {
   public:
      enum Type { BASIC, KRIPKE, PUSHDOWN, PUSHDOWN_KRIPKE };
   private:
      string _state_name;
      vector<string> _propositions;
      Type _type;
      StackSymbol _symbol;
		bool _accepting;
   public:
      State(const string &state_name, Type type)
			: _state_name(state_name), _type(type), _accepting(false) { }

      const string &GetName() const { return _state_name; }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }

      StackSymbol GetSymbol() const { return _symbol; }

      Type GetType() const { return _type; }

      void AddPropositions(const vector<string> &propositions) {
         copy(propositions.begin(), propositions.end(), back_inserter(_propositions));
      }
      vector<string> GetPropositions() const { return _propositions; }

		void SetAccepting() { _accepting = true; }
		bool GetAccepting() const { return _accepting; }

      string ToString() const {
         stringstream s;
         s << _state_name;
         if (_type == PUSHDOWN || _type == PUSHDOWN_KRIPKE) 
            s << "[" << _symbol << "]";
         if (_type == KRIPKE || _type == PUSHDOWN_KRIPKE)
            s << ": " << accumulate(_propositions.begin(), _propositions.end(), string(""), JoinWithComma);
         return s.str();
      }
   };

   class Configuration : public Showable {
   protected:
      string _state_name;
   public:
      Configuration(const string &state_name) : _state_name(state_name) { }
      string GetName() const { return _state_name; }
      virtual int GetID(const ConfigurationSpace &config_space) const {
         unsigned int state_id = config_space.GetStateID(_state_name);
         return state_id;
      };
      virtual string ToString() const {
         stringstream s;
         s << "Regular configuration; state name " << _state_name;
         return s.str();
      }
      virtual StackSymbol GetSymbol() const { return "_"; }
   };

   class PushDownConfiguration : public Configuration {
   private:
      string _stack_symbol;
   public:
      PushDownConfiguration(const string &state_name, const string &stack_symbol) : Configuration(state_name), _stack_symbol(stack_symbol) { }
      string GetSymbol() { return _stack_symbol; }
      virtual string ToString() const {
         stringstream s;
         s << "Pushdown configuration; state name " << _state_name
           << ", stack symbol " << _stack_symbol;
         return s.str();
      }

      virtual int GetID(const ConfigurationSpace &config_space) const {
         unsigned int state_id = config_space.GetStateID(_state_name);
         unsigned int symbol_id = config_space.GetSymbolID(_stack_symbol);
         return state_id * config_space.GetStackAlphabet().size() + symbol_id;
      };
   };

   class Automaton : public Showable {
   public:
      bool _owns_states;
      enum Type { DFA, PDA, LTS, PDS };
      struct Rule {
         Configuration *_config;
         Action *_action;
         Rule(Configuration *config, Action *action) : _config(config), _action(action) { }
      };
   private:
      vector<State*> _states;
      vector<Rule> _rules;
      Type _type;


   public:
      Automaton(State *state) : _owns_states(true) {
         _states.push_back(state);
      }

      Automaton(Configuration *config, Action *action) : _owns_states(true) {
         _rules.push_back(Rule(config, action));
      }
      ~Automaton() {

         if (_owns_states) {
            for_each(_states.begin(), _states.end(), boost::checked_deleter<State>() );
            typename vector<Rule>::const_iterator rule_iter;
            for (rule_iter = _rules.begin(); rule_iter != _rules.end(); ++rule_iter) {
               delete rule_iter->_action;
            }
         }
      }

      void SetType(Type type) {
         _type = type;
      }

      void Assimilate(Automaton &ts) {
         copy(ts._states.begin(), ts._states.end(), back_inserter(_states));
         copy(ts._rules.begin(), ts._rules.end(), back_inserter(_rules));
         ts._owns_states = false;
         // TODO
         //delete &ts;
      }

      const vector<State*>  &GetStates() const { return _states; }
      const vector<Rule> &GetRules()  const { return _rules;  }
      const Type            &GetType()   const { return _type;   }

      string ToString() const {
         stringstream s;
         s << "AUTOMATON ABSTRACT SYNTAX TREE" << endl;
         s << "Type: ";
         switch (_type) {
            case DFA: s << "DFA"; break;
            case LTS: s << "LTS"; break;
            case PDA: s << "PDA"; break;
            case PDS: s << "PDS"; break;
            default: s << "undefined"; break;
         }
         s << endl << "STATES:" << endl;
         vector<State*>::const_iterator state_iter;
         for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
            s << (*state_iter)->ToString() << endl;
         }
         vector<Rule>::const_iterator rule_iter;
         for (rule_iter = _rules.begin(); rule_iter != _rules.end(); ++rule_iter) {
				// TODO
            //s << "(" << rule_iter->state1 << ", " << (*rule_iter).action->ToString() << ", " << rule_iter->state2 << ")" << endl;
         }
         return s.str();
      }
   };

}

#endif
