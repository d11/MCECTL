/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef S_TRAV_RAN_HPP
#define S_TRAV_RAN_HPP
#define IN_S_TRAV_RAN_HPP

#include "com-misc.hpp"
#include "string.hpp"
#include <assert.h>
#include <iostream>

// Generate indices to index into a String.
// The RAN stands for `random'.

class STravRAN {
public:
	STravRAN( const String& s );

	// Traverse the String.
	int traverse( int index ) const;
	int traverseInverse( int index ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const STravRAN& t );
private:
	#include "stravran_p.hpp"
};

#include "stravran_i.hpp"

#undef IN_S_TRAV_RAN_HPP
#endif

