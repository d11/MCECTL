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
#include "exception/NonExistentAutomaton.h"
#include "exception/NonExistentTransitionSystem.h"
#include "exception/AlreadyExistsException.h"
#include "Showable.h"

class ResultsTable;
class CheckResults;

class Environment : public Showable {
private:
   map<string, const Formula::Formula*>  _formulas; 
   map<string, const DFA*>         _dfas; 
   map<string, const PDA*>         _pdas; 
   map<string, const KripkeStructure*>         _ltss; 
   map<string, const PushDownSystem*>         _pdss; 

   map<unsigned int, ResultsTable *> _computed_results;
public:
   Environment();
   ~Environment();

   const ResultsTable &GetCheckResults(const Automaton *transition_system)     ;
	void  SetCheckResults(const Automaton *transition_system, Formula::Formula::const_reference formula, CheckResults *results);

   Formula::Formula::const_reference GetFormula(const string &identifier)   const;

   /*
   void GetAutomaton(const Automaton *automaton, const string &identifier) const {
   }*/

   const DFA *GetDFA(const string &identifier) const {
      map<string, const DFA*>::const_iterator iter(_dfas.find(identifier));
      if (iter == _dfas.end()) { throw NonExistentAutomatonException(identifier); }
      return iter->second;
   }
   const PDA *GetPDA(const string &identifier) const {
      map<string, const PDA*>::const_iterator iter(_pdas.find(identifier));
      if (iter == _pdas.end()) { throw NonExistentAutomatonException(identifier); }
      return iter->second;
   }
   const KripkeStructure *GetLTS(const string &identifier) const {
      map<string, const KripkeStructure*>::const_iterator iter(_ltss.find(identifier));
      if (iter == _ltss.end()) { throw NonExistentAutomatonException(identifier); }
      return iter->second;
   }
   const PushDownSystem *GetPDS(const string &identifier) const {
      map<string, const PushDownSystem*>::const_iterator iter(_pdss.find(identifier));
      if (iter == _pdss.end()) { throw NonExistentAutomatonException(identifier); }
      return iter->second;
   }

   //const Automaton *GetAutomaton(const string &identifier) const;
   //const Automaton *GetSystem(const string &identifier)    const;

   void SetFormula(  const string &identifier, Formula::Formula::const_reference formula );

   void SetAutomaton( const string &identifier, const FiniteAutomaton<RegularAction,State> *automaton ) {
      SetDFA(identifier, automaton);
   }
   void SetAutomaton( const string &identifier, const FiniteAutomaton<RegularAction,KripkeState> *automaton ) {
      SetLTS(identifier, automaton);
   }
   void SetAutomaton( const string &identifier, const FiniteAutomaton<PushDownAction,State> *automaton ) {
      SetPDA(identifier, automaton);
   }
   void SetAutomaton( const string &identifier, const FiniteAutomaton<PushDownAction,KripkeState> *automaton ) {
      SetPDS(identifier, automaton);
   }


   
   void SetDFA( const string &identifier, const DFA *automaton ) {
      bool result = _dfas.insert(make_pair(identifier, automaton)).second;
      if (!result) { throw AlreadyExistsException(identifier); }
   }
   void SetPDA( const string &identifier, const PDA *automaton ) {
      bool result = _pdas.insert(make_pair(identifier, automaton)).second;
      if (!result) { throw AlreadyExistsException(identifier); }
   }
   void SetLTS( const string &identifier, const KripkeStructure *automaton ) {
      bool result = _ltss.insert(make_pair(identifier, automaton)).second;
      if (!result) { throw AlreadyExistsException(identifier); }
   }
   void SetPDS( const string &identifier, const PushDownSystem *automaton ) {
      bool result = _pdss.insert(make_pair(identifier, automaton)).second;
      if (!result) { throw AlreadyExistsException(identifier); }
   }

  // void SetSystem(   const string &identifier, const Automaton *transition_system);

   virtual string ToString() const;
};

#endif
