/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWCHAR_CPP) || defined(INLINING)

INLINE CharCW::CharCW( const CharCW& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE int CharCW::shift( const char a, const int z ) const {
	assert( c_inv() );
	return( rep[a] - z );
}

INLINE bool CharCW::c_inv() const {
	return( rep.c_inv() );
}

#endif

