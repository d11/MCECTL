/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef BMSLNONE_HPP
#define BMSLNONE_HPP
#define IN_BMSLNONE_HPP

#include "string.hpp"
#include <assert.h>
#include <iostream>

// One of the `skip-loop' classes. This one provides no
// skip distance (a shift of 0).

class SLNone {
public:
	SLNone( const String& kw );
	SLNone( const SLNone& r );

	// The skip itself. Return the next index in S after j.
	int skip( const String& S, const int j, const int last ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const SLNone& r );
private:
};

#include "bmslnone_i.hpp"

#undef IN_BMSLNONE_HPP
#endif

