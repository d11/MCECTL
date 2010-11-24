/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACMOPT_HPP
#define ACMOPT_HPP
#define IN_ACMOPT_HPP

#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "acgamma.hpp"
#include "acout.hpp"
#include <assert.h>
#include <iostream>

// This is the Aho-Corasick transition and output machine corresponding
// to the AC optimized algorithm.

class ACMachineOpt {
public:
	ACMachineOpt( const std::set<String>& P );
	ACMachineOpt( const ACMachineOpt& M );
	~ACMachineOpt();

	// The machine transition stuff.
	State transition( const State q, const char a ) const;
	const std::set<String>& output( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const ACMachineOpt& t );
private:
	#include "acmopt_p.hpp"
};

#include "acmopt_i.hpp"

#undef IN_ACMOPT_HPP
#endif

