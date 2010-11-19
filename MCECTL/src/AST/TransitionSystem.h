/*
 * =====================================================================================
 *
 *       Filename:  TransitionSystem.h
 *    Description:  AST for systems
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _AST_TRANSITIONSYSTEM_H_
#define  _AST_TRANSITIONSYSTEM_H_

#include <vector>

namespace AST {

   class RegularAction {
   private:
      string _action_name;
   public:
      RegularAction(const string &action_name) : _action_name(action_name) { }
      const string &GetName() const { return _action_name; }
   };

   class KripkeState { };
   class DummyState {
   private:
      string _state_name;
   public:
      DummyState(const string &state_name) : _state_name(state_name) { }
      const string &GetName() const { return _state_name; }
   };

   template <class A, class S>
   class TransitionSystem {
   public:
      struct Rule {
         S state1;
         A action;
         S state2;
         Rule(const S &s, const A &a, const S &t) : state1(s), action(a), state2(t) { }
      };
   private:
      vector<S> _states;
      vector<Rule> _rules;

   public:
      TransitionSystem(const S &state) {
         _states.push_back(state);
      }

      TransitionSystem(const S &state1, const A &action, const S &state2) {
         Rule rule(state1, action, state2);
         _rules.push_back(rule);
      }

      void Assimilate(const TransitionSystem<A,S> *ts) {
         typename vector<S>::const_iterator state_iter;
         for (state_iter = ts->_states.begin(); state_iter != ts->_states.end(); ++state_iter) {
            _states.push_back(*state_iter);
         }
         typename vector<Rule>::const_iterator rule_iter;
         for (rule_iter = ts->_rules.begin(); rule_iter != ts->_rules.end(); ++rule_iter) {
            _rules.push_back(*rule_iter);
         }
      }

      const vector<S> &GetStates() const { return _states; }
      const vector<Rule> &GetRules() const { return _rules; }
   };

   typedef TransitionSystem<RegularAction, DummyState> DFA;

}

#endif
