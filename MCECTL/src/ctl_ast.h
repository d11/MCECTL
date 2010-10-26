#include <string>

struct formula {
   virtual std::string toString() = 0;
};
typedef formula* formula_ref;

struct formula_false : public formula {
   std::string toString() {
      return "[false]";
   }
};
struct formula_true : public formula {
   std::string toString() {
      return "[true]";
   }
};
struct formula_pvar : public formula {
   char *_name;
   formula_pvar(char *name) : _name(name) { }
   std::string toString() {
      std::string tmp("[PVAR '");
      tmp += _name;
      tmp += "']";
      return tmp;
   }
};

struct formula_not : public formula {
   formula_ref _formula;
   formula_not(formula_ref subformula) : _formula(subformula) { }
   std::string toString() {
      std::string tmp("[NOT ");
      tmp += _formula->toString();
      tmp += "]";
      return tmp;
   }
};

struct formula_and : public formula {
   formula_ref _left_conjunct;
   formula_ref _right_conjunct;
   formula_and(formula_ref left, formula_ref right)
      : _left_conjunct(left), _right_conjunct(right) { }
   std::string toString() {
      std::string tmp("[");
      tmp += _left_conjunct->toString();
      tmp += " AND ";
      tmp += _right_conjunct->toString();
      tmp += "]";
      return tmp;
   }
};
struct formula_or : public formula {
   formula_ref _left_conjunct;
   formula_ref _right_conjunct;
   formula_or(formula_ref left, formula_ref right)
      : _left_conjunct(left), _right_conjunct(right) { }
   std::string toString() {
      std::string tmp("[");
      tmp += _left_conjunct->toString();
      tmp += " OR ";
      tmp += _right_conjunct->toString();
      tmp += "]";
      return tmp;
   }
};
struct formula_ax : public formula {
   formula_ref _formula;
   formula_ax(formula_ref subformula) : _formula(subformula) { }
   std::string toString() {
      std::string tmp("[AX");
      tmp += _formula->toString();
      tmp += "]";
      return tmp;
   }
};
