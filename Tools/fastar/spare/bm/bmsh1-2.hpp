/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMSH1_2_HPP
#define BMSH1_2_HPP
#define IN_BMSH1_2_HPP

#include "string.hpp"
#include "bms1.hpp"
#include "bmchar2.hpp"
#include <assert.h>
#include <iostream>

// This is a Boyer-Moore shifter. It makes use of the s_1 and the
// char_2 shift functions (and hence its name).
// It is template parameterized by the match order because all of
// the other shifters are too.

template<class MO>
class BMShift12 {
public:
	BMShift12( const String& p, const MO& mo );
	BMShift12( const BMShift12<MO>& r );

	// The shift itself.
	int shift( const int i, const char a ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const BMShift12<MO2>& r ); // forward
private:
	#include "bmsh1-2_p.hpp"
};

#include "bmsh1-2_i.hpp"

#undef IN_BMSH1_2_HPP
#endif

