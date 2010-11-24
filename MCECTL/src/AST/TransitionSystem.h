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
#include "Automata.h"
#include <numeric>
#include "Util.h"

namespace AST {

   class Action : public Showable { };
   class RegularAction : public Action {
   private:
      string _action_name;
   public:
      RegularAction(const string &action_name) : _action_name(action_name) { }
      const string &GetName() const { return _action_name; }
      string ToString() const {
         return _action_name;
      }
   };
   class PushDownAction : public RegularAction {
   private:
      enum { PUSH, POP, REWRITE } _type;
      StackSymbol _symbol;
   public:

   
   };

   class PushDownEffect { };

   class State : public Showable {
   protected:
      string _state_name;
   public:
      State(const string &state_name) : _state_name(state_name) { }
      const string &GetName() const { return _state_name; }
      virtual const ::State *Create() {
         return new ::State(_state_name);
      }
      string ToString() const {
         return _state_name;
      }
   };
   class KripkeState : public State {
   protected:
      vector<string> _propositions;
   public:
      KripkeState(const string &state_name, const vector<string> &propositions) : State(state_name), _propositions(propositions) { }
      vector<string> GetPropositions() const { return _propositions; }
      //const ::State *Create() {

      //}
      string ToString() const {
         stringstream s;
         s << _state_name << ": "
           << accumulate(_propositions.begin(), _propositions.end(), string(""), JoinWithComma);
         return s.str();
      }
   };

   class PushDownState : public State {
   protected:
      StackSymbol _symbol;
   public:
      PushDownState(const string &state_name, const StackSymbol &stack_symbol) : State(state_name), _symbol(stack_symbol) { }
      StackSymbol GetSymbol() const { return _symbol; }
      string ToString() const {
         stringstream s;
         s << _state_name << "[" << _symbol << "]";
         return s.str();
      }
   };

   class PushDownKripkeState : public KripkeState {
   protected:
      StackSymbol _symbol;
   public:
      PushDownKripkeState(const string &state_name, const StackSymbol &stack_symbol, const vector<string> &propositions) : KripkeState(state_name, propositions), _symbol(stack_symbol) { }
      StackSymbol GetSymbol() const { return _symbol; }
      string ToString() const {
         stringstream s;
         s << _state_name << "[" << _symbol << "]" << " : "
           << accumulate(_propositions.begin(), _propositions.end(), string(""), JoinWithComma);
         return s.str();
      }
   };

   class Automaton : public Showable {
   public:
      bool _owns_states;
      enum Type { DFA, PDA, LTS, PDS };
      struct Rule {
         string state1;
         const Action *action;
         string state2;
         Rule(const string &s, const Action *a, const string &t) : state1(s), action(a), state2(t) { }
      };
   private:
      vector<const State*> _states;
      vector<Rule> _rules;
      Type _type;

   public:
      Automaton(const State *state) : _owns_states(true) {
         _states.push_back(state);
      }

      Automaton(const string &state1, const Action *action, const string &state2) : _owns_states(true) {
         Rule rule(state1, action, state2);
         _rules.push_back(rule);
      }
      ~Automaton() {

         if (_owns_states) {
            typename vector<const State*>::const_iterator state_iter;
            for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
               delete *state_iter;
            }
            typename vector<Rule>::const_iterator rule_iter;
            for (rule_iter = _rules.begin(); rule_iter != _rules.end(); ++rule_iter) {
               delete rule_iter->action;
            }
         }
      }

      void SetType(Type type) {
         _type = type;
      }

      void Assimilate(Automaton *ts) {
         vector<const State*>::const_iterator state_iter;
         for (state_iter = ts->_states.begin(); state_iter != ts->_states.end(); ++state_iter) {
            _states.push_back(*state_iter);
         }
         vector<Rule>::const_iterator rule_iter;
         for (rule_iter = ts->_rules.begin(); rule_iter != ts->_rules.end(); ++rule_iter) {
            _rules.push_back(*rule_iter);
         }
         ts->_owns_states = false;
         delete ts;
      }

      const vector<const State*> &GetStates() const { return _states; }
      const vector<Rule>         &GetRules()  const { return _rules;  }
      const Type                 &GetType()   const { return _type;   }

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
         vector<const State*>::const_iterator state_iter;
         for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
            s << (*state_iter)->ToString() << endl;
         }
         vector<Rule>::const_iterator rule_iter;
         for (rule_iter = _rules.begin(); rule_iter != _rules.end(); ++rule_iter) {
            s << "(" << rule_iter->state1 << ", " << (*rule_iter).action->ToString() << ", " << rule_iter->state2 << ")" << endl;
         }
         return s.str();
      }
   };

//typedef Automaton<RegularAction, State> DFA;

}

#endif
