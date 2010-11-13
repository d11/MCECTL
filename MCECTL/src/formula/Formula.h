/*
 * =====================================================================================
 *
 *       Filename:  Formula.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _FORMULA_H_
#define  _FORMULA_H_

#include "Automata.h"

namespace Formula {

   class Visitor;

   class Formula {
   public:
      typedef Formula &reference;
      typedef const Formula &const_reference;
      virtual ~Formula() {};
      virtual void Accept(Visitor &visitor) const = 0;
   };

   class PVar : public Formula {
   private:

   public:
      virtual void Accept(Visitor &visitor) const;
   };

   class AutomatonFormula {
   private:
      Formula::const_reference _before;
      Formula::const_reference _after;
      Automaton::const_reference _automaton;
   public:
      AutomatonFormula(
         Formula::const_reference before,
         Formula::const_reference after,
         Automaton::const_reference automaton
      ) : _before(before), _after(after), _automaton(automaton) { }
      virtual ~AutomatonFormula() {};

      Formula::const_reference GetBefore() const { return _before; }
      Formula::const_reference GetAfter()  const { return _after; }
      Automaton::const_reference GetAutomaton() const { return _automaton; }
      virtual void Accept(Visitor &visitor) const = 0;
   };

   class Until : public AutomatonFormula {
   public:
      Until(
         Formula::const_reference before,
         Formula::const_reference after,
         Automaton::const_reference automaton
      ) : AutomatonFormula(before, after, automaton) { }

      virtual void Accept(Visitor &visitor) const;
   };

   class Release : public AutomatonFormula {
   public:
      Release(
         Formula::const_reference before,
         Formula::const_reference after,
         Automaton::const_reference automaton
      ) : AutomatonFormula(before, after, automaton) { }

      virtual void Accept(Visitor &visitor) const;
   };

   class Visitor {
   public:
      virtual void Visit(const Until &until)     = 0;
      virtual void Visit(const Release &release) = 0;
      virtual void Visit(const PVar &release)    = 0;
      virtual ~Visitor() = 0;
   };


}
#endif
