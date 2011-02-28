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
#include <algorithm>
#include <numeric>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "TransitionSystem.h"
#include "ModelChecker.h"
#include "Util.h"
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
   _result_map[result->GetID()] = result;
}

string CheckResults::ToString() const {
   stringstream s;
   s << "{" << endl;
   map<unsigned int, Result*>::const_iterator iter;
   for (iter = _result_map.begin(); iter != _result_map.end(); ++iter) {
      cout << "  State " << (*iter).first << " -> [" << (*iter).second->ToString() <<"]" << endl;
   }
   s << "}" << endl;
   return s.str();
}

const Result &CheckResults::GetResult(unsigned int id) const {
   map<unsigned int, Result *>::const_iterator iter = _result_map.find(id);
   if (iter == _result_map.end()) {
      throw runtime_error("Error retrieving result - bad ID");
   }
   return *(iter->second);
}

const map<unsigned int, Result *> &CheckResults::GetResults() const {
   return _result_map;
}

// PREDECESSOR CONFIGURATIONS

template <class A>
bool PredecessorConfigurations<A>::Contains( const KripkeState *state_1, const KripkeState *state_2, const A *action) const {
   // TODO
   return false;
}

// RESULTS TABLE

ResultsTable::ResultsTable(const Environment &env, const KripkeStructure &lts) : _environment(env), _system(lts) { 
   cout << "Creating new results table" << endl;
}

bool ResultsTable::HasEntry( Formula::Formula::const_reference formula ) const {
   map<unsigned int, CheckResults *>::const_iterator iter;
   iter = _entries.find(formula.GetID());
   return iter != _entries.end();
}

const CheckResults *ResultsTable::GetEntry( Formula::Formula::const_reference formula ) const {
   return _entries.find(formula.GetID())->second;
}

void ResultsTable::SetEntry(Formula::Formula::const_reference formula, CheckResults *check_results) {
   cout << "Setting results table entry" << endl;
   _entries.insert(make_pair(formula.GetID(), check_results));
}


class JoinStateResults {
private:
   const KripkeStructure &_system;
public:
   JoinStateResults(const KripkeStructure &lts) : _system(lts) { }

