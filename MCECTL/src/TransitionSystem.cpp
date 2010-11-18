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

#include "TransitionSystem.h"
#include "Automata.h"

// KRIPKE STATE

string KripkeState::ToString() const {
   stringstream s;
   Valuation::const_iterator iter;
   for ( iter = _valuation.begin(); iter != _valuation.end(); iter++ )
   {
      //tmp += "'";
      s << *iter;
      //tmp += (*iter).first;
      //tmp += " -> ";
      //tmp += (*iter).second ? "T" : "F";
      s << ", ";
   }
   return s.str();
}

// TRANSITION SYSTEM

TransitionSystem::~TransitionSystem() { }

// KRIPKE STRUCTURE

KripkeStructure::KripkeStructure(const vector<KripkeState *> &states, KripkeState *initial_state)
   : FiniteAutomaton<RegularAction, KripkeState>( states, initial_state ) { }

string KripkeStructure::ToString() const {
   stringstream s;
   s << "KRIPKE STRUCTURE:" << endl
     << FiniteAutomaton<RegularAction, KripkeState>::ToString();
   return s.str();
}

// PUSH DOWN SYSTEM

PushDownSystem::PushDownSystem(const vector<KripkeState *> &states, KripkeState *initial_state)
   : FiniteAutomaton<PushDownAction, KripkeState>( states, initial_state ) { }

string PushDownSystem::ToString() const {
   stringstream s;
   s << "PUSHDOWN SYSTEM:" << endl
     << FiniteAutomaton<PushDownAction, KripkeState>::ToString();
   return s.str();
}

