/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMCHAR1_HPP
#define BMCHAR1_HPP
#define IN_BMCHAR1_HPP

#include "alphabet.hpp"
//#include "array.hpp"
#include <vector>
#include "symbolto.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// This is the char_1 shift function for the Boyer-Moore
// type algorithms. It requires more space than the char_2.
// It depends upon the match order, and is template
// parameterized by it.

template<class MO>
class Char1 {
public:
	Char1( const String& p, const MO& mo );
	Char1( const Char1& r );

	// The shift itself.
	int shift( const int i, const char a ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const Char1<MO2>& r ); // forward
private:
	#include "bmchar1_p.hpp"
};

#include "bmchar1_i.hpp"

#undef IN_BMCHAR1_HPP
#endif

