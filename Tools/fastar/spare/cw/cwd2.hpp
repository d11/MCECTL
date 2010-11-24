/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWD2_HPP
#define CWD2_HPP
#define IN_CWD2_HPP

#include "com-misc.hpp"
#include "state.hpp"
#include "stateto.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include "tries.hpp"
#include "fails.hpp"
#include "cwout.hpp"
#include <assert.h>
#include <iostream>

// Commentz-Walter's d_2 shift function.

class D2 {
public:
	D2( const std::set<String>& P,
			const RTrie& t,
			const RFail& f,
			const CWOutput& out );
	D2( const D2& r );

	// The shift itself.
	int operator[]( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const D2& r );
private:
	#include "cwd2_p.hpp"
};

#include "cwd2_i.hpp"

#undef IN_CWD2_HPP
#endif

