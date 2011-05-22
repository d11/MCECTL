/*
 *    Filename:  TransitionSystem.h
 *    Description: Labelled Transition System
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

class KripkeState : public State, public Showable {
private:
   string _config_name;
   vector<string> _valuation;
   string _symbol;
public:
	// Constructor
   KripkeState(
			const string &name,
			const string &config_name,
			const string &symbol, 
			const Valuation &valuation
		)
		: State(name, false),
		  _config_name(config_name),
		  _valuation(valuation),
		  _symbol(symbol)
	{ }

	// Determine whether the given proposition variable is true at this state
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

typedef FiniteAutomaton<RegularAction, KripkeState> KripkeStructure;
typedef FiniteAutomaton<PushDownAction, KripkeState> PushDownSystem;

template <class __automaton_state, class __system_state>
class ProductState : public Showable {
private:
   __automaton_state _a;
   __system_state _b;
public:
   ProductState(
			const __automaton_state &a,
			const __system_state &b
		) : _a(a), _b(b)
	{ }
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

typedef	FiniteAutomaton<
				PushDownAction,
				ProductState<State,KripkeState>
			> ProductSystem;
typedef	RuleBook<
				PushDownAction,
				ProductState<State,KripkeState>
			>::Rule ProductRule;

#endif
