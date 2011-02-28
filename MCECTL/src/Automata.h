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

#include <boost/iterator/counting_iterator.hpp>
#include <boost/checked_delete.hpp>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>

#include "Showable.h"

typedef unsigned int Configuration;

/*struct numbering_ostream_iterator : public ostream_iterator<string> {
   private:
      unsigned int _count;
   public:
   numbering_ostream_iterator(ostream &s) : ostream_iterator<string>(s, "\n"), _count(0) { }
   numbering_ostream_iterator(const numbering_ostream_iterator& iter) : ostream_iterator<string>(iter), _count(iter._count) { }
         numbering_ostream_iterator&
      operator=(const string& s)
      {
         return *this;
      }
         
      ostream_iterator&
      operator*()
      { 
         return *this; }

      ostream_iterator&
      operator++()
      { return *this; }

      ostream_iterator&
      operator++(int)
      { return *this; }
};*/

class ConfigurationSpace : public Showable {
private:
   vector<string> _states;
   vector<string> _stack_alphabet;
public:
   ConfigurationSpace(const vector<string> &states, const vector<string> &stack_alphabet)
      : _states(states), _stack_alphabet(stack_alphabet) { }
   const vector<string> &GetStates() const { return _states; }
   const vector<string> &GetStackAlphabet() const { return _stack_alphabet; }

   unsigned int GetStateID(const string &state_name) const {
      typename vector<string>::const_iterator iter = _states.begin();
      unsigned int id = 0;
      while (iter->compare(state_name) != 0) {
         if (iter == _states.end()) {
            stringstream s;
            s << "Can't find ID for state " << state_name << " in configuration space";
            throw runtime_error(s.str());
         }
         ++id;
         ++iter;
      }
      return id;
   }
   unsigned int GetSymbolID(const string &stack_symbol) const {
      typename vector<string>::const_iterator iter = _stack_alphabet.begin();
      unsigned int id = 0;
      while (iter->compare(stack_symbol) != 0) {
         if (iter == _stack_alphabet.end()) {
            stringstream s;
            s << "Can't find ID for symbol " << stack_symbol << " in configuration space";
            throw runtime_error(s.str());
         }
         ++id;
         ++iter;
      }
      return id;
   }

   string GetStateName(unsigned int state_id) const {
      return _states.at(state_id / _stack_alphabet.size());
   }
   string GetStateNameByID(unsigned int state_id) const {
      return _states.at(state_id);
   }
   string GetSymbolName(unsigned int symbol_id) const {
      return _stack_alphabet.at(symbol_id % _stack_alphabet.size());
   }

   size_t GetStackAlphabetSize() const {
      return _stack_alphabet.size();
   }

   string ToString() const {
      stringstream s;
      s << "State names: " << endl;
      //copy( _states.begin(), _states.end(), ostream_iterator<string>(s, " "));
      //copy( _states.begin(), _states.end(), numbering_ostream_iterator(s));
      vector<string>::const_iterator iter;
      int index = 0;
      for (iter = _states.begin(); iter != _states.end(); ++iter) {
         s << index++ << ": " << *iter << endl;
      }
      s << endl << "Stack alphabet:" << endl;
      //copy( _stack_alphabet.begin(), _stack_alphabet.end(), ostream_iterator<string>(s, " "));
      index = 0;
      for (iter = _stack_alphabet.begin(); iter != _stack_alphabet.end(); ++iter) {
         s << index++ << ": " << *iter << endl;
      }
      s << endl;
      return s.str();
   }

   vector<Configuration> GetConfigurations() const {
      vector<Configuration> ids;
      std::copy(boost::counting_iterator<int>(0),
         boost::counting_iterator<int>(_states.size()*_stack_alphabet.size()),
         std::back_inserter(ids));
      return ids;
   }
   
};

