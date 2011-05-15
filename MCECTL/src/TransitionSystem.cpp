/*
 * =====================================================================================
 *
 *       Filename:  TransitionSystem.cpp
 *    Description:  Definitions for regular and pushdown transition systems
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <string>
#include <sstream>
#include <numeric>

#include "TransitionSystem.h"
#include "Automata.h"
#include "Util.h"

// KRIPKE STATE

string KripkeState::ToString() const {
   stringstream s;
   //vector<const string *> propositions(_valuation.GetEntries());
   s << "[" << GetConfigName() << ": "
      // TODO
     << accumulate(_valuation.begin(), _valuation.end(), string(""), JoinWithComma)
     << "]";
   return s.str();
}

// TRANSITION SYSTEM

/*
TransitionSystem::~TransitionSystem() { }

// KRIPKE STRUCTURE

KripkeStructure::KripkeStructure(const vector<KripkeState *> &states, KripkeState *initial_state, const ConfigurationSpace *config_space)
   : FiniteAutomaton<RegularAction, KripkeState>( states, initial_state, config_space ) { }

string KripkeStructure::ToString() const {
   stringstream s;
   s << "KRIPKE STRUCTURE:" << endl
     << FiniteAutomaton<RegularAction, KripkeState>::ToString();
   return s.str();
}

// PUSH DOWN SYSTEM

PushDownSystem::PushDownSystem(const vector<KripkeState *> &states, KripkeState *initial_state, const ConfigurationSpace *config_space)
   : FiniteAutomaton<PushDownAction, KripkeState>( states, initial_state, config_space ) { }

string PushDownSystem::ToString() const {
   stringstream s;
   s << "PUSHDOWN SYSTEM:" << endl
     << FiniteAutomaton<PushDownAction, KripkeState>::ToString();
   return s.str();
}
*/
