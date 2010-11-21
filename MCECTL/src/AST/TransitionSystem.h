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
   class PushDownAction : public RegularAction { };

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
      vector<string> _propositions;
   public:
      KripkeState(const string &state_name, const vector<string> &propositions) : State(state_name), _propositions(propositions) { }
      //const ::State *Create() {

      //}
      string ToString() const {
         stringstream s;
         s << _state_name << ": ";
         vector<string>::const_iterator iter;
         for (iter = _propositions.begin(); iter != _propositions.end(); ++iter) {
            s << *iter << ",";
         }
            
         return s.str();
      }
   };

   class Automaton : public Showable {
   public:
      bool _owns_states;
      enum Type { DFA, PDA, LTS, PDS };
      struct Rule {
         const State  *state1;
         const Action *action;
         const State  *state2;
         Rule(const State *s, const Action *a, const State *t) : state1(s), action(a), state2(t) { }
      };
   private:
      vector<const State*> _states;
      vector<Rule> _rules;
      Type _type;

   public:
      Automaton(const State *state) : _owns_states(true) {
         _states.push_back(state);
      }

      Automaton(const State *state1, const Action *action, const State *state2) : _owns_states(true) {
         _states.push_back(state1);
         _states.push_back(state2);
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
         s << "STATES:" << endl;
         vector<const State*>::const_iterator state_iter;
         for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
            s << (*state_iter)->ToString() << endl;
         }
         vector<Rule>::const_iterator rule_iter;
         for (rule_iter = _rules.begin(); rule_iter != _rules.end(); ++rule_iter) {
            s << "(" << (*rule_iter).state1->ToString() << ", " << (*rule_iter).action->ToString() << ", " << (*rule_iter).state2->ToString() << ")" << endl;
         }
         return s.str();
      }
   };

//typedef Automaton<RegularAction, State> DFA;

}

#endif
