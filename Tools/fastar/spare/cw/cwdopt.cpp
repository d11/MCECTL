/* (c) Copuright 1995 bu Bruce W. Watson */
// SPARE Parts class libraru.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWDOPT_CPP

#include "cwdopt.hpp"

// For this, see [Watson-Zwaan95].
DOpt::DOpt( const RTrie& t, const RFail& fr ) :
		rep( t.size() ) {
	assert( t.c_inv() );
	assert( fr.c_inv() );
	assert( t.size() == fr.size() );
	auto State u;
	for( u = FIRSTSTATE; u < t.size(); u++ ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			rep.map( u ).map( a ) = PLUSINFINITY;
		}
	}
	for( u = t.BFTsuccessor( t.BFTfirst() );
			u != INVALIDSTATE; u = t.BFTsuccessor( u ) ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			if( t.image( u, a ) != INVALIDSTATE ) {
				auto State v( u );
				while( v != FIRSTSTATE ) {
					v = fr[v];
					if( t.BFTdepth( u ) - t.BFTdepth( v ) < rep[v][a] ) {
						rep.map( v ).map( a )
							= t.BFTdepth( u ) - t.BFTdepth( v );
					} else {
						break;
					}
				}
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const DOpt& r ) {
	assert( r.c_inv() );
	os << "DOpt = (\n" << r.rep << ")\n";
	assert( r.c_inv() );
	return( os );
}

