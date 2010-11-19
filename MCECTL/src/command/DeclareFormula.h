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

//using namespace AST::Formula;

#include "Environment.h"
#include "GlobalOptions.h"
#include "formula/Formula.h"

#include "exception/FormulaBuilder.h"

namespace Command {
   class DeclareFormulaCommand : public Command {
      private:
         string _formula_name;
         AST::Formula::Formula::reference _formula_ast;

			class FormulaBuilder : public AST::Formula::Visitor {
         private:
            AST::Formula::Formula::reference _builder_input;
            Formula::Formula *_current_formula;
            const Environment &_environment;
			public:
            FormulaBuilder(AST::Formula::Formula::reference builder_input, const Environment &environment)
               : _builder_input(builder_input), _current_formula(NULL), _environment(environment) {
            }
            // TODO
            virtual void Visit(const AST::Formula::False &formula_false)     {
               _current_formula = new Formula::False();
            } 
            virtual void Visit(const AST::Formula::True &formula_true)       {
               _current_formula = new Formula::True();
            } 
            virtual void Visit(const AST::Formula::PVar &pvar)               {
               _current_formula = new Formula::PVar(pvar.GetName());
            } 
            virtual void Visit(const AST::Formula::Negation &negation)       {
               negation.GetSubFormula().Accept(*this);
               _current_formula = new Formula::Negation(*_current_formula);
            } 
            virtual void Visit(const AST::Formula::Conjunction &conjunction) {
               conjunction.GetSubFormula1().Accept(*this);
               Formula::Formula::reference left(*_current_formula);
               conjunction.GetSubFormula2().Accept(*this);
               Formula::Formula::reference right(*_current_formula);
               _current_formula = new Formula::Conjunction( left, right );
            } 

            virtual void Visit(const AST::Formula::Disjunction &disjunction) { throw runtime_error("TODO"); } 
            virtual void Visit(const AST::Formula::Implication &implication) { throw runtime_error("TODO"); } 
            virtual void Visit(const AST::Formula::AX &ax)                   { throw runtime_error("TODO"); } 
            virtual void Visit(const AST::Formula::Until &until)             {
               until.GetSubFormula1().Accept(*this);
               Formula::Formula::reference before(*_current_formula);
               until.GetSubFormula2().Accept(*this);
               Formula::Formula::reference after(*_current_formula);
               Automaton::const_reference automaton(_environment.GetAutomaton(until.GetAutomatonName()));
               _current_formula = new Formula::Until( before, after, automaton );
            } 
            virtual void Visit(const AST::Formula::Release &release)         {
               release.GetSubFormula1().Accept(*this);
               Formula::Formula::reference before(*_current_formula);
               release.GetSubFormula2().Accept(*this);
               Formula::Formula::reference after(*_current_formula);
               Automaton::const_reference automaton(_environment.GetAutomaton(release.GetAutomatonName()));
               _current_formula = new Formula::Until( before, after, automaton );
            } 

            Formula::Formula::reference Build() {
               _builder_input.Accept(*this);
               if (NULL == _current_formula) {
                  // Should never happen:
                  throw FormulaBuilderException();
               }
               return *_current_formula;
            };

			};

      public:
         DeclareFormulaCommand(string name, AST::Formula::Formula::reference formula_ast)
            : _formula_name(name), _formula_ast(formula_ast) { }
         virtual string ToString() const {
            return string("FORMULA ") + _formula_name + " { " + _formula_ast.ToString() + " }";
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) const {
            cout << "[FORMULA "  << _formula_name << "]" << endl;
            cout << "Declaring formula... "  << ToString() << endl;

            FormulaBuilder builder(_formula_ast, environment);
            Formula::Formula::const_reference formula = builder.Build();
            environment.SetFormula(_formula_name, formula);
         }
         virtual ~DeclareFormulaCommand() {
            delete &_formula_ast;
         }
   };
}
#endif
