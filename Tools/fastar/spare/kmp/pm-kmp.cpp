/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_PM_KMP_CPP

#include "pm-kmp.hpp"

void PMKMP::match( const String& S, bool callBack (int) ) {
	assert( c_inv() );
	// This is the classical KMP algorithm.
	auto int Slen = S.length();
	auto int pLen = keyword.length();

	auto int i = 0, j = 0;
	if( i == pLen ) {
		if( !callBack( j ) ) {
			// The client wants to quit the matching.
			return;
		}
	}
	while( j < Slen ) {
		while( 0 <= i && S[j] != keyword[i] ) {
			i = ff[i];
		}
		i++;
		j++;
		if( i == pLen ) {
			if( !callBack( j ) ) {
				// The client wants to quit the matching.
				return;
			}
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const PMKMP& t ) {
	assert( t.c_inv() );
	os << "PMKMP = (\n" << "keyword = " << t.keyword << '\n'
		<< t.ff << ")\n";
	assert( t.c_inv() );
	return( os );
}

