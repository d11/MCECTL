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

template <class A>
class PredecessorConfigurations {
private:
public:
   bool Contains(const KripkeState *state_1, const KripkeState *state_2, const A *action) const;
};


// Will contain example/counterexample if available
class Result {
public:
   Result(const KripkeState &, bool);
};

class CheckResults : public Showable {
public:
   CheckResults();
   void AddResult( const Result &result );
   string ToString() const;
};

class ResultsTable {
private:
public:
   ResultsTable();
   bool HasEntry( Formula::Formula::const_reference formula ) const;
	CheckResults GetEntry( Formula::Formula::const_reference formula ) const;
	void SetEntry( const CheckResults &formula );
};

class Environment;

class ModelChecker : public Formula::Visitor {
private:
   Environment &_environment;
   const KripkeStructure &_system;

public:
   ModelChecker(
      Environment &environment,
      const KripkeStructure &transition_system
   );

   // combine _system and automata
   boost::shared_ptr<PushDownSystem> ConstructProductSystem(
      const PDA &automaton,
      Formula::Formula::const_reference x,
      Formula::Formula::const_reference y
   );

   void Visit(const Formula::False       &formula_false);
   void Visit(const Formula::True        &formula_true);
   void Visit(const Formula::PVar        &release);
   void Visit(const Formula::Negation    &negation);
   void Visit(const Formula::Conjunction &conjunction);
   void Visit(const Formula::Until       &until);
   void Visit(const Formula::Release     &release);

   CheckResults Check( Formula::Formula::const_reference formula );
};

#endif
