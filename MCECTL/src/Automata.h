/*
 * =====================================================================================
 *
 *       Filename:  Automata.h
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef _AUTOMATA_H_
#define _AUTOMATA_H_

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/checked_delete.hpp>

#include "Showable.h"
#include "exception/CommandFailed.h"

// Foreward declararations
typedef unsigned int Configuration;
class KripkeState;
class State;
template<class S,class T>
class ProductState;

// TODO describe
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
      while (iter != _states.end()) {
         if (iter->compare(state_name) == 0) {
            return id;
         }
         ++id;
         ++iter;
      }
      stringstream s;
      s << "Can't find ID for state " << state_name << " in configuration space";
      throw CommandFailed(s.str());
   }
   unsigned int GetSymbolID(const string &stack_symbol) const {
      typename vector<string>::const_iterator iter = _stack_alphabet.begin();
      unsigned int id = 0;
      while (iter->compare(stack_symbol) != 0) {
         if (iter == _stack_alphabet.end()) {
            stringstream s;
            s << "Can't find ID for symbol " << stack_symbol << " in configuration space";
            throw CommandFailed(s.str());
         }
         ++id;
         ++iter;
      }
      return id;
   }
   Configuration MakeConfiguration(const string &state, const string &symbol) const {
      unsigned int state_id = GetStateID(state);
      return MakeConfiguration(state_id, symbol);
   }
   Configuration MakeConfiguration(unsigned int state_id, const string &symbol) const {
      unsigned int symbol_id = GetSymbolID(symbol);
      return state_id * _stack_alphabet.size() + symbol_id;
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

   size_t GetStateCount() const {
      return _states.size();
   }
   size_t GetStackAlphabetSize() const {
      return _stack_alphabet.size();
   }
   size_t GetConfigurationCount() const {
      return _states.size() * _stack_alphabet.size();
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
      s << "Configurations: " << endl;
      vector<Configuration> cs = GetConfigurations();
      vector<Configuration>::const_iterator i;
      for (i = cs.begin(); i != cs.end(); ++i) {
         s << *i << " " << GetStateName(*i) << "," << GetSymbolName(*i) << endl;
      }
      s << endl;
      return s.str();
   }

   vector<string> GetConfigurationNames() const {
      vector<string> out;
      vector<Configuration> cs = GetConfigurations();
      vector<Configuration>::const_iterator i;
      for (i = cs.begin(); i != cs.end(); ++i) {
         out.push_back(GetStateName(*i) + "," + GetSymbolName(*i));
      }
      return out;
   }

   vector<Configuration> GetConfigurations() const {
      vector<Configuration> ids;
      std::copy(boost::counting_iterator<int>(0),
         boost::counting_iterator<int>(_states.size()*_stack_alphabet.size()),
         std::back_inserter(ids));
      return ids;
   }

   vector<Configuration> GetProductConfigurations() const {
      vector<Configuration> ids;
      std::copy(boost::counting_iterator<int>(0),
         boost::counting_iterator<int>(_states.size()),
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

// Abstract base class
class Action : public Showable {
public:
   virtual ~Action() {}
};

class RegularAction : public Action {
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
      return config_space.GetSymbolName(_dest_id);
   }
};

typedef string StackSymbol;

// Abstract base class
class PushDownAction : public Action {
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

   virtual void ApplyToStackTop(string &symbol_1, string &symbol_2) const = 0;
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
   virtual void ApplyToStackTop(string &symbol_1, string &symbol_2) const {
      symbol_1 = _push_symbol;
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
   virtual void ApplyToStackTop(string &symbol_1, string &symbol_2) const {
      symbol_1 = _rewrite_symbol;
      symbol_2 = "";
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
   virtual void ApplyToStackTop(string &symbol_1, string &symbol_2) const {
      symbol_2 = "";
   }
};

/// Standard automaton state (as opposed to labelled)
class State {
protected:
   string _name;
   bool _accepting;
public:
   State(const string &name, bool accepting) : _name(name), _accepting(accepting) {}
   const string &GetName() const { return _name; }
   string ToString() const {
      stringstream s;
      s << "[" << _name << "]";
      return s.str();
   }
   bool GetAccepting() const { return _accepting; }
};

/// Compare two state pointers by referrant name
struct less_state_name {
   inline bool operator() (const State* s1, const State* s2)
   {
      return (s1->GetName() < s2->GetName());
   }
};

template <class A, class S>
class FiniteAutomaton : public Automaton {
public:
   struct Rule {
      unsigned int configuration;
      const A *action;
      Rule(unsigned int config, const A *a) : configuration(config), action(a) {}
      Rule() : configuration(0), action(NULL) {  }
   };
protected:
   vector<S*>     _states;
   S *            _initial_state;
   const ConfigurationSpace *_config_space;
   vector<Rule> _rule_list;
public:
   typedef unsigned int ConfigID;
	typedef       FiniteAutomaton &reference;
	typedef const FiniteAutomaton &const_reference;

   FiniteAutomaton(const vector<S*> &states, S *initial_state, const ConfigurationSpace *config_space)
      : _states(states), _initial_state(initial_state), _config_space(config_space)
   { }

   // TODO - needed ?
   FiniteAutomaton(const FiniteAutomaton<A,S> &automaton) {
      _states        = automaton._states;
      _initial_state = automaton._initial_state;
      _rule_list         = automaton._rule_list;
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
      _rule_list.push_back(Rule(start_id, action));
   };

   vector<S> GetStates() const {
      vector<S> states;
      typename vector<S*>::const_iterator iter;
      for ( iter = _states.begin(); iter != _states.end(); ++iter ) {
         states.push_back(**iter);
      }
      return states;
   };

   const vector<Rule> &GetRules() const {
      return _rule_list;
   }

   const S &GetState(Configuration c) const {
      return GetStateAux((S*)NULL, c);
   }
   // Dispatch based on the template parameter...
   const KripkeState &GetStateAux(const KripkeState *,Configuration c) const {
      return *(_states.at(c));
   }
   const State &GetStateAux(const State *,Configuration c) const {
      return *(_states.at(c / _config_space->GetStackAlphabetSize()));
   }
   const ProductState<State,KripkeState> &GetStateAux(const ProductState<State,KripkeState> *,Configuration c) const {
      return *(_states.at(c));
   }

   /* Return a copy of the initial state */
   S GetInitialState() const {
      return *_initial_state;
   };

   string ToString() const {
      stringstream s;
      s << "CONFIGURATION SPACE:" << endl;
      s << _config_space->ToString() << endl;
      s << "STATES:" << endl;
      typename vector<S*>::const_iterator i_state;
      size_t index = 0;
      for (i_state = _states.begin(); i_state != _states.end(); ++i_state) {
         s << (*i_state == _initial_state ? "*" : " ")
            << " " << _config_space->GetStateID((*i_state)->GetName())
            << ": " << (*i_state)->ToString();
         if ((*i_state)->GetAccepting()) {
            s << " [Accepting]";
         }
         s << endl;
         index++;
      }

      s << "RULES:" << endl;
      typename vector<Rule>::const_iterator i_rule;
      for (i_rule = _rule_list.begin(); i_rule != _rule_list.end(); ++i_rule) {
         s << "(" << i_rule->configuration << ") -> [";
         if (i_rule->action) {
            s << i_rule->action->ToString();
         } else {
            s << "!! NULL !!";
         }
         s << "]" << endl;
      }

      return s.str();
   }


   // Return a string containing a representation of the automaton in Graphviz
   // 'dot' format, to be rendered to an image.
   string ToDot() const {
      stringstream s;
      s << "digraph automaton { " << endl
        << "rankdir=LR;"          << endl
        << "size=\"8,5\""         << endl;

      // Print states / vertices
      typename vector<S*>::const_iterator state_iter;
      for (state_iter = _states.begin(); state_iter != _states.end(); ++state_iter) {
         if (!(*state_iter)) {
            throw CommandFailed("bad state");
         }
         s << "node [shape = \"";
         if ((*state_iter)->GetAccepting()) {
            s << "doublecircle";
         }
         else {
            s << "circle";   
         }
         s << "\"";
         s << ", label = \"" << (*state_iter)->GetName() << "\"]; ";
         s << (*state_iter)->GetName() << endl;
         if (*state_iter == _initial_state) {
            s << "null [shape = plaintext label=\"\"]" << endl
              << "null -> " << (*state_iter)->GetName() << endl;
         }
      }

      // Print rules / edges
      typename vector<Rule>::const_iterator rule_iter;
      for (rule_iter = _rule_list.begin(); rule_iter != _rule_list.end(); ++rule_iter) {

         if (!rule_iter->action) {
            throw CommandFailed("Bad action");
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

   // Obtain a set of the action names used in the rules of the automaton
   set<string> GetActionNames() const {
      set<string> actions;
      typename vector<Rule>::const_iterator iter;
      for (iter = _rule_list.begin(); iter != _rule_list.end(); ++iter) {
         actions.insert(iter->action->GetName());
      }
      return actions;
   }

   // Check whether the automaton is deterministic
   // This is true iff there is exactly one rule for any given head
   // configuration and action pair
   bool IsDeterministic() const {
      set< pair<Configuration, string> > all_heads;
      vector<Configuration> v_configs = GetConfigurations();
      set<string> v_actions = GetActionNames();
      vector<Configuration>::const_iterator iter_config;
      set<string>::const_iterator iter_action;
      for (iter_config = v_configs.begin(); iter_config != v_configs.end(); ++iter_config) {
         for (iter_action = v_actions.begin(); iter_action != v_actions.end(); ++iter_action) {
            all_heads.insert(make_pair(*iter_config, *iter_action));
         }
      }
      //temp
      cout << "All:" << endl;
      set< pair<Configuration, string> >::const_iterator h_iter ;
      for (h_iter = all_heads.begin(); h_iter != all_heads.end(); ++h_iter) {
         cout << h_iter->first << ", " << h_iter->second << endl;
      }

      cout << "Got:" << endl;
      set< pair<Configuration,string> > transitions;
      typename vector<Rule>::const_iterator iter;
      for (iter = _rule_list.begin(); iter != _rule_list.end(); ++iter) {
         Configuration head = iter->configuration;
         const string &action = iter->action->GetName();

         cout << head << ", " << action << endl;
         pair<Configuration,string> transition(head, action);
         // More than one rule with the same head
         if (transitions.find(transition) != transitions.end()) {
            cout << "Duplicate transition: " << endl;
            return false;
         }
         transitions.insert(transition);
      }

      cout << "transitions " << transitions.size() << endl;
      cout << "all heads " << all_heads.size() << endl;
      // Transition function must be total
      return transitions.size() == all_heads.size();
   }

};

// Finite Automaton (May actually be non-deterministic)
typedef FiniteAutomaton<RegularAction, State>  DFA;

// Push Down Automaton
typedef FiniteAutomaton<PushDownAction, State>  PDA;

#endif
