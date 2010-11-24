/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWD1_HPP
#define CWD1_HPP
#define IN_CWD1_HPP

#include "com-misc.hpp"
#include "state.hpp"
#include "stateto.hpp"
#include "tries.hpp"
#include "fails.hpp"
#include <assert.h>
#include <iostream>

// Commentz-Walter's d_1 shift function.

class D1 {
public:
	D1( const RTrie& t, const RFail& f );
	D1( const D1& r );

	// The shift itself.
	int operator[]( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const D1& r );
private:
	#include "cwd1_p.hpp"
};

#include "cwd1_i.hpp"

#undef IN_CWC1_HPP
#endif

