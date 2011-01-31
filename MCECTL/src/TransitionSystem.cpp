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
<<<<<<< HEAD
   //vector<const string *> propositions(_valuation.GetEntries());
   //s << "[" << _name << ": "
   //   // TODO
   // // << accumulate(propositions.begin(), propositions.end(), string(""), JoinWithComma)
   //  << "]";
=======
   s << "[" << _name << ": "
     << accumulate(_valuation.begin(), _valuation.end(), string(""), JoinWithComma)
     << "]";
>>>>>>> parent of 0b52dbb... more;
   return s.str();
}

// TRANSITION SYSTEM

TransitionSystem::~TransitionSystem() { }

// KRIPKE STRUCTURE

<<<<<<< HEAD
//KripkeStructure::KripkeStructure(const set<KripkeState *> &states, KripkeState *initial_state)
//   : FiniteAutomaton<RegularAction, RegularConfiguration, KripkeState>( states, initial_state ) { }
KripkeStructure::KripkeStructure(const set<KripkeState *> &states, KripkeState *initial_state) { }

string KripkeStructure::ToString() const {
   stringstream s;
   s << "KRIPKE STRUCTURE:" << endl;
   //  << FiniteAutomaton<RegularAction, RegularConfiguration, KripkeState>::ToString();
=======
KripkeStructure::KripkeStructure(const vector<KripkeState *> &states, KripkeState *initial_state)
   : FiniteAutomaton<RegularAction, KripkeState>( states, initial_state ) { }

string KripkeStructure::ToString() const {
   stringstream s;
   s << "KRIPKE STRUCTURE:" << endl
     << FiniteAutomaton<RegularAction, KripkeState>::ToString();
>>>>>>> parent of 0b52dbb... more;
   return s.str();
}

// PUSH DOWN SYSTEM

<<<<<<< HEAD
PushDownSystem::PushDownSystem(const set<KripkeState *> &states, KripkeState *initial_state) { }
   //: FiniteAutomaton<PushDownAction, PushDownConfiguration, KripkeState>( states, initial_state ) { }

string PushDownSystem::ToString() const {
   stringstream s;
   s << "PUSHDOWN SYSTEM:" << endl;
    // << FiniteAutomaton<PushDownAction, PushDownConfiguration, KripkeState>::ToString();
=======
PushDownSystem::PushDownSystem(const vector<KripkeState *> &states, KripkeState *initial_state)
   : FiniteAutomaton<PushDownAction, KripkeState>( states, initial_state ) { }

string PushDownSystem::ToString() const {
   stringstream s;
   s << "PUSHDOWN SYSTEM:" << endl
     << FiniteAutomaton<PushDownAction, KripkeState>::ToString();
>>>>>>> parent of 0b52dbb... more;
   return s.str();
}

