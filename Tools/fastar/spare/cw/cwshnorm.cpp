/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWSHIFTNORM_CPP

#include "cwshnorm.hpp"

CWShiftNorm::CWShiftNorm( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out ) :
		fr( new RFail( t ) ),
		d1( t, *fr ),
		d2( P, t, *fr, out ),
		charCW( t ) {
//	assert( P.c_inv() );
	assert( t.c_inv() );
	assert( out.c_inv() );
	// Now destroy the intermediate structure.
	delete fr;
	fr = 0;
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const CWShiftNorm& r ) {
	assert( r.c_inv() );
	os << "CWShiftNorm = (\n" << r.d1 << r.d2
		<< r.charCW << ")\n";
	assert( r.c_inv() );
	return( os );
}

