/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

template<class MO>
INLINE BMShift11<MO>::BMShift11( const String& p, const MO& mo ) :
		s1( p, mo ),
		char1( p, mo ) {
	assert( c_inv() );
}

template<class MO>
INLINE BMShift11<MO>::BMShift11( const BMShift11<MO>& r ) :
		s1( r.s1 ),
		char1( r.char1 ) {
	assert( c_inv() );
}

template<class MO>
INLINE int BMShift11<MO>::shift( const int i, const char a ) const {
	assert( c_inv() );
	// Expect a to be normalized already.
	return( max( s1.shift( i ), char1.shift( i, a ) ) );
}

template<class MO>
INLINE bool BMShift11<MO>::c_inv() const {
	return( s1.c_inv() && char1.c_inv() );
}

template<class MO>
std::ostream& operator<<( std::ostream& os, const BMShift11<MO>& r ) {
	assert( r.c_inv() );
	os << "BMShift11 = (\n" << r.s1 << r.char1 << ")\n";
	return( os );
}
