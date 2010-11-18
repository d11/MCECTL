/*
 * =====================================================================================
 *
 *       Filename:  Formula.cpp
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include "Formula.h"

namespace Formula {

   void False::Accept(Visitor &visitor) const       { visitor.Visit(*this); }
   void True::Accept(Visitor &visitor) const        { visitor.Visit(*this); }
   void PVar::Accept(Visitor &visitor) const        { visitor.Visit(*this); }
   void Negation::Accept(Visitor &visitor) const    { visitor.Visit(*this); }
   void Conjunction::Accept(Visitor &visitor) const { visitor.Visit(*this); }
   void Until::Accept(Visitor &visitor) const       { visitor.Visit(*this); }
   void Release::Accept(Visitor &visitor) const     { visitor.Visit(*this); }
   Visitor::~Visitor() { }

}
