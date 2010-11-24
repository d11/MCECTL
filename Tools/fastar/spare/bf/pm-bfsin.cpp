/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_PM_BFSIN_CPP

#include "pm-bfsin.hpp"

void PMBFSingle::match( const String& S, bool callBack (int) ) {
	assert( c_inv() );
	auto int pLen = kw.length();
	auto int Slast = S.length() - pLen;
	// Do simple Brute-Force matching.
	for( int i = 0; i <= Slast; i++ ) {
		auto int j;
		for( j = 0; j < pLen && kw[j] == S[i + j]; j++ ) {
			// Intentionally empty.
		}
		if( j == pLen ) {
			// There was a match.
			if( !callBack( i + pLen ) ) {
				// The client wants to quit.
				return;
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const PMBFSingle& r ) {
	assert( r.c_inv() );
	os << "PMBFSingle = (\n" << r.kw << ")\n";
	return( os );
}

