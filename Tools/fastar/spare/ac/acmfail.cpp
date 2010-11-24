/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#define IN_ACMFAIL_CPP

#include "acmfail.hpp"
#include "alphabet.hpp"

ACMachineFail::ACMachineFail( const std::set<String>& P ) :
		trie( new FTrie( P ) ),
		tauef( *trie ),
		ff( *trie ),
		out( P, *trie, ff ) {
//	assert( P.c_inv() );
	assert( !P.empty() );
	assert( trie->c_inv() );
	// Now destroy the intermediate stuff.
	delete trie;
	trie = 0;
	assert( c_inv() );
}

std::ostream& operator<<( std::ostream& os, const ACMachineFail& t ) {
	assert( t.c_inv() );
	os << "ACMachineFail = (\n" << t.tauef << t.ff
		<< t.out << ")\n";
	assert( t.c_inv() );
	return( os );
}

