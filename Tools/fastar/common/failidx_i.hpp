/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"

#if defined(IN_FAILIDX_CPP) || defined(INLINING)

INLINE FailIdx::FailIdx( const FailIdx& f ) :
		rep( f.rep ) {
	assert( c_inv() );
}

INLINE int FailIdx::operator[]( const int i ) const {
	assert( c_inv() );
	assert( 0 <= i && i < rep.size() );
	return( rep[i] );
}

INLINE bool FailIdx::c_inv() const {
//	return( rep.c_inv() );
	return( true );
}

#endif

