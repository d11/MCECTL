/*
 * =====================================================================================
 *
 *       Filename:  Environment.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <string>

using namespace std;

#include "formula/Formula.h" // TODO could use foredec
#include "TransitionSystem.h"
#include "Automata.h"
#include "Showable.h"

class ResultsTable;
class CheckResults;

class Environment : public Showable {
private:
   map<string, const Formula::Formula*>  _formulas; 
   map<string, const PDA*>         _automata; 
   map<string, const KripkeStructure*>  _systems;  

public:
   Environment();

   const ResultsTable &GetCheckResults(const KripkeStructure &transition_system)     const;
	void  SetCheckResults(const KripkeStructure &transition_system, const CheckResults &results);

   Formula::Formula::const_reference GetFormula(const string &identifier)   const;
   PDA::const_reference			 GetAutomaton(const string &identifier) const;
   KripkeStructure::const_reference GetSystem(const string &identifier)    const;

   void SetFormula(  const string &identifier, Formula::Formula::const_reference formula );
   void SetAutomata( const string &identifier, PDA::const_reference automaton );
   void SetSystem(   const string &identifier, KripkeStructure::const_reference transition_system);

   virtual string ToString() const;
};

#endif
