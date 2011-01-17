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
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "Automata.h"
#include "Showable.h"
#include "Lookup.h"

using namespace std;

// Abstract
class TransitionSystem {
public:   
   typedef const TransitionSystem &const_reference;
   typedef const TransitionSystem *const_ptr;
				
   virtual ~TransitionSystem() = 0;
};

class KripkeStructure : public FiniteAutomaton<RegularAction, RegularConfiguration, KripkeState>, public TransitionSystem {
public:
   typedef const KripkeStructure &const_reference;
   KripkeStructure(const set<KripkeState *> &states, KripkeState *initial_state);
   string ToString() const;
}; 

class PushDownSystem : public FiniteAutomaton<PushDownAction, PushDownConfiguration, KripkeState>, public TransitionSystem {
public:
   PushDownSystem(const set<KripkeState *> &states, KripkeState *initial_state);
   string ToString() const;
};

#endif
