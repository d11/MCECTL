/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMSHNAIV_HPP
#define BMSHNAIV_HPP
#define IN_BMSHNAIV_HPP

#include "string.hpp"
#include <assert.h>
#include <iostream>

// This is a naive shifter for the Boyer-Moore type algorithms.
// It simply makes a shift of 1 every time.
// It is template parameterized by the match order because all of
// the other shifters are too.

template<class MO>
class BMShiftNaive {
public:
	BMShiftNaive( const String& p, const MO& mo );
	BMShiftNaive( const BMShiftNaive<MO>& r );

	// The shift itself.
	int shift( const int i, const char a ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const BMShiftNaive<MO2>& r ); // forward
};

#include "bmshnaiv_i.hpp"

#undef IN_BMSHNAIV_HPP
#endif

