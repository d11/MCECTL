/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWCHAR_CPP

#include "cwchar.hpp"

CharCW::CharCW( const RTrie& t ) {
	// First set everything at infinity.
	for( char a = 0; a < ALPHABETSIZE; a++ ) {
		rep.map( a ) = PLUSINFINITY;
	}
	// Now a BFT of the trie; start on second level.
	for( State q = t.BFTsuccessor( t.BFTfirst() );
			q != INVALIDSTATE; q = t.BFTsuccessor( q ) ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			if( t.image( q, a ) != INVALIDSTATE ) {
				rep.map( a ) = min( rep[a], t.BFTdepth( q ) );
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const CharCW& r ) {
	assert( r.c_inv() );
	os << "CharCW = (\n" << r.rep << ")\n";
	assert( r.c_inv() );
	return( os );
}

