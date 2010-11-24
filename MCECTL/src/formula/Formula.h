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

   class Formula : public Showable {
   public:
      typedef Formula &reference;
      typedef const Formula &const_reference;
      virtual ~Formula() {};
      virtual void Accept(Visitor &visitor) const = 0;
   };

   class False : public Formula {
   public:
      virtual void Accept(Visitor &visitor) const;
      string ToString() const { return "0"; }
   };

   class True : public Formula {
   public:
      virtual void Accept(Visitor &visitor) const;
      string ToString() const { return "1"; }
   };

   class PVar : public Formula {
   private:
      string _name;
   public:
      PVar(const string &name) : _name(name) {};
      virtual void Accept(Visitor &visitor) const;
      string ToString() const { return _name; }
   };

   class Negation : public Formula {
   private:
      Formula::const_reference _sub_formula;
   public:
      Negation( Formula::const_reference sub_formula ) : _sub_formula(sub_formula) { }
      Formula::const_reference GetSubFormula() const { return _sub_formula; }
      virtual void Accept(Visitor &visitor) const;
      string ToString() const { stringstream s; s << "!" << _sub_formula.ToString(); return s.str(); }
   };

   // Abstract
   class BinaryFormula : public Formula {
   private:
      Formula::const_reference _before;
      Formula::const_reference _after;
   protected:
      Formula::const_reference GetLeft() const { return _before; }
      Formula::const_reference GetRight() const { return _after; }
   public:
      BinaryFormula(
         Formula::const_reference before,
         Formula::const_reference after
      ) : _before(before), _after(after) { }
      virtual ~BinaryFormula() {};
      virtual void Accept(Visitor &visitor) const = 0;
   };

   class Conjunction : public BinaryFormula {
   public:
      Conjunction(
         Formula::const_reference left,
         Formula::const_reference right
      ) : BinaryFormula(left, right) { }
      virtual void Accept(Visitor &visitor) const;
      string ToString() const { stringstream s; s << "(" << GetLeft().ToString() << " ^ " << GetRight().ToString() << ")"; return s.str(); }
   };

   // Abstract
   class AutomatonFormula : public BinaryFormula {
   private:
      const Automaton *_automaton;
   public:
      AutomatonFormula(
         Formula::const_reference before,
         Formula::const_reference after,
         const Automaton *automaton
      ) : BinaryFormula(before, after), _automaton(automaton) { }
      virtual ~AutomatonFormula() {};

      Formula::const_reference GetBefore() const { return GetLeft(); }
      Formula::const_reference GetAfter()  const { return GetRight(); }
      const Automaton *GetAutomaton() const { return _automaton; }
      virtual void Accept(Visitor &visitor) const = 0;
   };

   class Until : public AutomatonFormula {
   public:
      Until(
         Formula::const_reference before,
         Formula::const_reference after,
         const Automaton *automaton
      ) : AutomatonFormula(before, after, automaton) { }

      virtual void Accept(Visitor &visitor) const;
      string ToString() const { stringstream s; s << "E(" << GetLeft().ToString() << " U* " << GetRight().ToString() << ")"; return s.str(); }
   };

   class Release : public AutomatonFormula {
   public:
      Release(
         Formula::const_reference before,
         Formula::const_reference after,
         const Automaton *automaton
      ) : AutomatonFormula(before, after, automaton) { }

      virtual void Accept(Visitor &visitor) const;
      string ToString() const { stringstream s; s << "E(" << GetLeft().ToString() << " R* " << GetRight().ToString() << ")"; return s.str(); }
   };

   class Visitor {
   public:
      virtual void Visit(const False &formula_false)     = 0;
      virtual void Visit(const True &formula_true)       = 0;
      virtual void Visit(const PVar &release)            = 0;
      virtual void Visit(const Conjunction &conjunction) = 0;
      virtual void Visit(const Negation &negation)       = 0;
      virtual void Visit(const Until &until)             = 0;
      virtual void Visit(const Release &release)         = 0;
      virtual ~Visitor() = 0;
   };


}
#endif