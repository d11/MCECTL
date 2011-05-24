/*
 * =====================================================================================
 *
 *       Filename:  Util.cpp
 *    Description:  Misc utility functions
 *
 * =====================================================================================
 */

#include <string>
#include "Util.h"
using namespace std;

string JoinWithComma(const string &a, const string &b) {
   if (a.empty()) return b;
   return a + ", " + b;
}

