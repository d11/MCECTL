/*
 * =====================================================================================
 *
 *       Filename:  CTL.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/10/10 20:49:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef CTL_H
#define CTL_H

#include "Command.h"
#include <string>
#include <boost/shared_ptr.hpp>

class CommandParser {
private:

public:
   CommandParser() { }

   boost::shared_ptr<Command::Command> ParseString(string input);
};

#endif
