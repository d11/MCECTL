/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHIFTNORM_HPP
#define CWSHIFTNORM_HPP
#define IN_CWSHIFTNORM_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "fails.hpp"
#include "cwout.hpp"
#include "cwchar.hpp"
#include "cwd1.hpp"
#include "cwd2.hpp"
#include "com-misc.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// Normal Commentz-Walter shifter.

class CWShiftNorm {
public:
	CWShiftNorm( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftNorm( const CWShiftNorm& r );
	~CWShiftNorm();

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftNorm& r );
private:
	#include "cwshnorm_p.hpp"
};

#include "cwshnorm_i.hpp"

#undef IN_CWSHIFTNORM_HPP
#endif

