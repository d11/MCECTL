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

   Visitor::~Visitor() { }
   void PVar::Accept(Visitor &visitor) const    { visitor.Visit(*this); }
   void Until::Accept(Visitor &visitor) const   { visitor.Visit(*this); }
   void Release::Accept(Visitor &visitor) const { visitor.Visit(*this); }
}
