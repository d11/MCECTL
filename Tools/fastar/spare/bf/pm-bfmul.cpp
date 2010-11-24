/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_PM_BFMUL_CPP

#include "pm-bfmul.hpp"

void PMBFMulti::match( const String& S,
					  bool callBack (int, const std::set<String>&) ) {
	assert( c_inv() );
	auto int Slen = S.length();

	// Store the keywords matched.
	auto std::set<String> Matched;
	assert( Matched.empty() );

	// Do simple Brute-Force matching.
	for( int i = 0; i < Slen; i++ ) {
//		for( int k = 0; k < P.size(); k++ ) {
		for( std::set<String>::const_iterator k = P.begin(); k != P.end(); k++ ) {
			// Do keyword k.
//			auto int pLen = P.iterSelect( k ).length();
			auto int pLen = k->length();
			auto int j;
			for( j = 0;
					j < pLen
					&& i >= pLen
//					&& P.iterSelect( k )[P.iterSelect( k ).length() - j - 1]
					&& k->operator[](k->length() - j - 1)
						== S[i - j];
					j++ ) {
				// Intentionally empty.
			}
			if( j == pLen ) {
				// There was a match.
				// Add it to the match set.
//				Matched.insert( P.iterSelect( k ) );
				Matched.insert( *k );
			}
		}
		// If there are any matches, then report them to the client.
		if( !Matched.empty() ) {
			if( !callBack( i, Matched ) ) {
				return;
			}
			Matched.clear();
		}
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const PMBFMulti& r ) {
	assert( r.c_inv() );
	os << "PMBFMulti = (\n" << r.P << ")\n";
	return( os );
}

