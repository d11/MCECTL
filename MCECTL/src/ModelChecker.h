/*
 * =====================================================================================
 *
 *       Filename:  ModelChecker.h
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef MODELCHECKER_H
#define MODELCHECKER_H

#include <boost/shared_ptr.hpp>
#include "Environment.h"
#include "formula/Formula.h"
#include "Automata.h"

using namespace Formula;

class TraceStep {
private:
   string _state;
   vector<string> _stack;
   ProductRule _rule;
   bool _has_rule;
public:
   TraceStep(const string &state_name, const vector<string> &stack)
      : _state(state_name), _stack(stack), _has_rule(false) { }
   void AddRule(const ProductRule &rule) {
      _rule = rule;
      _has_rule = true;
   }
   string GetStateName() const {
      return _state;
   }
   string GetAction() const {
      if (!_has_rule) 
         return "";
      return _rule.action->GetName();
   }
   bool HasRule() const {
      return _has_rule;
   }
   const ProductRule &GetRule() const {
      return _rule;
   }
   const vector<string> &GetStack() const {
      return _stack;
   }
   
};

// Will contain example/counterexample if available
class Result : public Showable {
private:
   Configuration _config_id;
   string _config_name;
   bool _evaluation;
   vector<TraceStep> _trace_steps;
public:
   Result(unsigned int config_id, const string &config_name);
   Result(unsigned int config_id, const string &config_name, bool evaluation);
   Configuration GetID() const { return _config_id; }
   bool GetEvaluation() const { return _evaluation; }
   void SetEvaluation(bool evaluation);
   string ToString() const;
   void AddTraceStep(const TraceStep &trace_step) {
      _trace_steps.push_back(trace_step);
   }
};

class CheckResults : public Showable {
private:
   // Map from configuration to corresponding results
   map<Configuration, Result*> _result_map;
public:
   CheckResults();
   void AddResult(Result *result );
   string ToString() const;
   const Result &GetResult(Configuration id) const;
   const map<Configuration, Result *> &GetResults() const;
};

class ResultsTable : public Showable {
private:
   map<unsigned int, CheckResults *> _entries;
   const Environment &_environment;
public:
   ResultsTable(const Environment &env);
   bool HasEntry( Formula::Formula::const_reference formula ) const;
	const CheckResults *GetEntry( Formula::Formula::const_reference formula ) const;
	void SetEntry( Formula::Formula::const_reference formula, CheckResults *check_results );
   string ToString() const;
};

class Environment;
class WPDSProduct;
class ReleaseSystem;

class ModelChecker : public Formula::Visitor {
private:
   Environment &_environment;
   union {
      const KripkeStructure *lts;
      const PushDownSystem  *pds;
   } _system;
   bool _is_pds;

public:
   ModelChecker(
      Environment &environment,
      const KripkeStructure &transition_system
   );
   ModelChecker(
      Environment &environment,
      const PushDownSystem &transition_system
   );

   vector<Configuration> GetConfigurations();
   const KripkeState &GetSystemState(Configuration configuration);

	void SetCheckResults(Formula::Formula::const_reference formula, CheckResults *results);
	const ResultsTable &GetCheckResults();

   void ConstructConfigurationSetForLTS(
      WPDSProduct &wpds,
      const ProductSystem *product_system, 
      const CheckResults *y_results,
      unsigned int automaton_configuration_count
   );

   void ConstructConfigurationSetForPDS(
      WPDSProduct &wpds,
      const ProductSystem *product_system, 
      const CheckResults *y_results,
      unsigned int system_configuration_count
   );
   // combine _system and automata
   ProductSystem *ConstructProductSystemFromLTS(
      const KripkeStructure &lts,
      const PDA &automaton,
      Formula::Formula::const_reference x,
      Formula::Formula::const_reference y
   );
   ProductSystem *ConstructProductSystemFromPDS(
      const PushDownSystem &pds,
      const DFA &automaton,
      Formula::Formula::const_reference x,
      Formula::Formula::const_reference y
   );

   ReleaseSystem *ConstructReleaseSystemFromPDS(
      const PushDownSystem &pds,
      const DFA &automaton,
      Formula::Formula::const_reference x,
      Formula::Formula::const_reference y
   );
   ReleaseSystem *ConstructReleaseSystemFromLTS(
      const KripkeStructure &lts,
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

   const CheckResults *Check( Formula::Formula::const_reference formula );
};

#endif
