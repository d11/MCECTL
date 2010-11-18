/*
 * =====================================================================================
 *
 *       Filename:  FormulaBuilder.h
 *    Description:  Exception for when the formula builder has gone wrong
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _FORMULABUILDER_H_
#define  _FORMULABUILDER_H_

#include <exception>
#include <string>

class FormulaBuilderException : public exception
{
   public:
      virtual const char* what() const throw () {
         return "Formula builder error: finished, but _current_formula is NULL";
      }
      virtual ~FormulaBuilderException() throw () { }
};

#endif
