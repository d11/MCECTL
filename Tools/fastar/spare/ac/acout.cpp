/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACOUT_CPP

#include "alphabet.hpp"
#include "acout.hpp"

ACOutput::ACOutput( const std::set<String>& P,
			const FTrie& t,
			const FFail& f ) :
		rep( t.size() ) {
//	assert( P.c_inv() );
	assert( t.c_inv() );
	assert( rep.size() == t.size() );
	// Check that all of the sets are empty.
	auto State q;
	for( q = FIRSTSTATE; q < rep.size(); q++ ) {
		assert( rep[q].empty() );
	}

	// Do this in two phases: first the keywords, then work
	// on the trie.
//	for( int i = 0; i < P.size(); i++ ) {
	for( std::set<String>::const_iterator i = P.begin(); i != P.end(); i++ ) {
		// Iterate over keyword i.
		auto State u = FIRSTSTATE;
//		auto int j;
		auto String::const_iterator j;
//		for( j = 0; P.iterSelect( i )[j]; j++ ) {
		for( j = i->begin(); j != i->end(); j++ ) {
			// Note that the chars in P_i must be normalized to correspond
			// to the normalization already done in the ctor of trie t.
//			u = t.image( u, alphabetNormalize( P.iterSelect( i )[j] ) );
			u = t.image( u, alphabetNormalize( *j ) );
			// Since P and t should correspond, this can't be an
			// INVALIDSTATE.
			assert( u != INVALIDSTATE );
		}
		// u should correspond to keyword P_i.
//		assert( t.BFTdepth( u ) == j
		assert( t.BFTdepth( u ) == std::distance( i->begin(), j )
//			&& j == P.iterSelect( i ).length() );
			&& j == i->end() );
//		rep.map( u ).insert( P.iterSelect( i ) );
		rep.map( u ).insert( *i );
	}
	// Now do a BFT over the trie, computing the ACOutput function.
	// There's no need to do the first level of the trie.
	for( q = t.BFTsuccessor( t.BFTfirst() );
			q != INVALIDSTATE; q = t.BFTsuccessor( q ) ) {
		rep.map( q ).insert( rep[f[q]].begin(), rep[f[q]].end() );
	}
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const ACOutput& t ) {
	assert( t.c_inv() );
	os << "ACOutput = (\n" << t.rep << ")\n";
	assert( t.c_inv() );
	return( os );
}