using namespace std;
class Automaton : public Showable {
private:
	unsigned int _id;
	static unsigned int _next_id;
public:
   typedef Automaton &reference;
   typedef const Automaton &const_reference;
	Automaton() : _id(_next_id++) { }
   virtual string ToDot() const = 0;
   virtual ~Automaton() = 0;
   virtual unsigned int GetID() const { return _id; }
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
   unsigned int _dest_id;
public:
   RegularAction(const string &action_name, unsigned int dest_id) : _action_name(action_name), _dest_id(dest_id) {}
   const string &GetName() const { return _action_name; }
   string ToString() const {
      stringstream s;
      s << _action_name << " -> (" << _dest_id << ")";
      return s.str();
   }
   string GetDestStateName(const ConfigurationSpace &config_space) const {
      return config_space.GetStateName(_dest_id);
   }
   unsigned int GetDestStateID() const {
      return _dest_id;
   }

   virtual string GetDestSymbolName(const ConfigurationSpace &config_space) const {
      return config_space.GetSymbolName(_dest_id); // TODO
   }
};

typedef string StackSymbol;

class NondeterministicPushDownAction : public Action {

};


// Abstract
class PushDownAction : public NondeterministicPushDownAction {
protected:
   string _action_name;
   unsigned int _dest_id;
public:
   PushDownAction(const string &action_name, unsigned int dest_id) : _action_name(action_name), _dest_id(dest_id) { }
   virtual ~PushDownAction() {}
   const string &GetName() const { return _action_name; }
   string ToString() const {
      stringstream s;
      s << _action_name << " -> (" << _dest_id << ")";
      return s.str();
   }
   string GetDestStateName(const ConfigurationSpace &config_space) const {
      return config_space.GetStateNameByID(_dest_id);
   }
   void SetDestStateID(unsigned int dest_id) {
      _dest_id = dest_id;
   }
   unsigned int GetDestStateID() const {
      return _dest_id;
   }
   virtual PushDownAction *Clone() const = 0;
   virtual string GetDestSymbolName(const ConfigurationSpace &config_space) const = 0;
};

class PushAction : public PushDownAction {
private:
   StackSymbol _push_symbol;
public:
   PushAction(const string &action_name, unsigned int dest_id, const StackSymbol &push_symbol)
      : PushDownAction(action_name, dest_id), _push_symbol(push_symbol) { }
   string ToString() const {
      stringstream s;
      s << PushDownAction::ToString() << ": Push " << _push_symbol;
      return s.str();
   }
   virtual PushDownAction *Clone() const {
      return new PushAction(_action_name, _dest_id, _push_symbol);
   }
   virtual string GetDestSymbolName(const ConfigurationSpace &config_space) const {
      return "push " + _push_symbol; // TODO
   }
};

class RewriteAction : public PushDownAction {
private:
   StackSymbol _rewrite_symbol;
public:
   RewriteAction(const string &action_name, unsigned int dest_id, const StackSymbol &rewrite_symbol)
      : PushDownAction(action_name, dest_id), _rewrite_symbol(rewrite_symbol) { }
   string ToString() const {
      stringstream s;
      s << PushDownAction::ToString() << ": Rewrite to " << _rewrite_symbol;
      return s.str();
   }
   virtual PushDownAction *Clone() const {
      return new RewriteAction(_action_name, _dest_id, _rewrite_symbol);
   }
   virtual string GetDestSymbolName(const ConfigurationSpace &config_space) const {
      return _rewrite_symbol;
   }
};

class PopAction : public PushDownAction {
public:
   PopAction(const string &action_name, unsigned int dest_id)
      : PushDownAction(action_name, dest_id) { }
   string ToString() const {
      stringstream s;
      s << PushDownAction::ToString() << ": Pop";
      return s.str();
   }
   virtual PushDownAction *Clone() const {
      return new PopAction(_action_name, _dest_id);
   }
   virtual string GetDestSymbolName(const ConfigurationSpace &config_space) const {
      return "_";
   }
};

class PushDownEpsilonAction : public NondeterministicPushDownAction {
public:
   string ToString() const;
};

