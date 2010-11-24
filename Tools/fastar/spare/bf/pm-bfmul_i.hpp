/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_PM_BFMUL_CPP) || defined(INLINING)

INLINE PMBFMulti::PMBFMulti( const std::set<String>& keywords ) :
		P( keywords ) {
	assert( c_inv() );
}

INLINE bool PMBFMulti::c_inv() const {
//	return( P.c_inv() );
	return( true );
}

#endif

