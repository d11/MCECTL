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
#include <climits>
#include <cstdio>
#include <cstring>

#include <utility> // std::pair
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>

// temp
#include <boost/graph/graphviz.hpp>

#include "TransitionSystem.h"
#include "ModelChecker.h"
#include "Util.h"

extern "C" {
#include "wpds.h"
} // End 'extern C'


using namespace std;


// RESULT

Result::Result(unsigned int config_id, const string &config_name) : _config_id(config_id), _config_name(config_name) {
}
Result::Result(unsigned int config_id, const string &config_name, bool evaluation) : _config_id(config_id), _config_name(config_name), _evaluation(evaluation) {

}

void Result::SetEvaluation(bool evaluation) {
   _evaluation = evaluation;
}

string Result::ToString() const {
   stringstream s;
   s << _config_name << ": " << (_evaluation ? "T" : "F");
   // temp
   if (!_trace_steps.empty()) {
      s << "     [ ";
      vector<TraceStep>::const_iterator iter;
      for ( iter = _trace_steps.begin(); iter != _trace_steps.end(); ++iter ) {
         s << "<";
         const vector<string> &stack = iter->GetStack();
         copy(stack.begin(), stack.end(), ostream_iterator<string>(s,""));
         s << "> ";

         s << iter->GetStateName();

         if (iter->HasRule()) {
            const ProductRule &rule = iter->GetRule();
            s << " --" << rule.action->GetName() << "--> ";
         }

      }
      s << " ]";
   }
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

// RESULTS TABLE

ResultsTable::ResultsTable(const Environment &env) : _environment(env)
{ }

bool ResultsTable::HasEntry( Formula::Formula::const_reference formula ) const {
   map<unsigned int, CheckResults *>::const_iterator iter;
   iter = _entries.find(formula.GetID());
   return iter != _entries.end();
}

const CheckResults *ResultsTable::GetEntry( Formula::Formula::const_reference formula ) const {
   return _entries.find(formula.GetID())->second;
}

void ResultsTable::SetEntry(Formula::Formula::const_reference formula, CheckResults *check_results) {
   _entries.insert(make_pair(formula.GetID(), check_results));
}


class JoinStateResults {
private:
public:
   JoinStateResults(){ }

   string operator()( const string &a, const pair<unsigned int,Result*> &b ) {
      stringstream bs;
      bs << "   " << b.second->ToString() << "," << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

class JoinCheckResults {
private:
   const Environment &_environment;
public:

   JoinCheckResults(const Environment &env) : _environment(env){ }

   string operator()( const string &a, const pair<unsigned int,CheckResults*> &b ) {
      const map<unsigned int, Result *> &result_map = b.second->GetResults();
      stringstream bs;
      unsigned int formula_id = b.first;
      bs << "Results for " << _environment.GetFormulaByID(formula_id).ToString() << ": {" << endl
         << accumulate(result_map.begin(),
                    result_map.end(),
                    string(""),
                    JoinStateResults())
         << "}" << endl;
      if (a.empty()) return bs.str();
      return a + bs.str();
   }
};

string ResultsTable::ToString() const {
   stringstream s;
   s << "==================" << endl;
   s << "FULL RESULTS TABLE" << endl;
   s << accumulate(_entries.begin(), _entries.end(), string(""), JoinCheckResults(_environment));
   s << "==================" << endl;
   return s.str();
}

// Used by libwpds semiring
void *nullfn (void *a, void *b) { return NULL; }
int   eqfn (void *a, void *b)   { return a==b;    }
void* plusfn (void *a, void *b) { return (void*)((int)a+(int)b); }
void* minfn  (void *a, void *b) { return (a<b)? a : b; }
void* onefn ()		 	{ return NULL; }
void* null ()		 	{ return NULL; }
void* zerofn ()		 	{ return (void*)INT_MAX; }

/* Wrap wPDS functionality in an object */
class WPDSWrapper {
private:
   // Keeps track of how many objects are using libwpds, so that it can be
   // initialised/terminated appropriately
   static int _wpds_refcount;
protected:
   wPDS *_pds;
   wFA *_fa;
   wFA *_fa_pre;
   wSemiring _semiring;
   wSemiring *_p_semiring;
   map<string,wIdent> _state_idents;
   map<string,wIdent> _stack_idents;
   map<const wRule*,ProductRule> _rule_map;
public:
   WPDSWrapper()  {

      // Resource Allocation Is Initialisation
      if (_wpds_refcount == 0) {
         // Initialise libwpds
         wInit();
      }
      _wpds_refcount++;

      // Setup null semiring for wpds automata
      _semiring.extend   = &nullfn;
      _semiring.combine  = &nullfn;
      _semiring.diff     = NULL;
      _semiring.one      = &null;
      _semiring.zero     = &null;
      _semiring.quasione = NULL;
      _semiring.eq       = &eqfn;
      _semiring.ref      = NULL;
      _semiring.deref    = NULL;
//      _semiring.extend   = &plusfn;
//      _semiring.combine  = &minfn;
//      _semiring.diff     = NULL;
//      _semiring.one      = &onefn;
//      _semiring.zero     = &zerofn;
//      _semiring.quasione = NULL;
//      _semiring.eq       = &eqfn;
//      _semiring.ref      = NULL;
//      _semiring.deref    = NULL;
      _p_semiring = &_semiring;
      _fa = wFACreate(_p_semiring);
   }

   // from libwpds example TODO rewrite
   // For debugging
   void PrintAutomaton (wFA *fa, const string &s) const
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
   // For debugging
   void PrintRule (wRule *r) const
   {
      ProductRule rule = LookupRule(r);

      cout << "Rule: " << rule.action->GetName() << ": ";
      cout << "<" << wIdentString(r->from_state)<< ","<< wIdentString(r->from_stack)<< "> -> <" << wIdentString(r->to_state);
      if (r->to_stack1) cout << "," << wIdentString(r->to_stack1);
      if (r->to_stack2) cout << " " << wIdentString(r->to_stack2);
      cout << ">";
   }
   // For debugging
   void PrintTrace (wFA *fa, wPath *p) const
   {
      wIdent *id; wPath *pnext; wConfig *conf;

      wPathTraceAll(fa,p);
      do {
         conf = wPathConfig(fa,p);
         printf("[%d] <%s,%s",(int)p->value,wIdentString(conf->state),
               wIdentString(conf->stack[0]));
         id = conf->stack;
         while (*++id) printf(" %s",wIdentString(*id));
         printf(">\t\t");
         wConfigDelete(conf);

         wPathTraceStep(fa,p);
         if (p->transitions->rule) PrintRule(p->transitions->rule);
         wPathRef(fa,pnext = p->transitions->target);
         wPathDeref(fa,p);
         p = pnext;

         printf("\n");
      } while (p);

      printf("\n");
   }
   // For debugging
   void PrintPDS() const {
      wRule *r = _pds->rules;
      cout << "PDS:";
      while (r) {
         cout << endl << "  ";
         PrintRule(r);
         r = r->next;
      }
      cout << endl;
   }
   // For debugging
   void PrintConfigurationAutomaton() const {
      PrintAutomaton(_fa, "before pre*");
   }
   // For debugging
   void PrintPredecessorConfigurations() { 
      PrintAutomaton(_fa_pre, "after pre*");
   }

   // Store the found witnessing path in the given result object
   void StoreTrace(Result &result, wFA *fa, wPath *path) const {
      wIdent *id; wPath *pnext; wConfig *conf;
      wPathTraceAll(fa, path);

      // Walk along the path
      do {
         conf = wPathConfig(fa, path);
         // Retrieve the name of the state at this point in the run
         string state_name(wIdentString(conf->state));
         
         // Construct the stack at the current point in this run
         vector<string> stack;
         id = conf->stack;
         while (*++id) {
            stack.push_back( wIdentString(*id) );
         }
         wConfigDelete(conf);

         // Find the rule used at this point
         wPathTraceStep(fa, path); 
         TraceStep trace_step(state_name, stack);
         wRule *rule = path->transitions->rule;
         if (rule) {
            trace_step.AddRule(LookupRule(rule));
         }

         // Add the information about this step to the result object
         result.AddTraceStep(trace_step);

         // Advance to the next point on the path
         pnext = path->transitions->target;
         wPathRef(fa, pnext);
         wPathDeref(fa, path);
         path = pnext;
      } while(path);
   }

   // Map between the rule used by libwpds and our own objects
   ProductRule LookupRule(const wRule *wpds_rule) const {
      map<const wRule*,ProductRule>::const_iterator iter;
      iter = _rule_map.find(wpds_rule);
      if (iter == _rule_map.end()) {
         throw runtime_error("Tried to look up rule which is not in the map");
      }
      return iter->second;
   }

   // Use libwpds to find the predecessors, assuming the initial
   // multi-automaton has been constructed
   void ComputePredecessorConfigurations() {
      _fa_pre = wPrestar(_pds, _fa, TRACE_COPY);
   }

   virtual ~WPDSWrapper() {
      // Clean up libwpds data
      wFADelete(_fa);
      wFADelete(_fa_pre);
      wPDSDelete(_pds);

      // Dereference and possible terminate the usage of libwpds (RAII)
      _wpds_refcount--;
      if (_wpds_refcount == 0) {
         wFinish();
      }
   }

   void CreateAllStateIdents(const ConfigurationSpace &config_space) {
      // Create and store wpds idents for states
      vector<string>::const_iterator f;
      const vector<string> &state_names = config_space.GetStates();
      for (f = state_names.begin(); f != state_names.end(); ++f) {
         string f_mod = "s" + *f + "__";
         char *temp = strdup(f_mod.c_str());
         _state_idents[*f] = wIdentCreate(temp);
         free(temp);
         cout << "State: " << f_mod << " Ident: " << _state_idents[f_mod] << endl;
      }

   }

   void CreateAllStackIdents(const ConfigurationSpace &config_space) {
      // Create and store wpds idents for stack symbols
      vector<string>::const_iterator iter;
      const vector<string> &stack_alphabet = config_space.GetStackAlphabet();
      for (iter = stack_alphabet.begin(); iter != stack_alphabet.end(); ++iter) {
         char *temp = strdup(iter->c_str());
         if (_stack_idents.find(*iter) == _stack_idents.end()) {
            _stack_idents[*iter] = wIdentCreate(temp);
         }
         free(temp);
         cout << "Symbol: " << *iter << " ident: " << _stack_idents[*iter] << endl;
      }
   }

   // Add a state to the internal weighted pushdown system.
   void AddPDSState(const string &state_name, bool goal_state) {

      // The same state should not be added more than once, except in the case
      // where one instance is final and the other isn't
      if (!goal_state && _state_idents.find(state_name) != _state_idents.end()) {
         return;
      }

      // Construct an internal name for the 
      // libwpds requires that initial states begin with an alphanumeric
      // character. We use 's' as a standard.
      string f_mod = "s" + state_name;
      if (goal_state) { 
         // Similarly, final (accepting) states end with a double-underscore
         f_mod += "__";
      }

      // Obtain a wpds ident for this name, and store it in our own map
      char *temp = strdup(f_mod.c_str());
      _state_idents[state_name] = wIdentCreate(temp);
      free(temp);

      // debug
      cout << "State: " << f_mod << " Ident: " << _state_idents[state_name] << endl;
   }


   // Add a self-loop for the state, via the given stack symbol
   void AddConfiguration(const string &state_name, const string &stack_symbol) {
      wIdent state_ident = _state_idents[state_name.c_str()];
      wFAInsert(_fa, state_ident, _stack_idents[stack_symbol], state_ident, NULL, NULL);
   }

   // Convert a PDS to a libwpds one
   void CreatePDS( const vector<ProductRule> &product_rules, const ConfigurationSpace &config_space ) {

      _pds = wPDSCreate(_p_semiring);

      // Call wPDSInsert for each rule
      vector<ProductRule>::const_iterator rule_iter;
      for (rule_iter = product_rules.begin(); rule_iter != product_rules.end(); ++rule_iter) {

         // Obtain the names of the states and symbols for the rule
         Configuration from_configuration = rule_iter->configuration;
         string from_state_name   = config_space.GetStateName(from_configuration);
         string from_stack_symbol = config_space.GetSymbolName(from_configuration);
         const PushDownAction *action = rule_iter->action;
         string to_state_name   = config_space.GetStateNameByID(action->GetDestStateID());
         string to_symbol_name1 = "";
         string to_symbol_name2 = from_stack_symbol;
         action->ApplyToStackTop(to_symbol_name1, to_symbol_name2);

         // Look up corresponding wpds idents
         wIdent from_state_ident, from_stack_ident, to_state_ident,
                to_stack1_ident, to_stack2_ident;
         from_state_ident = _state_idents[from_state_name];
         from_stack_ident = _stack_idents[from_stack_symbol];
         to_state_ident   = _state_idents[to_state_name];
         to_stack1_ident  = (to_symbol_name1 == "") ? 0 : _stack_idents[to_symbol_name1];
         to_stack2_ident  = (to_symbol_name2 == "") ? 0 : _stack_idents[to_symbol_name2];

         // Insert a corresponding rule into the wpds
         wRule *wpds_rule = wPDSInsert(
            _pds, from_state_ident,
            from_stack_ident, to_state_ident, to_stack1_ident,
            to_stack2_ident, (void*)1
         );
         // Record the rule to which it corresponds
         _rule_map[wpds_rule] = *rule_iter;
      }
   }

   // Find configurations in the predecessor multi-automaton to which there are
   // transitions from the given state that are labelled with the given stack
   // symbol.  
   //
   // Precondition: predecessor configurations have been computed
   vector<string> GetSuccessors(const ConfigurationSpace &config_space, const string &state_name, const string &stack_symbol) {
      vector<string> successor_names;

      wIdent source_ident = _state_idents[state_name];
      wIdent symbol_ident = _stack_idents[stack_symbol];
      const vector<string> &state_names = config_space.GetStates();
      vector<string>::const_iterator iter;
      for (iter = state_names.begin(); iter != state_names.end(); ++iter) {
         wIdent target_ident = _state_idents[*iter];
         if (NULL != wFAFind(_fa_pre, source_ident, symbol_ident, target_ident)) {
            cout << "Found successor " << *iter << endl;
            successor_names.push_back(*iter);
         }
      }

      return successor_names;
   }

   // Determine whether a given configuration is in the set of predecessor
   // configurations, by checking whether it is accepted by the _fa_pre
   // multi-automaton.
   //
   // Precondition: predecessor configurations have been computed
   // Postcondition: result contains appropriate output data
   void CheckPreStar(const ProductState<State,KripkeState> &state, const string &stack_symbol, Result &result) {
      bool found = false; wTrans *t;
      string pre_state_name = state.GetName();
      for (t = _fa_pre->transitions; t && !found; t = t->next) {
         cout << "  ...found <" << wIdentString(t->from) << ","  << wIdentString(t->name) << "> ";
         if (t->from == _state_idents[pre_state_name] && t->name == _stack_idents[stack_symbol]) {
            wConfig *config = wConfigCreate(t->from, t->name, 0); 
            wPath *path = wPathFind(_fa_pre, config);
            bool done = false;
            if (path->transitions && path->transitions->target) {
               PrintTrace(_fa_pre, path->transitions->target);
               // TODO
//               StoreTrace(result, _fa_pre, path->transitions->target);
               done = true;
               result.SetEvaluation(true);
            }
            wConfigDelete(config);
            if (done) {
               return;
            }
         }
         cout << endl;
      }

      result.SetEvaluation(false);
      return;
   }

};

// Static initialiser for the reference count
int WPDSWrapper::_wpds_refcount = 0;

// Subclass containing routines specific to checking Until formulas
class WPDSProduct : public WPDSWrapper {
private:
   const ProductSystem &_product_system;
public:
   WPDSProduct(const ProductSystem &product_system) : _product_system(product_system) {
      WPDSWrapper::CreateAllStackIdents(_product_system.GetConfigurationSpace());

   }

   void CreatePDS() {
      WPDSWrapper::CreatePDS(_product_system.GetRules().GetRules(), _product_system.GetConfigurationSpace());
   }
   void AddPDSState(const ProductState<State,KripkeState> &state, bool goal_state) {
      WPDSWrapper::AddPDSState(state.GetName(), goal_state);
   }

   // Add a self-loop for the state, via the given stack symbol
   void AddConfiguration(const ProductState<State,KripkeState> &state, const string &stack_symbol) {
      WPDSWrapper::AddConfiguration(state.GetName(), stack_symbol);
   }

   void AddConfigurationTop(const ProductState<State,KripkeState> &state, const string &stack_symbol, const vector<string> &stack_alphabet) {

      string f_mod = "s"+ state.GetName();
      char *temp = strdup(f_mod.c_str());
//      _state_idents[state.GetName()+"__"] = _state_idents[state.GetName()];
      _state_idents[state.GetName()] = wIdentCreate(temp);
      f_mod = "_"+ state.GetName() +"__";
      free(temp);
      temp = strdup(f_mod.c_str());
      _state_idents[state.GetName()+"__"] = wIdentCreate(temp);
      free(temp);


      wIdent state_ident1 = _state_idents[state.GetName()];
      wIdent state_ident2 = _state_idents[state.GetName()+"__"];
      wFAInsert(_fa, state_ident1, _stack_idents[stack_symbol], state_ident2, NULL, NULL);

      vector<string>::const_iterator gamma;
      for (gamma = stack_alphabet.begin(); gamma != stack_alphabet.end(); ++gamma) {
         wFAInsert(_fa, state_ident2, _stack_idents[*gamma], state_ident2, NULL, NULL);
      }

   }

};

// Configurations for the release pushdown system
// Normal product configurations plus those for two extra states (g and b)
/*
class RConfigurationSpace : public Showable {
private:
   ConfigurationSpace *_product_space;
   Configuration _goal_state;
   Configuration _fail_state;
public:
   RConfigurationSpace(ConfigurationSpace *product_space) : _product_space(product_space) {
      _goal_state = product_space->GetStateCount()-1;
      _fail_state = _goal_state + 1;
   }

   ~RConfigurationSpace() {
      delete _product_space;
   }

   string GoalState() const { 
      return _goal_state;
   }
   string FailStateName() const { 
      return _
   }

   string GetStateName(Configuration c) const {
      const vector<string> stack_alphabet = GetStackAlphabet();
      Configuration state_id = c / stack_alphabet.size();
      if (state_id == _goal_state) {
         return GoalStateName();
      }
      else if (state_id == _fail_state) {
         return FailStateName();
      }
      else {
         return _product_space->GetStateName(c);
      }
   }
   string GetSymbolName(Configuration c) const {
      const vector<string> stack_alphabet = GetStackAlphabet();
      return stack_alphabet.at(c % stack_alphabet.size());
   }
   const vector<string> GetStackAlphabet() const {
      return _product_space->GetStackAlphabet();
   }
   string ToString() const {
      return _product_space->ToString();
   }
};
*/

class ReleaseSystem : public Showable {
private:
   ConfigurationSpace *_config_space;
   vector<ProductState<State,KripkeState>*> _product_states;

   typedef RuleBook<PushDownAction, ProductState<State, KripkeState> >::Rule Rule;
//   struct Rule {
//      unsigned int configuration;
//      const PushDownAction *action;
//      Rule(unsigned int config, const PushDownAction *a) : configuration(config), action(a) {}
//      Rule() : configuration(0), action(NULL) {  }
//   };
   vector<Rule> _rule_list;
public:
   ReleaseSystem(const vector<ProductState<State,KripkeState>*> &produce_states, ConfigurationSpace *config_space) : _config_space(config_space) {
      
   }
   const vector<Rule> &GetRules() const {
      return _rule_list;
   }

   ~ReleaseSystem() {
      delete _config_space;
   }

   void AddRule(unsigned int start_id, PushDownAction *action) {
      _rule_list.push_back(Rule(start_id, action));
   }
   const ConfigurationSpace GetConfigurationSpace() const {
      return *_config_space;
   }
   string ToString() const {
      ostringstream s;
      s << "RELEASE SYSTEM" << endl;
      s << _config_space->ToString() << endl;
      s << "Rules:"  << endl;
      vector<Rule>::const_iterator iter;
      for( iter = _rule_list.begin(); iter != _rule_list.end(); ++iter) {
         s << iter->action->GetName() << ": ";
         s << "<" << _config_space->GetStateName(iter->configuration) << ", " << _config_space->GetSymbolName(iter->configuration) << "> -> ";
         s << iter->action->GetDestStateName(*_config_space) << "  ";
         s << "[ " << iter->action->ToString() << " ]" << endl;
      }

      return s.str();
   }
};

typedef struct {
   Configuration from_config;
   bool reachability;
   Configuration dest_id;
} ReachabilityTransition;

class WPDSRelease : public WPDSWrapper {
private:
   const ReleaseSystem &_release_system;
   set<Configuration> _repeating_heads;
public:
   WPDSRelease(const ReleaseSystem &release_system) : _release_system(release_system) {

      WPDSWrapper::CreateAllStackIdents(_release_system.GetConfigurationSpace());
   }

   void CreatePDS() {
      WPDSWrapper::CreatePDS(_release_system.GetRules(), _release_system.GetConfigurationSpace());
   }

   void ConstructBottomConfigurations() {
      const ConfigurationSpace &config_space      = _release_system.GetConfigurationSpace();
      const vector<Configuration> &configurations = config_space.GetConfigurations();

      vector<Configuration>::const_iterator iter;
      for (iter = configurations.begin(); iter != configurations.end(); ++iter) {
         const string &stack_symbol = config_space.GetSymbolName(*iter);
         const string &state_name = config_space.GetStateName(*iter);
         if (stack_symbol == "_") {
            AddPDSState(state_name, true);
            AddConfiguration(state_name, stack_symbol);
         }
         else {
            AddPDSState(state_name, false);
         }
      }
   }

   // Implementation of Algorithm 2 described in "Efficient Algorithms for
   // Model Checking Pushdown Systems" by Esparza, Hansel, Rossmanith and
   // Schwoon
   void ComputeRepeatingHeads() {

      // Compute pre* of { <p, _> : p in P }
      cout << "Getting pre* of bottom configurations" << endl;
      WPDSRelease reachability(_release_system);
      reachability.ConstructBottomConfigurations();
      reachability.CreatePDS();
      reachability.PrintPDS();
      reachability.PrintConfigurationAutomaton();
      reachability.ComputePredecessorConfigurations();
      reachability.PrintPredecessorConfigurations();
      cout << endl;

      const ConfigurationSpace &config_space = _release_system.GetConfigurationSpace();

      // Create reachability graph
      typedef std::pair<Configuration, Configuration> Edge;
      // temp
      vector<Edge> edges;


      // for rules in _release_system
      const vector<ProductRule> &rules = _release_system.GetRules();
      vector<ProductRule>::const_iterator iter;
      for (iter = rules.begin(); iter != rules.end(); ++iter) {
         const ProductRule &rule = *iter;
         Configuration configuration = rule.configuration; // p
         const PushDownAction &action = *rule.action;

         Configuration dest_id = action.GetDestStateID(); // p'

         string top_symbol = "";
         string bottom_symbol = config_space.GetSymbolName(configuration);
         action.ApplyToStackTop(top_symbol, bottom_symbol);

         // A rewrite rule
         if (top_symbol != "") {
            if (bottom_symbol == "") {
               cout << "rewrite rule" ;
               Configuration dest_config = config_space.MakeConfiguration(dest_id, top_symbol);
               edges.push_back(Edge(configuration, dest_config));
               cout << "." << endl;
            } else {
               // A push rule
               cout << "push rule ";

//               /* TODO
               string dest_state = config_space.GetStateNameByID(dest_id);
               cout << "getting successors for " << dest_state << ", " << top_symbol << endl;
               vector<string> successors = reachability.GetSuccessors(config_space, dest_state, top_symbol);
               vector<string>::const_iterator iter;
               for (iter = successors.begin(); iter != successors.end(); ++iter) {
                  edges.push_back(Edge(configuration, config_space.MakeConfiguration(*iter, bottom_symbol)));
                  cout << ".";
               }

               //2
               edges.push_back(Edge(configuration, config_space.MakeConfiguration(dest_id, top_symbol)));
               cout << ".";
               cout << endl;
            }

         } else {
            cout << "pop rule" << endl;
            // pop
         }
      }

      // TODO
      using namespace boost;
      typedef adjacency_list<vecS, vecS, bidirectionalS, 
                 property<vertex_color_t, default_color_type>
              > ReachabilityGraph;

      ReachabilityGraph graph(edges.begin(), edges.end(), config_space.GetConfigurationCount());
      typedef graph_traits<ReachabilityGraph>::vertex_descriptor Vertex;

      /*
      class label_writer {
         public:
            label_writer(Name _name) : name(_name) {}
            template <class VertexOrEdge>
               void operator()(std::ostream& out, const VertexOrEdge& v) const {
                  out << "[label=\"" << name[v] << "\"]";
               }
         private:
            Name name;
         };
      */
      vector<string> config_names = config_space.GetConfigurationNames();
      /*
      ostringstream dotout;
      write_graphviz(dotout, graph, make_label_writer(config_names.begin()));
      FILE *dot = popen("dot -Tx11", "w");
      string out = dotout.str();
      fputs(out.c_str(), dot);
      pclose(dot);*/



      pair<graph_traits<ReachabilityGraph>::edge_iterator, graph_traits<ReachabilityGraph>::edge_iterator> edge_range = boost::edges(graph);
      graph_traits<ReachabilityGraph>::edge_iterator i_edge;
      for (i_edge = edge_range.first; i_edge != edge_range.second; ++i_edge) {
         ReachabilityGraph::edge_descriptor edge = *i_edge;
         Configuration start_config = source(edge, graph);
         Configuration dest_config = target(edge, graph);
         cout << "Edge " << config_names[start_config] << " -> " << config_names[dest_config] << endl;
      }

      
      // Find strongly connected components of the reachability graph
      //
      // Time O(|V|+|E|)
      vector<int> component(num_vertices(graph));
      unsigned int num = strong_components(graph, &component[0]);
      vector< vector<Configuration> > buckets(num);

      cout << "Total number of components: " << num << endl;
      vector<int>::size_type i;
      for (i = 0; i != component.size(); ++i) {
         cout << "Configuration " << config_names[i]
            <<" is in component " << component[i] << endl;
         buckets[component[i]].push_back(i);
      }

      // loop through components
      for ( i = 0; i != num; ++i ) {
         cout << "Component " << i << ": ";
         vector<int>::size_type component_size = buckets[i].size();

         // We need to check that the component actually contains an edge -
         // boost::graph only gives us the vertices.
         // If there is more than one vertex, by connectedness this is a given.
         if (component_size > 1) {
            vector<int>::size_type j;
            for (j = 0; j != component_size; ++j) {
               _repeating_heads.insert(buckets[i][j]);
               cout << buckets[i][j] << " ";
            }
         } 
         else {
            Configuration vertex = buckets[i][0];
            cout << vertex << " ";

            // TODO comment
            if (config_space.GetStateName(vertex) != "_fail_state_") { // TODO
               // If there are no successors, it counts as repeating for us
               typedef ReachabilityGraph::out_edge_iterator out_edge_iterator;
               pair<out_edge_iterator, out_edge_iterator> out_list = out_edges(vertex, graph);
               if (out_list.second == out_list.first) {
                  _repeating_heads.insert(vertex);
               }
               // If it has a self-loop, it's repeating
               typedef ReachabilityGraph::edge_descriptor edge_descriptor;
               pair<edge_descriptor, bool> has_loop = edge(vertex, vertex, graph);
               if (has_loop.second) {
                  _repeating_heads.insert(vertex);
               }
               
            }

         }
         cout << endl;
      }

      /* hopefully not needed
      set<ReachabilityTransition> rel;
      vector<ReachabilityTransition> trans;
      set<ReachabilityRule> delta_prime;

      typedef ProductRule PushDownRule;
      const vector<PushDownRule> &delta = _pds.GetRules();

// todo transform rules ?

      vector<PushDownRule>::const_iterator rule_iter;
      for (rule_iter = delta.begin(); rule_iter != delta.end(); rule_iter++) {
         ReachabilityTranstition temp;
         temp.from_config = rule_iter->configuration;
//         temp.reachability = rule_iter-> // TODO
         temp.dest_id = rule_iter->action->GetDestStateID();
         trans.push(temp)
      }

      while ( !trans.empty() ) {
         ReachabilityRule t = trans.pop();
         set<ReachabilityRule>::const_iterator iter;
         iter = rel.find(t);
         if ( iter != rel.end() ) {
            continue;
         }

         rel.insert(t);

      }
      
      */
   }

   void PrintRepeatingHeads() const {
      cout << "Repeating heads:" << endl;
      const ConfigurationSpace& config_space = _release_system.GetConfigurationSpace();
      set<Configuration>::const_iterator iter;
      for ( iter = _repeating_heads.begin(); iter != _repeating_heads.end(); ++iter ) {
         string state_name = config_space.GetStateName(*iter);
         string stack_symbol = config_space.GetSymbolName(*iter);
         cout << "<" << state_name << ", " << stack_symbol << ">" << endl;
      }
   }

   // Construct automaton for the initial configuration set. The automaton will
   // accept R . G* where R is the set of repeating heads and G is the stack
   // alphabet
   void ConstructFA() {

//      WPDSWrapper::CreateAllStateIdents(_release_system.GetConfigurationSpace());
      // TODO ?
      const ConfigurationSpace& config_space = _release_system.GetConfigurationSpace();

      // Create and store wpds idents for states
      vector<string>::const_iterator f;
      const vector<string> &state_names = config_space.GetStates();
      for (f = state_names.begin(); f != state_names.end(); ++f) {
         string f_mod = "s" + *f;
         char *temp = strdup(f_mod.c_str());
         _state_idents[*f] = wIdentCreate(temp);
         free(temp);
         cout << "State: " << f_mod << " Ident: " << _state_idents[f_mod] << endl;
      }

      // Add final state
      string final_state_name = "__final__";
      char *temp = strdup(final_state_name.c_str());
      wIdent final_state_ident = wIdentCreate(temp);
      free(temp);
      // TODO
      set<Configuration>::const_iterator iter;
      for ( iter = _repeating_heads.begin(); iter != _repeating_heads.end(); ++iter ) {

         string state_name = config_space.GetStateName(*iter);
         wIdent state_ident = _state_idents[state_name];
         string stack_symbol = config_space.GetSymbolName(*iter);
         wFAInsert(_fa, state_ident, _stack_idents[stack_symbol], final_state_ident, NULL, NULL);
      }

      const vector<string> &stack_symbols = config_space.GetStackAlphabet();
      vector<string>::const_iterator symbol_iter;
      for (symbol_iter = stack_symbols.begin(); symbol_iter != stack_symbols.end(); ++symbol_iter) {
         wFAInsert(_fa, final_state_ident, _stack_idents[*symbol_iter], final_state_ident, NULL, NULL);
      }

   }

};

// MODEL CHECKER

ModelChecker::ModelChecker(
   Environment &environment,
   const KripkeStructure &transition_system
) : _environment(environment), _is_pds(false) {
   _system.lts = &transition_system;
}
ModelChecker::ModelChecker(
   Environment &environment,
   const PushDownSystem &transition_system
) : _environment(environment), _is_pds(true) {
   _system.pds = &transition_system;
}
vector<Configuration> ModelChecker::GetConfigurations() {
   if (_is_pds) {
      return _system.pds->GetConfigurations();
   } 
   else {
      return _system.lts->GetConfigurations();
   }
}

const KripkeState &ModelChecker::GetSystemState(Configuration configuration) {
   if (_is_pds) {
      return _system.pds->GetState(configuration);
   } 
   else {
      return _system.lts->GetState(configuration);
   }
}

void ModelChecker::SetCheckResults(Formula::Formula::const_reference formula, CheckResults *results) {
   if (_is_pds) {
      _environment.SetCheckResults(_system.pds, formula, results);
   } 
   else {
      _environment.SetCheckResults(_system.lts, formula, results);
   }
}

const ResultsTable &ModelChecker::GetCheckResults() {
   if (_is_pds) {
      return _environment.GetCheckResults(_system.pds);
   } 
   else {
      return _environment.GetCheckResults(_system.lts);
   }
}

ReleaseSystem *ModelChecker::ConstructReleaseSystemFromLTS(
   const KripkeStructure &lts,
   const PDA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   cout << "Constructing release system from lts" << endl;

   vector<ProductState<State,KripkeState>*> product_states;
   ProductState<State,KripkeState> *initial_state = NULL;
	vector<string> product_state_names;
	vector<KripkeState> system_states(lts.GetStates());
	vector<State> automaton_states(automaton.GetStates());
	vector<KripkeState>::const_iterator i1;
	vector<State>::const_iterator i2;
	for (i1 = system_states.begin(); i1 != system_states.end(); ++i1) {
		for (i2 = automaton_states.begin(); i2 != automaton_states.end(); ++i2) {
			ProductState<State, KripkeState> *new_state;
         new_state = new ProductState<State,KripkeState>(*i2, *i1);

         product_states.push_back(new_state);
         product_state_names.push_back(new_state->GetName());

			if (i1->GetName() == lts.GetInitialState().GetName()
				&& i2->GetName() == automaton.GetInitialState().GetName()) {
				initial_state = new_state;
			}
		}
	}

	if (initial_state == NULL) {
		throw runtime_error("Failed to find initial state..!");
	}

	const ConfigurationSpace &automaton_config_space(automaton.GetConfigurationSpace());
	const ConfigurationSpace &system_config_space(lts.GetConfigurationSpace());
	const vector<unsigned int> &automaton_configurations = automaton_config_space.GetConfigurations();
	const vector<unsigned int> &system_configurations = system_config_space.GetConfigurations();
	const vector<string> &stack_alphabet = automaton_config_space.GetStackAlphabet();
   
   product_state_names.push_back("_goal_state_");
   product_state_names.push_back("_fail_state_");
   Configuration goal_state = product_state_names.size()-2;
   Configuration fail_state = product_state_names.size()-1;

	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);
   cout << config_space->ToString() << endl;

   ReleaseSystem *release_system = new ReleaseSystem(product_states, config_space);

   vector<string>::const_iterator iter;
   for (iter = stack_alphabet.begin(); iter != stack_alphabet.end(); ++iter) {
      if (*iter != "_") {
         Configuration start_id = config_space->MakeConfiguration(goal_state, *iter);
         PushDownAction *action = new PopAction("#goal#", goal_state);
         release_system->AddRule(start_id, action);
         start_id = config_space->MakeConfiguration(fail_state, *iter);
         action = new PopAction("#fail#", fail_state);
         release_system->AddRule(start_id, action);
      }
   }

	typedef RuleBook<PushDownAction,State>::Rule AutomatonRule;
	typedef RuleBook<RegularAction,KripkeState>::Rule SystemRule;
	const vector<AutomatonRule> &automaton_rules = automaton.GetRules().GetRules();
	const vector<SystemRule> &system_rules = lts.GetRules().GetRules();

   // Recursively check the first subformula (dynamic programming)
   const CheckResults *x_results = Check(x);
   const CheckResults *y_results = Check(y);

   set<Configuration> matched;

   vector<Configuration>::const_iterator automaton_config;
   vector<Configuration>::const_iterator system_config;
   for (automaton_config = automaton_configurations.begin();
         automaton_config != automaton_configurations.end();
         ++automaton_config) {
      const State &automaton_state = automaton.GetState(*automaton_config);
      bool final = automaton_state.GetAccepting();

      for (system_config = system_configurations.begin();
            system_config != system_configurations.end();
            ++system_config) {

//         cout << "X results: "<< x_results->ToString()<< endl;
//         cout << "Y results: "<< y_results->ToString()<< endl;
//         cout << "system config: "<< *system_config << endl;

         const Result &x_res = x_results->GetResult(*system_config);
         const Result &y_res = y_results->GetResult(*system_config);
         bool in_Lx = x_res.GetEvaluation();
         bool in_Ly = y_res.GetEvaluation();

         Configuration start_id = *automaton_config + *system_config * automaton_configurations.size();
         if (in_Lx && (!final || in_Ly)) {
//            cout << "ADDING GOAL LINK" << endl;
            Configuration dest_id = product_state_names.size()-2;
            PushDownAction *action = new RewriteAction("#goal#", dest_id, "_" );
				release_system->AddRule(start_id, action);
            matched.insert(start_id);
            continue;
         }

         if (final && !in_Ly) {
//            cout << "ADDING FAIL LINK" << endl;
            Configuration dest_id = product_state_names.size()-1;
            PushDownAction *action = new RewriteAction("#fail#", dest_id, "_" );
				release_system->AddRule(start_id, action);

            matched.insert(start_id);
         }
      }
   }

	vector<AutomatonRule>::const_iterator j1;
	vector<SystemRule>::const_iterator j2;
	for (j1 = automaton_rules.begin(); j1 != automaton_rules.end(); ++j1) {
		for (j2 = system_rules.begin(); j2 != system_rules.end(); ++j2) {
			if (j1->action->GetName() == j2->action->GetName()) {

				unsigned int start_id;
				start_id = j1->configuration + j2->configuration * automaton_configurations.size();
   
            if (matched.find(start_id) != matched.end()) {
               continue;
            }
   
				unsigned int dest_id;
				dest_id = j1->action->GetDestStateID() + j2->action->GetDestStateID() * automaton_states.size();

				PushDownAction *action = j1->action->Clone();
				action->SetDestStateID(dest_id);
				release_system->AddRule(start_id, action);
			}
      }
   }

   return release_system;
}

ReleaseSystem *ModelChecker::ConstructReleaseSystemFromPDS(
   const PushDownSystem &pds,
   const DFA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   cout << "Constructing release system from pds" << endl;

   // TODO
   return NULL;
}

ProductSystem *ModelChecker::ConstructProductSystemFromLTS(
   const KripkeStructure &lts,
   const PDA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   vector<ProductState<State,KripkeState>*> product_states;
   ProductState<State,KripkeState> *initial_state = NULL;

	vector<string> product_state_names;
	vector<KripkeState> system_states(lts.GetStates());
	vector<State> automaton_states(automaton.GetStates());
	vector<KripkeState>::const_iterator i1;
	vector<State>::const_iterator i2;
	for (i1 = system_states.begin(); i1 != system_states.end(); ++i1) {
		for (i2 = automaton_states.begin(); i2 != automaton_states.end(); ++i2) {
			ProductState<State, KripkeState> *new_state;
         new_state = new ProductState<State,KripkeState>(*i2, *i1);

         product_states.push_back(new_state);
         product_state_names.push_back(new_state->GetName());

			if (i1->GetName() == lts.GetInitialState().GetName()
				&& i2->GetName() == automaton.GetInitialState().GetName()) {
				initial_state = new_state;
			}
		}
	}

	if (initial_state == NULL) {
		throw runtime_error("Failed to find initial state..!");
	}

	const ConfigurationSpace &automaton_config_space(automaton.GetConfigurationSpace());
	const vector<unsigned int> &automaton_configurations = automaton_config_space.GetConfigurations();
	const vector<string> &stack_alphabet = automaton_config_space.GetStackAlphabet();
	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);

	ProductSystem *product_system = new ProductSystem(product_states, initial_state, config_space);

	typedef RuleBook<PushDownAction,State>::Rule AutomatonRule;
	typedef RuleBook<RegularAction,KripkeState>::Rule SystemRule;

	const vector<AutomatonRule> &automaton_rules = automaton.GetRules().GetRules();
	const vector<SystemRule> &system_rules = lts.GetRules().GetRules();

   // Recursively check the first subformula (dynamic programming)
	const CheckResults *x_results  = Check(x);

	vector<AutomatonRule>::const_iterator j1;
	vector<SystemRule>::const_iterator j2;
	for (j1 = automaton_rules.begin(); j1 != automaton_rules.end(); ++j1) {
		for (j2 = system_rules.begin(); j2 != system_rules.end(); ++j2) {
			if (j1->action->GetName() == j2->action->GetName()) {

				// Check system state satisfies x
				const Result &res = x_results->GetResult(j2->configuration);
				if (!res.GetEvaluation()) {
					continue;
				}

				unsigned int start_id;
				start_id = j1->configuration + j2->configuration * automaton_configurations.size();

				unsigned int dest_id;
				dest_id = j1->action->GetDestStateID() + j2->action->GetDestStateID() * automaton_states.size();

				PushDownAction *action = j1->action->Clone();
				action->SetDestStateID(dest_id);
				product_system->AddRule(start_id, action);
			}
		}
	}

   return product_system;
}

ProductSystem *ModelChecker::ConstructProductSystemFromPDS(
   const PushDownSystem &pds,
   const DFA &automaton,
   Formula::Formula::const_reference x,
   Formula::Formula::const_reference y
) {
   // Create product states
   vector<ProductState<State,KripkeState>*> product_states;
   Valuation v;
   ProductState<State,KripkeState> *initial_state = NULL;
	vector<string> product_state_names;
	vector<KripkeState> system_states(pds.GetStates());
	vector<State> automaton_states(automaton.GetStates());
	vector<KripkeState>::const_iterator i1;
	vector<State>::const_iterator i2;
   for (i2 = automaton_states.begin(); i2 != automaton_states.end(); ++i2) {
      for (i1 = system_states.begin(); i1 != system_states.end(); ++i1) {
			ProductState<State, KripkeState> *new_state;
         new_state = new ProductState<State,KripkeState>(*i2, *i1);
			product_states.push_back(new_state);
         if (i1->GetSymbol() == "_"){ 
            product_state_names.push_back(new_state->GetName());
         }
			if (i1->GetName() == pds.GetInitialState().GetName()
				&& i2->GetName() == automaton.GetInitialState().GetName()) {
				initial_state = new_state;
			}
		}
	}

	if (initial_state == NULL) {
		throw runtime_error("Failed to find initial state..!");
	}

	const ConfigurationSpace &system_config_space(pds.GetConfigurationSpace());
	const vector<unsigned int> &system_configurations = system_config_space.GetConfigurations();
	const vector<string> &stack_alphabet = system_config_space.GetStackAlphabet();
	ConfigurationSpace *config_space = new ConfigurationSpace(product_state_names, stack_alphabet);

   // Initialise the product system object
	ProductSystem *product_system = new ProductSystem(product_states, initial_state, config_space);

   // Create rules
	typedef RuleBook<RegularAction,State>::Rule AutomatonRule;
	typedef RuleBook<PushDownAction,KripkeState>::Rule SystemRule;
	const vector<AutomatonRule> &automaton_rules = automaton.GetRules().GetRules();
	const vector<SystemRule> &system_rules = pds.GetRules().GetRules();

   // Recursively check the first subformula (dynamic programming)
	const CheckResults *x_results  = Check(x);

	vector<AutomatonRule>::const_iterator j1;
	vector<SystemRule>::const_iterator j2;
	for (j1 = automaton_rules.begin(); j1 != automaton_rules.end(); ++j1) {
		for (j2 = system_rules.begin(); j2 != system_rules.end(); ++j2) {
			if (j1->action->GetName() == j2->action->GetName()) {

				// Check system state satisfies x
				const Result &res = x_results->GetResult(j2->configuration);
				if (!res.GetEvaluation()) {
					continue;
				}

				unsigned int start_id;
				start_id = j2->configuration + j1->configuration * system_configurations.size();

//            cout << "automaton rule: " << j1->configuration << " " << automaton_config_space.GetStateName(j1->configuration)
//                 << " -> " << j1->action->GetDestStateID() <<" "<< automaton_config_space.GetStateNameByID(j1->action->GetDestStateID()) << endl;
//            cout << "system rule: " << j2->configuration << " " << system_config_space.GetStateName(j2->configuration)
//                 << "[" << system_config_space.GetSymbolName(j2->configuration) << "]"
//                 << " -> " << j2->action->GetDestStateID() <<" "<< system_config_space.GetStateNameByID(j2->action->GetDestStateID()) << endl;
//            cout << "product rule: " << j2->action->GetName()  << ": ";
//            cout << "" << start_id << " " << config_space->GetStateName(start_id);
//            cout << "[" << config_space->GetSymbolName(start_id) << "]";

				unsigned int dest_id;
				dest_id = j2->action->GetDestStateID() + j1->action->GetDestStateID() * system_states.size() / stack_alphabet.size();
//            cout << " -> " << dest_id;
//            cout << " " << config_space->GetStateNameByID(dest_id) << endl;

//            cout << endl;

				PushDownAction *action = j2->action->Clone();
				action->SetDestStateID(dest_id);
				product_system->AddRule(start_id, action);
			}
		}
	}

   return product_system;
}
void ModelChecker::ConstructConfigurationSetForLTS(WPDSProduct &wpds, const ProductSystem *product_system, const CheckResults *y_results, unsigned int automaton_state_count) {

   // Construct a libwpds 'P-automaton' representing the set of configurations
   // see http://www.fmi.uni-stuttgart.de/szs/publications/info/schwoosn.EHRS00b.shtml
	// Find the accepting & after-satisfying configurations
   const vector<Configuration> &product_configurations = product_system->GetConfigurationSpace().GetProductConfigurations(); // for lts
   const vector<string> &stack_alphabet = product_system->GetConfigurationSpace().GetStackAlphabet();

   cout << "Accepting & satisfying product states:" << endl;
	vector<Configuration>::const_iterator iter;
	for (iter = product_configurations.begin(); iter != product_configurations.end(); ++iter) {

//      cout << *iter;
      // Is the state accepting?
      const ProductState<State,KripkeState> &state = product_system->GetState(*iter);
//      cout  << " " << state.ToString();
      bool accepting = state.GetFirst().GetAccepting();

      // Does it satisfy the 'after' formula?
      Configuration system_configuration = *iter / automaton_state_count; // works for lts
//      cout << "[" << system_configuration << "] ";
      const Result &result_after = y_results->GetResult(system_configuration);
      bool satisfying = result_after.GetEvaluation();

      // States which satisfy both have all of their configurations added to
      // the set.
      if (accepting && satisfying) {

         wpds.AddPDSState(state, true);
         vector<string>::const_iterator gamma;
         for (gamma = stack_alphabet.begin(); gamma != stack_alphabet.end(); ++gamma) {
            wpds.AddConfiguration(state, *gamma);
         }
         //wpds.AddConfiguration(state, state.GetSecond().GetSymbol());
      } else {
         wpds.AddPDSState(state, false);
      }
//      cout << endl;
	}
}

void ModelChecker::ConstructConfigurationSetForPDS(WPDSProduct &wpds, const ProductSystem *product_system, const CheckResults *y_results, unsigned int system_state_count) {

   // Construct a libwpds 'P-automaton' representing the set of configurations
   // see http://www.fmi.uni-stuttgart.de/szs/publications/info/schwoosn.EHRS00b.shtml
	// Find the accepting & after-satisfying configurations
   const vector<Configuration> &product_configurations = product_system->GetConfigurationSpace().GetConfigurations();
   const vector<string> &stack_alphabet = product_system->GetConfigurationSpace().GetStackAlphabet();

//      cout << y_results->ToString();
//   cout << "Accepting & satisfying product states:" << endl;
	vector<Configuration>::const_iterator iter;
	for (iter = product_configurations.begin(); iter != product_configurations.end(); ++iter) {

//      cout << *iter;
      // Is the state accepting?
      const ProductState<State,KripkeState> &state = product_system->GetState(*iter);
//      cout  << " " << state.ToString();
      bool accepting = state.GetFirst().GetAccepting();

//      if (accepting)
//         cout << " * ";
      // Does it satisfy the 'after' formula?
      Configuration system_configuration = *iter % system_state_count; 
//      cout << "[" << system_configuration << "] ";
      const Result &result_after = y_results->GetResult(system_configuration);
      bool satisfying = result_after.GetEvaluation();
//      if (satisfying)
//         cout << " & ";

      // States which satisfy both have all of their configurations added to
      // the set.
      if (accepting && satisfying) {
//         cout << *iter << " " << state.ToString() << endl;
         cout << " ## ";

         wpds.AddPDSState(state, true);

         wpds.AddConfigurationTop(state, state.GetSecond().GetSymbol(),stack_alphabet);
      } else {
         wpds.AddPDSState(state, false);
      }
//      cout << endl;
	}
}

void ModelChecker::Visit(const Formula::Until &until) {
   Formula::Formula::const_reference before = until.GetBefore();
   Formula::Formula::const_reference after  = until.GetAfter();
	const string &automaton_name = until.GetAutomaton();
   const ProductSystem *product_system = NULL;
   unsigned int automaton_state_count = 0;
   unsigned int system_state_count = 0;
   const State *automaton_initial_state = NULL;

   if (_is_pds) {
      const DFA &automaton = *_environment.GetDFA(automaton_name);

      automaton_state_count = automaton.GetStates().size(); //TODO
      system_state_count = _system.pds->GetStates().size();
      automaton_initial_state = new State(automaton.GetInitialState());
      product_system = ConstructProductSystemFromPDS(*_system.pds, automaton, before, after); 

   } else {
      // pushdown part should be in the automaton
      const PDA &automaton = *_environment.GetPDA(automaton_name);
      automaton_state_count = automaton.GetStates().size();
      system_state_count = _system.lts->GetConfigurationSpace().GetStateCount();
      automaton_initial_state = new State(automaton.GetInitialState());
      product_system = ConstructProductSystemFromLTS(*_system.lts, automaton, before, after); 
   }

	const CheckResults *y_results  = Check(after);

   WPDSProduct wpds(*product_system);

   if (_is_pds) {
      ConstructConfigurationSetForPDS(
         wpds, product_system, y_results,  system_state_count
      );
   } else {
      ConstructConfigurationSetForLTS(
         wpds, product_system, y_results, automaton_state_count
      );
   }
   wpds.CreatePDS();
   wpds.PrintPDS();
   wpds.PrintConfigurationAutomaton();
   wpds.ComputePredecessorConfigurations();
   wpds.PrintPredecessorConfigurations();

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(GetConfigurations()); 
   vector<Configuration>::const_iterator state_iter;
   for (state_iter = ids.begin(); state_iter != ids.end(); ++state_iter) {
      const KripkeState &system_state(GetSystemState(*state_iter));
      ProductState<State,KripkeState> pre_state(*automaton_initial_state, system_state);
      string pre_state_name = pre_state.GetConfigName();
      Result *result = new Result(*state_iter, pre_state.GetSecond().GetConfigName());
      wpds.CheckPreStar( pre_state, system_state.GetSymbol(), *result );
      results->AddResult( result );
   }

   delete automaton_initial_state;

   SetCheckResults(until, results);
}

void ModelChecker::Visit(const Formula::Release &release) {
   Formula::Formula::const_reference before = release.GetBefore();
   Formula::Formula::const_reference after  = release.GetAfter();
   ReleaseSystem *release_system  = NULL;
   const State *automaton_initial_state = NULL;

   // Retrieve automaton by name
   if (_is_pds) {
      const DFA &automaton = *_environment.GetDFA(release.GetAutomaton());
      automaton_initial_state = new State(automaton.GetInitialState());
      release_system = ConstructReleaseSystemFromPDS(*_system.pds, automaton, before, after);
   }
   else {
      const PDA &automaton = *_environment.GetPDA(release.GetAutomaton());
      automaton_initial_state = new State(automaton.GetInitialState());
      release_system = ConstructReleaseSystemFromLTS(*_system.lts, automaton, before, after);
   }

   cout << release_system->ToString() << endl;

   WPDSRelease wpds(*release_system);

   // TODO
//   wpds.AddAllStates();

   wpds.ComputeRepeatingHeads();
   wpds.PrintRepeatingHeads();
   wpds.ConstructFA();
   wpds.PrintConfigurationAutomaton();
   wpds.CreatePDS();
   wpds.PrintPDS();

   wpds.ComputePredecessorConfigurations();
   wpds.PrintPredecessorConfigurations();

   CheckResults *results = new CheckResults();

   vector<Configuration> ids(GetConfigurations()); 
   vector<Configuration>::const_iterator state_iter;
   for (state_iter = ids.begin(); state_iter != ids.end(); ++state_iter) {

      const KripkeState &system_state(GetSystemState(*state_iter));
      ProductState<State,KripkeState> pre_state(*automaton_initial_state, system_state);
      string pre_state_name = pre_state.GetConfigName();

      cout << "Seeking configuration " << pre_state_name  << endl;

      Result *result = new Result(*state_iter, pre_state.GetSecond().GetConfigName());
      wpds.CheckPreStar( pre_state, system_state.GetSymbol(), *result ); // ok for both?
      results->AddResult( result );
   }
   delete automaton_initial_state;

   SetCheckResults(release, results);
}
void ModelChecker::Visit(const Formula::PVar &pvar) {
   CheckResults *results = new CheckResults();
   vector<Configuration> ids(GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = GetSystemState(id);
      Result *res = new Result(id, state.GetConfigName(), state.Evaluate(pvar.GetVarName()));
      results->AddResult(res);
   }

   SetCheckResults(pvar, results);
}
void ModelChecker::Visit(const Formula::False &formula_false) {
   CheckResults *results = new CheckResults();
   vector<Configuration> ids(GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = GetSystemState(id);
      Result *res = new Result(id, state.GetConfigName(), false);
      results->AddResult(res);
   }
   SetCheckResults(formula_false, results);
}
void ModelChecker::Visit(const Formula::True &formula_true) {
   CheckResults *results = new CheckResults();
   vector<Configuration> ids(GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = GetSystemState(id);
      Result *res = new Result(id, state.GetConfigName(),true);
      results->AddResult(res);
   }
   SetCheckResults(formula_true, results);
}
void ModelChecker::Visit(const Formula::Conjunction &conjunction) {
   const CheckResults *left_results  = Check(conjunction.GetLeft());
   const CheckResults *right_results = Check(conjunction.GetRight());
   CheckResults *results = new CheckResults();
   vector<Configuration> ids(GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = GetSystemState(id);
      const Result &result_left  = left_results->GetResult(id);
      const Result &result_right = right_results->GetResult(id);
      Result *res = new Result(id, state.GetConfigName(), result_left.GetEvaluation() && result_right.GetEvaluation());
      results->AddResult(res);
   }

   SetCheckResults(conjunction, results);
}
void ModelChecker::Visit(const Formula::Negation &negation) {
   const CheckResults *sub_results = Check(negation.GetSubFormula());
   CheckResults *results = new CheckResults();
   vector<Configuration> ids(GetConfigurations());
   vector<Configuration>::const_iterator iter;
   for (iter = ids.begin(); iter != ids.end(); ++iter) {
      Configuration id = *iter;
      const KripkeState &state = GetSystemState(id);
      const Result &sub_result_for_configuration = sub_results->GetResult(id);
      Result *res = new Result(id, state.GetConfigName(), !sub_result_for_configuration.GetEvaluation());
      results->AddResult(res);
   }

   SetCheckResults(negation, results);
}

const CheckResults *ModelChecker::Check( Formula::Formula::const_reference formula ) {
   const ResultsTable &results_table = GetCheckResults();
   if ( results_table.HasEntry( formula ) ) {
      return results_table.GetEntry( formula );
   }

   formula.Accept(*this);
   cout << results_table.ToString() << endl;
   return results_table.GetEntry( formula );
}

