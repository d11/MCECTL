/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACMFAIL_CPP) || defined(INLINING)

INLINE ACMachineFail::ACMachineFail( const ACMachineFail& M ) :
		tauef( M.tauef ),
		ff( M.ff ),
		out( M.out ),
		trie( 0 ) {
	assert( c_inv() );
}

INLINE ACMachineFail::~ACMachineFail() {
	assert( c_inv() );
}

INLINE State ACMachineFail::transition( State q, const char a ) const {
	assert( c_inv() );
	// The char a will not have been normalized yet, so do it.
	// Do the linear search.
	while( tauef.image( q, alphabetNormalize( a ) ) == INVALIDSTATE ) {
		q = ff[q];
	}
	assert( c_inv() );
	return( tauef.image( q, alphabetNormalize( a ) ) );
}

INLINE const std::set<String>& ACMachineFail::output( const State q ) const {
	assert( c_inv() );
	return( out[q] );
}

INLINE bool ACMachineFail::c_inv() const {
	return( !trie && tauef.c_inv() && ff.c_inv()
		&& out.c_inv() && tauef.size() == ff.size()
		&& tauef.size() == out.size() );
}

#endif

