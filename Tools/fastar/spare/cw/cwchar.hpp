/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWCHAR_HPP
#define CWCHAR_HPP
#define IN_CWCHAR_HPP

#include "alphabet.hpp"
#include "com-misc.hpp"
#include "state.hpp"
#include "symbolto.hpp"
#include "tries.hpp"
#include <assert.h>
#include <iostream>

// Simple character shift function for the CW algorithm.

class CharCW {
public:
	CharCW( const RTrie& t );
	CharCW( const CharCW& r );

	// The shift itself.
	int shift( const char a, const int z ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CharCW& r );
private:
	#include "cwchar_p.hpp"
};

#include "cwchar_i.hpp"

#undef IN_CWCHAR_HPP
#endif

