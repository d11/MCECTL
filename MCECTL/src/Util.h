/*
 * =====================================================================================
 *
 *       Filename:  Util.h
 *    Description:  Misc utility functions
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _UTIL_H_
#define  _UTIL_H_

#include <string>
#include <sstream>

using namespace std;

string JoinWithComma(const string &a, const string &b);

template <class K, class V>
string JoinPairsWithColon( const string &a, const pair<K,V> &b ) {
//string JoinPairsWithColon( const string &a, const pair<const Formula::Formula*,CheckResults*> &b ) {
   stringstream bs;
   bs << b.first << ": " << b.second->ToString() << endl;
   if (a.empty()) return bs.str();
   return a + bs.str();
}

#endif
