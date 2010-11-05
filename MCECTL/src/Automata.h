/*
 * =====================================================================================
 *
 *       Filename:  Automata.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/11/10 10:28:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _AUTOMATA_H_
#define _AUTOMATA_H_

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>
#include <sstream>

using namespace std;
class Automaton {

};

template <class T>
class AutomatonIterator {

};

template <class T>
struct RegularAction {
   boost::weak_ptr<T> predecessor;
   boost::weak_ptr<T> successor;
   string action_name;
};

struct DummyState {
   string ToString() {
      return "[Node]";
   }
};

template <class T>
class DFA {
public:
   struct node;
   typedef boost::shared_ptr<T> state_ref;
   typedef boost::shared_ptr<node> node_ref;
   typedef vector<node_ref> node_vect;
   struct node {
      private:
         state_ref _state;
         map<string, node_ref> _successors;
         bool _terminal;
      public:
         node(state_ref state, bool terminal) : _state(state), _terminal(terminal) { }
         void AddSuccessor(string action, node_ref succ) {
         // TODO check deterministic
            _successors[action] = succ;
         }
         map<string, node_ref> GetSuccessors() const {
            return _successors;
         }
         string ToString() const {
            string s = "[Node]"; // TODO ?
            return s;
         }
   } ;
private:
   node_vect _nodes;
   node_ref _initial_node;
public:
   DFA(const vector<node_ref> &nodes, const node_ref &initial) : _nodes(nodes), _initial_node(initial) {
      cout << "Initting DFA" << endl;
   }
   DFA(string &action) {
      node_ref initial(new node(state_ref(static_cast<T*>(NULL)), false));
      node_ref final(new node(state_ref(static_cast<T*>(NULL)), true));
      initial->AddSuccessor(action, final);
      _nodes.push_back(initial);
      _nodes.push_back(final);
      _initial_node = initial;
   }
   vector< RegularAction<T> > _actions;
   AutomatonIterator< RegularAction<T> > &initialState();
   string ToString() const {
      stringstream s;
      s << "DFA {\n";
      typename node_vect::const_iterator iter;
      int i = 0;
      for (iter = _nodes.begin(); iter != _nodes.end(); ++iter) {
         ++i;
         node_ref node(*iter);
         s << i << ". " << node->ToString() << " ( ";
         
         typename map<string, node_ref>::const_iterator jter;
         map<string, node_ref> successors((*iter)->GetSuccessors());
         for (jter = successors.begin(); jter != successors.end(); ++jter) {
            s << jter->first + " -> ?, ";
         }
         s << " )\n";
      }
      s << "}\n";
      // TODO
      return s.str();
   }

   void Seq( DFA<T>* dfa ) { }
   void Or( DFA<T>* dfa ) { }
   void Kleene() { }
};

class DFABasic : public DFA<DummyState> {

   public:
   DFABasic(string &action) : DFA<DummyState>(action) { }
};

typedef DFABasic *dfa_ref;

class RegexBuilder {

};

#endif
