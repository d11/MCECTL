/*
 * =====================================================================================
 *
 *       Filename:  DeclareFormula.h
 *    Description:  Create formula objects from their ASTs
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
#include "formula/Formula.h"
#include "exception/FormulaBuilder.h"

namespace Command {

	class FormulaBuilder : public AST::Formula::Visitor {
	private:
		AST::Formula::Formula::reference _builder_input;
		Formula::Formula *_current_formula;
		const Environment &_environment;
	public:
		FormulaBuilder(AST::Formula::Formula::reference builder_input, const Environment &environment)
			: _builder_input(builder_input), _current_formula(NULL), _environment(environment) {
			}
		virtual void Visit(const AST::Formula::False &formula_false) {
			_current_formula = new Formula::False();
		} 
		virtual void Visit(const AST::Formula::True &formula_true) {
			_current_formula = new Formula::True();
		} 
		virtual void Visit(const AST::Formula::PVar &pvar) {
			_current_formula = new Formula::PVar(pvar.GetName());
		} 
		virtual void Visit(const AST::Formula::Negation &negation) {
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

		// (A | B) == !(!A & !B)
		virtual void Visit(const AST::Formula::Disjunction &disjunction) { 
			disjunction.GetSubFormula1().Accept(*this);
			Formula::Formula::reference left(*_current_formula);
			disjunction.GetSubFormula2().Accept(*this);
			Formula::Formula::reference right(*_current_formula);
			Formula::Formula *neg_left = new Formula::Negation(left);
			Formula::Formula *neg_right = new Formula::Negation(right);
			Formula::Formula *conj = new Formula::Conjunction(*neg_left, *neg_right);
			_current_formula = new Formula::Negation(*conj);

		} 
		// (A -> B) == !(A & !B)
		virtual void Visit(const AST::Formula::Implication &implication) { 
			implication.GetSubFormula1().Accept(*this);
			Formula::Formula::reference left(*_current_formula);
			implication.GetSubFormula2().Accept(*this);
			Formula::Formula::reference right(*_current_formula);
			Formula::Formula *neg_right = new Formula::Negation(right);
			Formula::Formula *conj = new Formula::Conjunction(left, *neg_right);
			_current_formula = new Formula::Negation(*conj);
		} 
		virtual void Visit(const AST::Formula::AX &ax) {
			AST::Formula::Formula *all_until
				= new AST::Formula::AllUntil(
							new AST::Formula::True(),
							ax.GetSubFormula().Clone(), // Subformula copy
							"#X#" // Special automaton which allows one step
						);
			all_until->Accept(*this);
			delete all_until; 
		} 
		virtual void Visit(const AST::Formula::EX &ex) {
			AST::Formula::Formula *until
				= new AST::Formula::Until(
							new AST::Formula::True(),
							ex.GetSubFormula().Clone(), // Subformula copy
							"#X#" // Special automaton which allows one step
						);
			until->Accept(*this);
			delete until;
		} 
		virtual void Visit(const AST::Formula::Until &until) {
			until.GetSubFormula1().Accept(*this);
			Formula::Formula::reference before(*_current_formula);
			until.GetSubFormula2().Accept(*this);
			Formula::Formula::reference after(*_current_formula);
			_current_formula = new Formula::Until( before, after, until.GetAutomatonName() );
		} 
		virtual void Visit(const AST::Formula::AllUntil &until)          {
			AST::Formula::Formula *before = until.GetSubFormula1().Clone();
			AST::Formula::Formula *after  = until.GetSubFormula2().Clone();
			AST::Formula::Formula *neg_before 
				= new AST::Formula::Negation(before);
			AST::Formula::Formula *neg_after 
				= new AST::Formula::Negation(after);
			AST::Formula::Formula *release = new AST::Formula::Release(neg_before, neg_after, until.GetAutomatonName());
			AST::Formula::Formula *neg_release = new AST::Formula::Negation(release);
			neg_release->Accept(*this);
			delete neg_release; // Subformulas are deleted recursively by the destructors
		} 
		virtual void Visit(const AST::Formula::Release &release)         {
			release.GetSubFormula1().Accept(*this);
			Formula::Formula::reference before(*_current_formula);
			release.GetSubFormula2().Accept(*this);
			Formula::Formula::reference after(*_current_formula);
			_current_formula = new Formula::Release( before, after, release.GetAutomatonName() );
		} 
		virtual void Visit(const AST::Formula::AllRelease &release)      {
			AST::Formula::Formula *before = release.GetSubFormula1().Clone();
			AST::Formula::Formula *after  = release.GetSubFormula2().Clone();
			AST::Formula::Formula *neg_before
				= new AST::Formula::Negation(before);
			AST::Formula::Formula *neg_after
				= new AST::Formula::Negation(after);
			AST::Formula::Formula *until
				= new AST::Formula::Until(
							neg_before, 
							neg_after, 
							release.GetAutomatonName()
						);
			AST::Formula::Formula *neg_until = new AST::Formula::Negation(until);
			neg_until->Accept(*this);
			delete neg_until; // Subformulas are deleted recursively by the destructors
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

   class DeclareFormulaCommand : public Command {
      private:
         string _formula_name;
         AST::Formula::Formula::reference _formula_ast;

      public:
         DeclareFormulaCommand(string name, AST::Formula::Formula::reference formula_ast)
            : _formula_name(name), _formula_ast(formula_ast) { }
         virtual string ToString() const {
				ostringstream s;
				s  << "FORMULA " << _formula_name
					<< " { " << _formula_ast.ToString() << " }";
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) {
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
