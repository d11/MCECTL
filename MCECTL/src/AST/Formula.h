#ifndef CTL_AST_H
#define CTL_AST_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <sstream>

using namespace std;

namespace AST {
   namespace Formula {

      class Formula {
      public:
         virtual string ToString() = 0;
         virtual ~Formula() {};
      };

      typedef Formula* FormulaRef;

      class False : public Formula {
      public:
         string ToString() {
            return "[FALSE]";
         }
      };

      class True : public Formula {
      public:
         string ToString() {
            return "[TRUE]";
         }
      };

      class PVar : public Formula {
      private:
         string _name;
      public:
         PVar(const string &name) : _name(name) { }
         string ToString() {
            stringstream output;
            output << "[PVAR " << _name << "]";
            return output.str();
         }
         virtual ~PVar() { }
      };

      class Unary : public Formula {
      protected:
         FormulaRef _sub_formula;
         virtual string Show(const char *name) {
            stringstream output;
            output << "[" << name << " " << _sub_formula->ToString() << "]";
            return output.str();
         }
      public:
         Unary(FormulaRef sub_formula) : _sub_formula(sub_formula) { }
         virtual ~Unary() {
            delete _sub_formula;
         }
      };

      class Negation : public Unary {
      public:
         Negation(FormulaRef sub_formula) : Unary(sub_formula) { }
         string ToString() {
            return Unary::Show("NOT");
         }
      };
      class Binary : public Formula {
      protected:
         FormulaRef _sub_formula_1;
         FormulaRef _sub_formula_2;
         string Show(const char *name) {
            stringstream tmp;
            tmp << "[" << _sub_formula_1->ToString() << " "
                << name
                << " " << _sub_formula_2->ToString() << "]";
            return tmp.str();
         }
      public:
         Binary(FormulaRef left, FormulaRef right)
            : _sub_formula_1(left), _sub_formula_2(right) { }
         virtual ~Binary() {
            delete _sub_formula_1;
            delete _sub_formula_2;
         }
      };

      class Conjunction : public Binary {
      public:
         Conjunction(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() {
            return Binary::Show("AND");
         }
      };

      class Disjunction : public Binary {
      public:
         Disjunction(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() {
            return Binary::Show("OR");
         }
      };

      class Implication : public Binary {
      public:
         Implication(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() {
            return Binary::Show("IMPLIES");
         }
      };

      class AX : public Unary {
      public:
         AX(FormulaRef subformula) : Unary(subformula) { }
         string ToString() {
            return Unary::Show("AX");
         }
      };
      class Until : public Binary {
      public:
         Until(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() {
            return Binary::Show("UNTIL");
         }
      };

      class Release : public Binary {
      public:
         Release(FormulaRef left, FormulaRef right)
            : Binary(left, right) { }
         string ToString() {
            return Binary::Show("RELEASE");
         }
      };

   }
}
#endif
