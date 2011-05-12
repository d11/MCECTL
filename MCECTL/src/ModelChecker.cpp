/*
 * =====================================================================================
 *
 *       Filename:  ModelChecker.cpp
 *    Description:  Extended CTL model-checking algorithm
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

#include "TransitionSystem.h"
#include "ModelChecker.h"
#include "Util.h"

extern "C" {
#include "wpds.h"
} // End 'extern C'

#include <cstdio>
// taken from libwpds example
void print_automaton (wFA *fa, const string &s)
{
	wTrans *t;
	cout << "Automaton " << s << ":" << endl;
	for (t = fa->transitions; t; t = t->next) {
      cout << "  "    << wIdentString(t->from)
           << " -- "  << wIdentString(t->name)
           << " --> " << wIdentString(t->to)
           << " (" << (int)t->label << ")"
           << endl;
   }
   cout << endl;
}
void print_rule (wRule *r)
{
	printf("rule: <%s,%s> -> <%s",wIdentString(r->from_state),
		wIdentString(r->from_stack),wIdentString(r->to_state));
	if (r->to_stack1) printf(",%s",wIdentString(r->to_stack1));
	if (r->to_stack2) printf(" %s",wIdentString(r->to_stack2));
	printf(">");
}
void print_trace (wFA *fa, wPath *p)
{
	wIdent *id;
	wPath *pnext;
	wConfig *conf;

	do {
		conf = wPathConfig(fa,p);
		printf("[%d] <%s,%s",(int)p->value,wIdentString(conf->state),
				wIdentString(conf->stack[0]));
		id = conf->stack;
		while (*++id) printf(" %s",wIdentString(*id));
		printf(">\t\t");
		wConfigDelete(conf);

		wPathTraceStep(fa,p);
		if (p->transitions->rule) print_rule(p->transitions->rule);
		wPathRef(fa,pnext = p->transitions->target);
		wPathDeref(fa,p);
		p = pnext;

		printf("\n");
	} while (p);

	printf("\n");
}
void print_pds (wPDS *pds, const string &s) {
   wRule *r = pds->rules;
   cout << s;
   while (r) {
      cout << endl;
      print_rule(r);
      r = r->next;
   }
   cout << endl;
}


#include <string.h>

using namespace std;

// RESULT

Result::Result(unsigned int config_id, const string &config_name, bool result) : _config_id(config_id), _config_name(config_name), _evaluation(result) {
   // TODO
}

string Result::ToString() const {
   stringstream s;
   s << _config_name << ": " << (_evaluation ? "T" : "F");
   return s.str();
}
// CHECK RESULTS

CheckResults::CheckResults() { }

void CheckResults::AddResult(Result *result) {
   _result_map[result->GetID()] = result;
}

string CheckResults::ToString() const {
   stringstream s;
   s << "Results: {" << endl;
   map<unsigned int, Result*>::const_iterator iter;
   for (iter = _result_map.begin(); iter != _result_map.end(); ++iter) {
      //s << "  State " << (*iter).first << " -> [" << (*iter).second->ToString() <<"]" << endl;
      s << "   " << (*iter).second->ToString() << endl;
   }
   s << "}" << endl;
   return s.str();
}

const Result &CheckResults::GetResult(unsigned int id) const {
   map<unsigned int, Result *>::const_iterator iter = _result_map.find(id);
   if (iter == _result_map.end()) {
      throw runtime_error("Error retrieving result - bad ID");
   }
   return *(iter->second);
}

const map<unsigned int, Result *> &CheckResults::GetResults() const {
   return _result_map;
}

// PREDECESSOR CONFIGURATIONS

template <class A>
bool PredecessorConfigurations<A>::Contains( const KripkeState *state_1, const KripkeState *state_2, const A *action) const {
   // TODO
   return false;
}

// RESULTS TABLE

ResultsTable::ResultsTable(const Environment &env, const KripkeStructure &lts) : _environment(env), _system(lts) { 
   cout << "Creating new results table" << endl;
}

bool ResultsTable::HasEntry( Formula::Formula::const_reference formula ) const {
   map<unsigned int, CheckResults *>::const_iterator iter;
   iter = _entries.find(formula.GetID());
   return iter != _entries.end();
}

const CheckResults *ResultsTable::GetEntry( Formula::Formula::const_reference formula ) const {
   return _entries.find(formula.GetID())->second;
}

void ResultsTable::SetEntry(Formula::Formula::const_reference formula, CheckResults *check_results) {
   cout << "Setting results table entry" << endl;
   _entries.insert(make_pair(formula.GetID(), check_results));
}


class JoinStateResults {
private:
   const KripkeStructure &_system;
public:
   JoinStateResults(const KripkeStructure &lts) : _system(lts) { }

   string operator()( const string &a, const pair<unsigned int,Result*> &b ) {
      stringstream bs;
      //unsigned int config_id = b.first;
      bs << "   " << b.second->ToString() << "," << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

class JoinCheckResults {
private:
   const Environment &_environment;
   const KripkeStructure &_system;
public:

   JoinCheckResults(const Environment &env, const KripkeStructure &lts) : _environment(env), _system(lts) { }

   string operator()( const string &a, const pair<unsigned int,CheckResults*> &b ) {
      const map<unsigned int, Result *> &result_map = b.second->GetResults();
      stringstream bs;
      unsigned int formula_id = b.first;
      bs << "Results for " << _environment.GetFormulaByID(formula_id).ToString() << ": {" << endl
         << accumulate(result_map.begin(),
                    result_map.end(),
                    string(""),
                    JoinStateResults(_system))
         << "}" << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

string ResultsTable::ToString() const {
   stringstream s;
   s << "==================" << endl;
   s << "FULL RESULTS TABLE" << endl;
   s << accumulate(_entries.begin(), _entries.end(), string(""), JoinCheckResults(_environment, _system));
   s << "==================" << endl;
   return s.str();
}

// MODEL CHECKER

ModelChecker::ModelChecker(
   Environment &environment,
   const KripkeStructure &transition_system
) : _environment(environment), _system(transition_system) { }

// combine _system and automata
ProductSystem *ModelChecker::ConstructProductSystem(
   const PDA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   cout << "Constructing product system" << endl;
   vector<ProductState<State,KripkeState>*> product_states;
   Valuation v;
   ProductState<State,KripkeState> *initial_state = NULL;

	vector<string> product_state_names;
	
	vector<KripkeState> system_states(_system.GetStates());
	vector<State> automaton_states(automaton.GetStates());
	vector<KripkeState>::const_iterator i1;
	vector<State>::const_iterator i2;
	for (i1 = system_states.begin(); i1 != system_states.end(); ++i1) {
		for (i2 = automaton_states.begin(); i2 != automaton_states.end(); ++i2) {
			//stringstream new_state_name;
			//new_state_name << "(" << i1->GetName() << "," << i2->GetName() << ")";
			ProductState<State, KripkeState> *new_state;
         new_state = new ProductState<State,KripkeState>(*i2, *i1);
			product_states.push_back(new_state);
			product_state_names.push_back(new_state->GetName());
			//product_state_names.push_back(new_state_name.str());
			if (i1->GetName() == _system.GetInitialState().GetName()
				&& i2->GetName() == automaton.GetInitialState().GetName()) {
				initial_state = new_state;
			}
		}
	}

	if (initial_state == NULL) {
		throw runtime_error("Failed to find initial state..!");
	}

	const ConfigurationSpace &automaton_config_space(automaton.GetConfigurationSpace());
	vector<string> stack_alphabet = automaton_config_space.GetStackAlphabet();
	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);

	ProductSystem *product_system = new ProductSystem(product_states, initial_state, config_space);

	cout << product_system->ToString() << endl;

	const ConfigurationSpace &system_config_space(_system.GetConfigurationSpace());
	typedef RuleBook<PushDownAction,State>::Rule AutomatonRule;
	typedef RuleBook<RegularAction,KripkeState>::Rule SystemRule;
	const vector<AutomatonRule> &automaton_rules = automaton.GetRules().GetRules();
	const vector<SystemRule> &system_rules = _system.GetRules().GetRules();

	//temp
	const vector<unsigned int> &system_configurations = system_config_space.GetConfigurations();
	const vector<unsigned int> &automaton_configurations = automaton_config_space.GetConfigurations();
	vector<unsigned int>::const_iterator e1;
	vector<unsigned int>::const_iterator e2;
	cout << "1\tn1\t2\tn2\t3\tn3" << endl;
	cout << "----------------------------------------" << endl;
	for (e1 = system_configurations.begin(); e1 != system_configurations.end(); ++e1) {
		for (e2 = automaton_configurations.begin(); e2 != automaton_configurations.end(); ++e2) {
			cout << *e1 << "\t" << system_config_space.GetStateName(*e1) << "\t";
			cout << *e2 << "\t" << automaton_config_space.GetStateName(*e2) << "\t";
			unsigned int start_id;
			start_id = *e1 * automaton_configurations.size() + *e2;
			cout << start_id << "\t" << config_space->GetStateName(start_id) << endl; 
		}
	}

	const CheckResults *x_results  = Check(x);

	vector<AutomatonRule>::const_iterator j1;
	vector<SystemRule>::const_iterator j2;
	for (j1 = automaton_rules.begin(); j1 != automaton_rules.end(); ++j1) {
		for (j2 = system_rules.begin(); j2 != system_rules.end(); ++j2) {
			if (j1->action->GetName() == j2->action->GetName()) {
				cout << "automaton rule: " << endl;
				cout << "config: " << j1->configuration << endl;
				cout << "AKA " << automaton_config_space.GetStateName(j1->configuration) << endl;
				cout << "(with symbol " << automaton_config_space.GetSymbolName(j1->configuration) << ")" << endl;
				cout << "to state: " << j1->action->GetDestStateName(automaton_config_space) << endl;
				cout << "action: " << j1->action->ToString() << endl;

				cout << "system rule: " << endl;
				cout << "config: " << j2->configuration << endl;
				cout << "AKA " << system_config_space.GetStateName(j2->configuration) << endl;
				cout << "to state: " << j2->action->GetDestStateName(system_config_space) << endl;
				cout << "action: " << j2->action->ToString() << endl;

				cout << endl;

				// Check system state satisfies x
				const Result &res = x_results->GetResult(j2->configuration);
				if (!res.GetEvaluation()) {
					cout << "x not in l(s)" << endl;
					continue;
				}

				unsigned int start_id;
				start_id = j1->configuration + j2->configuration * automaton_configurations.size();
				cout << "product rule:" << endl;
				cout << "config: " << start_id << endl;
				cout << "AKA " << config_space->GetStateName(start_id) << endl;
				cout << "(with symbol " << config_space->GetSymbolName(start_id) << ")" << endl;

				unsigned int dest_id;
				dest_id = j1->action->GetDestStateID() + j2->action->GetDestStateID() * automaton_states.size();
				cout << "to state: " << config_space->GetStateNameByID(dest_id) << endl;

				cout << endl;

				PushDownAction *action = j1->action->Clone();
				action->SetDestStateID(dest_id);
				product_system->AddRule(start_id, action);
			}
			else {
				cout << "names don't match" << endl;
			}
		}
	}

   return product_system;
}

// Used by libwpds semiring - TODO ??
void *nullfn (void *a, void *b) { return NULL; }
int   eqfn (void *a, void *b)   { return a==b;    }
void *null ()			{ return NULL; }

void ModelChecker::Visit(const Formula::Until &until) {
   cout << "visiting UNTIL" << endl;
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
	const string &automaton_name = until.GetAutomaton();
	cout << "getting automaton: " << automaton_name << endl;
	cout << "looking for a PDA" << endl;
	const PDA &automaton = *_environment.GetPDA(automaton_name);

	cout << "found automaton" << endl;
	cout << automaton.ToString() << endl;


   const ProductSystem *product_system = ConstructProductSystem(automaton, before, after); 
	cout << "PRODUCT SYSTEM:" << endl;
	cout << product_system->ToString() << endl;

	const CheckResults *y_results  = Check(after);

   wPDS *pds;
   wFA *fa;
   wFA *fa_pre;
   // TODO - figure out what this is about
   wSemiring semiring = { &nullfn, &nullfn, NULL, &null, &null, NULL, &eqfn, NULL, NULL };
   wSemiring *p_semiring = &semiring;

   // initialise libwpds
   wInit();

   // create idents for states
   map<string,wIdent> state_idents;
   vector<string>::const_iterator f;
   const vector<string> &state_names = product_system->GetConfigurationSpace().GetStates();
   for (f = state_names.begin(); f != state_names.end(); ++f) {
      string f_mod = "s" + *f + "__";
      char *temp = strdup(f_mod.c_str());
      state_idents[*f] = wIdentCreate(temp);
      free(temp);
      cout << "state: " << f_mod << " ident: " << state_idents[f_mod] << endl;
   }

   // create idents for stack symbols
   map<string,wIdent> stack_idents;
   vector<string>::const_iterator g;
   //stack_idents["_"] = 0; // TODO
   /*char *octothorpe = strdup("#");
   stack_idents["_"] = wIdentCreate(octothorpe); // TODO
   free(octothorpe);*/
   const vector<string> &stack_alphabet = product_system->GetConfigurationSpace().GetStackAlphabet();
   for (g = stack_alphabet.begin(); g != stack_alphabet.end(); ++g) {
      char *temp = strdup(g->c_str());
      if (stack_idents.find(*g) == stack_idents.end()) {
         stack_idents[*g] = wIdentCreate(temp);
      }
      free(temp);
      cout << "symbol: " << *g << " ident: " << stack_idents[*g] << endl;
   }

   

   // Convert PDS to a libwpds one
   pds = wPDSCreate(p_semiring);
   //wPDSInsert(...) for each rule
	typedef RuleBook<PushDownAction,ProductState<State,KripkeState> >::Rule ProductRule;
   const vector<ProductRule> &product_rules = product_system->GetRules().GetRules();
   vector<ProductRule>::const_iterator rule_iter;
   for (rule_iter = product_rules.begin(); rule_iter != product_rules.end(); ++rule_iter) {
      wIdent from_state_ident, from_stack_ident, to_state_ident, to_stack1_ident, to_stack2_ident;
      Configuration from_configuration = rule_iter->configuration;
      string from_state_name = product_system->GetConfigurationSpace().GetStateName(from_configuration);
      from_state_ident = state_idents[from_state_name];
      string from_stack_symbol = product_system->GetConfigurationSpace().GetSymbolName(from_configuration);
      from_stack_ident = stack_idents[from_stack_symbol];
      const PushDownAction *action = rule_iter->action;
      string to_state_name = product_system->GetConfigurationSpace().GetStateNameByID(action->GetDestStateID());
      to_state_ident = state_idents[to_state_name];
      string to_symbol_name1("_");
      string to_symbol_name2(from_stack_symbol);
      action->ApplyToStackTop(to_symbol_name1, to_symbol_name2);
      if (to_symbol_name1 == "_") {
         to_stack1_ident = 0;
      } else {
         to_stack1_ident = stack_idents[to_symbol_name1];
      }
      if (to_symbol_name2 == "_") {
         to_stack2_ident = 0;
      } else {
         to_stack2_ident = stack_idents[to_symbol_name2];
      }
      cout << "Inserting PDS rule:" << endl;
      cout << "from_state_ident: " << from_state_ident << endl;
      cout << "from_stack_ident: " << from_stack_ident << endl;
      cout << "to_state_ident: " << to_state_ident << endl;
      cout << "to_stack1_ident: " << to_stack1_ident << endl;
      cout << "to_stack2_ident: " << to_stack2_ident << endl;
      cout << endl;
      
      // TODO extract 'to' state/stack symbols - use polymorphism 
      // [ this is done ?? ]
      wPDSInsert(pds, from_state_ident, from_stack_ident, to_state_ident, to_stack1_ident, to_stack2_ident, NULL);
   }

   // Construct a libwpds 'P-automaton' representing the set of configurations
   // see http://www.fmi.uni-stuttgart.de/szs/publications/info/schwoosn.EHRS00b.shtml
	// find the accepting & after-satisfying configurations
   fa = wFACreate(p_semiring);
	const vector<Configuration> &product_configurations = product_system->GetConfigurations();
   Configuration system_configuration = 0;
   Configuration automaton_configuration = 0;
   cout << "accepting & satisfying product states:" << endl;
	vector<Configuration>::const_iterator iter;
	for (iter = product_configurations.begin(); iter != product_configurations.end(); ++iter) {
      const ProductState<State,KripkeState> &state = product_system->GetState(*iter);
      bool accepting = state.GetFirst().GetAccepting();
      const Result &result_after = y_results->GetResult(system_configuration);
      bool satisfying = result_after.GetEvaluation();

      if (accepting && satisfying) {
         cout << *iter << " " << state.ToString() << endl;
         wIdent state_ident = state_idents[state.GetName().c_str()];
         vector<string>::const_iterator gamma;
         for (gamma = stack_alphabet.begin(); gamma != stack_alphabet.end(); ++gamma) {
            // add self-loop via gamma
            cout << "Inserting automaton transition" << endl;
            cout << "from_state_ident: " << state_ident << endl;
            cout << "stack_ident: " << stack_idents[*gamma] << " [" << *gamma << "]" << wIdentString(stack_idents[*gamma]) << endl;
            cout << "to_state_ident: " << state_ident << endl;
            cout << endl;
            wFAInsert(fa, state_ident, stack_idents[*gamma], state_ident, NULL, NULL);
         }
      }

      automaton_configuration++;
      if (automaton_configuration >= automaton.GetConfigurations().size()) {
         automaton_configuration = 0;
         system_configuration++;
      }
	}


   print_pds(pds,"PDS:");
   print_automaton(fa, "before pre*");
   fa_pre = wPrestar(pds, fa, TRACE_COPY);
   print_automaton(fa_pre, "after pre*");

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator state_iter;
   for (state_iter = ids.begin(); state_iter != ids.end(); ++state_iter) {

      bool found = false;
      wTrans *t;

      const KripkeState &system_state(_system.GetState(*state_iter));
      ProductState<State,KripkeState> pre_state(automaton.GetInitialState(), system_state);
      string pre_state_name = pre_state.GetName();

      cout << "Seeking configuration <" << pre_state_name << ",*>" << endl;

      for (t = fa_pre->transitions; t && !found; t = t->next) {
         string state_name(wIdentString(t->from));
         cout << "  ...found <" << state_name << ","  << wIdentString(t->name) << "> ";
         // print trace
         wConfig *c = wConfigCreate(t->from, t->name, 0); // TODO state
         wPath *p = wPathFind(fa_pre, c);
         //wPathTraceAll(fa_pre,p);
         if (p->transitions->target) {
            cout << " [found paths] ";
            //asm("int $0x3\n");
            print_trace(fa_pre, p->transitions->target);
            //print_trace(fa_pre, p);
         }
         else {
            cout << "[no paths] ";
         }
         if (t->from == state_idents[pre_state_name] && t->name == stack_idents["_"]) {
            Result *result = new Result(*state_iter, pre_state.GetSecond().GetName(), true);
            results->AddResult( result );
            cout << " {TRUE}" << endl;
            found = true;

//            print_trace(fa_pre, p->transitions->target);

         }
         else {
            cout << endl;
         }
         wConfigDelete(c);
         wPathDeref(fa_pre,p);
      }
      if (!found) {
         cout << " {FALSE}" << endl;
         Result *result = new Result(*state_iter, pre_state.GetSecond().GetName(), false);
         results->AddResult( result );
      }
   }

   // Clean up libwpds data
   wFADelete(fa);
   wFADelete(fa_pre);
   wPDSDelete(pds);
   wFinish();

   _environment.SetCheckResults(&_system, until, results);

	/*
      PredecessorConfigurations<NondeterministicPushDownAction> pc; // Compute predecessor configurations
      if ( pc.Contains(_system.GetInitialState(), *iter, new PushDownEpsilonAction()) ) {
   */
}

