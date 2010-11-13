/*
 * =====================================================================================
 *
 *       Filename:  DeclareRegex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/10 14:05:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _DECLARE_REGEX_COMMAND_H_
#define _DECLARE_REGEX_COMMAND_H_

#include "Command.h"
#include "AST/Regex.h"
#include "Environment.h"

namespace Command {

  class DeclareRegexCommand : public Command {
      private:
         string _identifier;
         AST::Regex::Regex *_regex;
      public:
         DeclareRegexCommand(string name, AST::Regex::Regex *regex)
            : _identifier(name), _regex(regex) { } 
         virtual string ToString() const {
            stringstream s;
            s << "REGULAR " << _identifier << " { "
              << _regex->ToString() << " }";
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            // TODO
            cout << "TODO" << endl;
            cout << ToString() << endl;
         }
         virtual ~DeclareRegexCommand() {
            delete _regex;
         }
   };

}
#endif
