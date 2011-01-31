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

using namespace std;

typedef vector<string> Valuation;

class KripkeState : public State, Showable {
private:
   Valuation _valuation;
public:
   typedef const KripkeState &const_reference;
   typedef const KripkeState *const_ptr;
   KripkeState(const string &name, const Valuation &valuation) : State(name), _valuation(valuation) { }

   string ToString() const;
};

// Abstract
class TransitionSystem {
public:   
   typedef const TransitionSystem &const_reference;
   typedef const TransitionSystem *const_ptr;
				
   virtual ~TransitionSystem() = 0;
};

<<<<<<< HEAD
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
=======
class KripkeStructure : public FiniteAutomaton<RegularAction, KripkeState>, public TransitionSystem {
public:
   typedef const KripkeStructure &const_reference;
   KripkeStructure(const vector<KripkeState *> &states, KripkeState *initial_state);
   string ToString() const;
}; 

class PushDownSystem : public FiniteAutomaton<PushDownAction, KripkeState>, public TransitionSystem {
public:
   PushDownSystem(const vector<KripkeState *> &states, KripkeState *initial_state);
>>>>>>> parent of 0b52dbb... more;
   string ToString() const;
};

#endif
