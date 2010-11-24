/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHIFTNORM_CPP) || defined(INLINING)

INLINE CWShiftNorm::CWShiftNorm( const CWShiftNorm& r ) :
		d1( r.d1 ),
		d2( r.d2 ),
		charCW( r.charCW ) {
	assert( c_inv() );
}

INLINE CWShiftNorm::~CWShiftNorm() {
	assert( c_inv() );
}

INLINE int CWShiftNorm::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( min( max( charCW.shift( l, t.BFTdepth( v ) ),
		d1[v] ), d2[v] ) );
}

INLINE bool CWShiftNorm::c_inv() const {
	return( !fr && d1.c_inv() && d2.c_inv() && charCW.c_inv() );
}

#endif

