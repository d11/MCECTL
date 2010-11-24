/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#define IN_S_TRAV_REV_CPP

#include "stravrev.hpp"

std::ostream& operator<<( std::ostream& os, const STravREV& t ) {
	assert( t.c_inv() );
	os << "STravREV = (\n" << "len = " << t.len << "\n)\n";
	assert( t.c_inv() );
	return( os );
}

