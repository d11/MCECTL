/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHOPT_HPP
#define CWSHOPT_HPP
#define IN_CWSHOPT_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "fails.hpp"
#include "cwout.hpp"
#include "cwdopt.hpp"
#include "cwd2.hpp"
#include "com-misc.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// Optimized Commentz-Walter shifter (Fan and Su).

class CWShiftOpt {
public:
	CWShiftOpt( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftOpt( const CWShiftOpt& r );
	~CWShiftOpt();

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftOpt& r );
private:
	#include "cwshopt_p.hpp"
};

#include "cwshopt_i.hpp"

#undef IN_CWSHOPT_HPP
#endif

