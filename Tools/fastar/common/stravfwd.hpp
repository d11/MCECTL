/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef S_TRAV_FWD_HPP
#define S_TRAV_FWD_HPP
#define IN_S_TRAV_FWD_HPP

#include "com-misc.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// Generate indices to index into a String.
// The FWD stands for `forward'.

class STravFWD {
public:
	STravFWD( const String& s );
	int traverse( const int index ) const;
	int traverseInverse( const int index ) const;

	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const STravFWD& t );
};

#include "stravfwd_i.hpp"

#undef IN_S_TRAV_FWD_HPP
#endif

