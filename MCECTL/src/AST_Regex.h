/*
 * =====================================================================================
 *
 *       Filename:  AST_Regex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/10 17:03:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _AST_REGEX_H_
#define _AST_REGEX_H_

#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;

namespace AST {

   namespace Regex {

      class Base {
         public:
            virtual string ToString() = 0;
            virtual ~Base() {}
      };

      class Any : public Base {
         public:
            virtual string ToString() {
               string tmp("[ANY]");
               return tmp;
            }
            virtual ~Any() {}
      };

      class Action : public Base {
         private:
            string _action_name;
         public:
            Action(string action_name) : _action_name(action_name) { }
            virtual string ToString() {
               string tmp("[ACTION ");
               return tmp + _action_name + "]";
            }
            virtual ~Action() {}
      };

      class Kleene : public Base {
         private:
            Base *_sub_regex;
         public:
            Kleene(Base *sub_regex) : _sub_regex(sub_regex) { }
            virtual string ToString() {
               string tmp("[KLEENE ");
               return tmp + _sub_regex->ToString() + "]";
            }
            virtual ~Kleene() {
               delete _sub_regex;
            }
      };

      class Concat : public Base {
         private:
            Base *_sub_regex_1;
            Base *_sub_regex_2;
         public:
            Concat(Base *sub_regex_1, Base *sub_regex_2)
               : _sub_regex_1(sub_regex_1), _sub_regex_2(sub_regex_2) { }
            virtual string ToString() {
               string tmp("[");
               return tmp + _sub_regex_1->ToString() + " CONCAT " + _sub_regex_2->ToString() + "]";
            }
            virtual ~Concat() {
               delete _sub_regex_1;
               delete _sub_regex_2;
            }
      };
      class Union : public Base {
         private:
            Base *_sub_regex_1;
            Base *_sub_regex_2;
         public:
            Union(Base *sub_regex_1, Base *sub_regex_2)
               : _sub_regex_1(sub_regex_1), _sub_regex_2(sub_regex_2) { }
            virtual string ToString() {
               string tmp("[");
               return tmp + _sub_regex_1->ToString() + " UNION " + _sub_regex_2->ToString() + "]";
            }
            virtual ~Union() {
               delete _sub_regex_1;
               delete _sub_regex_2;
            }
      };
   }
};

#endif
