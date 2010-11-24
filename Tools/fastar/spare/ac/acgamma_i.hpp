/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACGAMMA_CPP) || defined(INLINING)

INLINE Gamma::Gamma( const Gamma& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE State Gamma::image( const State q, const char a ) const {
	assert( c_inv() );
	assert( FIRSTSTATE <= q && q < rep.size() );
	return( rep[q][a] );
}

INLINE int Gamma::size() const {
	assert( c_inv() );
	return( rep.size() );
}

INLINE bool Gamma::c_inv() const {
	return( rep.c_inv() );
}

#endif

