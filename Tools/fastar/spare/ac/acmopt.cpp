/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACMOPT_CPP

#include "alphabet.hpp"
#include "acmopt.hpp"
#include "tries.hpp"
#include "fails.hpp"

std::ostream& operator<<( std::ostream& os, const ACMachineOpt& t ) {
	assert( t.c_inv() );
	os << "ACMachineOpt = (\n" << t.gf << t.out << ")\n";
	assert( t.c_inv() );
	return( os );
}

