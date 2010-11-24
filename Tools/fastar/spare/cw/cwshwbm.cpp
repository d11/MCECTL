/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWSHIFTWBM_CPP

#include "cwshwbm.hpp"

CWShiftWBM::CWShiftWBM( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out ) :
		rep( t.size() ),
		charBM( t, P ) {
//	assert( P.c_inv() );
	assert( t.c_inv() );
	assert( out.c_inv() );
	// Now construct d_1 and d_2.
	auto RFail f( t );
	auto D1 d1( t, f );
	auto D2 d2( P, t, f, out );

	// Now merge d1 and d2 into one.
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		rep.map( q ) = min( d1[q], d2[q] );
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const CWShiftWBM& r ) {
	assert( r.c_inv() );
	os << "CWShiftWBM = (\n" << r.rep << r.charBM << ")\n";
	assert( r.c_inv() );
	return( os );
}

