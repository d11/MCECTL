/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHRLA_CPP) || defined(INLINING)

INLINE CWShiftRLA::CWShiftRLA( const CWShiftRLA& r ) :
		dopt( r.dopt ),
		d2( r.d2 ),
		rla( r.rla ) {
	assert( c_inv() );
}

INLINE CWShiftRLA::~CWShiftRLA() {
	assert( c_inv() );
}

INLINE int CWShiftRLA::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( max( min( dopt.shift( v, l ), d2[v] ), rla[r] ) );
}

INLINE bool CWShiftRLA::c_inv() const {
	return( !fr && dopt.c_inv() && d2.c_inv() && rla.c_inv() );
}

#endif

