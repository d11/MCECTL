/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"
#include "com-io.hpp"

template<class MO>
Char2<MO>::Char2( const String& p, const MO& mo ) :
		rep( p.length() + 1 ) {
	auto int pLen = p.length();
	// Use a simple linear search to compute the MIN quant.
	for( int i = 0; i <= pLen; i++ ) {
		// Do the linear search for this particular i.
		auto int k;
		for( k = 1; i < pLen && k <= mo.traverse( i )
				&& p[mo.traverse( i )] == p[mo.traverse( i ) - k];
				k++ ) {
			// Intentionally empty.
		}
		// k is the shift distance.
		rep.at( i ) = k;
	}
	assert( c_inv() );
}

template<class MO>
INLINE Char2<MO>::Char2( const Char2& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class MO>
INLINE int Char2<MO>::shift( const int i ) const {
	assert( c_inv() );
	return( rep[i] );
}

template<class MO>
INLINE bool Char2<MO>::c_inv() const {
//	return( rep.c_inv() );
	return( true );
}

template<class MO>
std::ostream& operator<<( std::ostream& os, const Char2<MO>& r ) {
	assert( r.c_inv() );
	os << "Char2 = (\n" << r.rep << ")\n";
	return( os );
}
