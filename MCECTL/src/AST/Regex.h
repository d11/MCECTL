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

#include "Showable.h"

using namespace std;

namespace AST {

   namespace Regex {

      class Regex : public Showable {
         public:

            virtual ~Regex() {}
      };

      class Any : public Regex {
         public:
            virtual string ToString() const {
               string tmp("[ANY]");
               return tmp;
            }
            virtual ~Any() {}
      };

      class Action : public Regex {
         private:
            string _action_name;
         public:
            Action(string action_name) : _action_name(action_name) { }
            virtual string ToString() const {
               string tmp("[ACTION ");
               return tmp + _action_name + "]";
            }
            virtual ~Action() { }
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
            virtual ~Union() {
               delete _sub_regex_1;
               delete _sub_regex_2;
            }
      };
   }
}

#endif