/// Standard automaton state (as opposed to labelled)
class State {
protected:
   string _name;
public:
   State(const string &name) : _name(name) {}
   const string &GetName() const { return _name; }
   string ToString() const {
      stringstream s;
      s << "[" << _name << "]";
      return s.str();
   }
};

/// Compare two state pointers by referrant name
struct less_state_name {
   inline bool operator() (const State* s1, const State* s2)
   {
      return (s1->GetName() < s2->GetName());
   }
};

template <class A, class S>
class RuleBook : public Showable {
public:
   struct Rule {
      unsigned int configuration;
      const A *action;
      Rule(unsigned int config, const A *a) : configuration(config), action(a) {}
   };
private:
   map<const S*, size_t> _index_lookup;
//TransitionTable *_transition_table;
   vector<Rule> _rule_list;
public:
   RuleBook( const vector<S*> &states ) {
      typename vector<S*>::const_iterator iter;
      size_t index = 0;
      for( iter = states.begin(); iter != states.end(); ++iter ) {
         _index_lookup.insert( make_pair<S*, size_t>(*iter, index));
         ++index;
      }
      //_transition_table = new TransitionTable(index, index);
   }

   void AddRule(unsigned int start_id, A *action) {
      /*
      typename map<const S*, size_t>::const_iterator iter;
      iter = _index_lookup.find(state1);
      if (iter == _index_lookup.end()) {
         throw runtime_error(string("Trying to add rule for action with unknown state: ") + state1->ToString());
      }
      */
      //size_t i = iter->second;
      /*
      iter = _index_lookup.find(state2);
      if (iter == _index_lookup.end()) {
         throw runtime_error(string("Trying to add rule for action with unknown state: ") + state1->ToString());
      }*/
      //size_t j = iter->second;
      //TransitionTable &tt = *_transition_table;
      //tt(i, j).push_back(action);
      //_rule_list.push_back(Rule(state1, action));
      _rule_list.push_back(Rule(start_id, action));
   }

   const vector<Rule> &GetRules() const {
      return _rule_list;
   }

   vector< pair<typename A::const_ptr, S* > > GetSuccessors( S* state );

   string ToString() const {
      stringstream s;
      typename vector<Rule>::const_iterator iter;
      for (iter = _rule_list.begin(); iter != _rule_list.end(); ++iter) {
         s << "(" << iter->configuration << ") -> [";
         if (iter->action) {
            s << iter->action->ToString();
         } else {
            s << "?? NULL !!";
         }
         s << "]" << endl;
      }

      /*
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
      */
      return s.str();
   }

};

template <class A, class S>
class FiniteAutomaton : public Automaton {
protected:
   vector<S*>     _states;
   S *            _initial_state;
   RuleBook<A, S> _rules;
   const ConfigurationSpace *_config_space;
public:
   typedef unsigned int ConfigID;
	typedef       FiniteAutomaton &reference;
	typedef const FiniteAutomaton &const_reference;

   FiniteAutomaton(const vector<S*> &states, S *initial_state, const ConfigurationSpace *config_space)
      : _states(states), _initial_state(initial_state), _rules(states), _config_space(config_space)
   { }

   // TODO - needed ?
   FiniteAutomaton(const FiniteAutomaton<A,S> &automaton) {
      _states        = automaton._states;
      _initial_state = automaton._initial_state;
      _rules         = automaton._rules;
      _config_space  = automaton._config_space;
   }

   virtual ~FiniteAutomaton() {
      for_each(_states.begin(), _states.end(), boost::checked_deleter<S>() );
   }

   const ConfigurationSpace &GetConfigurationSpace() const {
      return *_config_space;
   }

   vector<Configuration> GetConfigurations() const {
      return _config_space->GetConfigurations();
   }

	string GetConfigurationByID(unsigned int config_id) const {
		stringstream s;
		s << "(" << _config_space->GetStateName(config_id) + ", " << _config_space->GetSymbolName(config_id) << ")";
		return s.str();
	}