void ModelChecker::Visit(const Formula::Release &release) {
   cout << "visiting RELEASE" << endl;
   // TODO
   cout << "TODO" << endl;
}
void ModelChecker::Visit(const Formula::PVar &pvar) {
   cout << "visiting PVAR" << endl;
   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      Result *res = new Result(id, state.GetName(), state.Evaluate(pvar.GetVarName()));
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, pvar, results);
}
void ModelChecker::Visit(const Formula::False &formula_false) {
   cout << "visiting FALSE" << endl;
   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      Result *res = new Result(id, state.GetName(), false);
      results->AddResult(res);
   }
   _environment.SetCheckResults(&_system, formula_false, results);
}
void ModelChecker::Visit(const Formula::True &formula_true) {
   cout << "visiting TRUE" << endl;

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      Result *res = new Result(id, state.GetName(),true);
      results->AddResult(res);
   }
   _environment.SetCheckResults(&_system, formula_true, results);
}
void ModelChecker::Visit(const Formula::Conjunction &conjunction) {
   cout << "visiting CONJUNCTION" << endl;

   const CheckResults *left_results  = Check(conjunction.GetLeft());
   const CheckResults *right_results = Check(conjunction.GetRight());

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      const Result &result_left  = left_results->GetResult(id);
      const Result &result_right = right_results->GetResult(id);
      Result *res = new Result(id, state.GetName(), result_left.GetEvaluation() && result_right.GetEvaluation());
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, conjunction, results);
}
void ModelChecker::Visit(const Formula::Negation &negation) {
   cout << "visiting NEGATION" << endl;
   const CheckResults *sub_results = Check(negation.GetSubFormula());

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(_system.GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = _system.GetState(id);
      const Result &sub_result_for_configuration = sub_results->GetResult(id);
      Result *res = new Result(id, state.GetName(), !sub_result_for_configuration.GetEvaluation());
      results->AddResult(res);
   }

   _environment.SetCheckResults(&_system, negation, results);
}

const CheckResults *ModelChecker::Check( Formula::Formula::const_reference formula ) {
   const ResultsTable &results_table = _environment.GetCheckResults(&_system);
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   formula.Accept(*this);
   cout << results_table.ToString() << endl;
   return results_table.GetEntry( formula );
}







