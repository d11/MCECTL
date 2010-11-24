/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHIFTNLA_CPP) || defined(INLINING)

INLINE CWShiftNLA::CWShiftNLA( const CWShiftNLA& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE int CWShiftNLA::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( rep[v] );
}

INLINE bool CWShiftNLA::c_inv() const {
	return( rep.c_inv() );
}

#endif

