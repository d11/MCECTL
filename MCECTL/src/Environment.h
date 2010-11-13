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
#include "LTS.h"
#include "Automata.h"

class ResultsTable;

#include "Showable.h"

class Environment : public Showable {
private:
   map<string, const Formula::Formula*>  _formulas; 
   map<string, const Automaton*>         _automata; 
   map<string, const TransitionSystem*>  _systems;  

public:
   Environment();
   ResultsTable &GetCheckResults(const TransitionSystem &transition_system)  const;
   Formula::Formula::const_reference GetFormula(const string &identifier)    const;
   Automaton::const_reference GetAutomaton(const string &identifier)          const;
   TransitionSystem::const_reference GetSystem(const string &identifier)     const;

   void SetFormula( const string &identifier, Formula::Formula::const_reference formula );
   void SetAutomata( const string &identifier, Automaton::const_reference automaton );
   void SetSystem( const string &identifier, TransitionSystem::const_reference transition_system);

   virtual string ToString() const;
};

#endif
