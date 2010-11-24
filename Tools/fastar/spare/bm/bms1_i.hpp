/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"
#include "com-io.hpp"

template<class MO>
S1<MO>::S1( const String& p, const MO& mo ) :
		rep( p.length() + 1 ) {
	auto int pLen = p.length();
	// Use a simple linear search to compute the MIN quant.
	for( int i = 0; i <= pLen; i++ ) {
		// Do the linear search for this particular i.
		auto int k;
		for( k = 1; true; k++ ) {
			// Check the quantification given in I'_3.
			auto int h;
			for( h = 0; h < i; h++ ) {
				// The following if guard is taken from the range
				// of I'_3.
				if( k <= mo.traverse( h )
						&& p[mo.traverse( h )]
							!= p[mo.traverse( h ) - k] ) {
					// The quantification is not true.
					break;
				}
			}
			// It could be that the quantification is true,
			// if the above loop got all the way to the end.
			if( h == i ) {
				// I'_3 is true.
				break;
			}
		}
		// The above for loop should eventually end.
		assert( k <= pLen );
		// k is the shift distance.
		rep.at( i ) = k;
	}
	assert( c_inv() );
}

template<class MO>
INLINE S1<MO>::S1( const S1& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class MO>
INLINE int S1<MO>::shift( const int i ) const {
	assert( c_inv() );
	return( rep[i] );
}

template<class MO>
INLINE bool S1<MO>::c_inv() const {
//	return( rep.c_inv() );
	return( true );
}

template<class MO>
std::ostream& operator<<( std::ostream& os, const S1<MO>& r ) {
	assert( r.c_inv() );
	os << "S1 = (\n" << r.rep << ")\n";
	return( os );
}
