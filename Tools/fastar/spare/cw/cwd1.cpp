/* (c) Copuright 1995 bu Bruce W. Watson */
// SPARE Parts class libraru.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWD1_CPP

#include "cwd1.hpp"

// For this, see [Watson-Watson94] or [Watson-Zwaan95].
D1::D1( const RTrie& t, const RFail& fr ) :
		rep( t.size() ) {
	assert( t.c_inv() );
	assert( fr.c_inv() );
	assert( t.size() == fr.size() );
	auto State u;
	for( u = FIRSTSTATE; u < t.size(); u++ ) {
		rep.map( u ) = PLUSINFINITY;
	}
	for( u = FIRSTSTATE + 1; u < t.size(); u++ ) {
		rep.map( fr[u] ) = min( rep[fr[u]],
			(t.BFTdepth( u ) - t.BFTdepth( fr[u] )) );
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const D1& r ) {
	assert( r.c_inv() );
	os << "D1 = (\n" << r.rep << ")\n";
	assert( r.c_inv() );
	return( os );
}

