/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef S_TRAV_REV_HPP
#define S_TRAV_REV_HPP
#define IN_S_TRAV_REV_HPP

#include "string.hpp"
#include <assert.h>
#include <iostream>

// Generate indices to index into a String.
// The REV stands for `reverse'.

class STravREV {
public:
	STravREV( const String& s );

	// Traverse the String.
	int traverse( int const index ) const;
	int traverseInverse( const int index ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const STravREV& t );
private:
	#include "stravrev_p.hpp"
};

#include "stravrev_i.hpp"

#undef IN_S_TRAV_REV_HPP
#endif

