/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMSLFST1_HPP
#define BMSLFST1_HPP
#define IN_BMSLFST1_HPP

#include "com-misc.hpp"
#include "string.hpp"
#include "symbolto.hpp"
#include <assert.h>
#include <iostream>

// One of the `skip-loop' classes. This one is the `Fast
// skip' by Hume & Sunday. This implements the sl_1 skip
// distance for the J_2 skip predicate.

class SLFast1 {
public:
	SLFast1( const String& kw );
	SLFast1( const SLFast1& r );

	// The skip itself. Return the next index in S after j.
	int skip( const String& S, int j, const int last ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const SLFast1& r );
private:
	#include "bmslfst1_p.hpp"
};

#include "bmslfst1_i.hpp"

#undef IN_BMSLFST1_HPP
#endif

