/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_BMSLFST1_CPP

#include "alphabet.hpp"
#include "bmslfst1.hpp"
#include <assert.h>
#include <iostream>

SLFast1::SLFast1( const String& kw ) :
		lastIndex( kw.length() - 1 ),
		a( kw[lastIndex] ) {
	// Now compute the skip distance.
	for( char b = 0; b < ALPHABETSIZE; b++ ) {
		// Just do a linear search to find the appropriate skip.
		auto int k;
		for( k = 1; k <= lastIndex
				&& b != alphabetNormalize( kw[lastIndex - k] );
				k++ ) {
			// Intentionally empty.
		}
		// We've now found the appropriate skip.
		distance.map( b ) = k;
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const SLFast1& r ) {
	assert( r.c_inv() );
	os << "SLFast1 = (\na = " << r.a << "\ndistance = "
		<< r.distance << "\n)\n";
	return( os );
}

