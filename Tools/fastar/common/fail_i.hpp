/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"

template<class T>
Fail<T>::Fail( const Trie<T>& r ) :
		rep( r.size() ) {
	assert( r.c_inv() );
	// Do the first BFT level of the failure function separately
	// from the other levels.
	rep.map( FIRSTSTATE ) = INVALIDSTATE;
	// Iterate over the alphabet, and do symbols that have out-trans
	// from the FIRSTSTATE.
	for( char a = 0; a < ALPHABETSIZE; a++ ) {
		if( r.image( FIRSTSTATE, a ) != INVALIDSTATE ) {
			rep.map( r.image( FIRSTSTATE, a ) ) = FIRSTSTATE;
		}
	}
	// Now perform a BFT over the Trie to get the failure function
	// by doing the remaining layers.
	for( State q = r.BFTsuccessor( r.BFTfirst() ); q != INVALIDSTATE;
			q = r.BFTsuccessor( q ) ) {
		// Go over all the out-transitions of q and setup the next
		// level of the failure function.
		for( char a = 0; a < ALPHABETSIZE; a++ ) {
			if( r.image( q, a ) != INVALIDSTATE ) {
				// Now go through the successive failures of q, to
				// find one with an out-transition on a.
				auto State qfail = rep[q];
				while( r.image( qfail, a ) == INVALIDSTATE
						&& qfail != FIRSTSTATE ) {
					qfail = rep[qfail];
				}
				// qfail is it, or we've gotten to qfail == FIRSTSTATE
				// and there's still no out-transition on a.
				if( r.image( qfail, a ) != INVALIDSTATE ) {
					rep.map( r.image( q, a ) ) = r.image( qfail, a );
				} else {
					assert( qfail == FIRSTSTATE
						&& r.image( qfail, a ) == INVALIDSTATE );
					rep.map( r.image( q, a ) ) = FIRSTSTATE;
				}
			}
		}
	}
	assert( c_inv() );
}

template<class T>
INLINE Fail<T>::Fail( const Fail<T>& r ) :
		rep( r.rep ) {
	assert( c_inv() );
}

template<class T>
INLINE State Fail<T>::operator[]( const State q ) const {
	assert( c_inv() );
	assert( FIRSTSTATE <= q && q < size() );
	return( rep[q] );
}

template<class T>
INLINE int Fail<T>::size() const {
	assert( c_inv() );
	return( rep.size() );
}

template<class T>
INLINE bool Fail<T>::c_inv() const {
	return( rep.c_inv() );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const Fail<T>& t ) {
	assert( t.c_inv() );
	os << "Fail<T> = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}
