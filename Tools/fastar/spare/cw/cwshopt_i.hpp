/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHOPT_CPP) || defined(INLINING)

INLINE CWShiftOpt::CWShiftOpt( const CWShiftOpt& r ) :
		dopt( r.dopt ),
		d2( r.d2 ) {
	assert( c_inv() );
}

INLINE CWShiftOpt::~CWShiftOpt() {
	assert( c_inv() );
}

INLINE int CWShiftOpt::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( min( dopt.shift( v, l ), d2[v] ) );
}

INLINE bool CWShiftOpt::c_inv() const {
	return( !fr && dopt.c_inv() && d2.c_inv() );
}

#endif

