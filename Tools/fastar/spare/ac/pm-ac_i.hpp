/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

template<class T>
INLINE PMAC<T>::PMAC( const std::set<String>& P ) :
		machine( P ) {
	assert( c_inv() );
}

template<class T>
INLINE PMAC<T>::PMAC( const PMAC& M ) :
		machine( M.machine ) {
	assert( c_inv() );
}

template<class T>
void PMAC<T>::match( const String& S,
		bool callBack (int, const std::set<String>&) ) {
	assert( c_inv() );
	// Assume that the machine has the same idea of what the first
	// State is.
	auto State q = FIRSTSTATE;
	// Got to keep track of the index into the input String.
	auto int j = 0;

	if( !machine.output( q ).empty() ) {
		// There's something to output.
		if( !callBack( j, machine.output( q ) ) ) {
			assert( c_inv() );
			return;
		}
	}
	auto int Slen = S.length();
	while( j < Slen ) {
//		q = machine.transition( q, alphabetNormalize( S[j] ) );
//		this was incorrect, 2 out of 3 machines do the normalization
//		so for now, removed the alphabetNormalize call here and added it to
//		the other machine
		q = machine.transition( q, S[j] );
		// The transition function must be total.
		assert( q != INVALIDSTATE );
		j++;
		if( !machine.output( q ).empty() ) {
			if( !callBack( j, machine.output( q ) ) ) {
				assert( c_inv() );
				return;
			}
		}
	}
	assert( c_inv() );
}

template<class T>
INLINE bool PMAC<T>::c_inv() const {
	return( machine.c_inv() );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const PMAC<T>& r ) {
	assert( r.c_inv() );
	os << "PMAC<T> = (\n" << r.machine << ")\n";
	return( os );
}
