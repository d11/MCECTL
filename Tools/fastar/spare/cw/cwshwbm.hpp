/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWSHIFTWBM_HPP
#define CWSHIFTWBM_HPP
#define IN_CWSHIFTWBM_HPP

#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "fails.hpp"
#include "cwout.hpp"
#include "cwcharbm.hpp"
#include "cwd1.hpp"
#include "cwd2.hpp"
#include "com-misc.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// Weak Boyer-Moore Commentz-Walter shifter.

class CWShiftWBM {
public:
	CWShiftWBM( const std::set<String>& P,
			const RTrie& t,
			const CWOutput& out );
	CWShiftWBM( const CWShiftWBM& r );
	~CWShiftWBM();

	// The shifter itself.
	int shift( const RTrie& t,
			const char l,
			const State v,
			const char r ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWShiftWBM& r );
private:
	#include "cwshwbm_p.hpp"
};

#include "cwshwbm_i.hpp"

#undef IN_CWSHIFTWBM_HPP
#endif

