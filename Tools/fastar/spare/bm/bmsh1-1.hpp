/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMSH1_1_HPP
#define BMSH1_1_HPP
#define IN_BMSH1_1_HPP

#include "string.hpp"
#include "bms1.hpp"
#include "bmchar1.hpp"
#include <assert.h>
#include <iostream>

// This is a Boyer-Moore shifter. It makes use of the s_1 and the
// char_1 shift functions (and hence its name).
// It is template parameterized by the match order because all of
// the other shifters are too.

template<class MO>
class BMShift11 {
public:
	BMShift11( const String& p, const MO& mo );
	BMShift11( const BMShift11<MO>& r );

	// The shift itself.
	int shift( const int i, const char a ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const BMShift11<MO2>& r ); // forward
private:
	#include "bmsh1-1_p.hpp"
};

#include "bmsh1-1_i.hpp"

#undef IN_BMSH1_1_HPP
#endif

