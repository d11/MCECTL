/*
 * =====================================================================================
 *
 *       Filename:  Util.h
 *    Description:  Misc utility functions
 *
 * =====================================================================================
 */


#ifndef  _UTIL_H_
#define  _UTIL_H_

#include <string>
#include <sstream>

using namespace std;

// The following are used in various forms of output accumulation procedures

// Append b to a, inserting ", " between the two and returning the result
string JoinWithComma(const string &a, const string &b);

// 
template <class K, class V>
string JoinPairsWithColon( const string &a, const pair<K,V> &b ) {
   stringstream bs;
   bs << b.first << ": " << b.second->ToString() << endl;
   if (a.empty()) return bs.str();
   return a + bs.str();
}

template <class K, class V>
string JoinKeysWithSquareBrackets(const string &a, const pair<K,V> &b ) {
   stringstream bs;
   bs << "[" << b.first << "]" << endl;
   if (a.empty()) return bs.str();
   return a + bs.str();
}

#endif
