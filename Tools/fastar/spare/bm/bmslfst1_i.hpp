/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_BMSLFST1_CPP) || defined(INLINING)

INLINE SLFast1::SLFast1( const SLFast1& r ) :
		lastIndex( r.lastIndex ),
		a( r.a ),
		distance( r.distance ) {
	// Nothing to do here.
	assert( c_inv() );
}

INLINE int SLFast1::skip( const String& S, int j, const int last ) const {
	assert( c_inv() );
	assert( j <= last );
	// Do some skipping.
	// a is not normalized, for speed.
	while( j < last && S[j + lastIndex] != a ) {
		j += distance[alphabetNormalize( S[j + lastIndex] )];
	}
	return( min( j, last ) );
}

INLINE bool SLFast1::c_inv() const {
	return( true );
}

#endif

