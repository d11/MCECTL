/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWDOPT_CPP) || defined(INLINING)

INLINE DOpt::DOpt( const DOpt& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE int DOpt::shift( const State q, const char a ) const {
	assert( c_inv() );
	return( rep[q][a] );
}

INLINE bool DOpt::c_inv() const {
	return( rep.c_inv() );
}

#endif

