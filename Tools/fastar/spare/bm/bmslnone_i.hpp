/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_BMSLNONE_CPP) || defined(INLINING)

INLINE SLNone::SLNone( const String& kw ) {
	// Really nothing to do here.
	assert( c_inv() );
}

INLINE SLNone::SLNone( const SLNone& r ) {
	// Nothing to do here.
	assert( c_inv() );
}

INLINE int SLNone::skip( const String& S, const int j, const int last ) const {
	assert( c_inv() );
	assert( j <= last );
	// Don't skip at all.
	return( j );
}

INLINE bool SLNone::c_inv() const {
	return( true );
}

#endif

