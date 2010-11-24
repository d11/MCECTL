/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"

#if defined(IN_S_TRAV_REV_CPP) || defined(INLINING)

INLINE STravREV::STravREV( const String& s ) :
		len( s.length() ) {
	assert( c_inv() );
}

INLINE int STravREV::traverse( int const index ) const {
	assert( 0 <= index && index < len );
	return( len - index - 1 );
}

INLINE int STravREV::traverseInverse( const int index ) const {
	// The reverse one happens to be its own inverse.
	return( traverse( index ) );
}

INLINE bool STravREV::c_inv() const {
	return( 0 <= len );
}

#endif

