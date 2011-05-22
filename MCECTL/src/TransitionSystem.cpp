/*
 * =====================================================================================
 *
 *       Filename:  TransitionSystem.cpp
 *    Description:  Definitions for regular and pushdown transition systems
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
   s << "[" << GetConfigName() << ": "
     << accumulate(_valuation.begin(), _valuation.end(), string(""), JoinWithComma)
     << "]";
   return s.str();
}
