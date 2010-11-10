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
#ifndef _CTL_H_
#define _CTL_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Command.h"

using namespace std;

class CommandParser {
private:

public:
   CommandParser() { }

   vector<Command::Command*> ParseString(string input);
   vector<Command::Command*> ParseFile(FILE *input);
};

#endif
