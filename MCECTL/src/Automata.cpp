/*
 * =====================================================================================
 *
 *       Filename:  Automata.cpp
 *    Description:  Definitions for Automaton classes
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "Automata.h"

string EpsilonAction::ToString() const {
   return "ε";
}

Automaton::~Automaton() { }
