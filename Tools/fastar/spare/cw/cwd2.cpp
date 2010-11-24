/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWD2_CPP

#include "cwd2.hpp"

// For details on the following, see [Watson-Watson94] or
// [Watson-Zwaan95].

D2::D2( const std::set<String>& P,
		const RTrie& t,
		const RFail& fr,
		const CWOutput& out ) :
		rep( t.size() ) {
	assert( t.c_inv() );
	assert( fr.c_inv() );
	assert( t.size() == fr.size() );
	auto State u;
	for( u = FIRSTSTATE; u < rep.size(); u++ ) {
		rep.map( u ) = PLUSINFINITY;
	}
	// Check if the empty String is a keyword.
//	for( int i = 0; i < P.size(); i++ ) {
	for( std::set<String>::const_iterator i = P.begin(); i != P.end(); i++ ) {
		if( i->length() == 0 ) {
			rep.map( FIRSTSTATE ) = 1;
			break;
		}
	}
	for( u = FIRSTSTATE; u < rep.size(); u++ ) {
		if( out.isKeyword( u ) ) {
			auto State v = u;
			while( v != FIRSTSTATE ) {
				v = fr[v];
				if( t.BFTdepth( u ) - t.BFTdepth( v )
						< rep[v] ) {
					rep.map( v ) = t.BFTdepth( u )
						- t.BFTdepth( v );
				} else {
					break;
				}
			}
		}
	}
	// One last BFT for cleanup.
	for( u = t.BFTfirst(); u != INVALIDSTATE;
			u = t.BFTsuccessor( u ) ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			if( t.image( u, a ) != INVALIDSTATE ) {
				// u = (au \leftdrop 1)
				rep.map( t.image( u, a ) ) = min( rep[u],
					rep[t.image( u, a )] );
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const D2& r ) {
	assert( r.c_inv() );
	os << "D2 = (\n" << r.rep << ")\n";
	assert( r.c_inv() );
	return( os );
}

