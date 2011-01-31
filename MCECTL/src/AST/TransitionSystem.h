/*
 * =====================================================================================
 *
 *       Filename:  Automaton.h
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

   class Effect {
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

   };
   class PushDownEffect : public Effect {
   public:
      StackSymbol _symbol;
      enum Type { PUSH, POP, REWRITE };
      Type _type;
      PushDownEffect( const Type &type  ) : Effect(""), _symbol(""), _type(type) { }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }
      StackSymbol GetSymbol() const { return _symbol; }
   };

   class Action : public Showable {
   public:
      enum Type { REGULAR, PUSH, POP, REWRITE } _type;
   private:
      string _action_name;
      StackSymbol _symbol;
      Effect _effect;
   public:
      Action(const string &name, const Effect &effect) : _action_name(name), _effect(effect) { }
      const string &GetName() const { return _action_name; }
      string ToString() const {
         return _action_name;
      }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }
      StackSymbol GetSymbol() const { return _symbol; }
   };
   class State : public Showable {
   public:
      enum Type { BASIC, KRIPKE, PUSHDOWN, PUSHDOWN_KRIPKE };
   private:
      string _state_name;
      vector<string> _propositions;
      Type _type;
   public:
      State(const string &state_name, Type type) : _state_name(state_name), _type(type) { }
      const string &GetName() const { return _state_name; }

      Type GetType() const { return _type; }

      void AddPropositions(const vector<string> &propositions) {
         copy(propositions.begin(), propositions.end(), back_inserter(_propositions));
      }

      vector<string> GetPropositions() const { return _propositions; }
      string ToString() const {
         stringstream s;
         s << _state_name;
   //      if (_type == PUSHDOWN || _type == PUSHDOWN_KRIPKE) 
   //         s << "[" << _symbol << "]";
         if (_type == KRIPKE || _type == PUSHDOWN_KRIPKE)
            s << ": " << accumulate(_propositions.begin(), _propositions.end(), string(""), JoinWithComma);
         return s.str();
      }
   };

   class Configuration {
   protected:
      string _state_name;
   public:
      Configuration(const string &state_name) : _state_name(state_name) { }
      string GetName() const { return _state_name; }
   };

   class PushDownConfiguration : public Configuration {
   private:
      string _stack_symbol;
   public:
      PushDownConfiguration(const string &state_name, const string &stack_symbol) : Configuration(state_name), _stack_symbol(stack_symbol) { }
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
            //typename vector<const State*>::const_iterator state_iter;
            //for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
            //   delete *state_iter;
            //}
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
            //s << "(" << rule_iter->state1 << ", " << (*rule_iter).action->ToString() << ", " << rule_iter->state2 << ")" << endl;
         }
         return s.str();
      }
   };

//typedef Automaton<RegularAction, State> DFA;

}

#endif
