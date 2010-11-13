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
#include "LTS.h"

Environment::Environment() { }

ResultsTable &Environment::GetCheckResults(const TransitionSystem &transition_system) const {
   // TODO
   return *new ResultsTable();
}

Formula::Formula::const_reference Environment::GetFormula(const string &identifier) const {
   map<string, const Formula::Formula*>::const_iterator iter(_formulas.find(identifier));
   if (iter == _formulas.end()) {
      throw "TODO: no such formula";
   }
   return *(iter->second);
};

Automaton::const_reference Environment::GetAutomata(const string &identifier) const {
   map<string, const Automaton*>::const_iterator iter(_automata.find(identifier));
   if (iter == _automata.end()) {
      throw "TODO: no such automaton";
   }
   return *(iter->second);
};

TransitionSystem::const_reference Environment::GetSystem(const string &identifier) const {
   map<string, const TransitionSystem*>::const_iterator iter(_systems.find(identifier));
   if (iter == _systems.end()) {
      throw "TODO: no such transition system";
   }
   return *(iter->second);
};

void Environment::SetFormula( const string &identifier, Formula::Formula::const_reference formula ) {
   bool result = _formulas.insert(make_pair(identifier, &formula)).second;
   if (!result) {
      throw "TODO: formula already exists";
   }
}

void Environment::SetAutomata( const string &identifier, Automaton::const_reference automaton ) {
   bool result = _automata.insert(make_pair(identifier, &automaton)).second;
   if (!result) {
      throw "TODO: automaton already exists";
   }
}

void Environment::SetSystem( const string &identifier, TransitionSystem::const_reference transition_system) {
   bool result = _systems.insert(make_pair(identifier, &transition_system)).second;
   if (!result) {
      throw "TODO: system already exists";
   }
}

string Environment::ToString() const {
   return "TODO";
}
