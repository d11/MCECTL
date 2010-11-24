/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#include "com-opt.hpp"

template<class MO, class SL, class MI>
INLINE PMBM<MO,SL,MI>::PMBM( const String& kw ) :
		keyword( kw ),
		mo( keyword ),
		sl( keyword ),
		shifter( keyword, mo ) {
	assert( c_inv() );
}

template<class MO, class SL, class MI>
void PMBM<MO,SL,MI>::match( const String& S, bool callBack (int) ) {
	assert( c_inv() );
	// Use an index to denote the end of the current
	// match attempt. The name is chosen to go with v, in
	// the taxonomy.
	auto int pLen = keyword.length();
	// Keep track of the last possible place to be matching.
	auto int Slast = S.length() - pLen;
	auto int vBegin = 0;

	// Try matching, while we're not at the end of the input.
	while( vBegin <= Slast ) {
		// Do some skip looping.
		vBegin = sl.skip( S, vBegin, Slast );

		// Make sure that it worked out okay.
		assert( vBegin <= Slast );

		// Now time for a match attempt.
		auto int i;
		for( i = 0; i < pLen
				&& S[vBegin + mo.traverse( i )]
					== keyword[mo.traverse( i )]; i++ ) {
			// Intentionally empty.
		}

		if( i == pLen ) {
			// There was a match; call the client back.
			if( !callBack( vBegin + pLen ) ) {
				// The client wants to quit.
				return;
			}
		}

		// Now make the shift. Normalize the mismatched character
		// here since the shifter won't do it.
		// Just check that the i indexing into S is still valid though.
		vBegin += shifter.shift( i,
			alphabetNormalize( i >= pLen ? 0 :
				S[vBegin + mo.traverse( i )] ) );
	}
}

template<class MO, class SL, class MI>
INLINE bool PMBM<MO,SL,MI>::c_inv() const {
//	return( keyword.c_inv() && mo.c_inv()
	return( mo.c_inv()
		&& sl.c_inv() && shifter.c_inv() );
}

template<class MO, class SL, class MI>
std::ostream& operator<<( std::ostream& os, const PMBM<MO,SL,MI> r ) {
	assert( r.c_inv() );
	os << "PMBM = (\n" << r.keyword << r.mo << r.sl << r.shifter << ")\n";
	return( os );
}
