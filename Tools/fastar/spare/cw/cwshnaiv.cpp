/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_CWSHNAIV_CPP

#include "cwshnaiv.hpp"

std::ostream& operator<<( std::ostream& os, const CWShiftNaive& r ) {
	assert( r.c_inv() );
	os << "CWShiftNaive = ()\n";
	assert( r.c_inv() );
	return( os );
}

