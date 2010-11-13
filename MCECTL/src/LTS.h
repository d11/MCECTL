/*
 *
 *    Filename:  LTS.h
 *
 *    Description:  
 *
 *    Labelled Transition System
 */

#ifndef _LTS_H_
#define _LTS_H_

#include <map>
#include <string>
#include <set>
#include <vector>
#include "Automata.h"

using namespace std;

typedef set<string> Valuation;

//class Valuation {
//private:
//   std::set<std::string> _propositions; // propositions which are true
//public:
//   Valuation() { }
//
//};

class State {
private:
   Valuation _valuation;
public:
   string ToString() const {
      string tmp;
      Valuation::const_iterator iter;
      for ( iter = _valuation.begin(); iter != _valuation.end(); iter++ )
      {
         //tmp += "'";
         tmp += *iter;
         //tmp += (*iter).first;
         //tmp += " -> ";
         //tmp += (*iter).second ? "T" : "F";
         tmp += "\n";
      }
      return tmp;
   }
};

class TransitionSystem {
public:
	typedef TransitionSystem &reference;
	typedef const TransitionSystem &const_reference;

	TransitionSystem() {};

   set<State> GetStates() const;
   State GetInitialState() const;
};

class PushDownSystem {

};

typedef set<State>::const_iterator StateIterator;

#endif
