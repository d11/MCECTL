/*
 *
 *    Filename:  TransitionSystem.h
 *
 *    Description:  
 *
 *    Labelled Transition System
 */

#ifndef _TRANSITION_SYSTEM_H_
#define _TRANSITION_SYSTEM_H_

#include <map>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

#include "Automata.h"
#include "Showable.h"

using namespace std;

typedef vector<string> Valuation;

class KripkeState : public State, Showable {
private:
   string _config_name;
   vector<string> _valuation;
   string _symbol;
public:
   KripkeState(const string &name, const string &config_name, const string &symbol, const Valuation &valuation) : State(name, false), _config_name(config_name), _valuation(valuation), _symbol(symbol) { }

   bool Evaluate(const string &pvar) const {
      vector<string>::const_iterator iter;
      iter = find(_valuation.begin(), _valuation.end(), pvar);
      return iter != _valuation.end();
   }

	const vector<string> &GetValuation() const { return _valuation; }
   string ToString() const;
   string GetConfigName() const { return _config_name; }
   string GetSymbol() const { return _symbol; }
};

// Abstract
/*
class TransitionSystem {
public:   
   typedef const TransitionSystem &const_reference;
   typedef const TransitionSystem *const_ptr;
				
   virtual ~TransitionSystem() = 0;
};

class KripkeStructure : public FiniteAutomaton<RegularAction, KripkeState>, public TransitionSystem {
public:
   typedef const KripkeStructure &const_reference;
   KripkeStructure(const vector<KripkeState *> &states, KripkeState *initial_state, const ConfigurationSpace *config_space);
   string ToString() const;
}; 

class PushDownSystem : public FiniteAutomaton<PushDownAction, KripkeState>, public TransitionSystem {
public:
   PushDownSystem(const vector<KripkeState *> &states, KripkeState *initial_state, const ConfigurationSpace *config_space);
   string ToString() const;
};*/

typedef FiniteAutomaton<RegularAction, KripkeState> KripkeStructure;
typedef FiniteAutomaton<PushDownAction, KripkeState> PushDownSystem;

template <class __automaton_state, class __system_state>
class ProductState : public Showable {
private:
   __automaton_state _a;
   __system_state _b;
public:
   ProductState(const __automaton_state &a, const __system_state &b) : _a(a), _b(b) { }
   const __automaton_state &GetFirst()  const { return _a; }
   const __system_state &GetSecond() const { return _b; }
   string GetName() const {
      stringstream s;
      s << "(" << _a.GetName() << "," << _b.GetName() << ")";
      return s.str();
   }
   string GetConfigName() const {
      stringstream s;
      s << "(" << _a.GetName() << "," << _b.GetConfigName() << ")";
      return s.str();
   }
   string ToString() const {
      stringstream s;
      s << "(" << _a.ToString() << ", " << _b.ToString() << ")";
      return s.str();
   }
   bool GetAccepting() const {
      return _a.GetAccepting(); 
   }
};

typedef FiniteAutomaton<PushDownAction, ProductState<State,KripkeState> > ProductSystem;
typedef RuleBook<PushDownAction,ProductState<State,KripkeState> >::Rule ProductRule;

#endif
