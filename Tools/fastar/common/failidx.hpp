/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef FAILIDX_HPP
#define FAILIDX_HPP
#define IN_FAILIDX_HPP

#include "string.hpp"
//#include "array.hpp"
#include <vector>
#include <assert.h>
#include <iostream>

// Provide the functionality of the indexing version of the
// failure function for a single keyword.

class FailIdx {
public:
	FailIdx( const String& kw );
	FailIdx( const FailIdx& f );

	// The failure function itself.
	int operator[]( const int i ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const FailIdx& t );
private:
	#include "failidx_p.hpp"
};

#include "failidx_i.hpp"

#undef IN_FAILIDX_HPP
#endif

