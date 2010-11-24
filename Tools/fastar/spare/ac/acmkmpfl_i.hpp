/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACMKMPFAIL_CPP) || defined(INLINING)

INLINE ACMachineKMPFail::ACMachineKMPFail( const std::set<String>& P ) :
		tau( P ),
		ff( tau ),
		out( P, tau, ff ) {
//	assert( P.c_inv() );
	assert( !P.empty() );
	assert( c_inv() );
}

INLINE ACMachineKMPFail::ACMachineKMPFail( const ACMachineKMPFail& M ) :
		tau( M.tau ),
		ff( M.ff ),
		out( M.out ) {
	assert( c_inv() );
}

INLINE ACMachineKMPFail::~ACMachineKMPFail() {
	assert( c_inv() );
}

INLINE State ACMachineKMPFail::transition( State q, const char a ) const {
	assert( c_inv() );
	// The char a will not have been normalized yet, so do it.
	// Standard linear search.
	while( tau.image( q, alphabetNormalize( a ) ) == INVALIDSTATE && q != FIRSTSTATE ) {
		q = ff[q];
	}
	if( tau.image( q, alphabetNormalize( a ) ) != INVALIDSTATE ) {
		assert( c_inv() );
		return( tau.image( q, alphabetNormalize( a ) ) );
	} else {
		assert( c_inv() );
		assert( q == FIRSTSTATE );
		return( q );
	}
}

INLINE const std::set<String>& ACMachineKMPFail::output( const State q ) const {
	assert( c_inv() );
	return( out[q] );
}

INLINE bool ACMachineKMPFail::c_inv() const {
	return( tau.c_inv() && ff.c_inv() && out.c_inv()
		&& tau.size() == ff.size()
		&& tau.size() == out.size() );
}

#endif

