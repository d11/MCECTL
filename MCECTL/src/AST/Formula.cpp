/*
 * =====================================================================================
 *
 *       Filename:  Formula.cpp
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _AST_FORMULA_H_
#define  _AST_FORMULA_H_

#include "Formula.h"

namespace AST {
	namespace Formula {
		void False::Accept(Visitor &visitor)       const { visitor.Visit(*this); }
		void True::Accept(Visitor &visitor)        const { visitor.Visit(*this); }
		void PVar::Accept(Visitor &visitor)        const { visitor.Visit(*this); }
		void Negation::Accept(Visitor &visitor)    const { visitor.Visit(*this); }
		void Conjunction::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void Disjunction::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void Implication::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void AX::Accept(Visitor &visitor)          const { visitor.Visit(*this); }
		void Until::Accept(Visitor &visitor)       const { visitor.Visit(*this); }
		void Release::Accept(Visitor &visitor)     const { visitor.Visit(*this); }
	}
}

#endif
