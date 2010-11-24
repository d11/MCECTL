/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWCHARBM_CPP

#include "cwcharbm.hpp"

CharBM::CharBM( const RTrie& t, const std::set<String>& P ) {
	// Find out the length of the shortest keyword.
	auto int mP = PLUSINFINITY;
//	for( int i = 0; i < P.size(); i++ ) {
	for( std::set<String>::const_iterator i = P.begin(); i != P.end(); i++ ) {
		mP = min( mP, i->length() );
	}
	// First set everything at the length of the shortest kw.
	for( char a = 0; a < ALPHABETSIZE; a++ ) {
		rep.map( a ) = mP;
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

std::ostream& operator<<( std::ostream& os, const CharBM& r ) {
	assert( r.c_inv() );
	os << "CharBM = (\n" << r.rep << ")\n";
	assert( r.c_inv() );
	return( os );
}

