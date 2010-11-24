/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMS1_HPP
#define BMS1_HPP
#define IN_BMS1_HPP

#include "alphabet.hpp"
//#include "array.hpp"
#include <vector>
#include "symbolto.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// This is the s_1 shift function for the Boyer-Moore
// type algorithms. It depends upon the match order, and
// is template parameterized by it.

template<class MO>
class S1 {
public:
	S1( const String& p, const MO& mo );
	S1( const S1& r );

	// The shift itself.
	int shift( const int i ) const;
	bool c_inv() const;
	template<class MO2>
		friend std::ostream& operator<<( std::ostream& os, const S1<MO2>& r ); // forward
private:
	#include "bms1_p.hpp"
};

#include "bms1_i.hpp"

#undef IN_BMS1_HPP
#endif

