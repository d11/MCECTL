/*
 * =====================================================================================
 *
 *       Filename:  AutomataConcrete.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _AUTOMATACONCRETE_H_
#define  _AUTOMATACONCRETE_H_

#include <string>


// Abstract
class Action : public Showable {
public:
   typedef const Action &const_reference;
   typedef const Action *const_ptr;
   virtual ~Action() {}
};

/*
class NondeterministicAction : public Action {

};

class EpsilonAction : public NondeterministicAction {
public:
   string ToString() const;
};

class RegularAction : public NondeterministicAction {
private:
   string _action_name;
public:
   RegularAction(const string &action_name) : _action_name(action_name) {}
   const string &GetName() const { return _action_name; }
   string ToString() const {
      return _action_name;
   }
};

typedef string StackSymbol;

class NondeterministicPushDownAction : public Action {

};


// Abstract
class PushDownAction : public NondeterministicPushDownAction {
protected:
   string _action_name;
   StackSymbol _top_symbol;
public:
   PushDownAction(const string &action_name, const StackSymbol &top_symbol) : _action_name(action_name), _top_symbol(top_symbol) { }
   virtual ~PushDownAction() {}
};

class PushAction : public PushDownAction {
private:
   StackSymbol _push_symbol;
public:
   PushAction(const string &action_name, const StackSymbol &top_symbol, const StackSymbol &push_symbol)
      : PushDownAction(action_name, top_symbol), _push_symbol(push_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Push " << _push_symbol;
      return s.str();
   }
};

class RewriteAction : public PushDownAction {
private:
   StackSymbol _rewrite_symbol;
public:
   RewriteAction(const string &action_name, const StackSymbol &top_symbol, const StackSymbol &rewrite_symbol)
      : PushDownAction(action_name, top_symbol), _rewrite_symbol(rewrite_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Rewrite " << _top_symbol << " to " << _rewrite_symbol;
      return s.str();
   }
};

class PopAction : public PushDownAction {
public:
   PopAction(const string &action_name, const StackSymbol &top_symbol)
      : PushDownAction(action_name, top_symbol) { }
   string ToString() const {
      stringstream s;
      s << _action_name << ": Pop";
      return s.str();
   }
};

class PushDownEpsilonAction : public NondeterministicPushDownAction {
public:
   string ToString() const;
};
*/
class State {
protected:
   string _name;
public:
   State(const string &name) : _name(name) {}
   const string &GetName() const { return _name; }
   string ToString() const {
      stringstream s;
      s << "[" << _name << "]";
      return s.str();
   }
   bool operator<(const State &other) const {
      return _name < other._name;
   }
};

typedef vector<string> Valuation;

class KripkeState : public Showable {
private:
   Valuation _valuation;
public:
   typedef const KripkeState &const_reference;
   typedef const KripkeState *const_ptr;
   KripkeState(const string &name, const Valuation &valuation) : _valuation(valuation) { }

   string ToString() const;
   bool operator<(const KripkeState &other) const {
      return ToString() < other.ToString();
   }
};


/*
typedef vector<string> Va

class KripkeState : public State, Showable {
private:
   Valuation _valuation;
public:
   typedef const KripkeState &const_reference;
   typedef const KripkeState *const_ptr;
   KripkeState(const string &name, const Valuation &valuation) : State(name), _valuation(valuation) { }

   string ToString() const;
   bool operator<(const KripkeState &other) const {
      return ToString() < other.ToString();
   }
};

typedef unsigned int index_t;

typedef unsigned int RegularConfiguration;

struct PushDownConfiguration {
   index_t state_index;
   index_t symbol_index;
};

// Deterministic Finite Automaton
typedef FiniteAutomaton<RegularAction,           RegularConfiguration, State>  DFA;

// Non-determinic Finite Automaton
typedef FiniteAutomaton<NondeterministicAction,  RegularConfiguration, State>  NFA;

// Deterministic Push Down Automaton
typedef FiniteAutomaton<PushDownAction,          PushDownConfiguration, State>  PDA;


template <class S>
class RegularAutomatonIterator : public AutomatonIterator<RegularAction, S> {

};



*/

#endif
