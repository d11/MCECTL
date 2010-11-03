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

typedef 

class Valuation {
private:
   std::set<std::string> _propositions;
public:
   Valuation()

};

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
struct RegularAction {
   boost::weak_ptr<T> predecessor;
   boost::weak_ptr<T> successor;
   std::string action_name;
} RegularAction;

template <class T>
class DFA {
public:
   typedef boost::shared_ptr<T> state_ref;
   struct node;
   typedef boost::shared_ptr<node> node_ref;
private:
   struct node {
      state_ref state;
      std::map<std::string, node_ref> successors;
   };
   std::vector<node_ref> _nodes;
public:
   DFA(const std::vector<node_ref> &nodes) : _nodes(nodes) {
      std::cout << "Initting DFA" << std::endl;
   }
   std::vector<RegularAction<T>> _actions;
   AutomatonIterator<RegularAction<T>> &initialState()
};
