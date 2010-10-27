/*
 *
 *    Filename:  LTS.h
 *
 *    Description:  
 *
 *    Labelled Transition System
 */
#include <map>
#include <string>

typedef std::map<std::string, bool> Valuation;

struct State {
   Valuation _valuation;
   std::string toString() {
      std::string tmp;
      for ( iter = _valuation.begin(); iter != _valuation.end(); iter++ )
      {
         tmp += "'";
         tmp += (*it).first;
         tmp += " -> ";
         tmp += (*it).second ? "T" : "F";
         tmp += "\n";
      }
      return tmp;
   }
};

template <class T>
struct Action {
   boost::weak_ptr<T> predecessor;
   boost::weak_ptr<T> successor;
   std::string action_name;
} Action;

template <class T>
class DFA {
   std::vector<boost::shared_ptr<State>> _states;
   std::vector<Action> _actions;

   DFA()
};
