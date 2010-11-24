/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.2.2.1 $
// $Date: 2004/05/27 12:13:13 $

// Module for testing.
#include "com-misc.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "ac/acs.hpp"
#include "cw/cws.hpp"
#include "kmp/pm-kmp.hpp"
#include "bm/bms.hpp"
#include "tries.hpp"
#include <iostream>
#include <vector>

// Keep track of the number of matches found.
static int count = 0;

// Some functions for the call-backs.
bool f1( int a ) {
	count++;
	std::cout << "match: " << a << '\n';
	return( true );
}

bool f2( int a, const std::set<String>& O ) {
	count++;
	std::cout << "match: " << a << '\n' << O << '\n';
	return( true );
}

// The main test function.
int main() {
	auto String p( "he" );

	auto std::set<String> P;
	P.insert( "his" ); P.insert( "her" ); P.insert( "she" ); P.insert( "sher" );
	auto String S( "hishershey" );

	auto Trie<STravFWD> O1( P );
	std::cout << O1;

	auto PMKMP M1( p );
	M1.match( S, f1 );
	std::cout << count;

	count = 0;
	auto PMACOpt M2( P );
	M2.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMACFail M3( P );
	M3.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMACKMPFail M4( P );
	M4.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWNaive M48( P );
	M48.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWNLA M5( P );
	M5.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWWBM M58( P );
	M58.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWNorm M6( P );
	M6.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWOpt M7( P );
	M7.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMCWRLA M8( P );
	M8.match( S, f2 );
	std::cout << count;

	count = 0;
	auto PMBM< STravREV, SLNone, BMShift11<STravREV> > N1( p );
	N1.match( S, f1 );
	std::cout << count;

	count = 0;
	auto PMBM< STravFWD, SLNone, BMShift12<STravFWD> > N2( p );
	N2.match( S, f1 );
	std::cout << count;

	return( 0 );
}

