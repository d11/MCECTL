/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// fgrep example.
// $Revision: 1.1.1.1.2.2 $
// $Date: 2004/06/01 10:05:22 $

//#include <fstream.h>
#include <fstream>
#include "string.hpp"
#include <set>
//#include "set.hpp"
#include "cw/cws.hpp"

static int counter = 0;

static bool repAll( int where, const std::set<String>& what ) {
	counter++;
	std::cout << "\nEnding at " << where << " : " << what;
	return( true );
}

static bool countAll( int where, const std::set<String>& what ) {
	counter++;
	return( true );
}

int main( int argc, const char* argv[] ) {
	if( argc < 4 || !(argv[1][0] == '-'
			&& (argv[1][1] == 'n' || argv[1][1] == 'c')) ) {
		std::cerr << "Usage: cwgrep -{n,c} keyword1 ... keywordn inputfile\n";
		return( -1 );
	}
	
	auto std::set<String> P;
	for( int i = 2; i < argc - 1; i++ ) {
		P.insert( argv[i] );
	}
	auto PMCWRLA M( P );

	auto std::ifstream incoming( argv[argc - 1] );
	auto String S;
	auto char a;
	incoming.seekg( 0, std::ios::beg );
	while( incoming.get( a ) ) {
    char c = alphabetNormalize(a);
	if (0 <= c && c < ALPHABETSIZE) {
			S += a;
		}
	}

	M.match( S, argv[1][1] == 'n' ? repAll : countAll );
	std::cout << "\n\n" << "Count : " << counter << std::endl;

	return( 0 );
}

