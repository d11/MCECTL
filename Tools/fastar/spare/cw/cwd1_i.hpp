/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWD1_CPP) || defined(INLINING)

INLINE D1::D1( const D1& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE int D1::operator[]( const State q ) const {
	assert( c_inv() );
	return( rep[q] );
}

INLINE bool D1::c_inv() const {
	return( rep.c_inv() );
}

#endif

