/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWD2_CPP) || defined(INLINING)

INLINE D2::D2( const D2& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE int D2::operator[]( const State u ) const {
	assert( c_inv() );
	return( rep[u] );
}

INLINE bool D2::c_inv() const {
	return( rep.c_inv() );
}

#endif

