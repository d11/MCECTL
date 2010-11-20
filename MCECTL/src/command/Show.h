/*
 * =====================================================================================
 *
 *       Filename:  Show.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
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
				// :show() prints whole environment info
				if (_identifier.empty()) {
					cout << environment.ToString() << endl;
					return;
				}
            cout << "TODO" << endl;
         }
   };

}
#endif
