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

const ResultsTable &Environment::GetCheckResults(const Automaton *transition_system)  {
   cout << "Getting check results for a system" << endl;
   map<unsigned int, ResultsTable *>::const_iterator iter;
   iter = _computed_results.find( transition_system->GetID() );
   ResultsTable *table = NULL;
   if (iter == _computed_results.end()) {
      cout << "creating new results table" << endl;
      table = new ResultsTable();
      _computed_results.insert(make_pair(transition_system->GetID(), table));
   }
   else {
      table = iter->second;
   }
   return *table;
   //return *(_computed_results.find( transition_system->GetID() )->second);
}

void Environment::SetCheckResults(const Automaton *transition_system, Formula::Formula::const_reference formula, CheckResults *results) {
   cout << "Setting check results for a system" << endl;
   map<unsigned int, ResultsTable *>::iterator iter;
   iter = _computed_results.find( transition_system->GetID() );
   ResultsTable *table = NULL;
   if (iter == _computed_results.end()) {
      cout << "creating new results table" << endl;
      table = new ResultsTable();
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
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
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
