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

// We use 'libfa' for constructing the automaton and then convert it to our
// format afterwards; the following type definitions are reproduced from the
// libfa code in order to allow linking with that code.
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
      size_t        tused;
      size_t        tsize;
      struct trans *trans;
   };
   typedef unsigned char uchar;
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
   
} // End 'extern C' (and end of reproduced code).


namespace Command {

  class DeclareRegexCommand : public Command, public AST::Regex::Visitor {
      private:
			// Name for the regex and the DFA once it is produced
         string _identifier;
		
			// The AST for the regular expression
         AST::Regex::Regex *_regex;

         // Storage for use during conversion
         vector<State*> _states;
         // Map libfa states to our states
         map<struct state*, State*> _state_map;

			// Map between action names in libfa and in our format
         map<string, uchar> _action_name_map;
         map<uchar, string> _action_name_map_inverse;
			
			// The libfa automaton that will be constructed
         struct fa *_fa; 

			// Convert a single state
         State *ConvertState(struct state *libfa_state) {
            // Generate a name for the state
            static int id = 0;
            stringstream s;
            s << "re_" << id++; 

				// Create the State object and store it
            State *state = new State(s.str(), libfa_state->accept);
            _states.push_back(state);
            _state_map.insert(make_pair<struct state*,State*>(libfa_state,state));
            return state;
         }

			/* Convert all rules which have libfa_state as their source, and add them to dfa */
         void AddRulesForState(DFA *dfa, struct state *libfa_state) {
				/* Look up and retrieve the source configuration for the rule */
            map<struct state*, State*>::const_iterator iter;
            iter = _state_map.find(libfa_state);
            if (iter == _state_map.end()) {
               throw runtime_error("Transition refers to state which hasn't been added");
            }
            State *converted = iter->second;
				const ConfigurationSpace &config_space = dfa->GetConfigurationSpace();
				Configuration from_config = config_space.GetStateID(converted->GetName());

            /* Iterate through outgoing transitions and convert the rules */
            size_t index = 0;
            struct trans *trans = NULL;
            while (index < libfa_state->tused) {
               trans = libfa_state->trans + index;
               if (!trans->to) {
                  throw runtime_error("Destination state is NULL");
               }
					// Find the destination state
               iter = _state_map.find(trans->to);
               if (iter == _state_map.end()) {
                  throw runtime_error("Transition refers to state which hasn't been added");
               }
					State *converted_to = iter->second;
					Configuration to_id = config_space.GetStateID(converted_to->GetName());

					/* The 'any character' case */
               if (trans->min == 0 && trans->max == 0) {
						RegularAction *action = new RegularAction(".", to_id);
						dfa->AddRule(from_config, action);
						cout << "adding rule " << converted->ToString() << " -> " << converted_to->ToString() << " via " << action->ToString() << endl;
               }
					else { // 
                  uchar c = trans->min;
                  while (c <= trans->max) {
							RegularAction *action = new RegularAction(ActionNameFromChar(c), to_id);
							dfa->AddRule(from_config, action); 
							cout << "adding rule " << converted->ToString() << " -> " << converted_to->ToString() << " via " << action->ToString() << endl;
                     ++c;
                  }
               }
               ++index;
            }
         }

			// Convert between action names, since we use full strings and libfa
			// uses single characters. 
         uchar ConvertActionName(const string &name) {
            static uchar c = 1;
            map<string,uchar>::const_iterator iter = _action_name_map.find(name);
            if (iter != _action_name_map.end()) {
               return iter->second;
            } else {
               uchar d = c++;
               _action_name_map.insert(make_pair<string,uchar>(name, d));
               _action_name_map_inverse.insert(make_pair<uchar,string>(d, name));
               return d;
            }
         }

			// Convert back from the char 
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

			// Textual representation
         virtual string ToString() const {
            stringstream s;
            s << "REGULAR " << _identifier << " { "
              << _regex->ToString() << " }";
            return s.str();
         }

			// Using the Visitor pattern, we recurse over the AST of the regex to
			// build up an automaton, stored in _fa 
         virtual void Visit(const AST::Regex::Any &regex_any) {
            _fa = fa_make_char(0);
         }
         virtual void Visit(const AST::Regex::Action &regex_action) {
            uchar c = ConvertActionName(regex_action.GetName());
            _fa = fa_make_char(c);
         }
         virtual void Visit(const AST::Regex::Kleene &regex_kleene) {
            regex_kleene.GetSubRegex().Accept(*this);
            struct fa *subregex = _fa;
            _fa = fa_iter(subregex, 0, -1);
         }
         virtual void Visit(const AST::Regex::Concat &regex_concat) {
            regex_concat.GetSubRegex1().Accept(*this);
            struct fa *sub_fa_1 = _fa;
            regex_concat.GetSubRegex2().Accept(*this);
            struct fa *sub_fa_2 = _fa;
            _fa = fa_concat(sub_fa_1, sub_fa_2);
         }
         virtual void Visit(const AST::Regex::Union &regex_union) {
            regex_union.GetSubRegex1().Accept(*this);
            struct fa *sub_fa_1 = _fa;
            regex_union.GetSubRegex2().Accept(*this);
            struct fa *sub_fa_2 = _fa;
            _fa = fa_union(sub_fa_1, sub_fa_2);
         }

			// Run the command, constructing the automaton and storing it in the
			// environment
         virtual void Execute(Environment &environment, GlobalOptions &options) {

				// Initiate traversal of the AST, producing a libfa automaton (_fa)
            _regex->Accept(*this);

            if (NULL == _fa) {
               throw runtime_error("Failed to construct regex automaton");
            }

				/* Minimise the produced automaton */
            int r = fa_minimize(_fa);
            if ( r != REG_NOERROR ) {
               throw runtime_error("Failed to minimise regex automaton");
            }

				/* Convert the states to State objects */
            struct state *libfa_state = _fa->initial;
            while ( libfa_state != NULL ) {
               ConvertState(libfa_state);
               libfa_state = libfa_state->next;
            }

            State *initial_state = *(_states.begin());


				/* Calculate the possible configurations */
            vector<string> state_names;
				vector<State*>::const_iterator iter;
				for (iter = _states.begin(); iter != _states.end(); ++iter) {
					state_names.push_back((*iter)->GetName());
				}
            vector<string> stack_alphabet;
				stack_alphabet.push_back("_");
            ConfigurationSpace *config_space = new ConfigurationSpace(state_names, stack_alphabet);

				/* Create the final automaton object */
            DFA *dfa = new DFA(_states, initial_state, config_space);

            /* Convert and add the rules to it */
            libfa_state = _fa->initial;
            while ( libfa_state != NULL ) {
               AddRulesForState(dfa, libfa_state);
               libfa_state = libfa_state->next;
            }

				/* Store the result in the environment */
            environment.SetDFA( _identifier, dfa );
         }
         virtual ~DeclareRegexCommand() {
            delete _regex;
         }
   };

}
#endif
