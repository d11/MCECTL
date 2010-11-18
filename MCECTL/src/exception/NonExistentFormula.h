/*
 * =====================================================================================
 *
 *       Filename:  NonExistentFormula.h
 *    Description:  Tried to look up a formula that doesn't exist
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _NONEXISTENTFORMULA_H_
#define  _NONEXISTENTFORMULA_H_

#include <exception>
#include <string>

class NonExistentFormulaException : public exception
{
   private:
      string _formula_name;
   public:
      NonExistentFormulaException(const string &formula_name) : _formula_name(formula_name) { }
      virtual const char* what() const throw () {
         string s = "Couldn't find formula '";
         s += _formula_name + "'";
         return s.c_str();
      }
      virtual ~NonExistentFormulaException() throw () { }
};

#endif
