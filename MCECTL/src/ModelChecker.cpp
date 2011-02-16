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

Result::Result(unsigned int config_id, bool result) : _config_id(config_id), _evaluation(result) {
   // TODO
}

string Result::ToString() const {
   stringstream s;
   s << _config_id << ": " << (_evaluation ? "T" : "F");
   return s.str();
}
// CHECK RESULTS

CheckResults::CheckResults() { }

void CheckResults::AddResult(Result *result) {
   // TODO
   _result_map[result->GetID()] = result;
}

string CheckResults::ToString() const {
   stringstream s;
   s << "Check results:" << endl;
   map<unsigned int, Result*>::const_iterator iter;
   for (iter = _result_map.begin(); iter != _result_map.end(); ++iter) {
      cout << (*iter).first << " -> " << (*iter).second->ToString() << endl;
   }
   s << "TODO" << endl;
   return s.str();
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
   cout << "Setting results table entry" << endl;
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
   cout << "Constructing product system" << endl;
   vector<KripkeState*> product_states;
   Valuation v;
   KripkeState *initial_state = new KripkeState("temp",v); // Temp
	vector<string> product_state_names; // Temp; TODO
	vector<string> stack_alphabet; // Temp; TODO
	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);
   /*
   set<string*> propositions;
   Lookup<string> lookup(propositions);
   Valuation v(lookup);
   KripkeState *k = new KripkeState("temp", v); // Temp
   product_states.insert(k); // TEMP! 

   vector<KripkeState> system_states = _system.GetStates();
   vector<State>  automaton_states = automaton.GetStates();

   vector<KripkeState>::const_iterator system_state_iter;
   vector<State >::const_iterator  dummy_state_iter;
   for ( system_state_iter = system_states.begin(); system_state_iter != system_states.end(); ++system_state_iter ) {
      for ( dummy_state_iter = automaton_states.begin(); dummy_state_iter != automaton_states.end(); ++dummy_state_iter) {
         // TODO
      }
   }

   */
   cout << "TODO" << endl;
   return boost::shared_ptr<PushDownSystem>(new PushDownSystem(product_states, initial_state, config_space));
}

void ModelChecker::Visit(const Formula::Until &until) {
   cout << "visiting UNTIL" << endl;
   /*
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
   const Automaton *a(until.GetAutomaton());
   const PDA &automaton = *static_cast<const PDA *>(a); // temp
   // automata is a PDA - - TODO

   boost::shared_ptr<PushDownSystem> pds(ConstructProductSystem(automaton, before, after)); 

   CheckResults results;

   vector<KripkeState>::const_iterator iter;
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
   */
   cout << "TODO" << endl;
}

void ModelChecker::Visit(const Formula::Release &release) {
   cout << "visiting RELEASE" << endl;
   // TODO
   cout << "TODO" << endl;
}
void ModelChecker::Visit(const Formula::PVar &pvar) {
   cout << "visiting PVAR" << endl;
   CheckResults results;

   // TODO
   cout << "TODO" << endl;

   _environment.SetCheckResults(_system, results);
}
void ModelChecker::Visit(const Formula::False &formula_false) {
   cout << "visiting FALSE" << endl;
   // TODO
   cout << "TODO" << endl;
}
void ModelChecker::Visit(const Formula::True &formula_true) {
   cout << "visiting TRUE" << endl;

   CheckResults results;

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      Result *res = new Result(id, true);
      results.AddResult(res);
   }
   _environment.SetCheckResults(_system, results);
}
void ModelChecker::Visit(const Formula::Conjunction &conjunction) {
   cout << "visiting CONJUNCTION" << endl;

   CheckResults left_results  = Check(conjunction.GetLeft());
   CheckResults right_results = Check(conjunction.GetRight());

   CheckResults results;

   cout << "TODO" << endl;
   // TODO

   _environment.SetCheckResults(_system, results);
}
void ModelChecker::Visit(const Formula::Negation &negation) {
   cout << "visiting NEGATION" << endl;
   CheckResults sub_results = Check(negation.GetSubFormula());
   CheckResults results;
   // TODO
   cout << "TODO" << endl;
   _environment.SetCheckResults(_system, results);
}

CheckResults ModelChecker::Check( Formula::Formula::const_reference formula ) {
   const ResultsTable &results_table = _environment.GetCheckResults(_system);
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   formula.Accept(*this);
   return results_table.GetEntry( formula );
}







