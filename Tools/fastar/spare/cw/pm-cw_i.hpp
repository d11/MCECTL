/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

template<class T>
INLINE PMCW<T>::PMCW( const std::set<String>& P ) :
		trie( P ),
		out( P, trie ),
		shift( P, trie, out ) {
	assert( c_inv() );
}

template<class T>
INLINE PMCW<T>::PMCW( const PMCW<T>& r ) :
		trie( r.trie ),
		out( r.out ),
		shift( r.shift ) {
	assert( c_inv() );
}

template<class T>
INLINE void PMCW<T>::match( const String& S,
		bool callBack (int, const std::set<String>&) ) {
	assert( c_inv() );
	// The indexes for matching. No need to maintain u, it's just r-1.
	// u is encoded as follows:
	// the abstract l is represented as integer l such that
	// S[0..l] = abstract l.
	auto int r = 0, l = -1;
	auto State v = FIRSTSTATE;
	// Maintain a set of current matches.
	auto std::set<String> O;
	if( out.isKeyword( v ) ) {
		O.insert( out[v] );
		// There was a match, so call the client back with it.
		if( !callBack( 0, O ) ) {
			return;
		}
		// Wipe out the match information.
		O.clear();
	}
	// Make an initial shift, thereby phase-shifting the following
	// loop structure.
	// First make a shift. Take care of the case where l has
	// run off the left end of S.
	r += shift.shift( trie, alphabetNormalize( (l < 0 ?
		0 : S[l]) ), v, alphabetNormalize( S[r] ) );

	// Continue until we've passed the right end of the input String.
	auto int Slen = S.length();
	while( r <= Slen ) {
		// Match from right to left.
		l = r - 1;
		v = FIRSTSTATE;
		// Preliminary check for a match.
		if( out.isKeyword( v ) ) {
			O.insert( out[v] );
			// Don't call back the client yet.
		}
		while( l >= 0 && trie.image( v,
				alphabetNormalize( S[l] ) ) != INVALIDSTATE ) {
			v = trie.image( v, alphabetNormalize( S[l] ) );
			l--;
			// Now check if there's a match.
			if( out.isKeyword( v ) ) {
				O.insert( out[v] );
				// Don't call back the client yet.
			}
		}
		// Report any matches.
		if( !O.empty() ) {
			if( !callBack( r, O ) ) {
				// Client wants to quit.
				return;
			}
			O.clear();
		}

		// Make a shift. Take care of the case where l has
		// run off the left end of S.
		assert( shift.shift( trie, alphabetNormalize( (l < 0 ?
			0 : S[l]) ), v, alphabetNormalize( S[r] ) ) >= 1 );

		r += shift.shift( trie, alphabetNormalize( (l < 0 ?
			0 : S[l]) ), v, alphabetNormalize( S[r] ) );
	}
}

template<class T>
INLINE bool PMCW<T>::c_inv() const {
	return( trie.c_inv() && out.c_inv() && shift.c_inv() );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const PMCW<T> r ) {
	assert( r.c_inv() );
	os << "PMCW<T> = (\n" << r.trie << r.out << r.shift << ")\n";
	return( os );
}