   string operator()( const string &a, const pair<unsigned int,Result*> &b ) {
      stringstream bs;
      unsigned int config_id = b.first;
      bs << "{ " << _system.GetConfigurationByID(config_id) << ": "
         << b.second->ToString()
         << " }" << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

class JoinCheckResults {
private:
   const Environment &_environment;
   const KripkeStructure &_system;
public:

   JoinCheckResults(const Environment &env, const KripkeStructure &lts) : _environment(env), _system(lts) { }

   string operator()( const string &a, const pair<unsigned int,CheckResults*> &b ) {
      const map<unsigned int, Result *> &result_map = b.second->GetResults();
      stringstream bs;
      unsigned int formula_id = b.first;
      bs << "{ " << _environment.GetFormulaByID(formula_id).ToString()
         << ": " << 
         accumulate(result_map.begin(),
                    result_map.end(),
                    string(""),
                    JoinStateResults(_system))
         << " }" << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

string ResultsTable::ToString() const {
   stringstream s;
   s << "==================" << endl;
   s << "FULL RESULTS TABLE" << endl;
   s << accumulate(_entries.begin(), _entries.end(), string(""), JoinCheckResults(_environment, _system));
   s << "==================" << endl;
   return s.str();
}

// MODEL CHECKER

ModelChecker::ModelChecker(
   Environment &environment,
   const KripkeStructure &transition_system
) : _environment(environment), _system(transition_system) { }

// combine _system and automata
PushDownSystem *ModelChecker::ConstructProductSystem(
   const PDA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   cout << "Constructing product system" << endl;
   vector<KripkeState*> product_states;
   Valuation v;
   KripkeState *initial_state = NULL;

	vector<string> product_state_names;
	
	vector<KripkeState> system_states(_system.GetStates());
	vector<State> automaton_states(automaton.GetStates());
	vector<KripkeState>::const_iterator i1;
	vector<State>::const_iterator i2;
	for (i1 = system_states.begin(); i1 != system_states.end(); ++i1) {
		for (i2 = automaton_states.begin(); i2 != automaton_states.end(); ++i2) {
			stringstream new_state_name;
			new_state_name << "(" << i1->GetName() << "," << i2->GetName() << ")";
			KripkeState *new_state = new KripkeState(new_state_name.str(), i1->GetValuation());
			product_states.push_back(new_state);
			product_state_names.push_back(new_state_name.str());
			if (i1->GetName() == _system.GetInitialState().GetName()
				&& i2->GetName() == automaton.GetInitialState().GetName()) {
				initial_state = new_state;
			}
		}
	}

	if (initial_state == NULL) {
		throw runtime_error("Failed to find initial state..!");
	}

	const ConfigurationSpace &automaton_config_space(automaton.GetConfigurationSpace());
	vector<string> stack_alphabet = automaton_config_space.GetStackAlphabet();
	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);

	PushDownSystem *product_system = new PushDownSystem(product_states, initial_state, config_space);

	cout << product_system->ToString() << endl;

	const ConfigurationSpace &system_config_space(_system.GetConfigurationSpace());
	typedef RuleBook<PushDownAction,State>::Rule AutomatonRule;
	typedef RuleBook<RegularAction,KripkeState>::Rule SystemRule;
	const vector<AutomatonRule> &automaton_rules = automaton.GetRules().GetRules();
	const vector<SystemRule> &system_rules = _system.GetRules().GetRules();

	//temp
	const vector<unsigned int> &system_configurations = system_config_space.GetConfigurations();
	const vector<unsigned int> &automaton_configurations = automaton_config_space.GetConfigurations();
	vector<unsigned int>::const_iterator e1;
	vector<unsigned int>::const_iterator e2;
	cout << "1\tn1\t2\tn2\t3\tn3" << endl;
	cout << "----------------------------------------" << endl;
	for (e1 = system_configurations.begin(); e1 != system_configurations.end(); ++e1) {
		for (e2 = automaton_configurations.begin(); e2 != automaton_configurations.end(); ++e2) {
			cout << *e1 << "\t" << system_config_space.GetStateName(*e1) << "\t";
			cout << *e2 << "\t" << automaton_config_space.GetStateName(*e2) << "\t";
			unsigned int start_id;
			start_id = *e1 * automaton_configurations.size() + *e2;
			cout << start_id << "\t" << config_space->GetStateName(start_id) << endl; 
		}
	}



	vector<AutomatonRule>::const_iterator j1;
	vector<SystemRule>::const_iterator j2;
	for (j1 = automaton_rules.begin(); j1 != automaton_rules.end(); ++j1) {
		for (j2 = system_rules.begin(); j2 != system_rules.end(); ++j2) {
			if (j1->action->GetName() == j2->action->GetName()) {
				cout << "automaton rule: " << endl;
				cout << "config: " << j1->configuration << endl;
				cout << "AKA " << automaton_config_space.GetStateName(j1->configuration) << endl;
				cout << "(with symbol " << automaton_config_space.GetSymbolName(j1->configuration) << ")" << endl;
				cout << "to state: " << j1->action->GetDestStateName(automaton_config_space) << endl;
				cout << "action: " << j1->action->ToString() << endl;

				cout << "system rule: " << endl;
				cout << "config: " << j2->configuration << endl;
				cout << "AKA " << system_config_space.GetStateName(j2->configuration) << endl;
				cout << "to state: " << j2->action->GetDestStateName(system_config_space) << endl;
				cout << "action: " << j2->action->ToString() << endl;

				cout << endl;


				unsigned int start_id;
				start_id = j1->configuration + j2->configuration * automaton_configurations.size();
				cout << "product rule:" << endl;
				cout << "config: " << start_id << endl;
				cout << "AKA " << config_space->GetStateName(start_id) << endl;
				cout << "(with symbol " << config_space->GetSymbolName(start_id) << ")" << endl;

				unsigned int dest_id;
				dest_id = j1->action->GetDestStateID() + j2->action->GetDestStateID() * automaton_states.size();
				cout << "to state: " << config_space->GetStateNameByID(dest_id) << endl;

				cout << endl;

				/*

				 PushDownAction *action = j1->action->Clone();
					action->SetDestState(

					product_system->AddRule(start_id, action);
*/
			}
			else {
				cout << "names don't match" << endl;
			}
		}
	}



   cout << "TODO" << endl;
   return product_system;
}

void ModelChecker::Visit(const Formula::Until &until) {
   cout << "visiting UNTIL" << endl;
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
	const string &automaton_name = until.GetAutomaton();
	cout << "getting automaton: " << automaton_name << endl;
	cout << "looking for a PDA" << endl;
	const PDA &automaton = *_environment.GetPDA(automaton_name);

	cout << "found automaton" << endl;
	cout << automaton.ToString() << endl;


   const PushDownSystem *pds = ConstructProductSystem(automaton, before, after); 
	cout << "PRODUCT SYSTEM:" << endl;
	cout << pds->ToString() << endl;

	/*
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
   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      Result *res = new Result(id, state.Evaluate(pvar.GetVarName()));
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, pvar, results);
}
void ModelChecker::Visit(const Formula::False &formula_false) {
   cout << "visiting FALSE" << endl;
   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      Result *res = new Result(id, false);
      results->AddResult(res);
   }
   _environment.SetCheckResults(&_system, formula_false, results);
}
void ModelChecker::Visit(const Formula::True &formula_true) {
   cout << "visiting TRUE" << endl;

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      Result *res = new Result(id, true);
      results->AddResult(res);
   }
   _environment.SetCheckResults(&_system, formula_true, results);
}
void ModelChecker::Visit(const Formula::Conjunction &conjunction) {
   cout << "visiting CONJUNCTION" << endl;

   const CheckResults *left_results  = Check(conjunction.GetLeft());
   const CheckResults *right_results = Check(conjunction.GetRight());

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const Result &result_left  = left_results->GetResult(id);
      const Result &result_right = right_results->GetResult(id);
      Result *res = new Result(id, result_left.GetEvaluation() && result_right.GetEvaluation());
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, conjunction, results);
}
void ModelChecker::Visit(const Formula::Negation &negation) {
   cout << "visiting NEGATION" << endl;
   const CheckResults *sub_results = Check(negation.GetSubFormula());

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const Result &sub_result_for_configuration = sub_results->GetResult(id);
      Result *res = new Result(id, !sub_result_for_configuration.GetEvaluation());
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, negation, results);
}

const CheckResults *ModelChecker::Check( Formula::Formula::const_reference formula ) {
   const ResultsTable &results_table = _environment.GetCheckResults(&_system);
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   formula.Accept(*this);
   cout << results_table.ToString() << endl;
   return results_table.GetEntry( formula );
}







