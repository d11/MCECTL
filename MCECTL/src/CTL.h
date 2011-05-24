/*
 * =====================================================================================
 *
 *       Filename:  CTL.h
 *    Description:  
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
   vector<Command::Command*> Parse();
public:
   CommandParser() { }

   vector<Command::Command*> ParseString(string input);
   vector<Command::Command*> ParseFile(FILE *input);
};

#endif
