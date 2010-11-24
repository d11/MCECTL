/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACGAMMA_CPP

#include "alphabet.hpp"
#include "com-misc.hpp"
#include "acgamma.hpp"
#include <vector>

Gamma::Gamma( const FTrie& t, const FFail& f ) :
		rep( t.size() ) {
	assert( t.c_inv() );
	assert( f.c_inv() );
	// The failure function must be related to the trie.
	assert( t.size() == f.size() );
	assert( rep.size() == t.size() );

	// Perform a (more or less random) traversal of trie t,
	// constructing the rep along the way. (Whether it's BFT or
	// DFT is not relevant.)
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		// Iterate over the alphabet, constructing the transitions.
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			// Do the linear search.
			auto State qfail = q;
			while( t.image( qfail, a ) == INVALIDSTATE && qfail
					!= FIRSTSTATE) {
				// Use the failure function as the predecessor function.
				qfail = f[qfail];
			}
			assert( t.image( qfail, a ) != INVALIDSTATE
				|| qfail == FIRSTSTATE );
			if( t.image( qfail, a ) != INVALIDSTATE ) {
				rep.
    			map( q ).
    			map( a ) = t.image( qfail, a );
			} else {
				assert( qfail == FIRSTSTATE );
				rep.map( q ).map( a ) = FIRSTSTATE;
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const Gamma& t ) {
	assert( t.c_inv() );
	os << "Gamma = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}

