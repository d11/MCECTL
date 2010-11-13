#ifndef CTL_AST_H
#define CTL_AST_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <sstream>

#include "Showable.h"

using namespace std;

namespace AST {
   namespace Formula {

		class Visitor;

      class Formula : public Showable {
      public:
         typedef Formula &reference;
         typedef const Formula &const_reference;
         virtual ~Formula() {};
			virtual void Accept(Visitor &visitor) const = 0;
      };

      typedef Formula* FormulaRef;

      class False : public Formula {
      public:
         string ToString() const {
            return "[FALSE]";
         }
			virtual void Accept(Visitor &visitor) const;
      };

      class True : public Formula {
      public:
         string ToString() const {
            return "[TRUE]";
         }
			virtual void Accept(Visitor &visitor) const;
      };

      class PVar : public Formula {
      private:
         string _name;
      public:
         PVar(const string &name) : _name(name) { }
         string ToString() const {
            stringstream output;
            output << "[PVAR " << _name << "]";
            return output.str();
         }
         virtual ~PVar() { }
			virtual void Accept(Visitor &visitor) const;
      };

      class Unary : public Formula {
      protected:
         FormulaRef _sub_formula;
         virtual string Show(string name) const {
            stringstream output;
            output << "[" << name << " " << _sub_formula->ToString() << "]";
            return output.str();
         }
      public:
         Unary(FormulaRef sub_formula) : _sub_formula(sub_formula) { }
         virtual ~Unary() {
            delete _sub_formula;
         }
         Formula::reference GetSubFormula() { return *_sub_formula; }
			virtual void Accept(Visitor &visitor) const = 0;
      };

      class Negation : public Unary {
      public:
         Negation(FormulaRef sub_formula) : Unary(sub_formula) { }
         string ToString() const {
            return Unary::Show("NOT");
         }
			virtual void Accept(Visitor &visitor) const;
      };
      class Binary : public Formula {
      protected:
         FormulaRef _sub_formula_1;
         FormulaRef _sub_formula_2;
         string Show(string name) const {
            stringstream tmp;
            tmp << "[" << _sub_formula_1->ToString() << " "
                << name
                << " " << _sub_formula_2->ToString() << "]";
            return tmp.str();
         }
      public:
         Binary(FormulaRef left, FormulaRef right)
            : _sub_formula_1(left), _sub_formula_2(right) { }
         Formula::reference GetSubFormula1() const { return *_sub_formula_1; }
         Formula::reference GetSubFormula2() const { return *_sub_formula_2; }
         virtual ~Binary() {
            delete _sub_formula_1;
            delete _sub_formula_2;
         }
			virtual void Accept(Visitor &visitor) const = 0;
      };

      class Conjunction : public Binary {
      public:
         Conjunction(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() const {
            return Binary::Show("AND");
         }
			virtual void Accept(Visitor &visitor) const;
      };

      class Disjunction : public Binary {
      public:
         Disjunction(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() const {
            return Binary::Show("OR");
         }
			virtual void Accept(Visitor &visitor) const;
      };

      class Implication : public Binary {
      public:
         Implication(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() const {
            return Binary::Show("IMPLIES");
         }
			virtual void Accept(Visitor &visitor) const;
      };

      class AX : public Unary {
      public:
         AX(FormulaRef subformula) : Unary(subformula) { }
         string ToString() const {
            return Unary::Show("AX");
         }
			virtual void Accept(Visitor &visitor) const;
      };
      class Until : public Binary {
      private:
         string _automaton_name;
      public:
         Until(FormulaRef left, FormulaRef right, string automaton_name)
            : Binary(left, right), _automaton_name(automaton_name) { }
         string ToString() const {
            return Binary::Show(string("UNTIL {AUTOMATA ") + _automaton_name + "}");
         }
         string GetAutomatonName() const { return _automaton_name; }
			virtual void Accept(Visitor &visitor) const;
      };

      class Release : public Binary {
      private:
         string _automaton_name;
      public:
         Release(FormulaRef left, FormulaRef right, string automaton_name)
            : Binary(left, right), _automaton_name(automaton_name) { }
         string ToString() const {
            return Binary::Show(string("RELEASE[") + _automaton_name + "]");
         }
         string GetAutomatonName() const { return _automaton_name; }
			virtual void Accept(Visitor &visitor) const;
      };

		class Visitor {
		public:
			virtual void Visit(const False &formula_false)     = 0;
			virtual void Visit(const True &formula_true)       = 0;
			virtual void Visit(const PVar &pvar)               = 0;
			virtual void Visit(const Negation &negation)       = 0;
			virtual void Visit(const Conjunction &conjunction) = 0;
			virtual void Visit(const Disjunction &disjunction) = 0;
			virtual void Visit(const Implication &implication) = 0;
			virtual void Visit(const AX &ax)                   = 0;
			virtual void Visit(const Until &until)             = 0;
			virtual void Visit(const Release &release)         = 0;
			virtual ~Visitor() {};
		};

   }
}
#endif
