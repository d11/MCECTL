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

typedef std::set<std::string> Valuation;

//class Valuation {
//private:
//   std::set<std::string> _propositions; // propositions which are true
//public:
//   Valuation() { }
//
//};

struct State {
   Valuation _valuation;
   std::string toString() const {
      std::string tmp;
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

#endif
