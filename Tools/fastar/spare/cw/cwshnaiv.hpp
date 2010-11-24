/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHNAIV_HPP
#define CWSHNAIV_HPP
#define IN_CWSHNAIV_HPP

#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "cwout.hpp"
#include <assert.h>
#include <iostream>

// A naive Commentz-Walter shifter.

class CWShiftNaive {
public:
	CWShiftNaive( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftNaive( const CWShiftNaive& r );

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftNaive& r );
};

#include "cwshnaiv_i.hpp"

#undef IN_CWSHNAIV_HPP
#endif

