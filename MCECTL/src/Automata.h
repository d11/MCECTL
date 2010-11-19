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

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
//#include <boost/weak_ptr.hpp>
//#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "Showable.h"

using namespace std;
class Automaton : public Showable {
public:
   typedef Automaton &reference;
   typedef const Automaton &const_reference;
   virtual ~Automaton() = 0;
};

template <class T>
class AutomatonIterator {

};

// Abstract
class Action : public Showable {
public:
   typedef const Action &const_reference;
   typedef const Action *const_ptr;
   virtual ~Action() {}
};

class NondeterministicAction : public Action {

};

class EpsilonAction : public NondeterministicAction {
public:
   string ToString() const;
};

class RegularAction : public NondeterministicAction {
private:
   string _action_name;
public:
   RegularAction(const string &action_name) : _action_name(action_name) {}
   const string &GetName() const { return _action_name; }
   string ToString() const {
      return _action_name;
   }
};

typedef string StackSymbol;

class NondeterministicPushDownAction : public Action {

};


// Abstract
class PushDownAction : public NondeterministicPushDownAction {
protected:
   string _action_name;
   StackSymbol _top_symbol;
public:
   PushDownAction(const string &action_name, const StackSymbol &top_symbol) : _action_name(action_name), _top_symbol(top_symbol) { }
   virtual ~PushDownAction() {}
};

class PushAction : public PushDownAction {
private:
   StackSymbol _push_symbol;
public:
   PushAction(const string &action_name, const StackSymbol &top_symbol, const StackSymbol &push_symbol)
      : PushDownAction(action_name, top_symbol), _push_symbol(push_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Push " << _push_symbol;
      return s.str();
   }
};

class RewriteAction : public PushDownAction {
private:
   StackSymbol _rewrite_symbol;
public:
   RewriteAction(const string &action_name, const StackSymbol &top_symbol, const StackSymbol &rewrite_symbol)
      : PushDownAction(action_name, top_symbol), _rewrite_symbol(rewrite_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Rewrite " << _top_symbol << " to " << _rewrite_symbol;
      return s.str();
   }
};

class PopAction : public PushDownAction {
public:
   PopAction(const string &action_name, const StackSymbol &top_symbol)
      : PushDownAction(action_name, top_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Pop";
      return s.str();
   }
};

class PushDownEpsilonAction : public NondeterministicPushDownAction {
public:
   string ToString() const;
};

class DummyState {
private:
   string _name;
public:
   DummyState(const string &name) : _name(name) {}
   const string &GetName() const { return _name; }
   string ToString() const {
      stringstream s;
      s << "[" << _name << "]";
      return s.str();
   }
};

template <class A, class S>
class RuleBook : public Showable {
public:
   struct Rule {
      const S *state1;
      const A *action;
      const S *state2;
      Rule(const S *s1, const A *a, const S *s2) : state1(s1), action(a), state2(s2) {}
   };
private:
   map<const S*, size_t> _index_lookup;
   typedef boost::numeric::ublas::matrix< vector< A* > > TransitionTable;
   TransitionTable *_transition_table;
   vector<Rule> _rule_list;
public:
   RuleBook( const vector<S*> &states ) {
      typename vector<S*>::const_iterator iter;
      size_t index = 0;
      for( iter = states.begin(); iter != states.end(); ++iter ) {
         _index_lookup.insert( make_pair<S*, size_t>(*iter, index));
         ++index;
      }
      _transition_table = new TransitionTable(index, index);
   }

   void AddRule( const S* state1, A *action, const S* state2 ) {
      size_t i = _index_lookup.find(state1)->second;
      size_t j = _index_lookup.find(state2)->second;
      TransitionTable &tt = *_transition_table;
      tt(i, j).push_back(action);
      _rule_list.push_back(Rule(state1, action, state2));
   }

   const vector<Rule> &GetRules() const {
      return _rule_list;
   }

   vector< pair<typename A::const_ptr, S* > > GetSuccessors( S* state );

   string ToString() const {
      stringstream s;
      size_t columns = _transition_table->size2();

      size_t col = 0;
      s << "  ";
      while ( col < columns ) {
         s << col << "  ";
         col++;
      }
      s << endl;

      size_t row = 0;
      typename TransitionTable::const_iterator2 iter_row;
      for (iter_row = _transition_table->begin2(); iter_row != _transition_table->end2(); ++iter_row) {
         col = 0;
         s << row << "  ";
         typename TransitionTable::const_iterator1 iter_col;
         for (iter_col = iter_row.begin(); iter_col != iter_row.end(); ++iter_col) {
            typename vector<A*>::const_iterator iter_action;
            //s << "{";
            for (iter_action = (*iter_col).begin(); iter_action != (*iter_col).end(); ++iter_action) {
               s << (*iter_action)->ToString() << ",";
            }
            //s << "}";
            s << " ";
            col++;
         }
         s << endl;
         row++;
      }
      return s.str();
   }

};

template <class A, class S>
class FiniteAutomaton : public Automaton {
protected:
   vector<S*>     _states;
   S *            _initial_state;
   RuleBook<A, S> _rules;
public:
	typedef       FiniteAutomaton &reference;
	typedef const FiniteAutomaton &const_reference;

   FiniteAutomaton(const vector<S*> &states, S *initial_state)
      : _states(states), _initial_state(initial_state), _rules(states) 
   { }

   FiniteAutomaton(const FiniteAutomaton<A,S> &automaton) {
      // TODO - needed ?
      _states = automaton._states;
      _initial_state = automaton._initial_state;
      _rules = automaton._rules;
   }

