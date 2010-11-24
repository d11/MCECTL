/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACMKMPFAIL_CPP

#include "alphabet.hpp"
#include "acmkmpfl.hpp"
#include "tries.hpp"
#include "fails.hpp"

std::ostream& operator<<( std::ostream& os, const ACMachineKMPFail& t ) {
	assert( t.c_inv() );
	os << "ACMachineKMPFail = (\n" << t.tau << t.ff
		<< t.out << ")\n";
	assert( t.c_inv() );
	return( os );
}

