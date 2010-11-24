/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"

#if defined(IN_S_TRAV_RAN_CPP) || defined(INLINING)

INLINE STravRAN::STravRAN( const String& s ) {
	assert( c_inv() );
}

INLINE bool STravRAN::c_inv() const {
	return( true );
}

#endif

