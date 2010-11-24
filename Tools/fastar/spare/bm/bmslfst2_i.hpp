/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_BMSLFST2_CPP) || defined(INLINING)

INLINE SLFast2::SLFast2( const SLFast2& r ) :
		lastIndex( r.lastIndex ),
		a( r.a ),
		distance( r.distance ) {
	// Nothing to do here.
	assert( c_inv() );
}

INLINE int SLFast2::skip( const String& S, int j, const int last ) const {
	assert( c_inv() );
	assert( j <= last );
	// Do some skipping.
	while( j < last && S[j + lastIndex] != a ) {
		j += distance;
	}
	return( min( j, last ) );
}

INLINE bool SLFast2::c_inv() const {
	return( true );
}

#endif

