/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHNAIV_CPP) || defined(INLINING)

INLINE CWShiftNaive::CWShiftNaive( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out ) {
	// Intentionally empty.
}

INLINE CWShiftNaive::CWShiftNaive( const CWShiftNaive& r ) {
	assert( c_inv() );
}

INLINE int CWShiftNaive::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( 1 );
}

INLINE bool CWShiftNaive::c_inv() const {
	return( true );
}

#endif

