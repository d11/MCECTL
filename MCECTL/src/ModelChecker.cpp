/*
 * =====================================================================================
 *
 *       Filename:  ModelChecker.cpp
 *    Description:  Extended CTL model-checking algorithm
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <set>
#include <vector>
#include <map>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "TransitionSystem.h"
#include "ModelChecker.h"
using namespace std;

// RESULT

Result::Result(const KripkeState &state, bool result) {
   // TODO
}

// CHECK RESULTS

CheckResults::CheckResults() {

}

void CheckResults::AddResult(const Result &result) {
   // TODO
}

string CheckResults::ToString() const {
   return "TODO";
}

// PREDECESSOR CONFIGURATIONS

template <class A>
bool PredecessorConfigurations<A>::Contains( const KripkeState *state_1, const KripkeState *state_2, const A *action) const {
   // TODO
   return false;
}

// RESULTS TABLE

ResultsTable::ResultsTable() { }

bool ResultsTable::HasEntry( Formula::Formula::const_reference formula ) const {
   // TODO
   return false;
}

CheckResults ResultsTable::GetEntry( Formula::Formula::const_reference formula ) const {
   // TODO
   return CheckResults();
}

void ResultsTable::SetEntry(const CheckResults &check_results) {
   // TODO
}


// MODEL CHECKER

ModelChecker::ModelChecker(
   Environment &environment,
   const KripkeStructure &transition_system
) : _environment(environment), _system(transition_system) { }

// combine _system and automata
boost::shared_ptr<PushDownSystem> ModelChecker::ConstructProductSystem(
   const PDA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {

   set<KripkeState*> product_states;
   set<string*> propositions;
   Lookup<string> lookup(propositions);
   Valuation v(lookup);
   KripkeState *k = new KripkeState("temp", v); // Temp
   product_states.insert(k); // TEMP! 

   set<KripkeState> system_states = _system.GetStates();
   set<State>  automaton_states = automaton.GetStates();

   set<KripkeState>::const_iterator system_state_iter;
   set<State >::const_iterator  dummy_state_iter;
   for ( system_state_iter = system_states.begin(); system_state_iter != system_states.end(); ++system_state_iter ) {
      for ( dummy_state_iter = automaton_states.begin(); dummy_state_iter != automaton_states.end(); ++dummy_state_iter) {
         // TODO
      }
   }

   return boost::shared_ptr<PushDownSystem>(new PushDownSystem(product_states, k));
}

void ModelChecker::Visit(const Formula::Until &until) {
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
   const Automaton *a(until.GetAutomaton());
   const PDA &automaton = *static_cast<const PDA *>(a); // temp
   // automata is a PDA - - TODO

   boost::shared_ptr<PushDownSystem> pds(ConstructProductSystem(automaton, before, after)); 

   CheckResults results;

   set<KripkeState>::const_iterator iter;
   for (iter = _system.GetStates().begin(); iter != _system.GetStates().end(); ++iter) {
      //PredecessorConfigurations<NondeterministicPushDownAction> pc; // Compute predecessor configurations

      // TODOO!!
      //if ( pc.Contains(_system.GetInitialState(), *iter, new PushDownEpsilonAction()) ) {
      //   // Here, *iter |= E( before U[automata] after )
      //   Result result(*iter, true);
      //   results.AddResult( result );
      //}
      //else {
      //   Result result(*iter, false); // TODO: counterexample
      //   results.AddResult( result );
      //}
   }

   _environment.SetCheckResults(_system, results);
}

void ModelChecker::Visit(const Formula::Release &release) {
   // TODO
}
void ModelChecker::Visit(const Formula::PVar &release) {
   // TODO
}
void ModelChecker::Visit(const Formula::False &formula_false) {
   // TODO
}
void ModelChecker::Visit(const Formula::True &formula_true) {
   // TODO
}
void ModelChecker::Visit(const Formula::Conjunction &conjunction) {
   // TODO
}
void ModelChecker::Visit(const Formula::Negation &negation) {
   // TODO
}

CheckResults ModelChecker::Check( Formula::Formula::const_reference formula ) {
   const ResultsTable &results_table = _environment.GetCheckResults(_system);
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   formula.Accept(*this);

   return results_table.GetEntry( formula );
}







