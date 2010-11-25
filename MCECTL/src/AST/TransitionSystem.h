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

   class Action : public Showable {
   public:
      enum Type { REGULAR, PUSH, POP, REWRITE } _type;
   private:
      string _action_name;
      StackSymbol _symbol;
   public:
      Action(const string &name) : _action_name(name) { }
      const string &GetName() const { return _action_name; }
      string ToString() const {
         return _action_name;
      }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }
      StackSymbol GetSymbol() const { return _symbol; }
   };
   class PushDownEffect { };

   class State : public Showable {
   public:
      enum Type { BASIC, KRIPKE, PUSHDOWN, PUSHDOWN_KRIPKE };
   private:
      string _state_name;
      vector<string> _propositions;
      StackSymbol _symbol;
      Type _type;
   public:
      State(const string &state_name, Type type) : _state_name(state_name), _type(type) { }
      const string &GetName() const { return _state_name; }

      Type GetType() const { return _type; }

      void AddPropositions(const vector<string> &propositions) {
         copy(propositions.begin(), propositions.end(), back_inserter(_propositions));
      }

      vector<string> GetPropositions() const { return _propositions; }
      StackSymbol GetSymbol() const { return _symbol; }
      void SetSymbol(StackSymbol symbol) { _symbol = symbol; }
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
      vector<State*> _states;
      vector<Rule> _rules;
      Type _type;

   public:
      Automaton(State *state) : _owns_states(true) {
         _states.push_back(state);
      }

      Automaton(const string &state1, const Action *action, const string &state2) : _owns_states(true) {
         Rule rule(state1, action, state2);
         _rules.push_back(rule);
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
               delete rule_iter->action;
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
         delete &ts;
      }

      const vector<State*> &GetStates() const { return _states; }
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
         vector<State*>::const_iterator state_iter;
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
