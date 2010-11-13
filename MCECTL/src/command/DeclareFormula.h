/*
 * =====================================================================================
 *
 *       Filename:  DeclareFormula.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/10 13:58:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _DECLARE_FORMULA_H_
#define _DECLARE_FORMULA_H_

#include <string>
#include <iostream>
#include <sstream>

#include "AST/Formula.h"
#include "Command.h"

#include "Environment.h"
#include "GlobalOptions.h"

namespace Command {
   class DeclareFormulaCommand : public Command {
      private:
         string _formula_name;
         AST::Formula::FormulaRef _formula;
      public:
         DeclareFormulaCommand(string name, AST::Formula::FormulaRef formula)
            : _formula_name(name), _formula(formula) { }
         virtual string ToString() const {
            string s = "FORMULA ";
            return s + _formula_name + " { " + _formula->ToString() + " }";
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[FORMULA "  << _formula_name << "]" << endl;
            cout << "Declaring formula... "  << ToString() << endl;
            cout << "TODO" << endl;
            // TODO
         }
         virtual ~DeclareFormulaCommand() {
            delete _formula;
         }
   };
}
#endif
