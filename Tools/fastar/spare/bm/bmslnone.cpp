/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_BMSLNONE_CPP

#include "com-misc.hpp"
#include "bmslnone.hpp"
#include <assert.h>
#include <iostream>

std::ostream& operator<<( std::ostream& os, const SLNone& r ) {
	assert( r.c_inv() );
	os << "SLNone = ()\n";
	return( os );
}

