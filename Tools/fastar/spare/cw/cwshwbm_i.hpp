/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_CWSHIFTWBM_CPP) || defined(INLINING)

INLINE CWShiftWBM::CWShiftWBM( const CWShiftWBM& r ) :
		rep( r.rep ),
		charBM( r.charBM ) {
	assert( c_inv() );
}

INLINE CWShiftWBM::~CWShiftWBM() {
	assert( c_inv() );
}

INLINE int CWShiftWBM::shift( const RTrie& t,
		const char l,
		const State v,
		const char r ) const {
	assert( c_inv() );
	return( max( charBM[l] - t.BFTdepth( v ), rep[v] ) );
}

INLINE bool CWShiftWBM::c_inv() const {
	return( rep.c_inv() && charBM.c_inv() );
}

#endif

