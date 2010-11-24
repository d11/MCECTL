/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

template<class MO>
INLINE BMShiftNaive<MO>::BMShiftNaive( const String& p, const MO& mo ) {
	assert( c_inv() );
}

template<class MO>
INLINE BMShiftNaive<MO>::BMShiftNaive( const BMShiftNaive<MO>& r ) {
	assert( c_inv() );
}

template<class MO>
INLINE int BMShiftNaive<MO>::shift( const int i, const char a ) const {
	return( 1 );
}

template<class MO>
INLINE bool BMShiftNaive<MO>::c_inv() const {
	return( true );
}

template<class MO>
std::ostream& operator<<( std::ostream& os, const BMShiftNaive<MO>& r ) {
	assert( r.c_inv() );
	os << "BMShiftNaive = ()\n";
	return( os );
}
