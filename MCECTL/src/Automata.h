/*
 * =====================================================================================
 *
 *       Filename:  Automata.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef _AUTOMATA_H_
#define _AUTOMATA_H_

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cassert>

#include "Showable.h"
#include "Lookup.h"
#include "AutomataConcrete.h"

using namespace std;
class Automaton : public Showable {
public:
   typedef Automaton &reference;
   typedef const Automaton &const_reference;
   virtual string ToDot() const = 0;
   virtual ~Automaton() = 0;
};
template <class A, class C>
class RuleBook : public Showable {
public:
   typedef pair<C, const A*> Rule;
   typedef C configuration_type;

private:
   typedef map<C, vector<const A*> > map_type;
   map_type     _successor_map;
   vector<Rule> _rule_list;

public:
   RuleBook() { }

   void AddRule( const C &configuration, const A *action ) {
      _successor_map[configuration].push_back(action);
      _rule_list.push_back( Rule(configuration, action) );
   }

   const vector<const A*> &GetSuccessors( const C &configuration ) {
      typename map_type::const_iterator iter
         = _successor_map.find(configuration);

      assert(iter != _successor_map.end());
      return iter->second;
   }

   vector<Rule> GetRules() const {
      return _rule_list;
   }

   string ToString() const {
      return "TODO";
   }

};

template <class A, class C, class S>
class FiniteAutomaton : public Automaton {
protected:
   set<S*>     _states;
   S *            _initial_state;
   RuleBook<A, C> _rules;
   Lookup<S, C>   _state_lookup;

public:
	typedef       FiniteAutomaton &reference;
	typedef const FiniteAutomaton &const_reference;

   FiniteAutomaton(const set<S*> &states, S *initial_state)
      : _states(states), _initial_state(initial_state), _rules(), _state_lookup(states)
   { 
   }

   //FiniteAutomaton(const FiniteAutomaton<A,S> &automaton) {
   //   // TODO - needed ?
   //   _states = automaton._states;
   //   _initial_state = automaton._initial_state;
   //   _rules = automaton._rules;
   //}

   virtual ~FiniteAutomaton() {
   }

   void AddRule( const S *state1, A *action, const S *state2 ) {
      _rules.AddRule(state1, action, state2);
   };

   set<S> GetStates() const {
      set<S> states;
      typename set<S*>::const_iterator iter;
      for ( iter = _states.begin(); iter != _states.end(); ++iter ) {
         states.insert(**iter);
      }
      return states;
   };

   S GetInitialState() const {
      return *_initial_state;
   };

   string ToString() const {
      stringstream s;
      s << "STATES:" << endl;
      typename set<S*>::const_iterator iter; size_t index = 0;
      for (iter = _states.begin(); iter != _states.end(); ++iter) {
         s << (*iter == _initial_state ? "*" : " ")
            << " " << index << ": " << (*iter)->ToString() << endl;
         index++;
      }

      s << "RULES:" << endl << _rules.ToString() << endl;
      return s.str();
   }


   string ToDot() const {

      stringstream s;
     // s << "digraph automaton { " << endl
     //   << "rankdir=LR;"          << endl
     //   << "size=\"8,5\""         << endl;

     // typename vector<S*>::const_iterator state_iter;
     // for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
     //    s << "node [shape = circle"; //s << "doublecircle";
     //    s << ", label = \"" << (*state_iter)->ToString() << "\"]; ";
     //    s << (*state_iter)->GetName() << endl;
     //    if (*state_iter == _initial_state) {
     //       s << "null [shape = plaintext label=\"\"]" << endl
     //         << "null -> " << (*state_iter)->GetName() << endl;
     //    }
     // }

     // typedef typename RuleBook<A,C>::Rule Rule;
     // const vector<Rule> &rules(_rules.GetRules());

     // typename vector<Rule>::const_iterator rule_iter;
     // for (rule_iter = rules.begin(); rule_iter != rules.end(); ++rule_iter) {
     //    s << rule_iter->state1->GetName()
     //      << " -> "
     //      << rule_iter->state2->GetName()
     //      << "[ label = \""
     //      << rule_iter->action->ToString()
     //      << "\" ];"
     //      << endl;
     // }

     // s << "}" << endl;
      return s.str();
   }

};


template <class A, class S>
class AutomatonIterator {
   const S &_state;
public:
   AutomatonIterator<A, S> Next(const A &action) = 0;

   vector<const A*> GetAvailableActions() = 0;
   const S &GetState() { return _state; }
   virtual ~AutomatonIterator() { }
};


typedef string StackSymbol;

class PushDownState {

};

class PDA { 

};


#endif
