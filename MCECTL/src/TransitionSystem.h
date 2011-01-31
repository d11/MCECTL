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

class KripkeStructure : public Showable {
public:
   typedef const KripkeStructure &const_reference;
   KripkeStructure(const set<KripkeState *> &states, KripkeState *initial_state);
   set<KripkeState> GetStates() const;
   string ToString() const;
}; 

class PushDownSystem : public Showable {
public:
   PushDownSystem(const set<KripkeState *> &states, KripkeState *initial_state);
   set<KripkeState> GetStates() const;
   string ToString() const;
};

#endif
