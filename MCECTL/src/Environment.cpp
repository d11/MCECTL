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

PDA::const_reference Environment::GetAutomaton(const string &identifier) const {
   map<string, const PDA*>::const_iterator iter(_automata.find(identifier));
   if (iter == _automata.end()) {
      throw NonExistentAutomatonException(identifier);
   }
   return *(iter->second);
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
      throw "TODO: formula already exists";
   }
}

void Environment::SetAutomata( const string &identifier, PDA::const_reference automaton ) {
   bool result = _automata.insert(make_pair(identifier, &automaton)).second;
   if (!result) {
      throw "TODO: automaton already exists";
   }
}

void Environment::SetSystem( const string &identifier, KripkeStructure::const_reference transition_system) {
   bool result = _systems.insert(make_pair(identifier, &transition_system)).second;
   if (!result) {
      throw "TODO: system already exists";
   }
}

string Environment::ToString() const {
   return "TODO";
}
