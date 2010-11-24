/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef S_TRAV_OM_HPP
#define S_TRAV_OM_HPP
#define IN_S_TRAV_OM_HPP

#include "com-misc.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// Generate indices to index into a String.
// The OM stands for `Optimal Mismatch' (from the BM
// algorithms).

class STravOM {
public:
	STravOM( const String& s );

	// Traverse the String.
	int traverse( int index ) const;
	int traverseInverse( int index ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const STravOM& t );
private:
	#include "stravom_p.hpp"
};

#include "stravom_i.hpp"

#undef IN_S_TRAV_OM_HPP
#endif

