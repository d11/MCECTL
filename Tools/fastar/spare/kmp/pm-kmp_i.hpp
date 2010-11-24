/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_PM_KMP_CPP) || defined(INLINING)

INLINE PMKMP::PMKMP( const String& kw ) :
		keyword( kw ),
		ff( keyword ) {
	assert( c_inv() );
}

INLINE PMKMP::PMKMP( const PMKMP& r ) :
		keyword( r.keyword ),
		ff( r.ff ) {
	assert( c_inv() );
}

INLINE bool PMKMP::c_inv() const {
//	return( keyword.c_inv() && ff.c_inv() );
	return( ff.c_inv() );
}

#endif

