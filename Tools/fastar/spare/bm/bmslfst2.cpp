/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_BMSLFST2_CPP

#include "com-misc.hpp"
#include "bmslfst2.hpp"
#include <assert.h>
#include <iostream>

SLFast2::SLFast2( const String& kw ) :
		lastIndex( kw.length() - 1 ),
		a( kw[lastIndex] ) {
	// Now compute the skip distance.
	auto int k;
	for( k = 1; k <= lastIndex
			&& kw[lastIndex] == kw[lastIndex - k]; k++ ) {
		// Intentionally empty.
	}
	distance = k;

	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const SLFast2& r ) {
	assert( r.c_inv() );
	os << "SLFast2 = (\na = " << r.a << "\ndistance = "
		<< r.distance << "\n)\n";
	return( os );
}

