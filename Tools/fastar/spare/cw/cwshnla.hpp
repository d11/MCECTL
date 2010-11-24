/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHIFTNLA_HPP
#define CWSHIFTNLA_HPP
#define IN_CWSHIFTNLA_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "fails.hpp"
#include "cwout.hpp"
#include "cwd1.hpp"
#include "cwd2.hpp"
#include <assert.h>
#include <iostream>

// No lookahead Commentz-Walter shifter.

class CWShiftNLA {
public:
	CWShiftNLA( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftNLA( const CWShiftNLA& r );

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftNLA& r );
private:
	#include "cwshnla_p.hpp"
};

#include "cwshnla_i.hpp"

#undef IN_CWSHIFTNLA_HPP
#endif

