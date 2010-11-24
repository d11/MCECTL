/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// fgrep example.
// $Revision: 1.1.1.1.2.2 $
// $Date: 2004/06/01 10:05:22 $

//#include <fstream.h>
#include <fstream>
#include "string.hpp"
#include "kmp/pm-kmp.hpp"

static int counter = 0;

static bool repAll( int where ) {
	counter++;
	std::cout << "\nEnding at " << where;
	return( true );
}

static bool countAll( int where ) {
	counter++;
	return( true );
}

int main( int argc, const char* argv[] ) {
	if( argc != 4 || !(argv[1][0] == '-'
			&& (argv[1][1] == 'n' || argv[1][1] == 'c')) ) {
		std::cerr << "Usage: kmpgrep -{n,c} keyword inputfile\n";
		return( -1 );
	}
	
	auto String p( argv[2] );
	auto PMKMP M( p );

	auto std::ifstream incoming( argv[3] );
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