   virtual ~FiniteAutomaton() {
      typename vector<S*>::iterator iter;
      for ( iter = _states.begin(); iter != _states.end(); ++iter ) {
         delete *iter;
         *iter = NULL;
      }
   }

   void AddRule( const S *state1, A *action, const S *state2 ) {
      _rules.AddRule(state1, action, state2);
   };

   vector<S> GetStates() const {
      vector<S> states;
      typename vector<S*>::const_iterator iter;
      for ( iter = _states.begin(); iter != _states.end(); ++iter ) {
         states.push_back(**iter);
      }
      return states;
   };

   S GetInitialState() const {
      return *_initial_state;
   };

   string ToString() const {
      stringstream s;
      s << "STATES:" << endl;
      typename vector<S*>::const_iterator iter; size_t index = 0;
      for (iter = _states.begin(); iter != _states.end(); ++iter) {
         s << (*iter == _initial_state ? "*" : " ")
            << " " << index << ": " << (*iter)->ToString() << endl;
         index++;
      }

      s << "RULES:" << endl << _rules.ToString() << endl;
      return s.str();
   }


   string ToDot() const {
      /*
         digraph finite_state_machine {
         rankdir=LR;
         size="8,5"
         node [shape = doublecircle]; LR_0 LR_3 LR_4 LR_8;
         node [shape = circle];
         LR_0 -> LR_2 [ label = "SS(B)" ];
         LR_0 -> LR_1 [ label = "SS(S)" ];
         */

      stringstream s;
      s << "digraph automaton { " << endl
        << "rankdir=LR;"          << endl
        << "size=\"8,5\""         << endl;

      typename vector<S*>::const_iterator state_iter;
      for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
         s << "node [shape = circle]; " << (*state_iter)->GetName() << endl;
      }

      typedef typename RuleBook<A,S>::Rule Rule;
      const vector<Rule> &rules(_rules.GetRules());

      typename vector<Rule>::const_iterator rule_iter;
      for (rule_iter = rules.begin(); rule_iter != rules.end(); ++rule_iter) {
         s << rule_iter->state1->GetName()
           << " -> "
           << rule_iter->state2->GetName()
           << "[ label = \""
           << rule_iter->action->ToString()
           << "\" ];"
           << endl;
      }

      return s.str();
   }

};

class RegexBuilder {

};

// Deterministic Finite Automaton
typedef FiniteAutomaton<RegularAction,           DummyState>  DFA;

// Non-determinic Finite Automaton
typedef FiniteAutomaton<NondeterministicAction,  DummyState>  NFA;

// Deterministic Push Down Automaton
typedef FiniteAutomaton<PushDownAction,          DummyState>  PDA;

//template <class T>
//class DFA {
//public:
//   struct node;
//   typedef boost::shared_ptr<T> state_ref;
//   typedef boost::shared_ptr<node> node_ref;
//   typedef vector<node_ref> node_vect;
//   struct node {
//      private:
//         state_ref _state;
//         map<string, node_ref> _successors;
//         bool _terminal;
//      public:
//         node(state_ref state, bool terminal) : _state(state), _terminal(terminal) { }
//         void AddSuccessor(string action, node_ref succ) {
//         // TODO check deterministic
//            _successors[action] = succ;
//         }
//         map<string, node_ref> GetSuccessors() const {
//            return _successors;
//         }
//         string ToString() const {
//            string s = "[Node]"; // TODO ?
//            return s;
//         }
//   } ;
//private:
//   node_vect _nodes;
//   node_ref _initial_node;
//public:
//   DFA(const vector<node_ref> &nodes, const node_ref &initial) : _nodes(nodes), _initial_node(initial) {
//      cout << "Initting DFA" << endl;
//   }
//   DFA(string &action) {
//      node_ref initial(new node(state_ref(static_cast<T*>(NULL)), false));
//      node_ref final(new node(state_ref(static_cast<T*>(NULL)), true));
//      initial->AddSuccessor(action, final);
//      _nodes.push_back(initial);
//      _nodes.push_back(final);
//      _initial_node = initial;
//   }
//   vector< RegularAction > _actions;
//   AutomatonIterator< RegularAction > &initialState();
//
//   // Messy - temporary
//   string ToString() const {
//      stringstream s;
//      s << "DFA {\n";
//      typename node_vect::const_iterator iter;
//      int i = 0;
//      for (iter = _nodes.begin(); iter != _nodes.end(); ++iter) {
//         ++i;
//         node_ref node(*iter);
//         s << i << ". " << node->ToString() << " ( ";
//         
//         typename map<string, node_ref>::const_iterator jter;
//         map<string, node_ref> successors((*iter)->GetSuccessors());
//         for (jter = successors.begin(); jter != successors.end(); ++jter) {
//            s << jter->first + " -> ?, ";
//            int index = distance(_nodes.begin(), find(_nodes.begin(), _nodes.end(), jter->second));
//            s << index;
//         }
//         s << " )\n";
//      }
//      s << "}\n";
//      // TODO swizzle
//      return s.str();
//   }
//
//   void Seq( DFA<T>* dfa ) { }
//   void Or( DFA<T>* dfa ) { }
//   void Kleene() { }
//};
//
//class DFABasic : public DFA<DummyState> {
//
//   public:
//   DFABasic(string &action) : DFA<DummyState>(action) { }
//};

//typedef DFABasic *dfa_ref;


#endif
