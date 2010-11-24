/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

#if defined(IN_ACEFTRIE_CPP) || defined(INLINING)

INLINE EFTrie::EFTrie( const EFTrie& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

INLINE State EFTrie::image( const State q, const char a ) const {
	assert( c_inv() );
	return( rep[q][a] );
}

INLINE int EFTrie::size() const {
	assert( c_inv() );
	return( rep.size() );
}

INLINE bool EFTrie::c_inv() const {
	return( rep.c_inv() );
}

#endif

