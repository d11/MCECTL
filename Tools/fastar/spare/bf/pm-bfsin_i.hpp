/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_PM_BFSIN_CPP) || defined(INLINING)

INLINE PMBFSingle::PMBFSingle( const String& keyword ) :
		kw( keyword ) {
	assert( c_inv() );
}

INLINE bool PMBFSingle::c_inv() const {
	return( true );
}

#endif

