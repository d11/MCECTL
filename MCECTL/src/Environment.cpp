/*
 * =====================================================================================
 *
 *       Filename:  Environment.cpp
 *    Description:  Definitions for the Environment class
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <map>

#include "formula/Formula.h"
#include "Environment.h"
#include "ModelChecker.h"
#include "Automata.h"
#include "TransitionSystem.h"
#include "exception/NonExistentFormula.h"
#include "exception/AlreadyExistsException.h"

Environment::Environment() { }

const ResultsTable &Environment::GetCheckResults(const KripkeStructure *transition_system)  {
   cout << "Getting check results for a system" << endl;
   map<unsigned int, ResultsTable *>::const_iterator iter;
   iter = _computed_results.find( transition_system->GetID() );
   ResultsTable *table = NULL;
   if (iter == _computed_results.end()) {
      cout << "creating new results table" << endl;
      table = new ResultsTable(*this, *transition_system);
      _computed_results.insert(make_pair(transition_system->GetID(), table));
   }
   else {
      table = iter->second;
   }
   return *table;
}

void Environment::SetCheckResults(const KripkeStructure *transition_system, Formula::Formula::const_reference formula, CheckResults *results) {
   cout << "Setting check results for a system" << endl;
   map<unsigned int, ResultsTable *>::iterator iter;
   iter = _computed_results.find( transition_system->GetID() );
   ResultsTable *table = NULL;
   if (iter == _computed_results.end()) {
      cout << "creating new results table" << endl;
      table = new ResultsTable(*this, *transition_system);
      _computed_results.insert(make_pair(transition_system->GetID(), table));
   }
   else {
      table = iter->second;
   }
   table->SetEntry( formula, results );
}

Formula::Formula::const_reference Environment::GetFormula(const string &identifier) const {
   map<string, const Formula::Formula*>::const_iterator iter(_formulas.find(identifier));
   if (iter == _formulas.end()) {
      throw NonExistentFormulaException(identifier);
   }
   return *(iter->second);
};

Formula::Formula::const_reference Environment::GetFormulaByID(unsigned int formula_id) const {
   map<unsigned int, const Formula::Formula*>::const_iterator iter(_formulas_by_id.find(formula_id));
   if (iter == _formulas_by_id.end()) {
      throw NonExistentFormulaException("By ID"); // TODO
   }
   return *(iter->second);
};

/*
const Automaton *Environment::GetAutomaton(const string &identifier) const {
   map<string, const Automaton*>::const_iterator iter(_automata.find(identifier));
   if (iter == _automata.end()) {
      throw NonExistentAutomatonException(identifier);
   }
   return iter->second;
};

const Automaton *Environment::GetSystem(const string &identifier) const {
   map<string, const Automaton*>::const_iterator iter(_systems.find(identifier));
   if (iter == _systems.end()) {
      throw NonExistentTransitionSystemException(identifier);
   }
   return iter->second;
};*/

void Environment::SetFormula( const string &identifier, Formula::Formula::const_reference formula ) {
   bool result = _formulas.insert(make_pair(identifier, &formula)).second;
   if (!result) { throw AlreadyExistsException(identifier); }
	formula.Accept(*this);
}

void Environment::SetFormulaByID( Formula::Formula::const_reference formula ) {
   bool result = _formulas_by_id.insert(make_pair(formula.GetID(), &formula)).second;
   if (!result) {
		cout << "Warning: formula with id " << formula.GetID() << " already exists!" << endl; 
	}
}

void Environment::Visit(const Formula::False       &formula_false) {
	SetFormulaByID(formula_false);
}
void Environment::Visit(const Formula::True        &formula_true) {
	SetFormulaByID(formula_true);
}

void Environment::Visit(const Formula::PVar        &release) {
	SetFormulaByID(release);
}

void Environment::Visit(const Formula::Negation    &negation) {
	SetFormulaByID(negation);
	negation.GetSubFormula().Accept(*this);
}

void Environment::Visit(const Formula::Conjunction &conjunction) {
	SetFormulaByID(conjunction);
	conjunction.GetLeft().Accept(*this);
	conjunction.GetRight().Accept(*this);
}

void Environment::Visit(const Formula::Until       &until) {
	SetFormulaByID(until);
	until.GetBefore().Accept(*this);
	until.GetAfter().Accept(*this);
}

void Environment::Visit(const Formula::Release     &release) {
	SetFormulaByID(release);
	release.GetBefore().Accept(*this);
	release.GetAfter().Accept(*this);
}



/*
void Environment::SetAutomaton( const string &identifier, const Automaton *automaton ) {
   bool result = _automata.insert(make_pair(identifier, automaton)).second;
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
}

void Environment::SetSystem( const string &identifier, const Automaton *transition_system) {
   bool result = _systems.insert(make_pair(identifier, transition_system)).second;
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
}*/

string Environment::ToString() const {
   stringstream s;
   s << "AUTOMATA:" << endl;
   map<string, const DFA*>::const_iterator iter;
   for( iter = _dfas.begin(); iter != _dfas.end(); ++iter ) {
      s << "[" << iter->first << "]" << endl;
   }
   // TODO
   s << "FORMULAS:" << endl;
   map<string, const Formula::Formula*>::const_iterator formula_iter;
   for( formula_iter = _formulas.begin(); formula_iter != _formulas.end(); ++formula_iter ) {
      s << "[" << formula_iter->first << "]" << endl;
   }
   // TODO ?

   return s.str();
}

Environment::~Environment() {

   map<string, const DFA*>::const_iterator iter;
   for( iter = _dfas.begin(); iter != _dfas.end(); ++iter ) {
      delete iter->second;
   }
   // TODO
   map<string, const Formula::Formula*>::const_iterator formula_iter;
   for( formula_iter = _formulas.begin(); formula_iter != _formulas.end(); ++formula_iter ) {
      delete formula_iter->second;
   }
   // TODO ?
}