   void AddRule(unsigned int start_id, A *action) {
      _rules.AddRule(start_id, action);
   };

   vector<S> GetStates() const {
      vector<S> states;
      typename vector<S*>::const_iterator iter;
      for ( iter = _states.begin(); iter != _states.end(); ++iter ) {
         states.push_back(**iter);
      }
      return states;
   };

   const RuleBook<A,S> &GetRules() const {
      return _rules;
   }

   const S &GetState(Configuration c) const {
      return *(_states.at(c / _config_space->GetStackAlphabetSize()));
   }

   S GetInitialState() const {
      return *_initial_state;
   };

   string ToString() const {
      stringstream s;
      s << "CONFIGURATION SPACE:" << endl;
      s << _config_space->ToString() << endl;
      s << "STATES:" << endl;
      typename vector<S*>::const_iterator iter; size_t index = 0;
      for (iter = _states.begin(); iter != _states.end(); ++iter) {
         s << (*iter == _initial_state ? "*" : " ")
            << " " << _config_space->GetStateID((*iter)->GetName()) << ": " << (*iter)->ToString() << endl;
         index++;
      }

      s << "RULES:" << endl << _rules.ToString() << endl;
      return s.str();
   }


   string ToDot() const {

      cout << "DOTTING!" << endl;

      cout << "config space:" << endl;
      cout << _config_space->ToString() << endl;

      stringstream s;
      s << "digraph automaton { " << endl
        << "rankdir=LR;"          << endl
        << "size=\"8,5\""         << endl;

      typename vector<S*>::const_iterator state_iter;
      for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
         if (!(*state_iter)) {
            throw runtime_error("bad state");
         }
         s << "node [shape = circle"; //s << "doublecircle";
         //s << ", label = \"" << (*state_iter)->ToString() << "\"]; ";
         s << ", label = \"" << (*state_iter)->GetName() << "\"]; ";
         s << (*state_iter)->GetName() << endl;
         if (*state_iter == _initial_state) {
            s << "null [shape = plaintext label=\"\"]" << endl
              << "null -> " << (*state_iter)->GetName() << endl;
         }
      }

      typedef typename RuleBook<A,S>::Rule Rule;
      const vector<Rule> &rules(_rules.GetRules());

      typename vector<Rule>::const_iterator rule_iter;
      for (rule_iter = rules.begin(); rule_iter != rules.end(); ++rule_iter) {

         if (!rule_iter->action) {
            throw runtime_error("Bad action");
         }
         if (!rule_iter->configuration) {
            throw runtime_error("Bad configuration");
         }
         s << _config_space->GetStateName(rule_iter->configuration)
           << " -> ";
         s << rule_iter->action->GetDestStateName(*_config_space)
           << "[ label = \"";
         s << rule_iter->action->GetName()
           << ", ";
         s << _config_space->GetSymbolName(rule_iter->configuration)
           << "/";
         s  << rule_iter->action->GetDestSymbolName(*_config_space)
           << "\" ];"
           << endl;
      }

      s << "}" << endl;
      return s.str();
   }

};

// Deterministic Finite Automaton
typedef FiniteAutomaton<RegularAction,           State>  DFA;

// Non-determinic Finite Automaton
typedef FiniteAutomaton<NondeterministicAction,  State>  NFA;

// Deterministic Push Down Automaton
typedef FiniteAutomaton<PushDownAction,          State>  PDA;

template <class A, class S>
class AutomatonIterator {
   const S &_state;
public:
   AutomatonIterator<A, S> Next(const A &action) = 0;

   vector<const A*> GetAvailableActions() = 0;
   const S &GetState() { return _state; }
   virtual ~AutomatonIterator() { }
};

template <class S>
class RegularAutomatonIterator : public AutomatonIterator<RegularAction, S> {

};

class RegularConfiguration {

};

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


#endif
