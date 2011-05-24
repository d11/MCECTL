/*
 * =====================================================================================
 *
 *       Filename:  AST_Regex.h
 *    Description:  Abstract syntax tree for regular expressions
 *
 * =====================================================================================
 */

#ifndef _AST_REGEX_H_
#define _AST_REGEX_H_

#include <string>
#include "Showable.h"

using namespace std;

namespace AST {

   namespace Regex {

      class Visitor;

      class Regex : public Showable {
      public:
         virtual ~Regex() {}
         virtual void Accept(Visitor &visitor) const = 0;
      };

      class Any : public Regex {
      public:
         virtual string ToString() const {
            string tmp("[ANY]");
            return tmp;
         }
         virtual ~Any() {}
      public:
         virtual void Accept(Visitor &visitor) const;
      };

      class Action : public Regex {
      private:
         string _action_name;
      public:
         Action(const string &action_name) : _action_name(action_name) { }
         virtual string ToString() const {
            string tmp("[ACTION ");
            return tmp + _action_name + "]";
         }
         string GetName() const { return _action_name; }
         virtual ~Action() { }
         virtual void Accept(Visitor &visitor) const;
      };

      class Kleene : public Regex {
      private:
         Regex *_sub_regex;
      public:
         Kleene(Regex *sub_regex) : _sub_regex(sub_regex) { }
         virtual string ToString() const {
            string tmp("[KLEENE ");
            return tmp + _sub_regex->ToString() + "]";
         }
         virtual ~Kleene() {
            delete _sub_regex;
         }
         const Regex &GetSubRegex() const { return *_sub_regex; }
         virtual void Accept(Visitor &visitor) const;
      };

      class Concat : public Regex {
      private:
         Regex *_sub_regex_1;
         Regex *_sub_regex_2;
      public:
         Concat(Regex *sub_regex_1, Regex *sub_regex_2)
            : _sub_regex_1(sub_regex_1), _sub_regex_2(sub_regex_2) { }
         virtual string ToString() const {
            string tmp("[");
            return tmp + _sub_regex_1->ToString() + " CONCAT " + _sub_regex_2->ToString() + "]";
         }
         virtual ~Concat() {
            delete _sub_regex_1;
            delete _sub_regex_2;
         }
         const Regex &GetSubRegex1() const { return *_sub_regex_1; }
         const Regex &GetSubRegex2() const { return *_sub_regex_2; }
         virtual void Accept(Visitor &visitor) const;
      };
      class Union : public Regex {
      private:
         Regex *_sub_regex_1;
         Regex *_sub_regex_2;
      public:
         Union(Regex *sub_regex_1, Regex *sub_regex_2)
            : _sub_regex_1(sub_regex_1), _sub_regex_2(sub_regex_2) { }
         virtual string ToString() const {
            string tmp("[");
            return tmp + _sub_regex_1->ToString() + " UNION " + _sub_regex_2->ToString() + "]";
         }
         const Regex &GetSubRegex1() const { return *_sub_regex_1; }
         const Regex &GetSubRegex2() const { return *_sub_regex_2; }
         virtual ~Union() {
            delete _sub_regex_1;
            delete _sub_regex_2;
         }
         virtual void Accept(Visitor &visitor) const;
      };

      class Visitor {
      public:
         virtual void Visit(const Any    &regex_any)    = 0;
         virtual void Visit(const Action &regex_action) = 0;
         virtual void Visit(const Kleene &regex_kleene) = 0;
         virtual void Visit(const Concat &regex_concat) = 0;
         virtual void Visit(const Union  &regex_union)  = 0;
         virtual ~Visitor() {};
      };
   }
}

#endif
