/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.2.2.2 $
// $Date: 2004/05/27 12:13:12 $
#include "com-opt.hpp"
#include "com-misc.hpp"
#include <algorithm>

template<class T>
Trie<T>::Trie( const std::set<String>& P ) :
		rep( 1 ),
		depth( 1 ) {
//	assert( P.c_inv() );
	assert( !P.empty() );

	// Copy P to vector P2 and sort in ascending order of length
	std::vector<String> P2;
	for(std::set<std::string>::const_iterator i = P.begin(); i != P.end(); i++) {
		P2.push_back(*i);
	}
	std::sort(P2.begin(), P2.end(), shorter);

	// The trie now has at least the epsilon (empty String) in it.
	// Set all of the epsilon out-transitions to INVALIDSTATE.
	for( int a = 0; a < ALPHABETSIZE; a++ ) {
		rep.map( FIRSTSTATE ).map( a ) = INVALIDSTATE;
	}
	// Make sure that the FIRSTSTATE is at depth 0.
	depth.map( FIRSTSTATE ) = 0;

	// Iterate over all of the keywords.
//	for( int i = 0; i < P.size(); i++ ) {
	for( std::vector<String>::const_iterator i = P2.begin(); i != P2.end(); i++ ) {
		auto State q = FIRSTSTATE;
		// Create a traverser for the current keyword.
//		auto T trav( P.iterSelect( i ) );
		auto T trav( *i );

		// Iterate over the letters of each keyword.
//		for( int j = 0; P.iterSelect( i )[j]; j++ ) {
		for( String::const_iterator j = i->begin(); j != i->end(); j++) {
			// If there's already an out-transition, take it,
			// otherwise construct a new one.
//			if( image( q, alphabetNormalize( P.iterSelect( i )
//					[trav.traverse( j )] ) ) == INVALIDSTATE ) {
			if( image( q, alphabetNormalize( ( *i )
					[trav.traverse( std::distance( i->begin(), j ) )] ) )
					== INVALIDSTATE ) {
				// Create a new State and adjust everything.
				auto State latest = rep.size();
				rep.setSize( latest + 1 );
				depth.setSize( latest + 1 );

				// Set the out-transitions of the new State
				// to INVALIDSTATE.
				for( int a = 0; a < ALPHABETSIZE; a++ ) {
					rep.map( latest ).map( a ) = INVALIDSTATE;
				}
				// latest will be at one level below q, depth-wise.
				depth.map( latest ) = depth[q] + 1;

//				rep.map( q ).map( alphabetNormalize(
//						P.iterSelect( i )[trav.traverse( j )] ) )
//						= latest;
				rep.map( q ).map( alphabetNormalize( ( *i )
						[trav.traverse( std::distance( i->begin(), j ) )] ) )
						= latest;

			}
//			assert( image( q, alphabetNormalize( P.iterSelect( i )
//					[trav.traverse( j )] ) ) != INVALIDSTATE );
			assert( image( q, alphabetNormalize( ( *i )
					[trav.traverse( std::distance( i->begin(), j ) )] ) )
					!= INVALIDSTATE );
//			q = image( q, alphabetNormalize( P.iterSelect( i )
//					[trav.traverse( j )] ) );
			q = image( q, alphabetNormalize( ( *i )
					[trav.traverse( std::distance( i->begin(), j ) )] ) );
		}
	}
	assert( c_inv() );
}

template<class T>
INLINE Trie<T>::Trie( const Trie<T>& r ) :
		rep( r.rep ),
		depth( r.depth ) {
	assert( c_inv() );
}

template<class T>
INLINE State Trie<T>::image( const State q, const char a ) const {
	assert( c_inv() );
	assert( FIRSTSTATE <= q && q < size() );
	// Assume that a is already normalized by the caller.
	assert( 0 <= a && a < ALPHABETSIZE );
	return( rep[q][a] );
}

template<class T>
INLINE State Trie<T>::BFTfirst() const {
	assert( c_inv() );
	assert( depth[FIRSTSTATE] == 0 );
	// Assume that we always start allocating States at 0.
	return( FIRSTSTATE );
}

template<class T>
State Trie<T>::BFTsuccessor( const State q ) const {
	assert( c_inv() );
	// Short-cut for the last state in a BFT.
	// (Assumes that size() was the last state allocated.)
	if( q == size() - 1 ) {
		return( INVALIDSTATE );
	}
	// Search for the next state at this BFT level.
	auto State r;
	for( r = q + 1; r < size(); r++ ) {
		if( depth[r] == depth[q] ) {
			return( r );
		}
	}
	// There wasn't one at this level, check the next BFT level.
	for( r = FIRSTSTATE; r < size(); r++ ) {
		if( depth[r] == depth[q] + 1 ) {
			return( r );
		}
	}
	// There is nothing at a deeper level.
	// This shouldn't even be happening.
	assert( !"I shouldn't be here" );
	return( INVALIDSTATE );
}

template<class T>
INLINE int Trie<T>::BFTdepth( const State q ) const {
	assert( c_inv() );
	return( depth[q] );
}

template<class T>
INLINE int Trie<T>::size() const {
	assert( c_inv() );
	return( rep.size() );
}

template<class T>
INLINE bool Trie<T>::c_inv() const {
	return( rep.c_inv() 
 		&& depth.c_inv()
   	&& !depth[FIRSTSTATE]
		&& rep.size() == depth.size() );
}

template<class T>
std::ostream& operator<<( std::ostream& os, const Trie<T>& t ) {
	assert( t.c_inv() );
	os << "Trie<T> = (\n" << t.rep << t.depth << ")\n";
	assert( t.c_inv() );
	return( os );
}
