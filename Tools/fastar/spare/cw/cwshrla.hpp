/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHRLA_HPP
#define CWSHRLA_HPP
#define IN_CWSHRLA_HPP

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
#include "cwcharrl.hpp"
#include "com-misc.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// Optimized Commentz-Walter shifter (Fan and Su).

class CWShiftRLA {
public:
	CWShiftRLA( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftRLA( const CWShiftRLA& r );
	~CWShiftRLA();

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftRLA& r );
private:
	#include "cwshrla_p.hpp"
};

#include "cwshrla_i.hpp"

#undef IN_CWSHRLA_HPP
#endif

