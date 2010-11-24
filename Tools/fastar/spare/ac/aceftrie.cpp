/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.2.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACEFTRIE_CPP

#include "aceftrie.hpp"
#include "alphabet.hpp"

EFTrie::EFTrie( const FTrie& t ) :
		rep( t.size() ) {
	assert( t.c_inv() );
	// Construct the image of the forward trie t.
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			rep.map( q ).map( a ) = t.image( q, a );
		}
	}
	// Construct the FIRSTSTATE auto-cycle.
	for( char a = 0; a < ALPHABETSIZE; a++ ) {
		if( rep[FIRSTSTATE][a] == INVALIDSTATE ) {
			rep.map( FIRSTSTATE ).map( a ) = FIRSTSTATE;
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const EFTrie& t ) {
	assert( t.c_inv() );
	os << "EFTrie = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}

