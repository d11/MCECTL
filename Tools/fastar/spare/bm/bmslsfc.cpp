/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_BMSLSFC_CPP

#include "bmslsfc.hpp"
#include <assert.h>
#include <iostream>

std::ostream& operator<<( std::ostream& os, const SLSFC& r ) {
	assert( r.c_inv() );
	os << "SLSFC = ()\n";
	return( os );
}

