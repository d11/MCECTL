/*
 * =====================================================================================
 *
 *       Filename:  DeclareRegex.h
 *    Description:  Compile regex to automaton
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef _DECLARE_REGEX_COMMAND_H_
#define _DECLARE_REGEX_COMMAND_H_

#include "Command.h"
#include "AST/Regex.h"
#include "Environment.h"
#include "Automata.h"

// Use 'libfa' for the heavy lifting for now
extern "C" {
#include <fa.h>
   struct fa {
      struct state *initial;
      int           deterministic : 1;
      int           minimal : 1;
      unsigned int  nocase : 1;
      int           trans_re : 1;
   };
   typedef unsigned long hash_val_t;
   struct state {
      struct state *next;
      hash_val_t    hash;
      unsigned int  accept : 1;
      unsigned int  live : 1;
      unsigned int  reachable : 1;
      /* Array of transitions. The TUSED first entries are used, the array
         has allocated room for TSIZE */
      size_t        tused;
      size_t        tsize;
      struct trans *trans;
   };
   typedef unsigned char uchar;
   /* A transition. If the input has a character in the inclusive
    * range [MIN, MAX], move to TO
    */
   struct trans {
      struct state *to;
      union {
         struct {
            uchar         min;
            uchar         max;
         };
         struct re *re;
      };
   };
   
} // End 'extern C'




namespace Command {

  class DeclareRegexCommand : public Command, public AST::Regex::Visitor {
      private:
         string _identifier;
         AST::Regex::Regex *_regex;

         // Storage for use during conversion
         vector<State*> _states;
         // Map libfa states to our states
         map<struct state*, State*> _state_map;

         map<string, uchar> _action_name_map;
         map<uchar, string> _action_name_map_inverse;

         struct fa *_fa;

         State *ConvertState(struct state *libfa_state) {
            // Generate a name for the state
            static int id = 0;
            stringstream s;
            s << "re_" << id++; 

            State *state = new State(s.str());
            _states.push_back(state);
            _state_map.insert(make_pair<struct state*,State*>(libfa_state,state));
            return state;
         }

         void AddRulesForState(DFA *dfa, struct state *libfa_state) {
            map<struct state*, State*>::const_iterator iter;
            iter = _state_map.find(libfa_state);
            if (iter == _state_map.end()) {
               throw runtime_error("Transition refers to state which hasn't been added");
            }
            State *converted = iter->second;
            // Iterate through outgoing transitions
            size_t index = 0;
            struct trans *trans = NULL;
            cout << "Doing transitions from " << converted->ToString() << endl;
            cout << "tused: " << libfa_state->tused << endl;
            while (index < libfa_state->tused) {
               cout << "index: " << index << endl;
               trans = libfa_state->trans + index;
               cout << "min: " << (int)trans->min << " max: " << (int)trans->max << endl;
               if (!trans->to) {
                  throw runtime_error("Destination state is NULL");
               }
               iter = _state_map.find(trans->to);
               if (iter == _state_map.end()) {
                  throw runtime_error("Transition refers to state which hasn't been added");
               }
               //State *converted_to = iter->second;

               if (trans->min == 0 && trans->max == 0) {
                  //RegularAction *action = new RegularAction(".");
                  //dfa->AddRule(converted, action, converted_to);
                  //dfa->AddRule(converted, action); TODO
                  //cout << "adding rule " << converted->ToString() << " -> " << converted_to->ToString() << " via " << action->ToString() << endl;
               } else {
                  uchar c = trans->min;
                  while (c <= trans->max) {
                   //  RegularAction *action = new RegularAction(ActionNameFromChar(c));
                  //dfa->AddRule(converted, action, converted_to);
                   //  dfa->AddRule(converted, action); TODO
                  //   cout << "adding rule " << converted->ToString() << " -> " << converted_to->ToString() << " via " << action->ToString() << endl;
                     ++c;
                  }
               }
               ++index;
            }
         }


