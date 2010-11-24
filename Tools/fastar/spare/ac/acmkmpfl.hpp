/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACMKMPFAIL_HPP
#define ACMKMPFAIL_HPP
#define IN_ACMKMPFAIL_HPP

#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "acout.hpp"
#include <assert.h>
#include <iostream>

// This is the Aho-Corasick transition and output machine corresponding
// to the AC-KMP failure function algorithm.

class ACMachineKMPFail {
public:
	ACMachineKMPFail( const std::set<String>& P );
	ACMachineKMPFail( const ACMachineKMPFail& M );
	~ACMachineKMPFail();

	// The machine transition stuff.
	State transition( State q, const char a ) const;
	const std::set<String>& output( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const ACMachineKMPFail& t );
private:
	#include "acmkmpfl_p.hpp"
};

#include "acmkmpfl_i.hpp"

#undef IN_ACMKMPFAIL_HPP
#endif

