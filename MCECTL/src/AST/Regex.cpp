/*
 * =====================================================================================
 *
 *       Filename:  Regex.cpp
 *    Description:  Visitor implementation for Regex AST
 *
 * =====================================================================================
 */


#include "Regex.h"

namespace AST {
	namespace Regex {
		void Any::Accept(Visitor &visitor) const { visitor.Visit(*this); }   
		void Action::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void Kleene::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void Concat::Accept(Visitor &visitor) const { visitor.Visit(*this); }
		void Union::Accept(Visitor &visitor) const { visitor.Visit(*this); } 
	}
}
