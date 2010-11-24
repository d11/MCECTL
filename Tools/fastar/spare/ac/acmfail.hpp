/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACMFAIL_HPP
#define ACMFAIL_HPP
#define IN_ACMFAIL_HPP

#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "aceftrie.hpp"
#include "fails.hpp"
#include "tries.hpp"
#include "acout.hpp"
#include <assert.h>
#include <iostream>

// This is the Aho-Corasick transition and output machine
// corresponding to the failure function version of the AC algorithm.

class ACMachineFail {
public:
	ACMachineFail( const std::set<String>& P );
	ACMachineFail( const ACMachineFail& M );
	~ACMachineFail();

	// The machine transition stuff.
	State transition( State q, const char a ) const;
	const std::set<String>& output( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const ACMachineFail& t );
private:
	#include "acmfail_p.hpp"
};

#include "acmfail_i.hpp"

#undef IN_ACMFAIL_HPP
#endif