         uchar ConvertActionName(const string &name) {
            static uchar c = 1;
            map<string,uchar>::const_iterator iter = _action_name_map.find(name);
            if (iter != _action_name_map.end()) {
               cout << "returning mapping for " << name << "(" << iter->second << ")" << endl;
               return iter->second;
            } else {
               uchar d = c++;
               cout << "mapping " << d << " <-> " << name << endl;
               _action_name_map.insert(make_pair<string,uchar>(name, d));
               _action_name_map_inverse.insert(make_pair<uchar,string>(d, name));
               return d;
            }
         }

         string ActionNameFromChar(uchar c) const {
            map<uchar,string>::const_iterator iter = _action_name_map_inverse.find(c);
            if (iter != _action_name_map_inverse.end()) {
               return iter->second;
            }
            else {
               throw runtime_error("Could not find the action in the map");
            }
         }
      public:
         DeclareRegexCommand(const string &name, AST::Regex::Regex *regex)
            : _identifier(name), _regex(regex), _fa(NULL) { } 
         virtual string ToString() const {
            stringstream s;
            s << "REGULAR " << _identifier << " { "
              << _regex->ToString() << " }";
            return s.str();
         }


         virtual void Visit(const AST::Regex::Any &regex_any) {

            cout << "visiting any" << endl;
            //_fa = fa_make_basic(FA_TOTAL);
           // unsigned int all = -1;
//_fa = fa_make_char_set(&all, 0);
            _fa = fa_make_char(0);
            //throw runtime_error("TODO");
         }
         virtual void Visit(const AST::Regex::Action &regex_action) {
            uchar c = ConvertActionName(regex_action.GetName());
            _fa = fa_make_char(c);
         }
         virtual void Visit(const AST::Regex::Kleene &regex_kleene) {
            cout << "visiting kleene" << endl;
            regex_kleene.GetSubRegex().Accept(*this);
            struct fa *subregex = _fa;
            _fa = fa_iter(subregex, 0, -1);
         }
         virtual void Visit(const AST::Regex::Concat &regex_concat) {
            cout << "visiting concat" << endl;
            regex_concat.GetSubRegex1().Accept(*this);
            struct fa *sub_fa_1 = _fa;
            regex_concat.GetSubRegex2().Accept(*this);
            struct fa *sub_fa_2 = _fa;
            _fa = fa_concat(sub_fa_1, sub_fa_2);
         }
         virtual void Visit(const AST::Regex::Union &regex_union) {
            cout << "visiting union" << endl;
            regex_union.GetSubRegex1().Accept(*this);
            struct fa *sub_fa_1 = _fa;
            regex_union.GetSubRegex2().Accept(*this);
            struct fa *sub_fa_2 = _fa;
            _fa = fa_union(sub_fa_1, sub_fa_2);
         }

         virtual void Execute(Environment &environment, GlobalOptions &options) {
            cout << ToString() << endl;

            //int r = fa_compile("(a|b)*cc*d", 10, &_fa); // temp - test
            _regex->Accept(*this);

            if (NULL == _fa) {
               throw runtime_error("Failed to construct regex automaton");
            }

            int r = fa_minimize(_fa);
            if ( r != REG_NOERROR ) {
               throw runtime_error("Failed to minimise regex automaton");
            }

            struct state *libfa_state = _fa->initial;

            while ( libfa_state != NULL ) {
               ConvertState(libfa_state);
               libfa_state = libfa_state->next;
            }

            State *initial_state = *(_states.begin());
            // TODO
            vector<string> state_names;
            vector<string> stack_alphabet;
            ConfigurationSpace *config_space = new ConfigurationSpace(state_names, stack_alphabet);
            DFA *dfa = new DFA(_states, initial_state, config_space);

            // Add rules
            libfa_state = _fa->initial;
            while ( libfa_state != NULL ) {
               AddRulesForState(dfa, libfa_state);
               libfa_state = libfa_state->next;
            }

            environment.SetDFA( _identifier, dfa );
            cout << dfa->ToString() << endl;

         }
         virtual ~DeclareRegexCommand() {
            delete _regex;
         }
   };

}
#endif
