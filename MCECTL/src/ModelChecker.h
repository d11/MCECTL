/*
 * =====================================================================================
 *
 *       Filename:  ModelChecker.h
 *
 *    Description:  
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef MODELCHECKER_H
#define MODELCHECKER_H

#include <boost/shared_ptr.hpp>

#include "Environment.h"

#include "formula/Formula.h"
using namespace Formula;

#include "Automata.h"
#include "LTS.h"

class PredecessorConfigurations {
private:
public:
   bool Contains(State state_1, State state_2, Action action) const;
};


// Will contain example/counterexample if available
class Result {
public:
   Result(const State &, bool);
};

class CheckResults {
   public:
      CheckResults();
      void AddResult( const Result &result );
};

class ResultsTable {
private:
public:
   ResultsTable();
   bool HasEntry( Formula::Formula::const_reference formula );
	CheckResults GetEntry( Formula::Formula::const_reference formula );
	void SetEntry( const CheckResults &formula );
};

class Environment;

class ModelChecker : public Formula::Visitor {
private:
   const Environment &_environment;
   const TransitionSystem &_system;

public:
   ModelChecker(
      const Environment &environment,
      const TransitionSystem &transition_system
   );

   // combine _system and automata
   boost::shared_ptr<PushDownSystem> ConstructProductSystem(
      const Automaton &automaton,
      Formula::Formula::const_reference x,
      Formula::Formula::const_reference y
   );

   void Visit(const Formula::Until &until);
   void Visit(const Formula::Release &release);
   void Visit(const Formula::PVar &release);

   CheckResults Check( Formula::Formula::const_reference formula );
};

#endif
