/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMCHAR2_HPP
#define BMCHAR2_HPP
#define IN_BMCHAR2_HPP

#include "alphabet.hpp"
//#include "array.hpp"
#include <vector>
#include "string.hpp"
#include <assert.h>
#include <iostream>

// This is the char_2 shift function for the Boyer-Moore
// type algorithms. It is simpler than char_1.
// It depends upon the match order, and is template
// parameterized by it.

template<class MO>
class Char2 {
public:
	Char2( const String& p, const MO& mo );
	Char2( const Char2& r );

	// The shift itself.
	int shift( const int i ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const Char2<MO2>& r ); // forward
private:
	#include "bmchar2_p.hpp"
};

#include "bmchar2_i.hpp"

#undef IN_BMCHAR2_HPP
#endif

