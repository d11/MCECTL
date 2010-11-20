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
#include "exception/NonExistentAutomaton.h"
#include "exception/NonExistentTransitionSystem.h"
#include "exception/AlreadyExistsException.h"

Environment::Environment() { }

const ResultsTable &Environment::GetCheckResults(const KripkeStructure &transition_system) const {
   // TODO
   return *new ResultsTable();
}

void Environment::SetCheckResults(const KripkeStructure &transition_system, const CheckResults &results) {
 //  _computed_results.find( system )(SetEntry( results ));
 // TODO
}

Formula::Formula::const_reference Environment::GetFormula(const string &identifier) const {
   map<string, const Formula::Formula*>::const_iterator iter(_formulas.find(identifier));
   if (iter == _formulas.end()) {
      throw NonExistentFormulaException(identifier);
   }
   return *(iter->second);
};

const Automaton *Environment::GetAutomaton(const string &identifier) const {
   map<string, const Automaton*>::const_iterator iter(_automata.find(identifier));
   if (iter == _automata.end()) {
      throw NonExistentAutomatonException(identifier);
   }
   return iter->second;
};

KripkeStructure::const_reference Environment::GetSystem(const string &identifier) const {
   map<string, const KripkeStructure*>::const_iterator iter(_systems.find(identifier));
   if (iter == _systems.end()) {
      throw NonExistentTransitionSystemException(identifier);
   }
   return *(iter->second);
};

void Environment::SetFormula( const string &identifier, Formula::Formula::const_reference formula ) {
   bool result = _formulas.insert(make_pair(identifier, &formula)).second;
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
}

void Environment::SetAutomaton( const string &identifier, const Automaton *automaton ) {
   bool result = _automata.insert(make_pair(identifier, automaton)).second;
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
}

void Environment::SetSystem( const string &identifier, KripkeStructure::const_reference transition_system) {
   bool result = _systems.insert(make_pair(identifier, &transition_system)).second;
   if (!result) {
      throw AlreadyExistsException(identifier);
   }
}

string Environment::ToString() const {
   stringstream s;
   s << "AUTOMATA:" << endl;
   map<string, const Automaton*>::const_iterator iter;
   for( iter = _automata.begin(); iter != _automata.end(); ++iter ) {
      s << "[" << iter->first << "]" << endl;
   }
   s << "FORMULAS:" << endl;
   map<string, const Formula::Formula*>::const_iterator formula_iter;
   for( formula_iter = _formulas.begin(); formula_iter != _formulas.end(); ++formula_iter ) {
      s << "[" << formula_iter->first << "]" << endl;
   }
   // TODO ?

   return s.str();
}
