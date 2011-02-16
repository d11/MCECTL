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
class Result : public Showable {
private:
   Configuration _config_id;
   bool _evaluation;
public:
   Configuration GetID() const { return _config_id; }
   bool GetEvaluation() const { return _evaluation; }
   Result(unsigned int, bool);
   string ToString() const;
};

class CheckResults : public Showable {
private:
   map<unsigned int, Result*> _result_map; // from configuration to value
public:
   CheckResults();
   void AddResult(Result *result );
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
