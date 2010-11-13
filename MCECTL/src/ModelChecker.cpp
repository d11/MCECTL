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

#include "ModelChecker.h"

// RESULT

Result::Result(const State &state, bool result) {
   // TODO
}

// CHECK RESULTS

CheckResults::CheckResults() {

}

void CheckResults::AddResult(const Result &result) {
   // TODO
}

// TRANSITION SYSTEM

set<State> TransitionSystem::GetStates() const {
   // TODO
   return set<State>();
}

State TransitionSystem::GetInitialState() const {
   // TODO
   return State();
}

// PREDECESSOR CONFIGURATIONS

bool PredecessorConfigurations::Contains( State state_1, State state_2, Action action) const {
   // TODO
   return false;
}

// RESULTS TABLE

ResultsTable::ResultsTable() { }

bool ResultsTable::HasEntry( Formula::Formula::const_reference formula ) {
   // TODO
   return false;
}

CheckResults ResultsTable::GetEntry( Formula::Formula::const_reference formula ) {
   // TODO
   return CheckResults();
}

void ResultsTable::SetEntry(const CheckResults &check_results) {
   // TODO
}


// MODEL CHECKER

ModelChecker::ModelChecker(
   const Environment &environment,
   const TransitionSystem &transition_system
) : _environment(environment), _system(transition_system) { }

// combine _system and automata
boost::shared_ptr<PushDownSystem> ModelChecker::ConstructProductSystem(
   const Automaton &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   // TODO
   return boost::shared_ptr<PushDownSystem>(new PushDownSystem());
}

void ModelChecker::Visit(const Formula::Until &until) {
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
   const Automaton &automaton = until.GetAutomaton();
   // automata is a PDA

   boost::shared_ptr<PushDownSystem> pds(ConstructProductSystem(automaton, before, after)); 

   CheckResults results;

   StateIterator iter;
   for (iter = _system.GetStates().begin(); iter != _system.GetStates().end(); ++iter) {
      PredecessorConfigurations pc; // Compute predecessor configurations
      if ( pc.Contains(_system.GetInitialState(), *iter, EpsilonAction()) ) {
         // Here, *iter |= E( before U[automata] after )
         Result result(*iter, true);
         results.AddResult( result );
      }
      else {
         Result result(*iter, false); // TODO: counterexample
         results.AddResult( result );
      }
   }

   ResultsTable &results_table = _environment.GetCheckResults(_system);
   results_table.SetEntry( results );
}

void ModelChecker::Visit(const Formula::Release &release) {
   // TODO
}
void ModelChecker::Visit(const Formula::PVar &release) {
   // TODO
}

CheckResults ModelChecker::Check( Formula::Formula::const_reference formula ) {
   ResultsTable &results_table = _environment.GetCheckResults(_system);
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   //CheckResults results = CheckFormula( formula );
   formula.Accept(*this);

   return results_table.GetEntry( formula );
}







