/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACMOPT_CPP) || defined(INLINING)

INLINE ACMachineOpt::ACMachineOpt( const std::set<String>& P ) :
		// Make some assumptions about order of construction.
		trie( new FTrie( P ) ),
		fail( new FFail( *trie ) ),
		gf( *trie, *fail ),
		out( P, *trie, *fail ) {
//	assert( P.c_inv() );
	assert( !P.empty() );
	assert( trie->c_inv() );
	assert( fail->c_inv() );
	// Now get rid of the intermediates.
	delete trie;
	delete fail;
	trie = 0;
	fail = 0;
	assert( c_inv() );
}

INLINE ACMachineOpt::ACMachineOpt( const ACMachineOpt& M ) :
		gf( M.gf ),
		out( M.out ),
		trie( M.trie ),
		fail( M.fail ) {
	assert( c_inv() );
}

INLINE ACMachineOpt::~ACMachineOpt() {
	assert( c_inv() );
}

INLINE State ACMachineOpt::transition( const State q, const char a ) const {
	assert( c_inv() );
	// The char a will not have been normalized yet, so do it.
	return( gf.image( q, alphabetNormalize( a ) ) );
}

INLINE const std::set<String>& ACMachineOpt::output( const State q ) const {
	assert( c_inv() );
	return( out[q] );
}

INLINE bool ACMachineOpt::c_inv() const {
	return( !trie && !fail && gf.c_inv() && out.c_inv()
			&& gf.size() == out.size() );
}

#endif

