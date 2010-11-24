/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"
#include "com-io.hpp"

template<class MO>
Char1<MO>::Char1( const String& p, const MO& mo ) :
		rep( p.length() + 1 ) {
	auto int pLen = p.length();
	// Use a simple linear search to compute the MIN quant.
	for( int i = 0; i <= pLen; i++ ) {
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			// Do the linear search for this particular i.
			auto int k;
			for( k = 1; i < pLen && k <= mo.traverse( i )
					&& a != alphabetNormalize( p[mo.traverse( i ) - k] );
					k++ ) {
				// Intentionally empty.
			}
			// k is the shift distance.
			rep.at( i ).map( a ) = k;
		}
	}
	assert( c_inv() );
}

template<class MO>
INLINE Char1<MO>::Char1( const Char1& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class MO>
INLINE int Char1<MO>::shift( const int i, const char a ) const {
	assert( c_inv() );
	// expect a to be normalized.
	return( rep[i][a] );
}

template<class MO>
INLINE bool Char1<MO>::c_inv() const {
//	return( rep.c_inv() );
	return( true );
}

template<class MO>
std::ostream& operator<<( std::ostream& os, const Char1<MO>& r ) {
	assert( r.c_inv() );
	os << "Char1 = (\n" << r.rep << ")\n";
	return( os );
}
