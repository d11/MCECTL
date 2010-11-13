/*
 * =====================================================================================
 *
 *       Filename:  Show.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/10 14:04:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _SHOW_COMMAND_H_
#define _SHOW_COMMAND_H_

#include <string>
#include <iostream>

#include "Command.h"
#include "Environment.h"

struct GlobalOptions;

namespace Command {

   class ShowCommand : public Command {
      private:
         string _identifier;
      public:
         ShowCommand(string identifier) : _identifier(identifier) { }
         virtual string ToString() const {
            string s = ":show(";
            return s + _identifier + ")" ;
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "TODO" << endl;
         }
   };

}
#endif
